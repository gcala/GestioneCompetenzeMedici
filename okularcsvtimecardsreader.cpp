/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "okularcsvtimecardsreader.h"
#include "sqlqueries.h"
#include "dipendente.h"
#include "nomiunitadialog.h"
#include "utilities.h"
#include "sqldatabasemanager.h"

#include <QDate>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

OkularCsvTimeCardsReader::OkularCsvTimeCardsReader(QObject *parent) :
    QThread(parent)
{
    causaliFerie << "FEAP" << "FEAC" << "F2AP" << "FSPR";
    causaliMalattia << "MA" << "MAPR" << "MAPG" << "MACS" << "RICO" << "CSMA" << "INFO";
    causaliCongedi << "PRMP" << "ANA" << "CONV"
                   << "MAGI" << "DS" << "LUTT"
                   << "MATR" << "RIRA" << "RIAN"
                   << "104G" << "AAR1" << "AF10"
                   << "AF30" << "AF0" << "MBA3" << "MBAM";
    causaliRMP << "RMP";
    causaliRMC << "RMC";
    causaliDaValutare << "ECCR" << "RMC" << "GUAR" << "GREP";
    m_timeCardBegin = true;
    m_nomiDialog = new NomiUnitaDialog;
}

OkularCsvTimeCardsReader::~OkularCsvTimeCardsReader()
{
    delete m_nomiDialog;
}

void OkularCsvTimeCardsReader::setFile(const QString &file)
{
    fileName = file;
}

void OkularCsvTimeCardsReader::run()
{
    Utilities::m_connectionName = "OkularCsvTimeCardsReader";

    if(!The::dbManager()->createConnection()) {
        emit timeCardsRead();
        return;
    }

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
    int forseNotte = false;
    QDate dataCorrente;
    QString tableName = "";
    bool isRestDay = false;

    int rowCount = 0;

    while(!file.atEnd()) {
        QString s = file.readLine();
        rowCount++;
    }

    emit totalRows(rowCount);

    file.reset();

    int currRow = 0;
    m_nomiDialog->populateUnits();

    while(!file.atEnd()) {
        currRow++;
        emit currentRow(currRow);

        isRestDay = false;
        QString line = file.readLine();

        if(line.startsWith("Gio"))
            continue;

        if(line.startsWith("AZI")) {
            if(anno == 0 && mese == 0) {
                line = line.replace("~", "");
                QRegularExpression periodRx("Orologio\\s+(.+)\\s+Data");
                QRegularExpressionMatch match = periodRx.match(line);
                if (match.hasMatch()) {
                    QString period = match.captured(1).trimmed();
                    anno = period.right(4).toInt();
                    mese = mese2Int(period.split(period.right(4), Qt::SkipEmptyParts).at(0).trimmed());
                }
                // creiamo, se necessario, tabella cartellini
                if(anno == 0 && mese == 0) {
                    qDebug() << "Impossibile recuperare mese e anno di riferimento. Esco...";
                    emit timeCardsRead();
                    return;
                }
                tableName = "tc_" + QString::number(anno) + QString::number(mese).rightJustified(2, '0');
                if(!SqlQueries::tableExists(tableName)) {
                    if(!SqlQueries::createTimeCardsTable(tableName)) {
                        emit timeCardsRead();
                        return;
                    }
                }
            }
            continue;
        }

        if(line.trimmed() == "~~~~~~~~~~~~~~~~~~~~~") {
                    // fine cartellino mese
                    if(forseNotte) {
                        m_dipendente->addGuardiaNotturna(QString::number(dataCorrente.day()));
                    }

                    SqlQueries::addTimeCard(tableName, m_dipendente);

                    forseNotte = false;
                    tipoPrecedente.clear();
                    m_timeCardBegin = true;

                    continue;
                }

        if(line.startsWith("~~~~"))
            continue;

        QRegularExpression matRx("^\\d{3,}");
        QRegularExpressionMatch match = matRx.match(line);
        if (match.hasMatch()) {
            int matricola;
            // riga con matricola;
            line = line.replace("~", "");
            QRegularExpressionMatch match2 = matRx.match(line);
            if (match2.hasMatch()) {
                matricola = match2.captured(1).trimmed().toInt();
            }else {
                qDebug() << "matricola non trovata in" << line;
                continue;
            }

            line = line.replace(QString::number(matricola),"").trimmed();

            QStringList sl = line.split("AOSC");
            m_dipendente->setNome(sl.at(0).trimmed());

            m_dipendente = new Dipendente;
            m_dipendente->setMatricola(matricola);
            m_dipendente->setAnno(anno);
            m_dipendente->setMese(mese);

            continue;
        } else {
            //nuovo giorno del mese
            QStringList campi = line.split("~");
            if(campi.count() != 22) {
                qDebug() << "numero colonne non corretto" << line.trimmed();
                emit timeCardsRead();
                return;
            }

            dataCorrente.setDate(anno,mese,campi.at(0).trimmed().toInt());

            if(m_timeCardBegin) {
                if(dataCorrente.day() != 1) {
                    for(int i = 1; i < dataCorrente.day(); i++) {
                        m_dipendente->addRiposi(1);
                    }
                }
                m_timeCardBegin = false;
            }

            // conta se giorno di riposo o festivo
            if(campi.at(1).toLower().trimmed() == "f" || campi.at(1).toLower().trimmed() == "r") {
                m_dipendente->addRiposi(1);
                isRestDay = true;
            }

            m_dipendente->addMinutiFatti(inMinuti(campi.at(10).trimmed()));

            // cerchiamo ore giornaliere da fare
            if(m_dipendente->minutiGiornalieri() == 0) {
                if((!campi.at(10).trimmed().isEmpty() || !campi.at(11).trimmed().isEmpty()) && campi.at(14).trimmed() != "SEES") {
                    m_dipendente->setMinutiGiornalieri(inMinuti(campi.at(10).trimmed()) + inMinuti(campi.at(11).trimmed()));
                } else if(!campi.at(12).trimmed().isEmpty()) {
                    m_dipendente->setMinutiGiornalieri(inMinuti(campi.at(12).trimmed()));
                }
            }

            for(int i = 14; i <= 18; i += 2) {
                QString causale = campi.at(i).trimmed();

                if(causale.isEmpty())
                    continue;

                if(causaliDaValutare.contains(causale)) {
                    if(causale == "ECCR")
                        m_dipendente->addMinutiEccr(inMinuti(campi.at(i+1).trimmed()));
                    else if(causale == "GUAR")
                        m_dipendente->addMinutiGuar(inMinuti(campi.at(i+1).trimmed()));
                    else if(causale == "GREP")  {
                        if(!campi.at(20).trimmed().isEmpty()) {
                            if(campi.at(20).trimmed() != "0") {
                                QString s = campi.at(20).trimmed();
                                s = s.rightJustified(4, '0');
                                s = s.left(2) + ":" + s.right(2);
                                m_dipendente->addGrep(dataCorrente.day(), inMinuti(s), 1);  // diurno
                            }
                        }
                        if(!campi.at(21).trimmed().isEmpty()) {
                            if(campi.at(21).trimmed() != "00:00") {
                                m_dipendente->addGrep(dataCorrente.day(), inMinuti(campi.at(21).trimmed()), 0);  // notturno
                            }
                        }
                        m_dipendente->addMinutiGrep(inMinuti(campi.at(i+1).trimmed()));
                    }
                    else {
                        m_dipendente->addMinutiRmc(inMinuti(campi.at(i+1).trimmed()));
                        m_dipendente->addRmc(QString::number(dataCorrente.day()));
                    }
                } else if(causale == "RMP") {
                    m_dipendente->addRmp(QString::number(dataCorrente.day()));
                } else if(causale == "SEES") {
                    m_dipendente->addMinutiFatti(inMinuti(campi.at(i+1).trimmed()));
                }else if(causaliFerie.contains(causale)) {
                    if(!campi.at(2).trimmed().isEmpty() || !campi.at(3).trimmed().isEmpty())
                        m_dipendente->addMinutiFatti(inMinuti(campi.at(i+1).trimmed()));
                    else
                        m_dipendente->addFerie(QString::number(dataCorrente.day()));
                } else if(causaliCongedi.contains(causale)) {
                    if(!isRestDay) {
                        if(!campi.at(2).trimmed().isEmpty() || !campi.at(3).trimmed().isEmpty())
                            m_dipendente->addMinutiFatti(inMinuti(campi.at(i+1).trimmed()));
                        else {
                            m_dipendente->addCongedo(QString::number(dataCorrente.day()));
                            m_dipendente->addMinutiCongedo(inMinuti(campi.at(i+1).trimmed()));
                        }
                    }
                } else if(causaliMalattia.contains(causale)) {
                    if(!isRestDay) {
                        if(!campi.at(2).trimmed().isEmpty() || !campi.at(3).trimmed().isEmpty())
                            m_dipendente->addMinutiFatti(inMinuti(campi.at(i+1).trimmed()));
                        else
                            m_dipendente->addMalattia(QString::number(dataCorrente.day()));
                    }
                } else {
                    if(!isRestDay) {
                        if(!campi.at(2).trimmed().isEmpty() || !campi.at(3).trimmed().isEmpty())
                            m_dipendente->addMinutiFatti(inMinuti(campi.at(i+1).trimmed()));
                        else
                            m_dipendente->addAltraCausale(causale, QString::number(dataCorrente.day()), inMinuti(campi.at(i+1).trimmed()));
                    }
                }
            }

            QString causale = campi.at(14).trimmed();

            int numTimbrature = 0;
            for(int i = 2; i < 9; i++) {
                if(!campi.at(i).trimmed().isEmpty())
                    numTimbrature++;
            }

            if(forseNotte && numTimbrature == 0) {
                m_dipendente->addGuardiaDiurna(QString::number(dataCorrente.addDays(-1).day()));
                forseNotte = false;
            } else if(forseNotte && campi.at(2).trimmed().isEmpty()) {
                m_dipendente->addGuardiaNotturna(QString::number(dataCorrente.addDays(-1).day()));
                forseNotte = false;
            }

            if(forseNotte && !campi.at(2).trimmed().isEmpty()) {
                m_dipendente->addGuardiaDiurna(QString::number(dataCorrente.addDays(-1).day()));
                forseNotte = false;
            }

            if(causale == "GUAR") {
                forseNotte = true;
            }
        }
    }

    file.close();
    emit timeCardsRead();
}

int OkularCsvTimeCardsReader::mese2Int(const QString &mese)
{
    if(mese == "Gennaio")
        return 1;
    if(mese == "Febbraio")
        return 2;
    if(mese == "Marzo")
        return 3;
    if(mese == "Aprile")
        return 4;
    if(mese == "Maggio")
        return 5;
    if(mese == "Giugno")
        return 6;
    if(mese == "Luglio")
        return 7;
    if(mese == "Agosto")
        return 8;
    if(mese == "Settembre")
        return 9;
    if(mese == "Ottobre")
        return 10;
    if(mese == "Novembre")
        return 11;

    return 11;
}

int OkularCsvTimeCardsReader::inMinuti(const QString &time)
{
    if(time.isEmpty())
        return 0;

    QStringList t = time.split(":");
    if(t.count() != 2) {
        qDebug() << "errore nella conversione di" << time;
        return 0;
    }

    return t.at(0).toInt() * 60 + t.at(1).toInt();
}
