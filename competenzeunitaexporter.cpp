/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "competenzeunitaexporter.h"
//#include "sqlqueries.h"
#include "competenza.h"
#include "utilities.h"
#include "sqldatabasemanager.h"
#include "competenzepagate.h"
#include "dipendente.h"
#include "indennita.h"
#include "apiservice.h"

#include <QDate>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QPdfWriter>
#include <QPainter>
#include <QSqlError>
#include <cmath>

CompetenzeUnitaExporter::CompetenzeUnitaExporter(QObject *parent)
    : QThread(parent)
    , m_firstHeaderHeight(1)
    , m_secondHeaderHeight(2)
    , m_thirdHeaderHeight(5)
    , m_gridWidth(500)
    , m_gridHeight(260)
    , m_totalRows(20)
    , m_tableWidth(13000)
    , m_totalHeaderHeight(m_firstHeaderHeight + m_secondHeaderHeight + m_thirdHeaderHeight)
    , m_totalColumns(29)
    , m_tableHeight(m_gridHeight*m_totalHeaderHeight+m_gridHeight*m_totalRows)
{
    m_idUnita = -1;
    m_timecard.clear();
}

CompetenzeUnitaExporter::~CompetenzeUnitaExporter()
= default;

void CompetenzeUnitaExporter::setPath(const QString &path)
{
    m_path = path;
}

void CompetenzeUnitaExporter::setUnita(int id)
{
    m_idUnita = id;
}

void CompetenzeUnitaExporter::setMese(const QString &timecard)
{
    m_timecard = timecard;
}

void CompetenzeUnitaExporter::setPrintCasi(bool ok)
{
    m_printCasi = ok;
}

void CompetenzeUnitaExporter::setPrintData(bool ok)
{
    m_printData = ok;
}

void CompetenzeUnitaExporter::run()
{
    Utilities::m_connectionName = "CompetenzeUnitaExporter";

    bool ok = true;

    QSqlDatabase db = The::dbManager()->database(ok, Utilities::m_connectionName);

    if(!ok) {
        qDebug() << "Impossibile creare la connessione" << Utilities::m_connectionName;
        emit exportFinished(QString());
        return;
    }

    if(!QSqlDatabase::connectionNames().contains(Utilities::m_connectionName)) {
        if (!db.open()) {
            qDebug() << QLatin1String("Impossibile connettersi al database.") << db.lastError().text();
            emit exportFinished(QString());
            return;
        }
    }

    const auto now = QDateTime::currentDateTime();

    QVector<int> unitaIdList;
    const QString s = m_timecard.split("_").last();
    m_currentMonthYear = QDate::fromString(s+"01", "yyyyMMdd");
    QDate date = m_currentMonthYear.addDays(m_currentMonthYear.daysInMonth()-1);
    QString mese = QLocale().monthName(date.month()) + " " + s.left(4);
    QString pdfFileName = "Competenze_" + QString(mese).replace(" ","_");
    QString csvFileName = "Competenze_" + QString(mese).replace(" ","_");

    if(m_idUnita != -1) {
        unitaIdList << m_idUnita;
        const auto breve = ApiService::instance().getUnitaDataById(m_idUnita).breve;
        pdfFileName += "_" + QString::number(m_idUnita) + "_" + breve;
        csvFileName += "_" + QString::number(m_idUnita) + "_" + breve;
    } else {
        const auto units = ApiService::instance().getUnitaDataFromTimecard(m_timecard);
        for(const auto u : units) {
            unitaIdList << u.idUnita;
        }
    }

    emit totalRows(unitaIdList.count());

    pdfFileName += "_" + now.toString("yyyyMMdd_hhmm") + ".pdf";
    csvFileName += "_" + now.toString("yyyyMMdd_hhmm") + ".csv";

    QFile outFile(m_path + QDir::separator() + csvFileName);
    if(!outFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Impossibile aprire il file di destinazione:\n\n" + m_path + QDir::separator() + csvFileName;
        return;
    }

    QTextStream out(&outFile);

    out << "INPUT;" << "RIFERIMENTO;"
        << "CI;" << "ANNO;" << "MESE;"
        << "MENSILITA;" << "VOCE;" << "SUB;"
        << "ARR;" << "QTA;"
        << "IMP;" << "SEDE_DEL;" << "ANNO_DEL;"
        << "NUMERO_DEL;" << "DELIBERA" << "\n";

    const auto indennita = ApiService::instance().getIndennita(m_currentMonthYear.year(), m_currentMonthYear.month());

    const QString rowText =
            "LVARIDE;" +
            date.toString("dd/MM/yyyy") +
            ";%1;" +
            QString::number(date.addDays(1).year()) + ";" +
            QString::number(date.addDays(1).month()) + ";" +
            QLocale().monthName(date.addDays(1).month(), QLocale::ShortFormat).toUpper() + ";%2;%3;;%4;;;;;";

    QPdfWriter writer(m_path + QDir::separator() + pdfFileName);

#if QT_VERSION >= 0x060000
    writer.setPageSize(QPageSize::A4);
#else
    writer.setPageSize(QPagedPaintDevice::A4);
#endif
    writer.setPageMargins(QMargins(30, 30, 30, 30));
    writer.setPageOrientation(QPageLayout::Landscape);
    writer.setTitle(QString(pdfFileName).replace(".pdf", "").replace("_", " "));
    writer.setCreator("Gestione Competenze Medici");

    QPainter painter(&writer);
    painter.setRenderHint(QPainter::Antialiasing, true);

    bool isFileStart = true;

    int currRow = 0;

    m_casiIndennitaNotturna = 0;
    m_casiIndennitaFestiva = 0;
    m_casiStrRepaOrd = 0;
    m_casiStrRepaFesONott = 0;
    m_casiStrRepaFesENott = 0;
    m_casiStrRepeOrd = 0;
    m_casiStrRepeFesONott = 0;
    m_casiStrRepeFesENott = 0;
    m_casiStrGuarOrd = 0;
    m_casiStrGuarFesONott = 0;
    m_casiStrGuarFesENott = 0;
    m_casiRepeTurni = 0;
    m_casiRepeOre = 0;
    m_casiGuarDiur = 0;
    m_casiGuarNott = 0;
    m_casiGranFest = 0;

    if(!ApiService::instance().tableExists("tcp_" + m_currentMonthYear.toString("yyyyMM"))) {
        ApiService::instance().createPagatoTable(m_currentMonthYear.year(), m_currentMonthYear.month());
    }

    for (const auto unitaId : unitaIdList) {
        QStringList notes;
        currRow++;
        emit currentRow(currRow);
        if(!isFileStart)
            writer.newPage();

        disegnaTabella(painter);

        if(m_printData)
            printData(painter, now.toString("dd/MM/yyyy hh:mm:ss"));

        QString unitaName = ApiService::instance().getUnitaNomeCompleto(unitaId);

        printMonth(painter, mese);
        printUnitaName(painter, unitaName);
        printUnitaNumber(painter, unitaId);

        qDebug() << "1 ------------->" << m_timecard;
        QVector<int> dirigentiIdList = ApiService::instance().getDoctorsIdsFromUnitInTimecard(m_timecard, unitaId);

        int counter = 0;

        foreach (int dirigenteId, dirigentiIdList) {
            m_competenza = new Competenza(m_timecard,dirigenteId, true /* esportazione */);
            if(counter != 0 && (counter%m_totalRows) == 0) {
                counter = 0;
                writer.newPage();
                disegnaTabella(painter);
                if(m_printData)
                    printData(painter, now.toString("dd/MM/yyyy hh:mm:ss"));
                printMonth(painter, mese);
                printUnitaName(painter, unitaName);
                printUnitaNumber(painter, unitaId);
            }
            double whole, fractional;
            fractional = std::modf(m_competenza->numeroTurniProntaDisponibilita(), &whole);

            CompetenzePagate *pagato = new CompetenzePagate;
            pagato->setCi(m_competenza->dipendente()->matricola());
            pagato->setDeficit(m_competenza->deficitOrario() < 0 ? m_competenza->deficitOrario() : 0);
            pagato->setIndNotturna(0); // 26 non si paga più?
            pagato->setIndFestiva(m_competenza->dipendente()->indennitaFestiva()); // 62
            pagato->setStr_reparto_ord(0); // 66 solitamente nullo
            pagato->setStr_reparto_nof(0); // 68 solitamente nullo
            pagato->setStr_reparto_nef(0); // 67 solitamente nullo

            pagato->setStr_repe_ord(m_competenza->oreReperibilitaORDPagate()); // 70
            pagato->setStr_repe_nof(m_competenza->oreReperibilitaNOFPagate()); // 72
            pagato->setStr_repe_nef(m_competenza->oreReperibilitaNEFPagate()); // 71
            pagato->setStr_guard_ord(m_competenza->oreStraordinarioNotturnoORDPagate()); // 73
            pagato->setStr_guard_nof(m_competenza->oreStraordinarioNotturnoNOFPagate() + m_competenza->oreGuardieDiurnePagate()); // 75
            pagato->setStr_guard_nef(m_competenza->oreStraordinarioNotturnoNEFPagate()); // 74
            if(m_currentMonthYear >= Utilities::ccnl1921Date) {
//                pagato->setTurni_repe(whole > 10 ? 120.0 : whole); // 25
//                //                pagato->setOre_repe(fractional > 0.0 ? 6 : 0); // 40
//                pagato->setRepOltre10(whole > 10 ? (whole - 10)*12.0 : 0.0);

                if(whole > 10) {
                    pagato->setTurni_repe(120.0); // 25
                    pagato->setRepOltre10((whole - 10.0 + fractional)*12.0);
                } else if(whole > 0) { // 25
                    pagato->setTurni_repe((whole+fractional)*12.0); // 25
                    pagato->setRepOltre10(0.0);
                } else {
                    pagato->setTurni_repe(0.0); // 25
                    pagato->setRepOltre10(0.0);
                }
                // Indennità PS Dirigenza
                pagato->setOreLavorate(m_competenza->oreLavoratePagate());
                pagato->setGuard_diu(m_competenza->numeroGuardieDiurneFatte() + m_competenza->numeroMezzeGuardieDiurneFatte()); // 1512
            } else {
                pagato->setTurni_repe(whole > 10 ? 10 : whole); // 25
                pagato->setOre_repe(fractional > 0.0 ? 6 : 0); // 40
                pagato->setRepOltre10(whole > 10 ? whole - 10 : 0.0);
                pagato->setGuard_diu(m_competenza->numeroGuardieDiurneFatte()); // 1512
            }
            pagato->setGuard_not(m_competenza->numeroGuardieNotturneFatte() + m_competenza->numeroGrandiFestivitaFatte() - m_competenza->numeroGrandiFestivitaPagate()); // 1571
            pagato->setGrande_fes(m_competenza->numeroGrandiFestivitaPagate()); // 921
            pagato->setDateTime(now);
            pagato->setTeleconsulto(m_competenza->numeroTurniTeleconsulto());

            ApiService::instance().saveCompetenzePagate(pagato, m_currentMonthYear.year(), m_currentMonthYear.month());

            if(m_currentMonthYear < Utilities::regolamentoOrario2025Date) {
                if(m_competenza->oreReperibilitaORDPagate() > 0) // 70
                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::StraordinarioReperibilitaOrd, unitaId))
                           .arg(indennita->sub(Utilities::StraordinarioReperibilitaOrd, unitaId))
                           .arg(QString::number(m_competenza->oreReperibilitaORDPagate())) << "\n";

                if(m_competenza->oreReperibilitaNOFPagate() > 0) // 72
                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::StraordinarioReperibilitaNof, unitaId))
                           .arg(indennita->sub(Utilities::StraordinarioReperibilitaNof, unitaId))
                           .arg(QString::number(m_competenza->oreReperibilitaNOFPagate())) << "\n";

                if(m_competenza->oreReperibilitaNEFPagate() > 0) // 71
                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::StraordinarioReperibilitaNef, unitaId))
                           .arg(indennita->sub(Utilities::StraordinarioReperibilitaNef, unitaId))
                           .arg(QString::number(m_competenza->oreReperibilitaNEFPagate())) << "\n";

                if(m_competenza->oreStraordinarioNotturnoORDPagate() > 0) // 73
                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::StraordinarioGuardiaOrd, unitaId))
                           .arg(indennita->sub(Utilities::StraordinarioGuardiaOrd, unitaId))
                           .arg(QString::number(m_competenza->oreStraordinarioNotturnoORDPagate())) << "\n";

                if((m_competenza->oreStraordinarioNotturnoNOFPagate() + m_competenza->oreGuardieDiurnePagate()) > 0) // 75
                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::StraordinarioGuardiaNof, unitaId))
                           .arg(indennita->sub(Utilities::StraordinarioGuardiaNof, unitaId))
                           .arg(QString::number(m_competenza->oreStraordinarioNotturnoNOFPagate()+ m_competenza->oreGuardieDiurnePagate())) << "\n";

                if(m_competenza->oreStraordinarioNotturnoNEFPagate() > 0) // 74
                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::StraordinarioGuardiaNef, unitaId))
                           .arg(indennita->sub(Utilities::StraordinarioGuardiaNef, unitaId))
                           .arg(QString::number(m_competenza->oreStraordinarioNotturnoNEFPagate())) << "\n";

                if(m_competenza->numeroGrandiFestivitaPagate() > 0) { // 1571
                    out << rowText.arg(m_competenza->badgeNumber())
                               .arg(indennita->voce(Utilities::GranFestivita, unitaId))
                               .arg(indennita->sub(Utilities::GranFestivita, unitaId))
                               .arg(QString::number(m_competenza->numeroGrandiFestivitaPagate())) << "\n";
                    out << rowText.arg(m_competenza->badgeNumber())
                               .arg(indennita->voce(Utilities::GuardiaNotturna, unitaId))
                               .arg(indennita->sub(Utilities::GuardiaNotturna, unitaId))
                               .arg("-" + QString::number(m_competenza->numeroGrandiFestivitaPagate())) << "\n";
                }
            }

            if(m_currentMonthYear >= Utilities::ccnl1921Date) {
                if(whole > 10) {
                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::ProntaDisponibilitaOraria, unitaId))
                           .arg(indennita->sub(Utilities::ProntaDisponibilitaOraria, unitaId))
                           .arg(QString::number(120)) << "\n";

                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::ProntaDisponibilitaOraria30, unitaId))
                           .arg(indennita->sub(Utilities::ProntaDisponibilitaOraria30, unitaId))
                           .arg(QString::number((whole - 10 + fractional)*12)) << "\n";
                } else if(whole > 0) { // 25
                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::ProntaDisponibilitaOraria, unitaId))
                           .arg(indennita->sub(Utilities::ProntaDisponibilitaOraria, unitaId))
                           .arg(QString::number((whole+fractional)*12)) << "\n";
                }
            } else {
                if(whole > 10) {
                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::TurniReperibilita, unitaId))
                           .arg(indennita->sub(Utilities::TurniReperibilita, unitaId))
                           .arg(QString::number(10)) << "\n";

                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::ReperibilitaOltre10, unitaId))
                           .arg(indennita->sub(Utilities::ReperibilitaOltre10, unitaId))
                           .arg(QString::number(whole - 10)) << "\n";
                } else if(whole > 0) { // 25
                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::TurniReperibilita, unitaId))
                           .arg(indennita->sub(Utilities::TurniReperibilita, unitaId))
                           .arg(QString::number(whole)) << "\n";
                }

                if(fractional > 0.0) // 40
                    out << rowText.arg(m_competenza->badgeNumber())
                           .arg(indennita->voce(Utilities::OreReperibilita, unitaId))
                           .arg(indennita->sub(Utilities::OreReperibilita, unitaId))
                           .arg("6") << "\n";
            }

            if(m_competenza->numeroTurniTeleconsulto() > 0) {
                out << rowText.arg(m_competenza->badgeNumber())
                       .arg(indennita->voce(Utilities::Teleconsulto, unitaId))
                       .arg(indennita->sub(Utilities::Teleconsulto, unitaId))
                       .arg(QString::number(m_competenza->numeroTurniTeleconsulto())) << "\n";
            }

//            printCell(painter, counter, 1, 2, QString::number(m_competenza->badgeNumber()));

            printBadge(painter, m_competenza->badgeNumber(),counter);
            printName(painter, m_competenza->name(),counter);
            printDeficit(painter, m_competenza->deficitOrario() < 0 ?
                             Utilities::inOrario(abs(m_competenza->deficitOrario())) : "//",counter);

            if(m_currentMonthYear < Utilities::ccnl1618Date) {
                printNotturno(painter, m_competenza->notte(),counter); // 26
            } else {
                printNotturno(painter, 0,counter); // 26
            }
            printFestivo(painter, m_competenza->dipendente()->indennitaFestiva(),counter); // 62

            if(m_currentMonthYear >= Utilities::ccnl1921Date) {
                printRepNumTurni(painter, 0, counter); // 25
                printRepNumOre(painter, m_competenza->numeroTurniProntaDisponibilita() * 12.0, counter); //40
            } else {
                printRepNumTurni(painter, whole, counter); // 25
                printRepNumOre(painter, fractional > 0.0 ? 6 : 0, counter); //40
            }
            printStrRepartoOrdin(painter,0,counter);    // 66 solitamente nullo
            printStrRepartoFesONott(painter,0,counter); // 68 solitamente nullo
            printStrRepartoFesENott(painter,0,counter); // 67 solitamente nullo

            printNumGuarNott(painter, m_competenza->numeroGuardieNotturneFatte() + m_competenza->numeroGrandiFestivitaFatte() - m_competenza->numeroGrandiFestivitaPagate(), counter); //1571
            printNumGuarDiur(painter, m_competenza->numeroGuardieDiurneFatte(), counter); // 1512

            printNumGfFesNott(painter, m_competenza->numeroGrandiFestivitaPagate(), counter); // 921

            if(m_currentMonthYear < Utilities::regolamentoOrario2025Date) {
                printNumOreGuarFesENot(painter, m_competenza->oreStraordinarioNotturnoNEFPagate(), counter); // 74
                printNumOreGuarFesONot(painter, m_competenza->oreStraordinarioNotturnoNOFPagate() + m_competenza->oreGuardieDiurnePagate(), counter); // 75
                printNumOreGuarOrd(painter, m_competenza->oreStraordinarioNotturnoORDPagate(), counter); // 73
            }
            printNumOreRepFesENot(painter, m_competenza->oreReperibilitaNEFPagate(), counter); // 71
            printNumOreRepFesONot(painter, m_competenza->oreReperibilitaNOFPagate(), counter); // 72
            printNumOreRepOrd(painter, m_competenza->oreReperibilitaORDPagate(), counter); // 70
            if(!m_competenza->note().isEmpty()) {
                notes << QString::number(m_competenza->badgeNumber()) + " - " + m_competenza->name() + ": " + m_competenza->note();
            }
            counter++;
        }
        printNote(painter, notes);
        isFileStart = false;
    }

    if(m_printCasi)
        printCasi(painter);

    outFile.close();

    emit exportFinished(m_path + QDir::separator() + pdfFileName);
}

void CompetenzeUnitaExporter::disegnaTabella(QPainter &painter)
{
    // Sfondo colonna a destra
    painter.save();
    painter.setPen(QPen(Qt::black, 25, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, m_tableWidth, m_tableHeight)); // riquadro
    painter.drawLine(QPoint(0, m_gridHeight*m_firstHeaderHeight), QPoint(m_tableWidth, m_gridHeight*m_firstHeaderHeight)); // riga sotto unita
    painter.drawLine(QPoint(0, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight)), QPoint(m_tableWidth, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight))); // riga sotto numero
    painter.drawLine(QPoint(0, m_gridHeight*m_totalHeaderHeight), QPoint(m_tableWidth, m_gridHeight*m_totalHeaderHeight)); // 700 riga sotto cognome

    painter.drawLine(QPoint(m_gridWidth*2, m_gridHeight), QPoint(m_gridWidth*2, m_tableHeight)); // colonna numero
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth, m_gridHeight), QPoint(m_tableWidth-m_gridWidth, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*2, m_gridHeight), QPoint(m_tableWidth-m_gridWidth*2, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*3, m_gridHeight), QPoint(m_tableWidth-m_gridWidth*3, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*5, m_gridHeight), QPoint(m_tableWidth-m_gridWidth*5, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*8, m_gridHeight), QPoint(m_tableWidth-m_gridWidth*8, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*11, m_gridHeight), QPoint(m_tableWidth-m_gridWidth*11, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*14, m_gridHeight), QPoint(m_tableWidth-m_gridWidth*14, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*16, m_gridHeight), QPoint(m_tableWidth-m_gridWidth*16, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*18, m_gridHeight), QPoint(m_tableWidth-m_gridWidth*18, m_tableHeight));
    painter.restore();

    // Righe
    painter.save();
    painter.setPen(QPen(Qt::black, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    for(int i = 0; i < m_totalRows; i++) {
        painter.drawLine(QPoint(0, (i+m_totalHeaderHeight)*m_gridHeight), QPoint(m_tableWidth, (i+m_totalHeaderHeight)*m_gridHeight));
    }

    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*4, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight)), QPoint(m_tableWidth-m_gridWidth*4, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*6, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight)), QPoint(m_tableWidth-m_gridWidth*6, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*7, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight)), QPoint(m_tableWidth-m_gridWidth*7, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*9, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight)), QPoint(m_tableWidth-m_gridWidth*9, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*10, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight)), QPoint(m_tableWidth-m_gridWidth*10, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*12, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight)), QPoint(m_tableWidth-m_gridWidth*12, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*13, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight)), QPoint(m_tableWidth-m_gridWidth*13, m_tableHeight));
    painter.drawLine(QPoint(m_tableWidth-m_gridWidth*15, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight)), QPoint(m_tableWidth-m_gridWidth*15, m_tableHeight));
    painter.restore();

    int rotation = 0;

    printIntestazione(painter, 1, 2, 2, rotation, "Matricola");
    printIntestazione(painter, 3, 9, 2, rotation, "NOMINATIVO");
    printIntestazione(painter, 12, 2, 2, rotation, "Deficit Orario");
    printIntestazione(painter, 14, 2, 2, rotation, "Indennità");
    printIntestazione(painter, 16, 3, 2, rotation, "Straordinario Reparto");
    printIntestazione(painter, 19, 3, 2, rotation, "Straordinario Reperibilità");
    printIntestazione(painter, 22, 3, 2, rotation, "Straordinario Guardia");
    printIntestazione(painter, 25, 2, 2, rotation, "Turni Reperibilità");

    if(m_currentMonthYear < Utilities::ccnl1618Date) {
        printIntestazione(painter, 27, 1, 2, rotation, "Guard. Nott.");
        printIntestazione(painter, 28, 1, 2, rotation, "Grand. Fes. Nott.");
        printIntestazione(painter, 29, 1, 2, rotation, "Vitto");
    } else {
        printIntestazione(painter, 27, 1, 2, rotation, "Guard. Diurna");
        printIntestazione(painter, 28, 1, 2, rotation, "Guard. Nott.");
        printIntestazione(painter, 29, 1, 2, rotation, "Grand. Fes. Nott.");
    }

    rotation = -90;

    // Divisione Indennità 1
    printIntestazione(painter, 14, 1, 5, rotation, "Notturna");
    printIntestazione(painter, 15, 1, 5, rotation, "Festiva");

    // straordinario reparto
    printIntestazione(painter, 16, 1, 5, rotation, "Ordinario");
    printIntestazione(painter, 17, 1, 5, rotation, "Notturno o Festivo");
    printIntestazione(painter, 18, 1, 5, rotation, "Notturno E Festivo");

    // straordinario reperibilità
    printIntestazione(painter, 19, 1, 5, rotation, "Ordinario");
    printIntestazione(painter, 20, 1, 5, rotation, "Notturno o Festivo");
    printIntestazione(painter, 21, 1, 5, rotation, "Notturno E Festivo");

    // straordinario guardia
    printIntestazione(painter, 22, 1, 5, rotation, "Ordinario");
    printIntestazione(painter, 23, 1, 5, rotation, "Notturno o Festivo");
    printIntestazione(painter, 24, 1, 5, rotation, "Notturno E Festivo");

    printIntestazione(painter, 25, 1, 5, rotation, "Numero Turni");
    printIntestazione(painter, 26, 1, 5, rotation, "Numero Ore");

    if(m_currentMonthYear < Utilities::ccnl1618Date) {
        printIntestazione(painter, 27, 1, 5, rotation, "€ 50,00");
        printIntestazione(painter, 28, 1, 5, rotation, "€ 480,00");
        printIntestazione(painter, 29, 1, 5, rotation, "");
    } else {
        printIntestazione(painter, 27, 1, 5, rotation, "€ 100,00");
        printIntestazione(painter, 28, 1, 5, rotation, "€ 100,00");
        printIntestazione(painter, 29, 1, 5, rotation, "€ 480,00");
    }
}

void CompetenzeUnitaExporter::printIntestazione(QPainter &painter, int column, int rowSpan, int colSpan, int rotation, const QString &text)
{
    painter.save();
    painter.rotate(rotation);
    painter.setPen(Qt::black);
    painter.setFont(headerFont());
    if(rotation == 0)
        painter.drawText(QRect(m_tableWidth-m_gridWidth*(m_totalColumns-column+1), m_gridHeight, m_gridWidth*rowSpan, m_gridHeight*colSpan), Qt::AlignCenter | Qt::TextWordWrap, text);
    else
        painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*(m_totalColumns-column+1), m_gridHeight*colSpan, m_gridWidth*rowSpan), Qt::AlignCenter, text);
    painter.restore();
}

void CompetenzeUnitaExporter::printCell(QPainter &painter, int row, int column, int colSpan, const QString &text)
{
    const auto rect = QRect(m_tableWidth-m_gridWidth*(m_totalColumns-column+1), m_gridHeight*row+m_totalHeaderHeight, m_gridWidth, m_gridHeight*colSpan);
    painter.save();
    painter.setPen(Qt::black);
//    painter.setFont(badgeFont());
    if(text.isEmpty()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(rect);
    } else {
//        m_casiStrRepaOrd += value;
//        painter.drawText(getRect(row, 14), Qt::AlignHCenter | Qt::AlignVCenter, text);
        painter.drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, text);
    }
    painter.restore();
}

QFont CompetenzeUnitaExporter::numberFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(300);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeUnitaExporter::unitaFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(180);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeUnitaExporter::badgeFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(150);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeUnitaExporter::headerFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(120);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeUnitaExporter::headerLightFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(110);
    font.setFamily("Sans Serif");
    return font;
}

void CompetenzeUnitaExporter::printMonth(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(unitaFont());
    painter.drawText(QRect(0, 0, m_tableWidth-600, m_gridHeight), Qt::AlignRight | Qt::AlignVCenter, text);
    painter.restore();
}

void CompetenzeUnitaExporter::printUnitaName(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(unitaFont());
    painter.drawText(QRect(600, 0, m_tableWidth, m_gridHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.restore();
}

void CompetenzeUnitaExporter::printUnitaNumber(QPainter &painter, const int &id)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(numberFont());
    painter.drawText(QRect(0, m_gridHeight, m_gridWidth*2, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter, QString::number(id));
    painter.restore();
}

void CompetenzeUnitaExporter::printBadge(QPainter &painter, const int &text, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    painter.drawText(QRect(20, m_gridHeight*m_totalHeaderHeight+(row*m_gridHeight), m_gridWidth*2-20, m_gridHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, QString::number(text));
    painter.restore();
}

void CompetenzeUnitaExporter::printName(QPainter &painter, const QString &text, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    painter.drawText(QRect(m_gridWidth*2+20, m_gridHeight*m_totalHeaderHeight+(row*m_gridHeight), m_tableWidth-m_gridWidth*20-20, m_gridHeight), Qt::AlignLeft | Qt::AlignBottom, text);
    painter.restore();
}

void CompetenzeUnitaExporter::printDeficit(QPainter &painter, const QString &text, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*18, m_gridHeight*m_totalHeaderHeight+(row*m_gridHeight), m_gridWidth*2, m_gridHeight), Qt::AlignCenter | Qt::AlignBottom, text == "//" ? "" : text);
    painter.restore();
}

void CompetenzeUnitaExporter::printNotturno(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 16));
    } else {
        m_casiIndennitaNotturna += value;
        painter.drawText(getRect(row, 16), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printFestivo(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 15));
    } else {
        m_casiIndennitaFestiva += value;
        painter.drawText(getRect(row, 15), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
        painter.restore();
}

void CompetenzeUnitaExporter::printStrRepartoOrdin(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 14));
    } else {
        m_casiStrRepaOrd += value;
        painter.drawText(getRect(row, 14), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printStrRepartoFesONott(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 13));
    } else {
        m_casiStrRepaFesONott += value;
        painter.drawText(getRect(row, 13), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printStrRepartoFesENott(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 12));
    } else {
        m_casiStrRepaFesENott += value;
        painter.drawText(getRect(row, 12), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printRepNumTurni(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 5));
    } else {
        m_casiRepeTurni += value;
        painter.drawText(getRect(row, 5), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printRepNumOre(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 4));
    } else {
        m_casiRepeOre += value;
        painter.drawText(getRect(row, 4), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumGuarDiur(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 3));
    } else {
        m_casiGuarDiur += value;
        painter.drawText(getRect(row, 3), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumGuarNott(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        if(m_currentMonthYear < Utilities::ccnl1618Date) {
            painter.drawRect(getRect(row, 3));
        } else {
            painter.drawRect(getRect(row, 2));
        }
    } else {
        m_casiGuarNott += value;
        if(m_currentMonthYear < Utilities::ccnl1618Date) {
            painter.drawText(getRect(row, 3), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
        } else {
            painter.drawText(getRect(row, 2), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
        }

    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumGfFesNott(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        if(m_currentMonthYear < Utilities::ccnl1618Date) {
            painter.drawRect(getRect(row, 2));
        } else {
            painter.drawRect(getRect(row, 1));
        }
    } else {
        m_casiGranFest += value;
        if(m_currentMonthYear < Utilities::ccnl1618Date) {
            painter.drawText(getRect(row, 2), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
        } else {
            painter.drawText(getRect(row, 1), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
        }
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumOreGuarFesENot(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 6));
    } else {
        m_casiStrGuarFesENott += value;
        painter.drawText(getRect(row, 6), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumOreGuarFesONot(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 7));
    } else {
        m_casiStrGuarFesONott += value;
        painter.drawText(getRect(row, 7), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumOreGuarOrd(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 8));
    } else {
        m_casiStrGuarOrd += value;
        painter.drawText(getRect(row, 8), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumOreRepFesENot(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 9));
    } else {
        m_casiStrRepeFesENott += value;
        painter.drawText(getRect(row, 9), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumOreRepFesONot(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 10));
    } else {
        m_casiStrRepeFesONott += value;
        painter.drawText(getRect(row, 10), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumOreRepOrd(QPainter &painter, const int value, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(value == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 11));
    } else {
        m_casiStrRepeOrd += value;
        painter.drawText(getRect(row, 11), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(value));
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printCasi(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());

    painter.drawText(QRect(m_tableWidth-m_gridWidth*18, m_gridHeight*m_totalHeaderHeight+(m_totalRows*m_gridHeight), m_gridWidth*2, m_gridHeight), Qt::AlignRight | Qt::AlignVCenter, "Casi:");

    painter.drawText(getRect(m_totalRows, 16), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiIndennitaNotturna));
    painter.drawText(getRect(m_totalRows, 15), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiIndennitaFestiva));
    painter.drawText(getRect(m_totalRows, 14), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiStrRepaOrd));
    painter.drawText(getRect(m_totalRows, 13), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiStrRepaFesONott));
    painter.drawText(getRect(m_totalRows, 12), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiStrRepaFesENott));
    painter.drawText(getRect(m_totalRows, 11), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiStrRepeOrd));
    painter.drawText(getRect(m_totalRows, 10), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiStrRepeFesONott));
    painter.drawText(getRect(m_totalRows, 9), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiStrRepeFesENott));
    painter.drawText(getRect(m_totalRows, 8), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiStrGuarOrd));
    painter.drawText(getRect(m_totalRows, 7), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiStrGuarFesONott));
    painter.drawText(getRect(m_totalRows, 6), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiStrGuarFesENott));
    painter.drawText(getRect(m_totalRows, 5), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiRepeTurni));
    painter.drawText(getRect(m_totalRows, 4), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiRepeOre));
    if(m_currentMonthYear < Utilities::ccnl1618Date) {
        painter.drawText(getRect(m_totalRows, 3), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiGuarNott));
        painter.drawText(getRect(m_totalRows, 2), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiGranFest));
    } else {
        painter.drawText(getRect(m_totalRows, 3), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiGuarDiur));
        painter.drawText(getRect(m_totalRows, 2), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiGuarNott));
        painter.drawText(getRect(m_totalRows, 1), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiGranFest));
    }

    painter.restore();
}

void CompetenzeUnitaExporter::printNote(QPainter &painter, const QStringList &note)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());

    const int offset = m_gridHeight*m_totalHeaderHeight+(m_totalRows*m_gridHeight) + m_gridHeight;

    for(int i = 0; i < note.size(); i++) {
        painter.drawText(QRect(0, offset + m_gridHeight*i, m_tableWidth, m_gridHeight), Qt::AlignLeft | Qt::AlignVCenter, note.at(i));
    }

    painter.restore();
}

void CompetenzeUnitaExporter::printData(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, text);
    painter.restore();
}

QRect CompetenzeUnitaExporter::getRect(int row, int column) const
{
    int xOffeset = 5;
    int yOffeset = 5;
    int widthStretch = 10;
    int heightStretch = 10;

    if(row == 0) {
        yOffeset = 12;
        heightStretch = 17;
    }

    if(column == 16 || column == 14 || column == 5) {
        xOffeset = 12;
        widthStretch = 17;
    }

    if(column == 15 || column == 12 || column == 4) {
        widthStretch = 18;
    }

    if(column == 3 || column == 2) {
        xOffeset = 12;
        widthStretch = 24;
    }

    if(row == (m_totalRows-1)) {
        heightStretch = 17;
    }

    return {
        m_tableWidth-m_gridWidth*column+xOffeset,
        m_gridHeight*m_totalHeaderHeight+(row*m_gridHeight)+yOffeset,
        m_gridWidth-widthStretch,
        m_gridHeight-heightStretch
    };
}
