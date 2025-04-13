/*
 *  SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>
 * 
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cartellinocompletoreader.h"
//#include "sqlqueries.h"
#include "utilities.h"
#include "dipendente.h"
#include "cartellinocompleto.h"
#include "giornocartellinocompleto.h"
#include "totalicartellinocompleto.h"
#include "sqldatabasemanager.h"
#include "almanac.h"
#include "competenza.h"
#include "apiservice.h"

#include <QDate>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
// #include <QSqlQuery>
// #include <QSqlError>
#include <QRegularExpression>

CartellinoCompletoReader::CartellinoCompletoReader(QObject *parent)
    : QThread(parent)
{
    m_timeCardBegin = true;
    m_matricola = 0;
    causaliRMP << "RMP" << "RHER";
    causaliRMC << "RMC";
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

void CartellinoCompletoReader::setDriver(const QString &driver)
{
    Utilities::m_driver = driver;
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
    CartellinoCompleto *cartellino;

    int rowCount = 0;
    bool ok = true;

    Utilities::m_connectionName = "CartellinoCompletoReader";
    // QSqlDatabase db = The::dbManager()->database(ok, Utilities::m_connectionName);

    // if(!ok) {
    //     qDebug() << "Impossibile creare la connessione" << Utilities::m_connectionName;
    //     emit timeCardsRead();
    //     return;
    // }

    // if(!QSqlDatabase::connectionNames().contains(Utilities::m_connectionName)) {
    //     if (!db.open()) {
    //         qDebug() << QLatin1String("Impossibile connettersi al database.") << db.lastError().text();
    //         emit timeCardsRead();
    //         return;
    //     }
    // }

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

    QStringList anomalie;

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
            m_giorniEccr.clear();   // per evitare di conteggiare due volte lo stesso giorno
            m_giorniGrep.clear();
            m_giorniGuar.clear();
            m_giorniGuarDiu.clear();
            m_giorniMezzaGuarDiu.clear();
            m_giorniGuarNot.clear();
            m_giorniIndFes.clear();
            m_giorniAltraCausale.clear();

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

            if(!ApiService::instance().tableExists(tableName)) {
                if(!ApiService::instance().createTimeCardsTable(tableName)) {
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
            line = line.replace(",", "");
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

            line = line.replace(QString::number(matricola),"").trimmed();

            QStringList sl = line.split("AOSC");
            m_dipendente->setNome(sl.at(0).trimmed().replace("\"",""));

            int unitaId = ApiService::instance().unitId(m_dipendente->matricola());
            if(unitaId == -1) {
                emit selectUnit(m_dipendente->nome(), unitaId);
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

                bool ok = false;
                fields.at(0).toInt(&ok);
                if(ok) {
                    if(fields.at(0).toInt() <= QDate(anno,mese,1).daysInMonth())
                        cartellino->addGiorno(giorno(line, anno, mese));
                }
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
//                if(f.size() == 12) {
//                    cartellino->addTotali(totali(f));
//                }
                if(f.size() >= 11) {
                    if(f.size() == 11) {
                        f << "00:00";
                    }
                    cartellino->addTotali(totali(f));
                }

                m_dipendente->setMinutiGiornalieri(cartellino->totali().disponibile());
                m_dipendente->addMinutiFatti(cartellino->totali().ordinario());
                bool guarFound;

                if(!cartellino->isValid()) {
                    emit cartellinoInvalido("Cartellino con errori per il dipendente " + QString::number(m_matricola) + ", mese " + currentYearMonth.toString("MMMM yyyy"));
                    anomalie << "Cartellino con errori per il dipendente " + QString::number(m_matricola) + ", mese " + currentYearMonth.toString("MMMM yyyy");
                    continue;
                }

                for(const auto &giorno : cartellino->giorni()) {
                    guarFound = false;
                    QDate dataCorrente(anno, mese, giorno.giorno());

                    if(giorno.numeroTimbratureOrdinarie() > 0 &&
                            Utilities::inMinuti(giorno.ordinario()) == 0 &&
                            giorno.causale1().isEmpty() &&
                            giorno.causale2().isEmpty() &&
                            giorno.causale2().isEmpty() &&
                            giorno.indennita() != "N") {
                        const QString s = QString::number(m_matricola) + ";" + m_dipendente->nome() + ";" + dataCorrente.toString("dd/MM/yyyy");
                        anomalie << s;
                    }

                    if(!giorno.causale1().isEmpty())
                        valutaCausale(giorno.causale1(), dataCorrente, giorno, giorno.ore1(), guarFound, cartellino->isLastDay(giorno.giorno()));
                    if(!giorno.causale2().isEmpty())
                        valutaCausale(giorno.causale2(), dataCorrente, giorno, giorno.ore2(), guarFound, cartellino->isLastDay(giorno.giorno()));
                    if(!giorno.causale3().isEmpty())
                        valutaCausale(giorno.causale3(), dataCorrente, giorno, giorno.ore3(), guarFound, cartellino->isLastDay(giorno.giorno()));

                    valutaIndennitaFestiva(anno, mese, giorno);

                    if(giorno.tipo().toUpper() == "R" || giorno.tipo().toUpper() == "F") {
                        m_dipendente->addRiposo(giorno.giorno());
                    }

                    if(!guarFound) {
                        if(giorno.indennita().toUpper() == "N") {
                            if(giorno.giorno() -1 > 0) {
                                if(giorno.numeroTimbratureOrdinarie() > 0) {
                                    // controllare se c'è ECCR il giorno di inizio notte
                                    if(cartellino->giorno(giorno.giorno()-1).minutiCausale("ECCR") > 0 || cartellino->giorno(giorno.giorno()-1).minutiCausale("GUAR") > 0) {
                                        if(!m_giorniGuarNot.contains(dataCorrente.day()-1)) {
                                            m_dipendente->addGuardiaNotturna(giorno.giorno()-1);
                                            m_giorniGuarNot << dataCorrente.day()-1;
                                        }
                                        if(giorno.numeroTimbratureOrdinarie() % 2 == 0 && cartellino->isLastDay(giorno.giorno()) && giorno.minutiCausale("ECCR") > 0) {
                                            if(!m_giorniGuarNot.contains(dataCorrente.day())) {
                                                m_dipendente->addGuardiaNotturna(giorno.giorno());
                                                m_giorniGuarNot << dataCorrente.day();
                                            }
                                        }
                                    }
                                }
                            }
                        } else {
                            if(The::almanac()->isFestivo(dataCorrente)) {
                                if(giorno.causale1() == "ECCR" || giorno.causale2() == "ECCR" || giorno.causale3() == "ECCR") {
                                    if(cartellino->isLastDay(giorno.giorno())) {
                                        if(giorno.numeroTimbratureOrdinarie() % 2 == 0) {
                                            if(giorno.minutiCausale("ECCR") >= 660) {
                                                if(!m_giorniGuarDiu.contains(dataCorrente.day())) {
                                                    m_dipendente->addGuardiaDiurna(giorno.giorno());
                                                    m_giorniGuarDiu << dataCorrente.day();
                                                }
                                            } else if(giorno.minutiCausale("ECCR") >= 330) {
                                                if(!m_giorniMezzaGuarDiu.contains(dataCorrente.day())) {
                                                    m_dipendente->addMezzaGuardiaDiurna(giorno.giorno());
                                                    m_giorniMezzaGuarDiu << dataCorrente.day();
                                                }
                                            }
                                        } else {
                                            if(giorno.montoNotte()) {
                                                if(!m_giorniGuarNot.contains(dataCorrente.day())) {
                                                    m_dipendente->addGuardiaNotturna(giorno.giorno());
                                                    m_giorniGuarNot << dataCorrente.day();
                                                }
                                            }
                                        }
                                    } else {
                                        const auto g = cartellino->giorno(giorno.giorno()+1);
                                        if(g.indennita().toUpper() != "N") {
                                            if(giorno.minutiCausale("ECCR") >= 660) {
                                                if(!m_giorniGuarDiu.contains(dataCorrente.day())) {
                                                    m_dipendente->addGuardiaDiurna(giorno.giorno());
                                                    m_giorniGuarDiu << dataCorrente.day();
                                                }
                                            } else if(giorno.minutiCausale("ECCR") >= 330) {
                                                if(!m_giorniMezzaGuarDiu.contains(dataCorrente.day())) {
                                                    m_dipendente->addMezzaGuardiaDiurna(giorno.giorno());
                                                    m_giorniMezzaGuarDiu << dataCorrente.day();
                                                }
                                            }
                                        }
                                    }
                                }
                            } else if(cartellino->isLastDay(giorno.giorno())) {
                                // ultimo giorno nel cartellino
                                if(giorno.montoNotte() && giorno.minutiCausale("ECCR") > 0) {
                                    if(!m_giorniGuarNot.contains(dataCorrente.day())) {
                                        m_dipendente->addGuardiaNotturna(giorno.giorno());
                                        m_giorniGuarNot << dataCorrente.day();
                                    }
                                }
                            }
                        }
                    }
                }
                m_dipendente->addNumGiorniCartellino(cartellino->giorni().count());
                ApiService::instance().addTimeCard(tableName, m_dipendente);
                const QDate dataCorrente(anno,mese,1);
                if(dataCorrente >= Utilities::regolamentoOrario2025Date) {
                    const auto competenza = Competenza(tableName, ApiService::instance().doctorId(m_dipendente->matricola()));
                    const auto saldo = competenza.saldoMinuti();
                    const auto dmp = competenza.dmp();
                    const auto rmp = m_dipendente->minutiCausale("RMP");
                    const auto idSituazioneSaldo = ApiService::instance().idSituazioneSaldo(anno, mese, m_dipendente->matricola());
                    if(idSituazioneSaldo > 0) {
                        ApiService::instance().updateSituazioneSaldo(idSituazioneSaldo, saldo, rmp, dmp);
                    } else {
                        ApiService::instance().addSituazioneSaldo(anno, mese, m_dipendente->matricola(), saldo, rmp, dmp);
                    }
                }
            }
        }
    }

    emit timeCardsRead();
}

GiornoCartellinoCompleto CartellinoCompletoReader::giorno(const QString &line, const int anno, const int mese)
{
    const QStringList fields = line.split(",");
    GiornoCartellinoCompleto giorno;
    giorno.setData(anno, mese, fields.at(0).toInt());
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
    if(!fields.at(19).trimmed().isEmpty() && !giorno.ore1().isValid())
        giorno.setValid(false);
    giorno.setCausale2(fields.at(20));
    giorno.setOre2(QTime::fromString(fields.at(21), "h:mm"));
    if(!fields.at(21).trimmed().isEmpty() && !giorno.ore2().isValid())
        giorno.setValid(false);
    giorno.setCausale3(fields.at(22));
    giorno.setOre3(QTime::fromString(fields.at(23), "h:mm"));
    if(!fields.at(23).trimmed().isEmpty() && !giorno.ore3().isValid())
        giorno.setValid(false);
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

void CartellinoCompletoReader::valutaCausale(const QString &causale,
                                             const QDate &dataCorrente,
                                             const GiornoCartellinoCompleto &giorno,
                                             const QTime &orario,
                                             bool &guardia,
                                             const bool lastDay)
{
    if(causale == "ECCR") {
        if(!m_giorniEccr.contains(dataCorrente.day())) {
            if(orario > giorno.eccedenza()) {
                m_dipendente->addMinutiEccr(Utilities::inMinuti(giorno.eccedenza()));
            } else {
                m_dipendente->addMinutiEccr(Utilities::inMinuti(orario));
            }

            m_giorniEccr << dataCorrente.day();
        }
    } else if(causale == "GUAR") {
        guardia = true;
        if(!m_giorniGuar.contains(dataCorrente.day())) {
            m_dipendente->addMinutiGuar(Utilities::inMinuti(orario));
            m_giorniGuar << dataCorrente.day();
        }
        if(The::almanac()->isFestivo(dataCorrente)) {
            if(giorno.indennita().toUpper().isEmpty()) {
                if(giorno.numeroTimbratureOrdinarie() % 2 == 0) {
                    if( Utilities::inMinuti(orario) >= 660) {
                        if(!m_giorniGuarDiu.contains(dataCorrente.day())) {
                            m_dipendente->addGuardiaDiurna(giorno.giorno());
                            m_giorniGuarDiu << dataCorrente.day();
                        }
                    } else if(Utilities::inMinuti(orario) >= 330) {
                        if(!m_giorniMezzaGuarDiu.contains(dataCorrente.day())) {
                            m_dipendente->addMezzaGuardiaDiurna(giorno.giorno());
                            m_giorniMezzaGuarDiu << dataCorrente.day();
                        }
                    }
                } else if(lastDay) {
                    // ultimo giorno nel cartellino
                    if(giorno.montoNotte()) {
                        if(!m_giorniGuarNot.contains(dataCorrente.day())) {
                            m_dipendente->addGuardiaNotturna(giorno.giorno());
                            m_giorniGuarNot << dataCorrente.day();
                        }
                    }
                }
            }
        } else if(giorno.indennita().toUpper() == "N") {
                if(!m_giorniGuarNot.contains(dataCorrente.day()-1)) {
                    m_dipendente->addGuardiaNotturna(giorno.giorno()-1);
                    m_giorniGuarNot << dataCorrente.day()-1;
                }
        } else if(lastDay) {
            // ultimo giorno nel cartellino
            if(giorno.montoNotte()) {
                if(!m_giorniGuarNot.contains(dataCorrente.day())) {
                    m_dipendente->addGuardiaNotturna(giorno.giorno());
                    m_giorniGuarNot << dataCorrente.day();
                }
            }
        }
    } else if(causale == "GREP") {
        if(!m_giorniGrep.contains(dataCorrente.day())) {
            m_dipendente->addMinutiGrep(Utilities::inMinuti(orario));
            m_giorniGrep << dataCorrente.day();
        }
        if(giorno.repDiurna().isValid() && giorno.repDiurna() > QTime(0,0))
            m_dipendente->addGrep(dataCorrente.day(), Utilities::inMinuti(giorno.repDiurna()), 1);  // diurno
        if(giorno.repNotturna().isValid() && giorno.repNotturna() > QTime(0,0))
            m_dipendente->addGrep(dataCorrente.day(), Utilities::inMinuti(giorno.repNotturna()), 0);  // notturno
    } else {
        m_dipendente->addAltraCausale(causale, QString::number(dataCorrente.day()), Utilities::inMinuti(orario));
        if(!causaliRMC.contains(causale)) {
            if(!m_giorniAltraCausale.contains(dataCorrente.day())) {
                m_dipendente->addMinutiFatti(Utilities::inMinuti(orario));
                m_giorniAltraCausale << dataCorrente.day();
            }
        }
    }
}

void CartellinoCompletoReader::valutaIndennitaFestiva(const int anno, const int mese, const GiornoCartellinoCompleto &giorno)
{
    if(giorno.numeroTimbratureOrdinarie() == 0)
        return;

    const QDate dataCorrente(anno, mese, giorno.giorno());

    if(dataCorrente.dayOfWeek() != 7 && !The::almanac()->isGrandeFestivita(dataCorrente))
        return;

    if(!m_giorniIndFes.contains(dataCorrente.day())) {
        m_giorniIndFes << dataCorrente.day();
    }

    if(giorno.indennita().toUpper() == "N") { // fine notte
        if(giorno.minutiLavoratiNelGiorno() > 360) {
            m_dipendente->addIndennitaFestiva(1);
        } else if(giorno.minutiLavoratiNelGiorno() >= 120) {
            m_dipendente->addIndennitaFestiva(0.5);
        }
    } else {
        // bisogna determinare che tipologia di servizio è tra smonto notte, mattina, pomeriggio, etc
        if(giorno.tipoTurno() & Utilities::InizioNotte) {
            if(giorno.minutiLavoratiNelGiorno() > 360) {
                m_dipendente->addIndennitaFestiva(1);
            } else if(giorno.minutiLavoratiNelGiorno() >= 120) {
                m_dipendente->addIndennitaFestiva(0.5);
            }
        } else {
            if(giorno.minutiLavoratiNelGiorno() > 180) {
                m_dipendente->addIndennitaFestiva(1);
            } else if(giorno.minutiLavoratiNelGiorno() >= 120) {
                m_dipendente->addIndennitaFestiva(0.5);
            }
        }
    }
}

void CartellinoCompletoReader::valutaEccr(const QString &causale, const QDate &dataCorrente, const QTime &orario)
{
    if(causale.isEmpty() || causale != "ECCR")
        return;

    if(!m_giorniEccr.contains(dataCorrente.day())) {
        m_dipendente->addMinutiEccr(Utilities::inMinuti(orario));
        m_giorniEccr << dataCorrente.day();
    }
}

void CartellinoCompletoReader::valutaGrep(const QString &causale, const QDate &dataCorrente, const GiornoCartellinoCompleto &giorno, const QTime &orario)
{
    if(causale.isEmpty() || causale != "GREP")
        return;

    if(!m_giorniGrep.contains(dataCorrente.day())) {
        m_dipendente->addMinutiGrep(Utilities::inMinuti(orario));
        m_giorniGrep << dataCorrente.day();
    }

    if(giorno.repDiurna().isValid() && giorno.repDiurna() > QTime(0,0))
        m_dipendente->addGrep(dataCorrente.day(), Utilities::inMinuti(giorno.repDiurna()), 1);  // diurno

    if(giorno.repNotturna().isValid() && giorno.repNotturna() > QTime(0,0))
        m_dipendente->addGrep(dataCorrente.day(), Utilities::inMinuti(giorno.repNotturna()), 0);  // notturno
}

void CartellinoCompletoReader::valutaAltraCausale(const QString &causale, const QDate &dataCorrente, const QTime &orario)
{
    if(causale.isEmpty() || causale == "ECCR" || causale == "GREP" || causale == "GUAR" )
        return;

    m_dipendente->addAltraCausale(causale, QString::number(dataCorrente.day()), Utilities::inMinuti(orario));
    if(!causaliRMC.contains(causale)) {
        if(!m_giorniAltraCausale.contains(dataCorrente.day())) {
            m_dipendente->addMinutiFatti(Utilities::inMinuti(orario));
            m_giorniAltraCausale << dataCorrente.day();
        }
    }
}

void CartellinoCompletoReader::valutaGuardiaDiurna(const QDate &dataCorrente, const GiornoCartellinoCompleto &giorno, const GiornoCartellinoCompleto &giornoSuccessivo, const bool lastDay)
{
    const int minutiValidati = giorno.minutiCausale("ECCR")  + giorno.minutiCausale("GUAR");

    if(The::almanac()->isFestivo(dataCorrente)) {
        if(giorno.causale1() == "ECCR" || giorno.causale2() == "ECCR" || giorno.causale3() == "ECCR" ||
           giorno.causale1() == "GUAR" || giorno.causale2() == "GUAR" || giorno.causale3() == "GUAR") {
            if(lastDay) {
                if(giorno.numeroTimbratureOrdinarie() % 2 == 0) {
                    if(minutiValidati >= 660) {
                        if(!m_giorniGuarDiu.contains(dataCorrente.day())) {
                            m_dipendente->addGuardiaDiurna(giorno.giorno());
                            m_giorniGuarDiu << dataCorrente.day();
                        }
                    } else if(minutiValidati >= 330) {
                        if(!m_giorniMezzaGuarDiu.contains(dataCorrente.day())) {
                            m_dipendente->addMezzaGuardiaDiurna(giorno.giorno());
                            m_giorniMezzaGuarDiu << dataCorrente.day();
                        }
                    }
                }
            } else {
                if(giornoSuccessivo.indennita().toUpper() != "N" && minutiValidati >= 660) {
                    if(!m_giorniGuarDiu.contains(dataCorrente.day())) {
                        m_dipendente->addGuardiaDiurna(giorno.giorno());
                        m_giorniGuarDiu << dataCorrente.day();
                    }
                } else if(giornoSuccessivo.indennita().toUpper() != "N" && minutiValidati >= 330) {
                    if(!m_giorniMezzaGuarDiu.contains(dataCorrente.day())) {
                        m_dipendente->addMezzaGuardiaDiurna(giorno.giorno());
                        m_giorniMezzaGuarDiu << dataCorrente.day();
                    }
                }
            }
        }
    }
}

void CartellinoCompletoReader::valutaGuardiaNotturna(const QDate &dataCorrente, const GiornoCartellinoCompleto &giorno, const GiornoCartellinoCompleto &giornoPrecedente, const bool lastDay)
{
    const int minutiValidati = giorno.minutiCausale("ECCR")  + giorno.minutiCausale("GUAR");
    const int minutiValidatiGiornoPrecedente = giornoPrecedente.minutiCausale("ECCR")  + giornoPrecedente.minutiCausale("GUAR");

    if(giorno.indennita().toUpper() == "N") {
        if(giorno.giorno() -1 > 0) {
            if(giorno.numeroTimbratureOrdinarie() > 0) {
                // controllare se c'è ECCR il giorno di inizio notte
                if(minutiValidatiGiornoPrecedente > 0) {
                    if(!m_giorniGuarNot.contains(dataCorrente.day()-1)) {
                        m_dipendente->addGuardiaNotturna(giorno.giorno()-1);
                        m_giorniGuarNot << dataCorrente.day()-1;
                    }
                    if(giorno.numeroTimbratureOrdinarie() % 2 == 0 && lastDay && minutiValidati > 0) {
                        if(!m_giorniGuarNot.contains(dataCorrente.day())) {
                            m_dipendente->addGuardiaNotturna(giorno.giorno());
                            m_giorniGuarNot << dataCorrente.day();
                        }
                    }
                }
            }
        }
    } else {
        if(The::almanac()->isFestivo(dataCorrente)) {
            if(giorno.causale1() == "ECCR" || giorno.causale2() == "ECCR" || giorno.causale3() == "ECCR" ||
               giorno.causale1() == "GUAR" || giorno.causale2() == "GUAR" || giorno.causale3() == "GUAR") {
                if(lastDay && giorno.montoNotte()) {
                    if(!m_giorniGuarNot.contains(dataCorrente.day())) {
                        m_dipendente->addGuardiaNotturna(giorno.giorno());
                        m_giorniGuarNot << dataCorrente.day();
                    }
                }
            }
        } else if(lastDay) { // ultimo giorno nel cartellino
            if(giorno.montoNotte() && minutiValidati > 0) {
                if(!m_giorniGuarNot.contains(dataCorrente.day())) {
                    m_dipendente->addGuardiaNotturna(giorno.giorno());
                    m_giorniGuarNot << dataCorrente.day();
                }
            }
        }
    }
}
