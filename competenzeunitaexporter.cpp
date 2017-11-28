/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017 Giuseppe Calà <giuseppe.cala@mailbox.org>
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

#include "competenzeunitaexporter.h"
#include "sqlqueries.h"
#include "competenza.h"

#include <QDate>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QPdfWriter>
#include <QPainter>

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
    , m_tableHeight(m_gridHeight*m_totalHeaderHeight+m_gridHeight*m_totalRows)
{
    m_idUnita = -1;
    m_timecard.clear();
}

CompetenzeUnitaExporter::~CompetenzeUnitaExporter()
{

}

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
    QVector<int> unitaIdList;
    const QString s = m_timecard.split("_").last();
    QString mese = QDate::longMonthName(s.right(2).toInt(), QDate::StandaloneFormat) + " " + s.left(4);
    QString fileName = "Competenze_" + QString(mese).replace(" ","_");

    if(m_idUnita != -1) {
        unitaIdList << m_idUnita;
        fileName += "_" + QString::number(m_idUnita) + "_" + SqlQueries::getUnitaNomeBreve(m_idUnita);
    } else {
        unitaIdList << SqlQueries::getUnitaIdsInTimecard(m_timecard);
    }

    emit totalRows(unitaIdList.count());

    fileName += ".pdf";

    QPdfWriter writer(m_path + QDir::separator() + fileName);

    writer.setPageSize(QPagedPaintDevice::A4);
    writer.setPageMargins(QMargins(30, 30, 30, 30));
    writer.setPageOrientation(QPageLayout::Landscape);

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
    m_casiGuarNott = 0;
    m_casiGranFest = 0;

    foreach (int unitaId, unitaIdList) {
        QStringList notes;
        currRow++;
        emit currentRow(currRow);
        if(!isFileStart)
            writer.newPage();

        disegnaTabella(painter);

        if(m_printData)
            printData(painter);

        QString unitaName = SqlQueries::getUnitaNomeCompleto(unitaId);

        printMonth(painter, mese);
        printUnitaName(painter, unitaName);
        printUnitaNumber(painter, unitaId);

        QVector<int> dirigentiIdList = SqlQueries::getDoctorsIdsFromUnitInTimecard(m_timecard, unitaId);

        int counter = 0;

        foreach (int dirigenteId, dirigentiIdList) {
            m_competenza = new Competenza(m_timecard,dirigenteId);
            if(counter != 0 && (counter%m_totalRows) == 0) {
                counter = 0;
                writer.newPage();
                disegnaTabella(painter);
                if(m_printData)
                    printData(painter);
                printMonth(painter, mese);
                printUnitaName(painter, unitaName);
                printUnitaNumber(painter, unitaId);
            }
            printBadge(painter, m_competenza->badgeNumber(),counter);
            printName(painter, m_competenza->name(),counter);
            printDeficit(painter, m_competenza->deficitOrario(),counter);
            printNotturno(painter, m_competenza->notte(),counter);
            printFestivo(painter, m_competenza->festivo(),counter);
            printRepNumTurni(painter,m_competenza->repCount().split(".").first(),counter);
            printRepNumOre(painter,(m_competenza->repCount().contains(".") ? "6" : "//"),counter);
            printStrRepartoOrdin(painter,"//",counter);
            printStrRepartoFesONott(painter,"//",counter);
            printStrRepartoFesENott(painter,"//",counter);
            printNumGfFesNott(painter,m_competenza->numGrFestPagabili() > 0 ? QString::number(m_competenza->numGrFestPagabili()) : "//",counter);
            printNumGuarNott(painter,m_competenza->numGuarGFNonPag() > 0 ? QString::number(m_competenza->numGuarGFNonPag()) : "//",counter);
            printNumOreGuarFesENot(painter,m_competenza->numOreGuarFesENot() > 0 ? QString::number(m_competenza->numOreGuarFesENot()) : "//",counter);
            printNumOreGuarFesONot(painter,m_competenza->numOreGuarFesONot() > 0 ? QString::number(m_competenza->numOreGuarFesONot()) : "//",counter);
            printNumOreGuarOrd(painter,m_competenza->numOreGuarOrd() > 0 ? QString::number(m_competenza->numOreGuarOrd()) : "//",counter);
            printNumOreRepFesENot(painter,m_competenza->numOreRepFesENot() > 0 ? QString::number(m_competenza->numOreRepFesENot()) : "//",counter);
            printNumOreRepFesONot(painter,m_competenza->numOreRepFesONot() > 0 ? QString::number(m_competenza->numOreRepFesONot()) : "//",counter);
            printNumOreRepOrd(painter,m_competenza->numOreRepOrd() > 0 ? QString::number(m_competenza->numOreRepOrd()) : "//",counter);
            if(!m_competenza->note().isEmpty()) {
                notes << m_competenza->badgeNumber() + " - " + m_competenza->name() + ": " + m_competenza->note();
            }
            counter++;
        }
        printNote(painter, notes);
        isFileStart = false;
    }

    if(m_printCasi)
        printCasi(painter);

    emit exportFinished(m_path + QDir::separator() + fileName);
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

void CompetenzeUnitaExporter::printBadge(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    painter.drawText(QRect(20, m_gridHeight*m_totalHeaderHeight+(row*m_gridHeight), m_gridWidth*2-20, m_gridHeight), Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWordWrap, text);
    painter.restore();
}

void CompetenzeUnitaExporter::printName(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    painter.drawText(QRect(m_gridWidth*2+20, m_gridHeight*m_totalHeaderHeight+(row*m_gridHeight), m_tableWidth-m_gridWidth*20-20, m_gridHeight), Qt::AlignLeft | Qt::AlignBottom, text);
    painter.restore();
}

void CompetenzeUnitaExporter::printDeficit(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    painter.drawText(QRect(m_tableWidth-m_gridWidth*18, m_gridHeight*m_totalHeaderHeight+(row*m_gridHeight), m_gridWidth*2, m_gridHeight), Qt::AlignCenter | Qt::AlignBottom, text == "//" ? "" : text);
    painter.restore();
}

void CompetenzeUnitaExporter::printNotturno(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 16));
    } else {
        m_casiIndennitaNotturna++;
        painter.drawText(getRect(row, 16), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printFestivo(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 15));
    } else {
        m_casiIndennitaFestiva++;
        painter.drawText(getRect(row, 15), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
        painter.restore();
}

void CompetenzeUnitaExporter::printStrRepartoOrdin(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 14));
    } else {
        m_casiStrRepaOrd++;
        painter.drawText(getRect(row, 14), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printStrRepartoFesONott(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 13));
    } else {
        m_casiStrRepaFesONott++;
        painter.drawText(getRect(row, 13), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printStrRepartoFesENott(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 12));
    } else {
        m_casiStrRepaFesENott++;
        painter.drawText(getRect(row, 12), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printRepNumTurni(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 5));
    } else {
        m_casiRepeTurni++;
        painter.drawText(getRect(row, 5), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printRepNumOre(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 4));
    } else {
        m_casiRepeOre++;
        painter.drawText(getRect(row, 4), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumGuarNott(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 3));
    } else {
        m_casiGuarNott++;
        painter.drawText(getRect(row, 3), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumGfFesNott(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 2));
    } else {
        m_casiGranFest++;
        painter.drawText(getRect(row, 2), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumOreGuarFesENot(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 6));
    } else {
        m_casiStrGuarFesENott++;
        painter.drawText(getRect(row, 6), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumOreGuarFesONot(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 7));
    } else {
        m_casiStrGuarFesONott++;
        painter.drawText(getRect(row, 7), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumOreGuarOrd(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 8));
    } else {
        m_casiStrGuarOrd++;
        painter.drawText(getRect(row, 8), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumOreRepFesENot(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 9));
    } else {
        m_casiStrRepeFesENott++;
        painter.drawText(getRect(row, 9), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumOreRepFesONot(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 10));
    } else {
        m_casiStrRepeFesONott++;
        painter.drawText(getRect(row, 10), Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
    painter.restore();
}

void CompetenzeUnitaExporter::printNumOreRepOrd(QPainter &painter, const QString &text, int row)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(badgeFont());
    if(text.isEmpty() || text == "//") {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRect(getRect(row, 11));
    } else {
        m_casiStrRepeOrd++;
        painter.drawText(getRect(row, 11), Qt::AlignHCenter | Qt::AlignVCenter, text);
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
    painter.drawText(getRect(m_totalRows, 3), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiGuarNott));
    painter.drawText(getRect(m_totalRows, 2), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(m_casiGranFest));

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

void CompetenzeUnitaExporter::printData(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(headerLightFont());
    painter.rotate(-90);
//    painter.setFont(headerFont());
    painter.drawText(QRect(-m_gridHeight*m_totalHeaderHeight, m_tableWidth, m_gridHeight*m_thirdHeaderHeight, m_gridWidth), Qt::AlignCenter, QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss"));
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

    return QRect(m_tableWidth-m_gridWidth*column+xOffeset,
                 m_gridHeight*m_totalHeaderHeight+(row*m_gridHeight)+yOffeset,
                 m_gridWidth-widthStretch,
                 m_gridHeight-heightStretch);
}
