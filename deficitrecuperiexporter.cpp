/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "deficitrecuperiexporter.h"
//#include "sqlqueries.h"
#include "competenza.h"
#include "sqldatabasemanager.h"
#include "utilities.h"
#include "apiservice.h"

#include <QDate>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QPdfWriter>
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>
#include <QSqlError>

DeficitRecuperiExporter::DeficitRecuperiExporter(QObject *parent)
    : QThread(parent)
    , m_pageWidth(8900)
    , m_pageHeight(13000)
    , m_badgeWidth(800)
    , m_nameWidth(3850)
    , m_deficitWidth(900)
    , m_cellSpacing(100)
    , m_titleHeight(1000)
    , m_unitHeight(380)
    , m_rowHeight(280)
    , m_rowFontPixels(240)
    , m_unitFontPixels(280)
    , m_titleFontPixels(400)
    , m_pageFontPixels(200)
{
    m_idUnita = -1;
    m_timecard.clear();
}

DeficitRecuperiExporter::~DeficitRecuperiExporter()
{

}

void DeficitRecuperiExporter::setPath(const QString &path)
{
    m_path = path;
}

void DeficitRecuperiExporter::setUnita(int id)
{
    m_idUnita = id;
}

void DeficitRecuperiExporter::setMese(const QString &timecard)
{
    m_timecard = timecard;
}

void DeficitRecuperiExporter::setType(const QString &type)
{
    m_type = type;
}

void DeficitRecuperiExporter::run()
{
    Utilities::m_connectionName = "DeficitRecuperiExporter";

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

    QVector<int> unitaIdList;
    const QString s = m_timecard.split("_").last();
    QString mese = QLocale().monthName(s.right(2).toInt()) + " " + s.left(4);
    QString fileName = "Deficit_" + s;

    if(m_idUnita != -1) {
        unitaIdList << m_idUnita;
        fileName += "_" + QString::number(m_idUnita) + "_" + ApiService::instance().getUnitaDataById(m_idUnita).breve;
    } else {
        const auto units = ApiService::instance().getUnitaDataFromTimecard(m_timecard);
        for(const auto u : units) {
            unitaIdList << u.idUnita;
        }
    }

    emit totalRows(unitaIdList.count());

    fileName += "." + m_type;

    if(m_type == "pdf")
        printPdf(fileName, mese, unitaIdList);
    else
        printCsv(fileName, mese, unitaIdList);

    emit exportFinished(m_path + QDir::separator() + fileName);
}

void DeficitRecuperiExporter::printPdf(const QString &fileName, const QString &mese, const QVector<int> &unitaIdList)
{
    QPdfWriter writer(m_path + QDir::separator() + fileName);

#if QT_VERSION >= 0x060000
    writer.setPageSize(QPageSize::A4);
#else
    writer.setPageSize(QPagedPaintDevice::A4);
#endif
    writer.setPageMargins(QMargins(30, 30, 30, 30));
    writer.setPageOrientation(QPageLayout::Portrait);
    writer.setTitle("Deficit " + QString(mese));
    writer.setCreator("Gestione Competenze Medici");

    QPainter painter(&writer);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int currRow = 0;
    m_offset = 0;
    int page = 1;

    printTitle(painter, mese);
    printPageNumber(painter, page);

    foreach (int unitaId, unitaIdList) {
        currRow++;
        emit currentRow(currRow);

        QString unitaName = ApiService::instance().getUnitaNomeCompleto(unitaId);

        qDebug() << "2 ------------->" << m_timecard;
        QVector<int> dirigentiIdList = ApiService::instance().getDoctorsIdsFromUnitInTimecard(m_timecard, unitaId);

        int counter = 0;

        QList<Doctor> doctors;

        foreach (int dirigenteId, dirigentiIdList) {
            m_competenza = new Competenza(m_timecard,dirigenteId);

            if(m_competenza->deficitOrario() >= 0)
                continue;

            Doctor doctor;
            doctor.badge = m_competenza->badgeNumber();
            doctor.name = m_competenza->name();
            doctor.deficitProgressivo = Utilities::inOrario(m_competenza->deficitOrario());
            doctor.deficitPuntuale = m_competenza->deficitPuntuale();

            doctors << doctor;
        }

        if(doctors.size() > 0) {
            if((m_offset + (m_unitHeight + 100 + doctors.size()*m_rowHeight)) > m_pageHeight) {
                writer.newPage();
                m_offset = 0;
                page++;
                printTitle(painter, mese);
                printPageNumber(painter, page);
            }

            printUnita(painter, unitaId, unitaName);
            Q_FOREACH(const auto &doc, doctors) {
                printBadge(painter, doc.badge,counter);
                printName(painter, doc.name,counter);
                printDeficit(painter, doc.deficitProgressivo,counter);
                m_offset += m_rowHeight;
                counter++;
            }
            m_offset += 500;
        }
    }
}

void DeficitRecuperiExporter::printCsv(const QString &fileName, const QString &mese, const QVector<int> &unitaIdList)
{
    Q_UNUSED(mese)
    QFile outFile(m_path + QDir::separator() + fileName);
    if(!outFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Impossibile aprire il file di destinazione";
        return;
    }

    int currRow = 0;

    QTextStream out(&outFile);
    out.setAutoDetectUnicode(true);

    out << "UNITA';MESE;MATR.;NOMINATIVO;DEFICIT MESE;DEFICIT PROGRESSIVO;ORE REC;ORE NON REC;FERIE;MALATTIA;CONGEDI;RECUPERI\n";

    foreach (int unitaId, unitaIdList) {
        currRow++;
        emit currentRow(currRow);

        QString unitaName = ApiService::instance().getUnitaNomeCompleto(unitaId);

        qDebug() << "3 ------------->" << m_timecard;
        QVector<int> dirigentiIdList = ApiService::instance().getDoctorsIdsFromUnitInTimecard(m_timecard, unitaId);

        QList<Doctor> doctors;

        foreach (int dirigenteId, dirigentiIdList) {
            m_competenza = new Competenza(m_timecard,dirigenteId);

//            if(m_competenza->deficitOrario() == "//")
//                continue;

            Doctor doctor;
            doctor.badge = m_competenza->badgeNumber();
            doctor.name = m_competenza->name();
            if(m_competenza->deficitOrario() >= 0)
                doctor.deficitProgressivo = "00:00";
            else
                doctor.deficitProgressivo = Utilities::inOrario(m_competenza->deficitOrario());

            if(m_competenza->deficitPuntuale() == "//")
                doctor.deficitPuntuale = "00:00";
            else
                doctor.deficitPuntuale = m_competenza->deficitPuntuale();

            if(m_competenza->minutiRecuperabili() == 0 && m_competenza->recuperiMeseSuccessivo().second == 0)
                doctor.oreRecuperabili = "00:00";
            else {
                doctor.oreRecuperabili = Utilities::inOrario(m_competenza->minutiRecuperabili());
            }

            if(m_competenza->minutiNonRecuperabili() == 0)
                doctor.oreNonRecuperabili = "00:00";
            else {
                doctor.oreNonRecuperabili = Utilities::inOrario(m_competenza->minutiNonRecuperabili());
            }

            doctor.ferie = m_competenza->ferieDates();
            doctor.congedi = m_competenza->congediDates();
            doctor.malattie = m_competenza->malattiaDates();
            doctor.recuperi = m_competenza->rmcDates();
            doctor.recuperi << m_competenza->rmpDates();

            doctors << doctor;
        }

        if(doctors.size() > 0) {
            Q_FOREACH(const auto &doc, doctors) {
                QMap<int, QString> ferie;
                QMap<int, QString> malattie;
                QMap<int, QString> congedi;
                QMap<int, QString> recuperi;

                for(QDate date : doc.ferie) {
                    ferie[date.day()] = QString::number(date.day());
                }

                for(QDate date : doc.malattie) {
                    malattie[date.day()] = QString::number(date.day());
                }

                for(QDate date : doc.congedi) {
                    congedi[date.day()] = QString::number(date.day());
                }

                for(QDate date : doc.recuperi) {
                    recuperi[date.day()] = QString::number(date.day());
                }

                const QString mese = m_timecard.split("_").last().right(2) + "." + m_timecard.split("_").last().left(4);

                out << unitaName + ";" + mese + ";"
                    + QString::number(doc.badge) + ";" + doc.name + ";" + doc.deficitPuntuale + ";" + doc.deficitProgressivo + ";" + doc.oreRecuperabili + ";" + doc.oreNonRecuperabili + ";" + ferie.values().join(",") + ";" + malattie.values().join(",") + ";" + congedi.values().join(",") + ";" + recuperi.values().join(",") + "\n";
            }
        }
    }

    outFile.close();
}

void DeficitRecuperiExporter::printTitle(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(titleFont());
    painter.drawText(QRect(0, m_offset, m_pageWidth, m_titleHeight), Qt::AlignCenter | Qt::AlignTop, "Deficit " + text);
    painter.restore();
    m_offset += m_titleHeight;
}

void DeficitRecuperiExporter::printUnita(QPainter &painter, const int &id, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(unitFont());
    painter.drawText(QRect(0, m_offset, m_pageWidth, m_unitHeight), Qt::AlignLeft | Qt::AlignVCenter, QString::number(id) + " - " + text);
    painter.restore();
    m_offset += m_unitHeight;
    m_offset += 100;
}

void DeficitRecuperiExporter::printBadge(QPainter &painter, const int &text, int row)
{
    Q_UNUSED(row)
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(rowFont());
//    painter.drawRect(0, m_offset, m_badgeWidth, m_rowHeight);
    painter.drawText(QRect(0, m_offset, m_badgeWidth, m_rowHeight), Qt::AlignRight | Qt::AlignVCenter | Qt::TextWordWrap, QString::number(text));
    painter.restore();
}

void DeficitRecuperiExporter::printName(QPainter &painter, const QString &text, int row)
{
    Q_UNUSED(row)
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(rowFont());
//    painter.drawRect(m_badgeWidth+m_cellSpacing, m_offset, m_nameWidth, m_rowHeight);
    painter.drawText(QRect(m_badgeWidth+m_cellSpacing, m_offset, m_nameWidth, m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.restore();
}

void DeficitRecuperiExporter::printDeficit(QPainter &painter, const QString &text, int row)
{
    Q_UNUSED(row)
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(rowFont());
//    painter.drawRect(m_badgeWidth + m_nameWidth+m_cellSpacing*2, m_offset, m_deficitWidth, m_rowHeight);
    painter.drawText(QRect(m_badgeWidth + m_nameWidth+m_cellSpacing*2, m_offset, m_deficitWidth, m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, text == "//" ? "" : text);
    painter.restore();
}

void DeficitRecuperiExporter::printPageNumber(QPainter &painter, int page)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(pageFont());
//    painter.drawRect(m_badgeWidth + m_nameWidth+m_cellSpacing*2, m_offset, m_deficitWidth, m_rowHeight);
    painter.drawText(QRect(0, 0, m_pageWidth, m_pageHeight + 150), Qt::AlignCenter | Qt::AlignBottom, "Pagina " + QString::number(page));
    painter.restore();
}

QFont DeficitRecuperiExporter::titleFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(m_titleFontPixels);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont DeficitRecuperiExporter::unitFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(m_unitFontPixels);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont DeficitRecuperiExporter::rowFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(m_rowFontPixels);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont DeficitRecuperiExporter::pageFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(m_pageFontPixels);
    font.setBold(false);
    font.setFamily("Sans Serif");
    return font;
}

