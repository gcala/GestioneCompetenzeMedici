/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cartellinocompletoreader.h"
#include "sqlqueries.h"
#include "utilities.h"
#include "dipendente.h"
#include "cartellinocompleto.h"
#include "giornocartellinocompleto.h"
#include "totalicartellinocompleto.h"

#include <QDate>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>

CartellinoCompletoReader::CartellinoCompletoReader(QObject *parent)
    : QThread(parent)
{
    m_timeCardBegin = true;
    m_matricola = 0;
}

CartellinoCompletoReader::~CartellinoCompletoReader()
{
}

void CartellinoCompletoReader::setFile(const QString &file)
{
    fileName = file;
}

void CartellinoCompletoReader::setDbFile(const QString &file)
{
    m_dbFile = file;
}

void CartellinoCompletoReader::run()
{
    if(fileName.isEmpty()) {
        qDebug() << "stringa vuota";
        emit timeCardsRead();
        return;
    }

    if(!QFileInfo::exists(fileName)) {
        qDebug() << "file non esiste";
        emit timeCardsRead();
        return;
    }

    QFile file(fileName);

    if(!file.open(QIODevice::Text | QIODevice::ReadOnly)) {
        qDebug() << "impossibile leggere file";
        emit timeCardsRead();
        return;
    }

    QString tipoPrecedente = "";
    int anno = 0;
    int mese = 0;
    QString tableName = "";
    QDate dataCorrente;
    CartellinoCompleto *cartellino;

    int rowCount = 0;

    Utilities::m_connectionName = "CompletoReader";
    if(!QSqlDatabase::connectionNames().contains(Utilities::m_connectionName)) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", Utilities::m_connectionName);
        db.setDatabaseName(m_dbFile);
        if (!db.open()) {
            qDebug() << QLatin1String("Impossibile connettersi al database Cartellini Orologio.") << db.lastError().text();
            emit timeCardsRead();
            return;
        }
    }

    while(!file.atEnd()) {
        QString s = file.readLine();
        rowCount++;
    }

    emit totalRows(rowCount);

    file.reset();

    int currRow = 0;
    QTextStream in(&file);
    in.setAutoDetectUnicode (true);
    bool isFileStart = true;
    QDate currentYearMonth(1900,1,1);

    while(!in.atEnd()) {
        currRow++;
        emit currentRow(currRow);

        QString line = in.readLine().trimmed();

        if(isFileStart && !line.startsWith("\"\",A,ZIEN")) {
            file.close();
//            QFile::remove(fileName);
            emit timeCardsError("Il file non contiene cartellini completi.");
            emit timeCardsRead();
            return;
        }
        isFileStart = false;

        if(line.startsWith("Me,se"))
            continue;
        if(line.startsWith("GG"))
            continue;

        if(line.startsWith("\"\",A,ZIEN")) {
            anno = 0;
            mese = 0;
            m_matricola=0;
            cartellino = new CartellinoCompleto;

            line = line.replace(",", "");
            QRegularExpression periodRx("OROLOGIO\\s*(.+)\\s*Data");
            QRegularExpressionMatch match = periodRx.match(line);
            if (match.hasMatch()) {
                QString period = match.captured(1).trimmed();
                anno = period.right(4).toInt();
                mese = Utilities::mese2Int(period.split(period.right(4), Qt::SkipEmptyParts).at(0).trimmed());
            }
            // creiamo, se necessario, tabella cartellini
            if(anno == 0 || mese == 0) {
                emit timeCardsError("Impossibile recuperare mese e anno di riferimento. Esco...");
                return;
            }

            tableName = "tc_" + QString::number(anno) + QString::number(mese).rightJustified(2, '0');
            currentYearMonth.setDate(anno, mese, 1);

            if(!SqlQueries::tableExists(tableName)) {
                if(!SqlQueries::createTimeCardsTable(tableName)) {
                    emit timeCardsRead();
                    return;
                }
            }
            continue;
        }

        QRegularExpression matRx("^\\d+");
        QString temp = line;
        temp.replace(",", "");
        temp.replace("\"", "");
        QRegularExpressionMatch match = matRx.match(temp);
        if (match.hasMatch() && temp.contains("AOSC")) {
            int matricola;
            // riga con matricola;
//            line = line.replace(",", "");
            if (match.hasMatch()) {
                matricola = match.captured(0).trimmed().toInt();
            } else {
                qDebug() << "matricola non trovata in" << line;
                continue;
            }

            m_matricola = matricola;

            m_dipendente = new Dipendente;
            m_dipendente->setMatricola(m_matricola);
            m_dipendente->setAnno(anno);
            m_dipendente->setMese(mese);

            int unitaId = SqlQueries::unitId(m_dipendente->matricola());
            if(unitaId == -1) {
                emit selectUnit(m_dipendente->matricola(), unitaId);
            }

            m_dipendente->setUnita(unitaId);

            continue;
        } else {
            const auto fields = line.split(",");
            if (match.hasMatch()) {
                //nuovo giorno del mese
                // controlla se il giorno contiene "" invece del numero
                if(fields.at(0).trimmed().contains("\"\""))
                    continue;

                cartellino->addGiorno(giorno(line));
                continue;
            }

            QString textDisponibile = fields.at(0) + fields.at(1) + fields.at(2) + fields.at(3) + fields.at(4);

            if(textDisponibile.startsWith(Utilities::int2MeseBreve(mese))) {
                line.remove(",");
                line.remove(" ");
                line.remove(Utilities::int2MeseBreve(mese));
                QStringList f;
                QString temp;
                for(int i = 0; i < line.length(); i++) {
                    if(line.at(i) != ':' && (line.at(i).isDigit() || line.at(i) == '-')) {
                        temp.append(line.at(i));
                    } else {
                        if(line.at(i) == ':') {
                            temp.append(":");
                            temp.append(line.at(i+1));
                            temp.append(line.at(i+2));
                            f << temp;
                            temp.clear();
                            i += 2;
                        } else {
                            break;
                        }
                    }
                }
                if(f.size() == 12) {
                    cartellino->addTotali(totali(f));
                }

                // Inserimento dei nuovi valori
                /*
                CREATE TABLE `tc_202209` (
                  `id` integer NOT NULL PRIMARY KEY AUTOINCREMENT
                ,  `id_medico` integer NOT NULL
                ,  `id_unita` integer NOT NULL
                ,  `anno` integer NOT NULL
                ,  `mese` integer NOT NULL
                ,  `riposi` integer NOT NULL
                ,  `minuti_giornalieri` integer NOT NULL
                ,  `ferie` varchar(128) DEFAULT ''
                ,  `congedi` varchar(128) DEFAULT ''
                ,  `malattia` varchar(128) DEFAULT ''
                ,  `rmp` varchar(128) DEFAULT ''
                ,  `rmc` varchar(128) DEFAULT ''
                ,  `altre_causali` varchar(400) DEFAULT ''
                ,  `guardie_diurne` varchar(256) DEFAULT ''
                ,  `guardie_notturne` varchar(128) DEFAULT ''
                ,  `grep` varchar(512) DEFAULT ''
                ,  `congedi_minuti` integer DEFAULT 0
                ,  `eccr_minuti` integer DEFAULT 0
                ,  `grep_minuti` integer DEFAULT 0
                ,  `guar_minuti` integer DEFAULT 0
                ,  `rmc_minuti` integer DEFAULT 0
                ,  `minuti_fatti` integer DEFAULT 0
                ,  `scoperti` varchar(128) DEFAULT ''
                )
                */
            }
        }
    }

    file.close();
    QFile::remove(fileName);
    emit timeCardsRead();
}

GiornoCartellinoCompleto CartellinoCompletoReader::giorno(const QString &line)
{
    const QStringList fields = line.split(",");
    GiornoCartellinoCompleto giorno;
    giorno.setGiorno(fields.at(0).toInt());
    giorno.setTurno(fields.at(2).toInt());
    giorno.setIndennita(fields.at(3));
    giorno.setTipo(fields.at(4));
    giorno.setEntrata1(Timbratura(fields.at(5)));
    giorno.setUscita1(Timbratura(fields.at(6)));
    giorno.setEntrata2(Timbratura(fields.at(7)));
    giorno.setUscita2(Timbratura(fields.at(8)));
    giorno.setEntrata3(Timbratura(fields.at(9)));
    giorno.setUscita3(Timbratura(fields.at(10)));
    giorno.setEntrata4(Timbratura(fields.at(11)));
    giorno.setUscita4(Timbratura(fields.at(12)));
    giorno.setOrdinario(QTime::fromString(fields.at(13), "h:mm"));
    giorno.setAssenza(QTime::fromString(fields.at(14), "h:mm"));
    giorno.setEccedenza(QTime::fromString(fields.at(15), "h:mm"));
    giorno.setDaRecuperare(QTime::fromString(fields.at(16), "h:mm"));
    giorno.setRecuperate(QTime::fromString(fields.at(17), "h:mm"));
    giorno.setCausale1(fields.at(18));
    giorno.setOre1(QTime::fromString(fields.at(19), "h:mm"));
    giorno.setCausale2(fields.at(20));
    giorno.setOre2(QTime::fromString(fields.at(21), "h:mm"));
    giorno.setCausale3(fields.at(22));
    giorno.setOre3(QTime::fromString(fields.at(23), "h:mm"));
    giorno.setRepDiurna(QTime::fromString(fields.at(24), "h:mm"));
    giorno.setRepNotturna(QTime::fromString(fields.at(25), "h:mm"));
    return giorno;
}

TotaliCartellinoCompleto CartellinoCompletoReader::totali(const QStringList &fields)
{
    TotaliCartellinoCompleto totali;
    totali.setDisponibile(Utilities::inMinuti(fields.at(0)));
    totali.setOrdinario(Utilities::inMinuti(fields.at(1)));
    totali.setRecuperato(Utilities::inMinuti(fields.at(2)));
    totali.setGiustificato(Utilities::inMinuti(fields.at(3)));
    totali.setDaRecuperare(Utilities::inMinuti(fields.at(4)));
    totali.setEccedenza(Utilities::inMinuti(fields.at(5)));
    totali.setStrPagato(Utilities::inMinuti(fields.at(6)));
    totali.setStrReperibilita(Utilities::inMinuti(fields.at(7)));
    totali.setBancaOre(Utilities::inMinuti(fields.at(8)));
    totali.setScarRes(Utilities::inMinuti(fields.at(9)));
    totali.setSaldoMese(Utilities::inMinuti(fields.at(10)));
    totali.setTotale(Utilities::inMinuti(fields.at(11)));
    return totali;
}
