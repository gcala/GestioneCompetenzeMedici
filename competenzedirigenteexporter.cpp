/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017-2019 Giuseppe Calà <giuseppe.cala@mailbox.org>
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

#include "competenzedirigenteexporter.h"
#include "sqlqueries.h"
#include "competenza.h"
#include "utilities.h"
#include "sqldatabasemanager.h"

#include <QDate>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QPdfWriter>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QSqlError>

CompetenzeDirigenteExporter::CompetenzeDirigenteExporter(QObject *parent)
    : QThread(parent)
    , m_maxPageWidth(8850)
    , m_rowHeight(350)
    , m_assenzeVOffset(2000)
    , m_boxVOffset(3900)
    , m_boxHeight(3000)
    , m_boxSpacing(100)
    , m_riepilogoVOffset(10100)
{
    m_idUnita = -1;
    m_tableName.clear();
    m_idDirigente = -1;
}

CompetenzeDirigenteExporter::~CompetenzeDirigenteExporter()
= default;

void CompetenzeDirigenteExporter::setPath(const QString &path)
{
    m_path = path;
}

void CompetenzeDirigenteExporter::setUnita(int id)
{
    m_idUnita = id;
}

void CompetenzeDirigenteExporter::setTable(const QString &tableName)
{
    m_tableName = tableName;
}

void CompetenzeDirigenteExporter::setDirigente(int id)
{
    m_idDirigente = id;
}

void CompetenzeDirigenteExporter::run()
{
    Utilities::m_connectionName = "CompetenzeDirigenteExporter";

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
    const QString s = m_tableName.split("_").last();
    m_mese = QLocale().monthName(s.rightRef(2).toInt()) + " " + s.left(4);
    QString fileName = "Competenze_" + QString(m_mese).replace(" ","_");
    QString dipName;

    int rowCount = 1;

    if(m_idDirigente != -1) {
        emit totalRows(rowCount);
        // stampo un singolo report
        const QPair<int, QString> values = SqlQueries::getMatricolaNome(m_idDirigente);
        dipName = QString::number(values.first) + " - " + values.second;
        fileName += "_" + QString::number(values.first) + "_" + values.second;
    } else {
        if(m_idUnita != -1) {
            // stampo tutti i medici di una singola unità
            unitaIdList << m_idUnita;
            fileName += "_" + QString::number(m_idUnita) + "_" + SqlQueries::getUnitaNomeBreve(m_idUnita);

            // recupero numero totale medici di data unita nel cartellino del mese
            rowCount = SqlQueries::numDoctorsFromUnitInTimecard(m_tableName, m_idUnita);

            emit totalRows(rowCount);
        } else {
            // stampo tutti medici di tutte unità
            unitaIdList << SqlQueries::getUnitaIdsInTimecard(m_tableName);

            // recupero numero totale medici nel cartellino del mese
            rowCount = SqlQueries::numDoctorsInTimecard(m_tableName);

            emit totalRows(rowCount);
        }
    }

    fileName += ".pdf";

    QPdfWriter writer(m_path + QDir::separator() + fileName);

    writer.setPageSize(QPagedPaintDevice::A4);
    writer.setPageMargins(QMargins(30, 30, 30, 30));
    writer.setCreator("Gestione Competenze Medici");

    QPainter painter(&writer);
    painter.setRenderHint(QPainter::Antialiasing, true);

    bool isFileStart = true;

    int currRow = 0;

    if(m_idDirigente != -1) {
        currRow++;
        emit currentRow(currRow);
        m_idUnita = SqlQueries::getDoctorUnitaIdFromTimecard(m_tableName, m_idDirigente);

        m_unitaName = SqlQueries::getUnitaNomeCompleto(m_idUnita);
        m_competenza = new Competenza(m_tableName,m_idDirigente);

        // stampo un singolo report
        printDirigente(painter);
    } else {
        foreach (int unitaId, unitaIdList) {
            m_unitaName = SqlQueries::getUnitaNomeCompleto(unitaId);
            m_idUnita = unitaId;
            QVector<int> dirigentiIdList = SqlQueries::getDoctorsIdsFromUnitInTimecard(m_tableName, unitaId);

            foreach (int dirigenteId, dirigentiIdList) {
                currRow++;
                emit currentRow(currRow);
                if(!isFileStart)
                    writer.newPage();
                m_competenza = new Competenza(m_tableName,dirigenteId);
                printDirigente(painter);
                isFileStart = false;
            }
        }
    }

    emit exportFinished(m_path + QDir::separator() + fileName);
}

void CompetenzeDirigenteExporter::printDirigente(QPainter &painter)
{
    disegnaScheletro(painter);
    printMonth(painter, m_mese);
    printUnitaName(painter, m_unitaName);
    printUnitaNumber(painter, QString::number(m_idUnita));
    printName(painter, m_competenza->badgeNumber() + " - " + m_competenza->name());
    printGiorniLavorati(painter);
    printFerie(painter);
    printCongedi(painter);
    printMalattia(painter);
    printRmp(painter);
    printRmc(painter);
    printGuardieDiurne(painter);
    printGuardieNotturne(painter);
    printDistribuzioneOreGuardia(painter);
    printDistribuzioneOreReperibilita(painter);
    printTurniProntaDisponibilita(painter);
    printOreLavoroDovute(painter);
    printOreLavoroEffettuate(painter);
    printNettoOre(painter);
    printOreStraordinarioGuardie(painter);
    printOreProntaDisponibilita(painter);
    printRep(painter);
    printDeficit(painter);
    printNotte(painter);
    printFestivo(painter);
}

void CompetenzeDirigenteExporter::disegnaScheletro(QPainter &painter)
{
    // disegno banner
    QRectF target(1300.0, 0.0, m_maxPageWidth-2400, 1000.0);
    QImage image(":/images/sc_header.png");
    painter.drawImage(target, image);

    painter.setPen(Qt::black);
    painter.setFont(bodyFont());

    // riquadro guardie
    painter.save();
    painter.setPen(QPen(Qt::black, 30, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0,m_boxVOffset,m_maxPageWidth,m_boxHeight));
    painter.restore();

    painter.setFont(sectionFont());
    painter.drawText(QRect(0,m_boxVOffset+m_boxSpacing,m_maxPageWidth,m_rowHeight+30), Qt::AlignCenter | Qt::AlignVCenter, "GUARDIE");

    painter.setFont(bodyFont());
    painter.drawText(QRect(50,m_boxVOffset+m_boxHeight/3-250,5000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "diurne");
    painter.drawText(QRect(2000,m_boxVOffset+m_boxHeight/3+200,2000,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, "_____________________________");
    painter.drawText(QRect(50,m_boxVOffset+m_boxHeight/3*2-100,5000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "notturne");

    // riquadro reperibilità
    painter.save();
    painter.setPen(QPen(Qt::black, 30, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0,m_boxVOffset+m_boxHeight+m_boxSpacing,m_maxPageWidth,m_boxHeight));
    painter.restore();

    painter.setFont(sectionFont());
    painter.drawText(QRect(0,m_boxVOffset+m_boxHeight+m_boxSpacing*2-40,m_maxPageWidth,m_rowHeight+30+40), Qt::AlignCenter | Qt::AlignVCenter, "REPERIBILITÀ");

    painter.setFont(bodyFont());
    painter.drawText(QRect(50,m_boxVOffset+m_boxSpacing+m_boxHeight/2-250+m_boxHeight,5000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "turni");

    painter.setFont(bodyFont());    
    painter.drawText(QRect(0,m_riepilogoVOffset+(4*m_rowHeight),6200,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "lavoro straordinario per altri motivi documentato ore");
    painter.drawText(QRect(5200,m_riepilogoVOffset+(6*m_rowHeight),2000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Mensa");
}

QFont CompetenzeDirigenteExporter::numberFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(340);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeDirigenteExporter::unitaFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(180);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeDirigenteExporter::meseFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(200);
    font.setBold(true);
    font.setFamily("Sans Serif");
    font.setUnderline(true);
    return font;
}

QFont CompetenzeDirigenteExporter::bodyFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(250);
    font.setBold(false);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeDirigenteExporter::bodyFontBold()
{
    QFont font(bodyFont());
    font.setBold(true);
    return font;
}

QFont CompetenzeDirigenteExporter::totalsFont()
{
    QFont font(bodyFontBold());
    font.setPixelSize(280);
    return font;
}

QFont CompetenzeDirigenteExporter::sectionFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(300);
    font.setBold(true);
    font.setUnderline(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeDirigenteExporter::nameFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(320);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

void CompetenzeDirigenteExporter::printMonth(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(meseFont());
    painter.drawText(QRect(0, 1100, m_maxPageWidth,220), Qt::AlignRight | Qt::AlignVCenter, text);
    painter.restore();
}

void CompetenzeDirigenteExporter::printUnitaName(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(meseFont());
    painter.drawText(QRect(650, 1100, 7000,220), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.restore();
}

void CompetenzeDirigenteExporter::printUnitaNumber(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(QPen(Qt::black, 25, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.setFont(numberFont());
    painter.drawText(QRect(0,1100,600,600), Qt::AlignCenter, text);
    painter.drawRect(QRect(0,1100,600,600));
    painter.restore();
}

void CompetenzeDirigenteExporter::printName(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(nameFont());
    painter.drawText(QRect(650, 1300,m_maxPageWidth-650,400), Qt::AlignCenter | Qt::AlignVCenter, text);
    painter.restore();
}

void CompetenzeDirigenteExporter::printGiorniLavorati(QPainter &painter)
{
    const int vOffset = m_assenzeVOffset+(0*m_rowHeight);
    const QString text = "Giorni Lavorati:";

    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 50;

    painter.drawText(QRect(0,vOffset,4000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.setFont(bodyFontBold());
    painter.drawText(QRect(sectionWidth,vOffset,m_maxPageWidth-sectionWidth,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->giorniLavorativi() + " - " + m_competenza->assenzeTotali() + " = " + m_competenza->giorniLavorati());
    painter.restore();
}

void CompetenzeDirigenteExporter::printFerie(QPainter &painter)
{
    const int vOffset = m_assenzeVOffset+(1*m_rowHeight);
    const QString text = "FERIE:";

    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 50;

    painter.drawText(QRect(0,vOffset,4000,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, text);
    painter.setFont(bodyFontBold());

    QAbstractTextDocumentLayout::PaintContext context;
    QTextDocument doc;
    doc.setDefaultFont(bodyFontBold());

    QList<QDate> dates = m_competenza->ferieDates();
    QStringList list;

    foreach (QDate d, dates) {
        list << QString::number(d.day());
    }

    doc.setHtml(list.join(" - "));
    const double ratio = doc.documentLayout()->documentSize().width() / (m_maxPageWidth-sectionWidth);
    painter.translate(sectionWidth,vOffset+50);
    if(ratio > 1.0)
        painter.scale(1.0/ratio,1.0);
    doc.documentLayout()->draw(&painter, context);

    painter.restore();
}

void CompetenzeDirigenteExporter::printCongedi(QPainter &painter)
{
    const int vOffset = m_assenzeVOffset+(2*m_rowHeight);
    const QString text = "C.S.:";

    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 50;

    painter.drawText(QRect(0,vOffset,4000,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, text);
    painter.setFont(bodyFontBold());

    QAbstractTextDocumentLayout::PaintContext context;
    QTextDocument doc;
    doc.setDefaultFont(bodyFontBold());

    QList<QDate> dates = m_competenza->congediDates();
    QStringList list;

    foreach (QDate d, dates) {
        list << QString::number(d.day());
    }

    doc.setHtml(list.join(" - "));
    const double ratio = doc.documentLayout()->documentSize().width() / (m_maxPageWidth-sectionWidth);
    painter.translate(sectionWidth,vOffset+50);
    if(ratio > 1.0)
        painter.scale(1.0/ratio,1.0);
    doc.documentLayout()->draw(&painter, context);

    painter.restore();
}

void CompetenzeDirigenteExporter::printMalattia(QPainter &painter)
{
    const int vOffset = m_assenzeVOffset+(3*m_rowHeight);
    const QString text = "Malattia:";

    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 50;

    painter.drawText(QRect(0,vOffset,4000,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, text);
    painter.setFont(bodyFontBold());

    QAbstractTextDocumentLayout::PaintContext context;
    QTextDocument doc;
    doc.setDefaultFont(bodyFontBold());

    QList<QDate> dates = m_competenza->malattiaDates();
    QStringList list;

    foreach (QDate d, dates) {
        list << QString::number(d.day());
    }

    doc.setHtml(list.join(" - "));
    const double ratio = doc.documentLayout()->documentSize().width() / (m_maxPageWidth-sectionWidth);
    painter.translate(sectionWidth,vOffset+50);
    if(ratio > 1.0)
        painter.scale(1.0/ratio,1.0);
    doc.documentLayout()->draw(&painter, context);

    painter.restore();
}

void CompetenzeDirigenteExporter::printRmp(QPainter &painter)
{
    const int vOffset = m_assenzeVOffset+(4*m_rowHeight);
    const QString text = "RMP:";

    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 50;

    painter.drawText(QRect(0,vOffset,m_maxPageWidth/2-50,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, text);
    painter.setFont(bodyFontBold());

    QAbstractTextDocumentLayout::PaintContext context;
    QTextDocument doc;
    doc.setDefaultFont(bodyFontBold());

    QList<QDate> dates = m_competenza->rmpDates();
    QStringList list;

    foreach (QDate d, dates) {
        list << QString::number(d.day());
    }

    doc.setHtml(list.join(" - "));
    const double ratio = doc.documentLayout()->documentSize().width() / (m_maxPageWidth/2-sectionWidth);
    painter.translate(sectionWidth,vOffset+50);
    if(ratio > 1.0)
        painter.scale(1.0/ratio,1.0);
    doc.documentLayout()->draw(&painter, context);

    painter.restore();
}

void CompetenzeDirigenteExporter::printRmc(QPainter &painter)
{
    const int vOffset = m_assenzeVOffset+(4*m_rowHeight);
    const QString text = "RMC:";

    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 50;

    painter.drawText(QRect(m_maxPageWidth/2,vOffset+50,m_maxPageWidth/2,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.setFont(bodyFontBold());

    QAbstractTextDocumentLayout::PaintContext context;
    QTextDocument doc;
    doc.setDefaultFont(bodyFontBold());

    QList<QDate> dates = m_competenza->rmcDates();
    QStringList list;

    foreach (QDate d, dates) {
        list << QString::number(d.day());
    }

    doc.setHtml(list.join(" - "));
    const double ratio = doc.documentLayout()->documentSize().width() / (m_maxPageWidth/2-sectionWidth);
    painter.translate(m_maxPageWidth/2+sectionWidth,vOffset+100);
    if(ratio > 1.0)
        painter.scale(1.0/ratio,1.0);
    doc.documentLayout()->draw(&painter, context);

    painter.restore();
}

void CompetenzeDirigenteExporter::printAltreAssenze(QPainter &painter)
{
    const int vOffset = m_assenzeVOffset+(6*m_rowHeight);
    const QString text = "Altre assenze:";

    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 40;

    painter.drawText(QRect(0,vOffset,4000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.setFont(bodyFontBold());

    QAbstractTextDocumentLayout::PaintContext context;
    QTextDocument doc;
    doc.setDefaultFont(bodyFontBold());

    QList<QDate> dates = m_competenza->altreAssenzeDates();
    QStringList list;

    foreach (QDate d, dates) {
        list << QString::number(d.day());
    }

    doc.setHtml(list.join(" - "));
    const double ratio = doc.documentLayout()->documentSize().width() / (m_maxPageWidth-sectionWidth);
    painter.translate(sectionWidth,vOffset+50);
    if(ratio > 1.0)
        painter.scale(1.0/ratio,1.0);
    doc.documentLayout()->draw(&painter, context);

    painter.restore();
}

void CompetenzeDirigenteExporter::printGuardieDiurne(QPainter &painter)
{
    const int cellSize = 500;
    const int spacing = 80;

    int elementiPerRiga = 7;
    int numRows = 1;

    if(m_competenza->guardiaDiurnaMap().count() > 14)
        elementiPerRiga = (m_competenza->guardiaDiurnaMap().count()%2 == 0 ? m_competenza->guardiaDiurnaMap().count()/2 : m_competenza->guardiaDiurnaMap().count()/2+1);

    if(m_competenza->guardiaDiurnaMap().count() > 7)
        numRows = 2;

    QPixmap pix(cellSize*elementiPerRiga+(elementiPerRiga-1)*spacing,numRows*cellSize + (numRows == 0 ? 0 : spacing));
    pix.fill(Qt::white);
    QPainter p(&pix);

    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(15);

    QFont font = painter.font();
    font.setPixelSize(300);
    font.setBold(true);
    p.setFont(font);
    p.setPen(pen);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setBrush(Qt::NoBrush);

    int counter = 0;
    int row = 0;
    int column = 0;
    bool inizio = true;

    QMap<int, GuardiaType>::const_iterator i = m_competenza->guardiaDiurnaMap().constBegin();
    while (i != m_competenza->guardiaDiurnaMap().constEnd()) {
        if(counter < elementiPerRiga)
            row = 0;
        else
            row = 1;

        p.save();
        p.translate(column*cellSize,cellSize*row + (row == 0 ? 0 : spacing));
        if(!inizio)
            p.translate(spacing*column,0);

        QRect rect(0,0,cellSize,cellSize);
        p.drawText(rect,Qt::AlignCenter | Qt::AlignVCenter,QString::number(i.key()));
        p.drawEllipse(rect.adjusted(15,15,-15,-15));
        counter++;
        if(counter == elementiPerRiga) {
            inizio = true;
            column = 0;
        } else {
            inizio = false;
            column++;
        }
        i++;
        p.restore();
    }

    const int offset = numRows == 1 ? 350 : 400;

    painter.save();
    painter.drawPixmap(1200,m_boxVOffset+m_boxHeight/3-offset,pix.width() > 3980 ? pix.scaledToWidth(3980, Qt::SmoothTransformation) : pix);
    painter.restore();
}

void CompetenzeDirigenteExporter::printGuardieNotturne(QPainter &painter)
{
    const int cellSize = 500;
    const int spacing = 80;

    int elementiPerRiga = 7;
    int numRows = 1;

    if(m_competenza->guardiaNotturnaMap().count() > 14)
        elementiPerRiga = (m_competenza->guardiaNotturnaMap().count()%2 == 0 ? m_competenza->guardiaNotturnaMap().count()/2 : m_competenza->guardiaNotturnaMap().count()/2+1);

    if(m_competenza->guardiaNotturnaMap().count() > 7)
        numRows = 2;

    QPixmap pix(cellSize*elementiPerRiga+(elementiPerRiga-1)*spacing,numRows*cellSize + (numRows == 0 ? 0 : spacing));
    pix.fill(Qt::white);
    QPainter p(&pix);

    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(15);

    QFont font = painter.font();
    font.setPixelSize(300);
    font.setBold(true);
    p.setFont(font);
    p.setPen(pen);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setBrush(Qt::NoBrush);

    int counter = 0;
    int row = 0;
    int column = 0;
    bool inizio = true;

    QMap<int, GuardiaType>::const_iterator i = m_competenza->guardiaNotturnaMap().constBegin();
    while (i != m_competenza->guardiaNotturnaMap().constEnd()) {
        if(counter < elementiPerRiga)
            row = 0;
        else
            row = 1;

        p.save();
        p.translate(column*cellSize,cellSize*row + (row == 0 ? 0 : spacing));
        if(!inizio)
            p.translate(spacing*column,0);

        QRect rect(0,0,cellSize,cellSize);
        p.drawText(rect,Qt::AlignCenter | Qt::AlignBottom,QString::number(i.key()));
        switch (i.value()) {
        case GuardiaType::Sabato:
            p.drawText(rect,Qt::AlignHCenter | Qt::AlignTop,"*");
            break;
        case GuardiaType::Domenica:
            p.drawEllipse(rect.adjusted(60,120,-60,0));
            break;
        case GuardiaType::GrandeFestivita:
            p.drawText(rect,Qt::AlignHCenter | Qt::AlignTop,"*");
            p.drawRect(rect.adjusted(45,180,-45,-45));
            break;
        default:
            break;
        }

        counter++;
        if(counter == elementiPerRiga) {
            inizio = true;
            column = 0;
        } else {
            inizio = false;
            column++;
        }
        i++;
        p.restore();
    }

    const int offset = numRows == 1 ? 250 : 450;

    painter.save();
    painter.drawPixmap(1200,m_boxVOffset+m_boxHeight/3*2-offset,pix.width() > 3980 ? pix.scaledToWidth(3980, Qt::SmoothTransformation) : pix);
    painter.restore();
}

void CompetenzeDirigenteExporter::printDistribuzioneOreGuardia(QPainter &painter)
{
    painter.save();
    painter.setFont(bodyFont());
    painter.drawText(QRect(5300,m_boxVOffset+m_boxHeight/5-100,1700,m_rowHeight), Qt::AlignRight | Qt::AlignVCenter, "feriali d. ore");
    painter.drawText(QRect(7050,m_boxVOffset+m_boxHeight/5-100,1300,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "______________________");

    painter.drawText(QRect(5300,m_boxVOffset+m_boxHeight/5*2-150,1700,m_rowHeight), Qt::AlignRight | Qt::AlignVCenter, "festive d. ore");
    painter.drawText(QRect(7050,m_boxVOffset+m_boxHeight/5*2-150,1300,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "______________________");

    painter.drawText(QRect(5300,m_boxVOffset+m_boxHeight/5*3-100,1700,m_rowHeight), Qt::AlignRight | Qt::AlignVCenter, "feriali n. ore");
    painter.drawText(QRect(7050,m_boxVOffset+m_boxHeight/5*3-100,1300,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "_______________________");

    painter.drawText(QRect(5300,m_boxVOffset+m_boxHeight/5*4-150,1700,m_rowHeight), Qt::AlignRight | Qt::AlignVCenter, "festive n. ore");
    painter.drawText(QRect(7050,m_boxVOffset+m_boxHeight/5*4-150,1300,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "_______________________");

    painter.setFont(bodyFontBold());
    painter.drawText(QRect(7700,m_boxVOffset+m_boxHeight-350,1000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "T.");
    painter.drawText(QRect(8050,m_boxVOffset+m_boxHeight-350,800,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "_______________________");
    painter.restore();

    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    QStringList list;
    list << QString::number(m_competenza->g_d_fer_F()) << QString::number(m_competenza->g_d_fer_S()) << QString::number(m_competenza->g_d_fer_D());
    list.removeAll("0");

    painter.drawText(QRect(7050,m_boxVOffset+m_boxHeight/5-100,1300,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, list.join("+"));

    list.clear();
    list << QString::number(m_competenza->g_d_fes_F()) << QString::number(m_competenza->g_d_fes_S()) << QString::number(m_competenza->g_d_fes_D());
    list.removeAll("0");

    painter.drawText(QRect(7050,m_boxVOffset+m_boxHeight/5*2-150,1300,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, list.join("+"));

    list.clear();
    list << QString::number(m_competenza->g_n_fer_F()) << QString::number(m_competenza->g_n_fer_S()) << QString::number(m_competenza->g_n_fer_D());
    list.removeAll("0");

    painter.drawText(QRect(7050,m_boxVOffset+m_boxHeight/5*3-100,1300,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, list.join("+"));

    list.clear();
    list << QString::number(m_competenza->g_n_fes_F()) << QString::number(m_competenza->g_n_fes_S()) << QString::number(m_competenza->g_n_fes_D());
    list.removeAll("0");

    painter.drawText(QRect(7050,m_boxVOffset+m_boxHeight/5*4-150,1300,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, list.join("+"));

    painter.setFont(totalsFont());

    if(m_competenza->totOreGuardie() != 0)
        painter.drawText(QRect(8050,m_boxVOffset+m_boxHeight-350,800,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, QString::number(m_competenza->totOreGuardie()));

    painter.restore();
}

void CompetenzeDirigenteExporter::printDistribuzioneOreReperibilita(QPainter &painter)
{
    painter.save();
    painter.setFont(bodyFont());
    painter.drawText(QRect(5300,m_boxVOffset+m_boxSpacing+m_boxHeight/5-100+m_boxHeight,1700,m_rowHeight), Qt::AlignRight | Qt::AlignVCenter, "feriali d. ore");
    painter.drawText(QRect(7050,m_boxVOffset+m_boxSpacing+m_boxHeight/5-100+m_boxHeight,1300,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "______________________");

    painter.drawText(QRect(5300,m_boxVOffset+m_boxSpacing+m_boxHeight/5*2-150+m_boxHeight,1700,m_rowHeight), Qt::AlignRight | Qt::AlignVCenter, "festive d. ore");
    painter.drawText(QRect(7050,m_boxVOffset+m_boxSpacing+m_boxHeight/5*2-150+m_boxHeight,1300,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "______________________");

    painter.drawText(QRect(5300,m_boxVOffset+m_boxSpacing+m_boxHeight/5*3-100+m_boxHeight,1700,m_rowHeight), Qt::AlignRight | Qt::AlignVCenter, "feriali n. ore");
    painter.drawText(QRect(7050,m_boxVOffset+m_boxSpacing+m_boxHeight/5*3-100+m_boxHeight,1300,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "_______________________");

    painter.drawText(QRect(5300,m_boxVOffset+m_boxSpacing+m_boxHeight/5*4-150+m_boxHeight,1700,m_rowHeight), Qt::AlignRight | Qt::AlignVCenter, "festive n. ore");
    painter.drawText(QRect(7050,m_boxVOffset+m_boxSpacing+m_boxHeight/5*4-150+m_boxHeight,1300,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "_______________________");

    painter.setFont(bodyFontBold());
    painter.drawText(QRect(7700,m_boxVOffset+m_boxHeight*2+m_boxSpacing-350,1000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "T.");
    painter.drawText(QRect(8050,m_boxVOffset+m_boxHeight*2+m_boxSpacing-350,800,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "_______________________");
    painter.restore();

    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    if(m_competenza->r_d_fer() != 0)
        painter.drawText(QRect(7050,m_boxVOffset+m_boxSpacing+m_boxHeight/5-100+m_boxHeight,1300,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, Utilities::inOrario(m_competenza->r_d_fer()));
    if(m_competenza->r_d_fes() != 0)
        painter.drawText(QRect(7050,m_boxVOffset+m_boxSpacing+m_boxHeight/5*2-150+m_boxHeight,1300,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, Utilities::inOrario(m_competenza->r_d_fes()));
    if(m_competenza->r_n_fer() != 0)
        painter.drawText(QRect(7050,m_boxVOffset+m_boxSpacing+m_boxHeight/5*3-100+m_boxHeight,1300,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, Utilities::inOrario(m_competenza->r_n_fer()));
    if(m_competenza->r_n_fes() != 0)
        painter.drawText(QRect(7050,m_boxVOffset+m_boxSpacing+m_boxHeight/5*4-150+m_boxHeight,1300,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, Utilities::inOrario(m_competenza->r_n_fes()));
    painter.setFont(totalsFont());

    if(m_competenza->oreGrep() != "0")
        painter.drawText(QRect(8050,m_boxVOffset+m_boxHeight*2+m_boxSpacing-350,800,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, m_competenza->oreGrep());
    painter.restore();
}

void CompetenzeDirigenteExporter::printTurniProntaDisponibilita(QPainter &painter)
{
    const int cellSize = 500;
    const int spacing = 80;
    const int elementiPerRiga = 7;
    int numRows;
    if(m_competenza->rep().count() <= 7)
        numRows = 1;
    else if(m_competenza->rep().count() <=14)
        numRows = 2;
    else if(m_competenza->rep().count() <=21)
        numRows = 3;
    else if(m_competenza->rep().count() <=28)
        numRows = 4;
    else
        numRows = 5;


    QPixmap pix(cellSize*elementiPerRiga+(elementiPerRiga-1)*spacing,numRows*cellSize);
    pix.fill(Qt::white);
    QPainter p(&pix);
    bool inizio = true;

    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(15);

    QFont font = painter.font();
    font.setPixelSize(300);
    font.setBold(true);

    p.setFont(font);
    p.setPen(pen);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setBrush(Qt::NoBrush);

    QFont fontApice;
    fontApice.setPixelSize(200);
    fontApice.setBold(false);

    int counter = 0;
    int row = 0;
    int column = 0;

    QMap<QDate, ValoreRep>::const_iterator i = m_competenza->rep().constBegin();
    while (i != m_competenza->rep().constEnd()) {
        if(counter < 7)
            row = 0;
        else if(counter < 14)
            row = 1;
        else if(counter < 21)
            row = 2;
        else if(counter < 28)
            row = 3;
        else
            row = 4;

        if(counter % 7 == 0) {
            inizio = true;
            column = 0;
        }

        p.save();
        p.translate(column*cellSize,cellSize*row);
        if(!inizio)
            p.translate(spacing*column,0);

        QRect rect(0,0,cellSize,cellSize);
        QDate d = i.key();
        p.drawText(rect,Qt::AlignCenter | Qt::AlignBottom,QString::number(d.day()));
        switch (i.value()) {
        case ValoreRep::Mezzo: {
            p.setFont(fontApice);
            p.drawText(rect,Qt::AlignRight | Qt::AlignTop,"½");
            break;}
        case ValoreRep::Uno:
            break;
        case ValoreRep::UnoMezzo:
            p.drawLine(rect.x()+8,rect.y()+rect.height()-8,rect.x()+rect.width()-8,rect.y()+rect.height()-8);
            break;
        case ValoreRep::Due:
            p.drawEllipse(rect.adjusted(60,120,-60,0));
            break;
        case ValoreRep::DueMezzo: {
            p.setFont(fontApice);
            p.drawEllipse(rect.adjusted(60,120,-60,0));
            p.drawText(rect,Qt::AlignRight | Qt::AlignTop,"½");
            break;}
        default:
            break;
        }
        counter++;
        i++;
        inizio = false;
        column++;
        p.restore();
    }

    int offset = 0;
    switch (numRows) {
    case 1:
        offset = 400;
        break;
    case 2:
        offset = 550;
        break;
    case 3:
        offset = 800;
        break;
    default:
        offset = 1100;
        break;
    }

    painter.save();
    painter.drawPixmap(1200,m_boxVOffset+m_boxSpacing+m_boxHeight/2-offset+m_boxHeight,pix);
    painter.restore();
}

void CompetenzeDirigenteExporter::printOreLavoroDovute(QPainter &painter)
{
    const int vOffset = m_riepilogoVOffset+(0*m_rowHeight);
    const QString text = "Ore di lavoro dovute";

    painter.save();
    painter.setPen(Qt::black);

    painter.setFont(bodyFont());
    painter.drawText(QRect(0,vOffset,3000,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, text);

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 100;

    painter.setFont(bodyFontBold());
    painter.drawText(QRect(0+sectionWidth,vOffset,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, m_competenza->oreDovute());
    painter.restore();
}

void CompetenzeDirigenteExporter::printOreLavoroEffettuate(QPainter &painter)
{
    const int vOffset = m_riepilogoVOffset+(1*m_rowHeight);
    const QString text = "Ore di lavoro effettuate";

    painter.save();
    painter.setPen(Qt::black);

    painter.setFont(bodyFont());
    painter.drawText(QRect(0,vOffset,3000,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, text);

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 100;

    painter.setFont(bodyFontBold());
    painter.drawText(QRect(0+sectionWidth,vOffset,6200,m_rowHeight), Qt::AlignBottom | Qt::AlignVCenter, m_competenza->oreEffettuate());
    painter.restore();
}

void CompetenzeDirigenteExporter::printNettoOre(QPainter &painter)
{
    const int vOffset = m_riepilogoVOffset+(0.5*m_rowHeight);

    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    QString text = m_competenza->differenzaOreSenzaDmp();

    if(m_competenza->dmp() != 0) {
        text += " - d.m.p. " + QString::number(m_competenza->dmp()/60) + ":" + QString::number(m_competenza->dmp()%60) + " = " + m_competenza->differenzaOre();
    }

    painter.drawText(QRect(4100,vOffset,5000,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, text);
    painter.restore();
}

void CompetenzeDirigenteExporter::printOreStraordinarioGuardie(QPainter &painter)
{
    const int vOffset = m_riepilogoVOffset+(2*m_rowHeight);
    const QString text = "Lavoro straordinario per guardia ore";

    painter.save();
    painter.setPen(Qt::black);

    painter.setFont(bodyFont());
    painter.drawText(QRect(0,vOffset,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, text);

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 100;

    painter.setFont(bodyFontBold());
    painter.drawText(QRect(0+sectionWidth,vOffset,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, m_competenza->oreStraordinarioGuardie());
    painter.restore();
}

void CompetenzeDirigenteExporter::printOreProntaDisponibilita(QPainter &painter)
{
    const int vOffset = m_riepilogoVOffset+(3*m_rowHeight);
    const QString text = "Lavoro straordinario in pronta disponibilità ore";

    painter.save();
    painter.setPen(Qt::black);

    painter.drawText(QRect(0,vOffset,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, text);

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 100;

    painter.setFont(bodyFontBold());
    painter.drawText(QRect(0+sectionWidth,vOffset,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, QString::number(m_competenza->oreProntaDisp()));
    painter.restore();
}

void CompetenzeDirigenteExporter::printRep(QPainter &painter)
{
    const int vOffset = m_riepilogoVOffset+(5*m_rowHeight);
    const QString text = "Rep";

    painter.save();
    painter.setPen(Qt::black);

    painter.setFont(bodyFont());
    painter.drawText(QRect(0,vOffset,2000,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, text);

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width("Rep") + 100;

    painter.setFont(bodyFontBold());
    painter.drawText(QRect(0+sectionWidth,vOffset,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, m_competenza->repCount());
    painter.restore();
}

void CompetenzeDirigenteExporter::printDeficit(QPainter &painter)
{
    const int vOffset = m_riepilogoVOffset+(5*m_rowHeight);
    const QString text = "Deficit orario";

    painter.save();
    painter.setPen(Qt::black);

    painter.setFont(bodyFont());
    painter.drawText(QRect(2100,vOffset,2000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 100;

    painter.setFont(bodyFontBold());
    painter.drawText(QRect(2100 + sectionWidth,vOffset,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, m_competenza->deficitOrario());
    painter.restore();
}

void CompetenzeDirigenteExporter::printNotte(QPainter &painter)
{
    const int vOffset = m_riepilogoVOffset+(6*m_rowHeight);
    const QString text = "Notte";

    painter.save();
    painter.setPen(Qt::black);

    painter.setFont(bodyFont());
    painter.drawText(QRect(0,vOffset,2000,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, text);

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 100;

    painter.setFont(bodyFontBold());
    painter.drawText(QRect(0+sectionWidth,vOffset,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, m_competenza->notte() > 0 ? QString::number(m_competenza->notte()) : "//");
    painter.restore();
}

void CompetenzeDirigenteExporter::printFestivo(QPainter &painter)
{
    const int vOffset = m_riepilogoVOffset+(6*m_rowHeight);
    const QString text = "Festivo";

    painter.save();
    painter.setPen(Qt::black);

    painter.setFont(bodyFont());
    painter.drawText(QRect(2100,vOffset,2000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);

    QFontMetrics fm(bodyFont());
    const int sectionWidth = fm.width(text) + 100;

    painter.setFont(bodyFontBold());
    painter.drawText(QRect(2100+sectionWidth,vOffset,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignBottom, m_competenza->festivo() > 0 ? QString::number(m_competenza->festivo()) : "//");
    painter.restore();
}
