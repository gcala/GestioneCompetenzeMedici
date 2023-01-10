/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "differenzeexporter.h"
#include "sqlqueries.h"
#include "competenza.h"
#include "utilities.h"
#include "sqldatabasemanager.h"
#include "competenzepagate.h"
#include "dipendente.h"

#include <QDate>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QPdfWriter>
#include <QPainter>
#include <QSqlError>
#include <cmath>

DifferenzeExporter::DifferenzeExporter(QObject *parent)
    : QThread(parent)
    , m_firstHeaderHeight(1)
    , m_secondHeaderHeight(2)
    , m_thirdHeaderHeight(5)
    , m_gridWidth(500)
    , m_gridHeight(260)
    , m_totalRows(20)
    , m_tableWidth(13000)
    , m_totalHeaderHeight(m_firstHeaderHeight + m_secondHeaderHeight + m_thirdHeaderHeight)
    , m_tableHeight(m_gridHeight*m_totalHeaderHeight+m_gridHeight*m_totalRows)
{
    m_idUnita = -1;
    m_timecard.clear();
}

DifferenzeExporter::~DifferenzeExporter()
= default;

void DifferenzeExporter::setPath(const QString &path)
{
    m_path = path;
}

void DifferenzeExporter::setUnita(int id)
{
    m_idUnita = id;
}

void DifferenzeExporter::setMese(const QString &timecard)
{
    m_timecard = timecard;
}

void DifferenzeExporter::setPrintPdf(bool ok)
{
    m_printPdf = ok;
}

void DifferenzeExporter::setStoricizza(bool ok)
{
    m_storicizza = ok;
}

void DifferenzeExporter::run()
{
    Utilities::m_connectionName = "DifferenzeExporter";

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
    QString pdfFileName = "Differenze_Competenze_" + QString(mese).replace(" ","_");
    QString csvFileName = "Differenze_Competenze_" + QString(mese).replace(" ","_");

    if(m_idUnita != -1) {
        unitaIdList << m_idUnita;
        pdfFileName += "_" + QString::number(m_idUnita) + "_" + SqlQueries::getUnitaNomeBreve(m_idUnita);
        csvFileName += "_" + QString::number(m_idUnita) + "_" + SqlQueries::getUnitaNomeBreve(m_idUnita);
    } else {
        unitaIdList << SqlQueries::getUnitaIdsInTimecard(m_timecard);
    }

    emit totalRows(unitaIdList.count());

    pdfFileName += ".pdf";
    csvFileName += ".csv";

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

    const QString rowText =
            "LVARIDE;" +
            date.toString("dd/MM/yyyy") +
            ";%1;" +
            QString::number(date.addDays(1).year()) + ";" +
            QString::number(date.addDays(1).month()) + ";" +
            QLocale().monthName(date.addDays(1).month(), QLocale::ShortFormat).toUpper() + ";%2;%3;;%4;;;;;";

    QPdfWriter writer(m_path + QDir::separator() + pdfFileName);
    QPainter painter(&writer);
    if(m_printPdf) {
#if QT_VERSION >= 0x060000
        writer.setPageSize(QPageSize::A4);
#else
        writer.setPageSize(QPagedPaintDevice::A4);
#endif
        writer.setPageMargins(QMargins(30, 30, 30, 30));
        writer.setPageOrientation(QPageLayout::Landscape);
        writer.setTitle(QString(pdfFileName).replace(".pdf", "").replace("_", " "));
        writer.setCreator("Gestione Competenze Medici");
        painter.setRenderHint(QPainter::Antialiasing, true);
    }

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

    if(!SqlQueries::tableExists("tcp_" + m_currentMonthYear.toString("yyyyMM"))) {
        SqlQueries::createPagatoTable(m_currentMonthYear.year(), m_currentMonthYear.month());
    }

    foreach (int unitaId, unitaIdList) {
        QVector<int> dirigentiIdList = SqlQueries::getDoctorsIdsFromUnitInTimecard(m_timecard, unitaId);

        QList<CompetenzePagate *> daPagare;

        foreach (int dirigenteId, dirigentiIdList) {
            m_competenzePagate = SqlQueries::competenzePagate(SqlQueries::doctorMatricola(dirigenteId), m_currentMonthYear.year(), m_currentMonthYear.month());
            m_competenze = new Competenza(m_timecard,dirigenteId, true /* esportazione */);
            double whole, fractional;
            fractional = std::modf(m_competenze->repCount(), &whole);

            CompetenzePagate *competenzeDaPagare = new CompetenzePagate;

            competenzeDaPagare->setCi(m_competenze->dipendente()->matricola());
            competenzeDaPagare->setDeficit(m_competenze->deficitOrario() < 0 ? m_competenze->deficitOrario() : 0);
            competenzeDaPagare->setIndNotturna(0); // 26 non si paga più?
            competenzeDaPagare->setIndFestiva(m_competenze->numGuarDiurne() + m_competenze->dipendente()->indennitaFestiva().count()); // 62
            competenzeDaPagare->setStr_reparto_ord(0); // 66 solitamente nullo
            competenzeDaPagare->setStr_reparto_nof(0); // 68 solitamente nullo
            competenzeDaPagare->setStr_reparto_nef(0); // 67 solitamente nullo
            competenzeDaPagare->setStr_repe_ord(m_competenze->numOreRep(Reperibilita::Ordinaria)); // 70
            competenzeDaPagare->setStr_repe_nof(m_competenze->numOreRep(Reperibilita::FestivaONotturna)); // 72
            competenzeDaPagare->setStr_repe_nef(m_competenze->numOreRep(Reperibilita::FestivaENotturna)); // 71
            competenzeDaPagare->setStr_guard_ord(m_competenze->numOreGuarOrd()); // 73
            competenzeDaPagare->setStr_guard_nof(m_competenze->numOreGuarFesONot()); // 75
            competenzeDaPagare->setStr_guard_nef(m_competenze->numOreGuarFesENot()); // 74
            competenzeDaPagare->setTurni_repe(whole); // 25
            competenzeDaPagare->setOre_repe(fractional > 0.0 ? 6 : 0); // 40
            competenzeDaPagare->setGuard_diu(m_competenze->numGuarDiurne()); // 1512
            competenzeDaPagare->setGuard_not(m_competenze->numGuar() + m_competenze->numGuarGFNonPag()); // 1571
            competenzeDaPagare->setGrande_fes(m_competenze->numGrFestPagabili()); // 921
            competenzeDaPagare->setDateTime(now);

            if(m_competenzePagate != competenzeDaPagare) {
                CompetenzePagate *differenzaCompetenze = new CompetenzePagate;
                differenzaCompetenze->setCi(m_competenze->dipendente()->matricola());
                differenzaCompetenze->setDeficit(m_competenze->deficitOrario() < 0 ? m_competenze->deficitOrario() : 0);
                differenzaCompetenze->setIndNotturna(0); // 26 non si paga più
                differenzaCompetenze->setIndFestiva(competenzeDaPagare->indFestiva() - m_competenzePagate->indFestiva()); // 62
                differenzaCompetenze->setStr_reparto_ord(0); // 66 solitamente nullo
                differenzaCompetenze->setStr_reparto_nof(0); // 68 solitamente nullo
                differenzaCompetenze->setStr_reparto_nef(0); // 67 solitamente nullo
                differenzaCompetenze->setStr_repe_ord(competenzeDaPagare->str_repe_ord() - m_competenzePagate->str_repe_ord()); // 70
                differenzaCompetenze->setStr_repe_nof(competenzeDaPagare->str_repe_nof() - m_competenzePagate->str_repe_nof()); // 72
                differenzaCompetenze->setStr_repe_nef(competenzeDaPagare->str_repe_nef() - m_competenzePagate->str_repe_nef()); // 71
                differenzaCompetenze->setStr_guard_ord(competenzeDaPagare->str_guard_ord() - m_competenzePagate->str_guard_ord()); // 73
                differenzaCompetenze->setStr_guard_nof(competenzeDaPagare->str_guard_nof() - m_competenzePagate->str_guard_nof()); // 75
                differenzaCompetenze->setStr_guard_nef(competenzeDaPagare->str_guard_nef() - m_competenzePagate->str_guard_nef()); // 74
                differenzaCompetenze->setTurni_repe(competenzeDaPagare->turni_repe() - m_competenzePagate->turni_repe()); // 25
                differenzaCompetenze->setOre_repe(competenzeDaPagare->ore_repe() - m_competenzePagate->ore_repe()); // 40
                differenzaCompetenze->setGuard_diu(competenzeDaPagare->guard_diu() - m_competenzePagate->guard_diu()); // 1512
                differenzaCompetenze->setGuard_not(competenzeDaPagare->guard_not() - m_competenzePagate->guard_not()); // 1571
                differenzaCompetenze->setGrande_fes(competenzeDaPagare->grande_fes() - m_competenzePagate->grande_fes()); // 921
                differenzaCompetenze->setDateTime(now);
                daPagare << differenzaCompetenze;
            }
        }
        if(daPagare.count() > 0) {
            QStringList notes;
            currRow++;
            emit currentRow(currRow);
            QString unitaName = SqlQueries::getUnitaNomeCompleto(unitaId);

            if(m_printPdf) {
                if(!isFileStart)
                    writer.newPage();

                disegnaTabella(painter);
                printData(painter, now.toString("dd/MM/yyyy hh:mm:ss"));
                printMonth(painter, mese);
                printUnitaName(painter, unitaName);
                printUnitaNumber(painter, unitaId);
            }

            int counter = 0;
            for(auto pag : qAsConst(daPagare)) {
                if(counter != 0 && (counter%m_totalRows) == 0) {
                    counter = 0;
                    writer.newPage();
                    disegnaTabella(painter);
                    printData(painter, now.toString("dd/MM/yyyy hh:mm:ss"));
                    printMonth(painter, mese);
                    printUnitaName(painter, unitaName);
                    printUnitaNumber(painter, unitaId);
                }

                if(m_storicizza)
                    SqlQueries::saveCompetenzePagate(pag, m_currentMonthYear.year(), m_currentMonthYear.month());


                if(pag->indFestiva() > 0) // 62
                    out << rowText.arg(pag->ci()).arg("IND.FES").arg("*").arg(QString::number(pag->indFestiva())) << "\n";

                if(pag->guard_diu() > 0) // 1512
                    out << rowText.arg(pag->ci()).arg("GUARD.NOT").arg("1").arg(QString::number(pag->guard_diu())) << "\n";

                if(pag->grande_fes() > 0) { // 921
                    out << rowText.arg(pag->ci()).arg("GR.FES.NOT").arg("*").arg(QString::number(pag->grande_fes())) << "\n";
                    out << rowText.arg(pag->ci()).arg("GUARD.NOT").arg("*").arg("-" + QString::number(pag->grande_fes())) << "\n";
                }

                if(pag->str_repe_ord() > 0) // 70
                    out << rowText.arg(pag->ci()).arg("REP.ORD").arg("*").arg(QString::number(pag->str_repe_ord())) << "\n";

                if(pag->str_repe_nof() > 0) // 72
                    out << rowText.arg(pag->ci()).arg("REP.NOF").arg("*").arg(QString::number(pag->str_repe_nof())) << "\n";

                if(pag->str_repe_nef() > 0) // 71
                    out << rowText.arg(pag->ci()).arg("REP.NEF").arg("*").arg(QString::number(pag->str_repe_nef())) << "\n";

                if(pag->str_guard_ord() > 0) // 73
                    out << rowText.arg(pag->ci()).arg("STR.ORD.GU").arg("*").arg(QString::number(pag->str_guard_ord())) << "\n";

                if(pag->str_guard_nof() > 0) // 75
                    out << rowText.arg(pag->ci()).arg("STR.NOF.GU").arg("*").arg(QString::number(pag->str_guard_nof())) << "\n";

                if(pag->str_guard_nef() > 0) // 74
                    out << rowText.arg(pag->ci()).arg("STR.NEF.GU").arg("*").arg(QString::number(pag->str_guard_nef())) << "\n";

                if(pag->turni_repe() > 0) // 25
                    out << rowText.arg(pag->ci()).arg("IND.PR.REP").arg("*").arg(QString::number(pag->turni_repe())) << "\n";

                if(pag->ore_repe() > 0) // 40
                    out << rowText.arg(pag->ci()).arg("IND.REP.OR").arg("*").arg(QString::number(pag->ore_repe())) << "\n";

                if(pag->guard_not() > 0) { // 1571
                    out << rowText.arg(pag->ci()).arg("GUARD.NOT").arg("*").arg("-" + QString::number(pag->guard_not())) << "\n";
                }

                printBadge(painter, pag->ci(),counter);
                printName(painter, SqlQueries::doctorName(pag->ci()),counter);
                printDeficit(painter, pag->deficit() < 0 ?
                                 Utilities::inOrario(abs(pag->deficit())) : "//",counter);

                if(m_currentMonthYear < Utilities::ccnl1618Date) {
                    printNotturno(painter, 0,counter); // 26
                } else {
                    printNotturno(painter, 0,counter); // 26
                }
                printFestivo(painter, pag->indFestiva(), counter); // 62
                printRepNumTurni(painter, pag->turni_repe(), counter); // 25
                printRepNumOre(painter, pag->ore_repe(), counter); //40
                printStrRepartoOrdin(painter,0,counter); // 66 solitamente nullo
                printStrRepartoFesONott(painter,0,counter); // 68 solitamente nullo
                printStrRepartoFesENott(painter,0,counter); // 67 solitamente nullo
                printNumGuarNott(painter, pag->guard_not(), counter); //1571
                printNumGuarDiur(painter, pag->guard_diu(), counter); // 1512
                printNumGfFesNott(painter, pag->grande_fes(), counter); // 921
                printNumOreGuarFesENot(painter, pag->str_guard_ord(), counter); // 74
                printNumOreGuarFesONot(painter, pag->str_guard_ord(), counter); // 75
                printNumOreGuarOrd(painter, pag->str_guard_ord(), counter); // 73
                printNumOreRepFesENot(painter, pag->str_repe_nef(), counter); // 71
                printNumOreRepFesONot(painter, pag->str_repe_nof(), counter); // 72
                printNumOreRepOrd(painter, pag->str_repe_ord(), counter); // 70
//                if(!m_competenza->note().isEmpty()) {
//                    notes << QString::number(m_competenza->badgeNumber()) + " - " + m_competenza->name() + ": " + m_competenza->note();
//                }
                counter++;
            }
        }
//        printNote(painter, notes);
        isFileStart = false;
    }

    outFile.close();

    emit exportFinished(m_path + QDir::separator() + pdfFileName);
}

void DifferenzeExporter::disegnaTabella(QPainter &painter)
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

    // Deficit Orario
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*18, m_gridHeight, m_gridWidth*2, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter | Qt::TextWordWrap, "Deficit Orario");
    painter.restore();

    // Indennità
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*16, m_gridHeight, m_gridWidth*2, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter | Qt::TextWordWrap, "Indennità");
    painter.restore();

    // Straordinario Reparto
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*14, m_gridHeight, m_gridWidth*3, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter | Qt::TextWordWrap, "Straordinario Reparto");
    painter.restore();

    // Straordinario Reperibilità
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*11, m_gridHeight, m_gridWidth*3, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter | Qt::TextWordWrap, "Straordinario Reperibilità");
    painter.restore();

    // Straordinario Guardia
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*8, m_gridHeight, m_gridWidth*3, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter | Qt::TextWordWrap, "Straordinario Guardia");
    painter.restore();

    // Turni Reperibilità
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*5, m_gridHeight, m_gridWidth*2, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter | Qt::TextWordWrap, "Turni Reperibilità");
    painter.restore();

    if(m_currentMonthYear < Utilities::ccnl1618Date) {
        // Guardia Notturna
        painter.save();
        painter.setPen(Qt::black);
        painter.setFont(headerFont());
        painter.drawText(QRect(m_tableWidth-m_gridWidth*3, m_gridHeight, m_gridWidth*1, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter | Qt::TextWordWrap, "Guard. Nott.");
        painter.restore();
        // Grande Festività Notturna
        painter.save();
        painter.setPen(Qt::black);
        painter.setFont(headerFont());
        painter.drawText(QRect(m_tableWidth-m_gridWidth*2, m_gridHeight, m_gridWidth*1, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter | Qt::TextWordWrap, "Grand. Fes. Nott.");
        painter.restore();

        // Vitto
        painter.save();
        painter.setPen(Qt::black);
        painter.setFont(headerFont());
        painter.drawText(QRect(m_tableWidth-m_gridWidth*1, m_gridHeight, m_gridWidth*1, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter | Qt::TextWordWrap, "Vitto");
        painter.restore();
    } else {
        // Guardia Diurna
        painter.save();
        painter.setPen(Qt::black);
        painter.setFont(headerFont());
        painter.drawText(QRect(m_tableWidth-m_gridWidth*3, m_gridHeight, m_gridWidth*1, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter | Qt::TextWordWrap, "Guard. Diurna");
        painter.restore();

        // Guardia Notturna
        painter.save();
        painter.setPen(Qt::black);
        painter.setFont(headerFont());
        painter.drawText(QRect(m_tableWidth-m_gridWidth*2, m_gridHeight, m_gridWidth*1, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter | Qt::TextWordWrap, "Guard. Nott.");
        painter.restore();

        // Grande Festività Notturna
        painter.save();
        painter.setPen(Qt::black);
        painter.setFont(headerFont());
        painter.drawText(QRect(m_tableWidth-m_gridWidth*1, m_gridHeight, m_gridWidth*1, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter | Qt::TextWordWrap, "Grand. Fes. Nott.");
        painter.restore();
    }

    // Matricola
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerFont());
    painter.drawText(QRect(0, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth*2, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "Matricola");
    painter.restore();

    // Cognome
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerFont());
    painter.drawText(QRect(m_gridWidth*2, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_tableWidth-m_gridWidth*20, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "COGNOME");
    painter.restore();

    // Divisione Indennità 1
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*16, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "26");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*16, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Notturna");
    painter.restore();

    // Divisione Indennità 2
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*15, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "62");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*15, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Festiva");
    painter.restore();

    // Straordinario Reparto 1
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*14, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "66");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*14, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Ordinario");
    painter.restore();

    // Straordinario Reparto 2
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*13, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "68");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*13, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Festivo o Notturno");
    painter.restore();

    // Straordinario Reparto 3
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*12, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "67");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*12, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Festivo e Notturno");
    painter.restore();

    // Straordinario Reperibilità 1
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*11, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "70");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*11, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Ordinario");
    painter.restore();

    // Straordinario Reperibilità 2
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*10, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "72");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*10, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Festivo o Notturno");
    painter.restore();

    // Straordinario Reperibilità 3
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*9, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "71");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*9, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Festivo e Notturno");
    painter.restore();

    // Straordinario Guardia 1
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*8, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "73");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*8, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Ordinario");
    painter.restore();

    // Straordinario Guardia 2
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*7, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "75");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*7, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Festivo o Notturno");
    painter.restore();

    // Straordinario Guardia 3
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*6, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "74");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*6, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Festivo e Notturno");
    painter.restore();

    // Reperibilità Turni 1
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*5, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "25");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*5, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Numero Turni");
    painter.restore();

    // Reperibilità Turni 2
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*4, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "40");
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*4, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "Numero Ore");
    painter.restore();

    if(m_currentMonthYear < Utilities::ccnl1618Date) {
        // Guardia Nott
        painter.save();
        painter.setPen(Qt::black);
        painter.setFont(headerLightFont());
        painter.drawText(QRect(m_tableWidth-m_gridWidth*3, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "1512");
        painter.rotate(-90);
        painter.setFont(headerFont());
        painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*3, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "€ 50,00");
        painter.restore();

        // Grand. Fes Nott
        painter.save();
        painter.setPen(Qt::black);
        painter.setFont(headerLightFont());
        painter.drawText(QRect(m_tableWidth-m_gridWidth*2, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "1571");
        painter.rotate(-90);
        painter.setFont(headerFont());
        painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*2, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "€ 480,00");
        painter.restore();

        // Vitto
        painter.save();
        painter.setPen(Qt::black);
        painter.setFont(headerLightFont());
        painter.drawText(QRect(m_tableWidth-m_gridWidth, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "921");
        painter.restore();
    } else {
        // Guardia Nott
        painter.save();
        painter.setPen(Qt::black);
        painter.setFont(headerLightFont());
        painter.drawText(QRect(m_tableWidth-m_gridWidth*3, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "1512");
        painter.rotate(-90);
        painter.setFont(headerFont());
        painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*3, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "€ 100,00");
        painter.restore();

        // Grand. Fes Nott
        painter.save();
        painter.setPen(Qt::black);
        painter.setFont(headerLightFont());
        painter.drawText(QRect(m_tableWidth-m_gridWidth*2, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "1571");
        painter.rotate(-90);
        painter.setFont(headerFont());
        painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth*2, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "€ 100,00");
        painter.restore();

        // Vitto
        painter.save();
        painter.setPen(Qt::black);
        painter.setFont(headerLightFont());
        painter.drawText(QRect(m_tableWidth-m_gridWidth, m_gridHeight*(m_firstHeaderHeight + m_secondHeaderHeight), m_gridWidth, m_gridHeight*m_thirdHeaderHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, "921");
        painter.rotate(-90);
        painter.setFont(headerFont());
        painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth-m_gridWidth, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, "€ 480,00");
        painter.restore();
    }
}

QFont DifferenzeExporter::numberFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(300);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont DifferenzeExporter::unitaFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(180);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont DifferenzeExporter::badgeFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(150);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont DifferenzeExporter::headerFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(120);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont DifferenzeExporter::headerLightFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(110);
    font.setFamily("Sans Serif");
    return font;
}

void DifferenzeExporter::printMonth(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(unitaFont());
    painter.drawText(QRect(0, 0, m_tableWidth-600, m_gridHeight), Qt::AlignRight | Qt::AlignVCenter, text);
    painter.restore();
}

void DifferenzeExporter::printUnitaName(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(unitaFont());
    painter.drawText(QRect(600, 0, m_tableWidth, m_gridHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.restore();
}

void DifferenzeExporter::printUnitaNumber(QPainter &painter, const int &id)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(numberFont());
    painter.drawText(QRect(0, m_gridHeight, m_gridWidth*2, m_gridHeight*m_secondHeaderHeight), Qt::AlignCenter, QString::number(id));
    painter.restore();
}

void DifferenzeExporter::printBadge(QPainter &painter, const int &text, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    painter.drawText(QRect(20, m_gridHeight*m_totalHeaderHeight+(row*m_gridHeight), m_gridWidth*2-20, m_gridHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, QString::number(text));
    painter.restore();
}

void DifferenzeExporter::printName(QPainter &painter, const QString &text, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    painter.drawText(QRect(m_gridWidth*2+20, m_gridHeight*m_totalHeaderHeight+(row*m_gridHeight), m_tableWidth-m_gridWidth*20-20, m_gridHeight), Qt::AlignLeft | Qt::AlignBottom, text);
    painter.restore();
}

void DifferenzeExporter::printDeficit(QPainter &painter, const QString &text, const int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*18, m_gridHeight*m_totalHeaderHeight+(row*m_gridHeight), m_gridWidth*2, m_gridHeight), Qt::AlignCenter | Qt::AlignBottom, text == "//" ? "" : text);
    painter.restore();
}

void DifferenzeExporter::printNotturno(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printFestivo(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printStrRepartoOrdin(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printStrRepartoFesONott(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printStrRepartoFesENott(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printRepNumTurni(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printRepNumOre(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printNumGuarDiur(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printNumGuarNott(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printNumGfFesNott(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printNumOreGuarFesENot(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printNumOreGuarFesONot(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printNumOreGuarOrd(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printNumOreRepFesENot(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printNumOreRepFesONot(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printNumOreRepOrd(QPainter &painter, const int value, const int row)
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

void DifferenzeExporter::printCasi(QPainter &painter)
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

void DifferenzeExporter::printNote(QPainter &painter, const QStringList &note)
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

void DifferenzeExporter::printData(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.rotate(-90);
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, text);
    painter.restore();
}

QRect DifferenzeExporter::getRect(int row, int column) const
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
