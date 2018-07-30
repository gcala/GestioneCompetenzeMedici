/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017-2018 Giuseppe Calà <giuseppe.cala@mailbox.org>
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

#include "deficitrecuperiexporter.h"
#include "sqlqueries.h"
#include "competenza.h"

#include <QDate>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QPdfWriter>
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>

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
    , m_rowFontPixels(240)
    , m_unitFontPixels(280)
    , m_titleFontPixels(400)
    , m_pageFontPixels(200)
    , m_rowHeight(280)
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
    QVector<int> unitaIdList;
    const QString s = m_timecard.split("_").last();
    QString mese = QDate::longMonthName(s.right(2).toInt(), QDate::StandaloneFormat) + " " + s.left(4);
    QString fileName = "Deficit_" + QString(mese).replace(" ","_");

    if(m_idUnita != -1) {
        unitaIdList << m_idUnita;
        fileName += "_" + QString::number(m_idUnita) + "_" + SqlQueries::getUnitaNomeBreve(m_idUnita);
    } else {
        unitaIdList << SqlQueries::getUnitaIdsInTimecard(m_timecard);
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

    writer.setPageSize(QPagedPaintDevice::A4);
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

        QString unitaName = SqlQueries::getUnitaNomeCompleto(unitaId);

        QVector<int> dirigentiIdList = SqlQueries::getDoctorsIdsFromUnitInTimecard(m_timecard, unitaId);

        int counter = 0;

        QList<Doctor> doctors;

        foreach (int dirigenteId, dirigentiIdList) {
            m_competenza = new Competenza(m_timecard,dirigenteId);

            if(m_competenza->deficitOrario() == "//")
                continue;

            Doctor doctor;
            doctor.badge = m_competenza->badgeNumber();
            doctor.name = m_competenza->name();
            doctor.deficit = m_competenza->deficitOrario();

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
            for(Doctor doc : doctors) {
                printBadge(painter, doc.badge,counter);
                printName(painter, doc.name,counter);
                printDeficit(painter, doc.deficit,counter);
                m_offset += m_rowHeight;
                counter++;
            }
            m_offset += 500;
        }
    }
}

void DeficitRecuperiExporter::printCsv(const QString &fileName, const QString &mese, const QVector<int> &unitaIdList)
{
    QFile outFile(m_path + QDir::separator() + fileName);
    if(!outFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Impossibile aprire il file di destinazione";
        return;
    }

    int currRow = 0;

    QTextStream out(&outFile);
    out.setAutoDetectUnicode(true);

//    out << "Deficit " + mese + ";;\n";
//    out << ";;\n";

    out << "UNITA';MATR.;NOMINATIVO;DEFICIT;FERIE;MALATTIA;CONGEDI;RECUPERI\n";

    foreach (int unitaId, unitaIdList) {
        currRow++;
        emit currentRow(currRow);

        QString unitaName = SqlQueries::getUnitaNomeCompleto(unitaId);

        QVector<int> dirigentiIdList = SqlQueries::getDoctorsIdsFromUnitInTimecard(m_timecard, unitaId);

        QList<Doctor> doctors;

        foreach (int dirigenteId, dirigentiIdList) {
            m_competenza = new Competenza(m_timecard,dirigenteId);

//            if(m_competenza->deficitOrario() == "//")
//                continue;

            Doctor doctor;
            doctor.badge = m_competenza->badgeNumber();
            doctor.name = m_competenza->name();
            if(m_competenza->deficitOrario() == "//")
                doctor.deficit = "00:00";
            else
                doctor.deficit = m_competenza->deficitOrario();
            doctor.ferie = m_competenza->ferieDates();
            doctor.congedi = m_competenza->congediDates();
            doctor.malattie = m_competenza->malattiaDates();
            doctor.recuperi = m_competenza->rmcDates();
            doctor.recuperi << m_competenza->rmpDates();

            doctors << doctor;
        }

        if(doctors.size() > 0) {
            for(Doctor doc : doctors) {
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

                out << unitaName + ";"
                    + doc.badge + ";" + doc.name + ";" + doc.deficit + ";" + ferie.values().join(",") + ";" + malattie.values().join(",") + ";" + congedi.values().join(",") + ";" + recuperi.values().join(",") + "\n";
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

void DeficitRecuperiExporter::printBadge(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(rowFont());
//    painter.drawRect(0, m_offset, m_badgeWidth, m_rowHeight);
    painter.drawText(QRect(0, m_offset, m_badgeWidth, m_rowHeight), Qt::AlignRight | Qt::AlignVCenter | Qt::TextWordWrap, text);
    painter.restore();
}

void DeficitRecuperiExporter::printName(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(rowFont());
//    painter.drawRect(m_badgeWidth+m_cellSpacing, m_offset, m_nameWidth, m_rowHeight);
    painter.drawText(QRect(m_badgeWidth+m_cellSpacing, m_offset, m_nameWidth, m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.restore();
}

void DeficitRecuperiExporter::printDeficit(QPainter &painter, const QString &text, int row)
{
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

