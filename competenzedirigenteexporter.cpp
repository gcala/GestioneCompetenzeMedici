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

#include "competenzedirigenteexporter.h"
#include "sqlqueries.h"
#include "competenza.h"

#include <QDate>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QPdfWriter>
#include <QPainter>

CompetenzeDirigenteExporter::CompetenzeDirigenteExporter(QObject *parent)
    : QThread(parent)
    , m_maxPageWidth(8850)
    , m_rowHeight(140)
    , m_assenzeVOffset(3200)
    , m_lineSpacing(300)
{
    m_idUnita = -1;
    m_idMese = -1;
    m_idDirigente = -1;
}

CompetenzeDirigenteExporter::~CompetenzeDirigenteExporter()
{

}

void CompetenzeDirigenteExporter::setPath(const QString &path)
{
    m_path = path;
}

void CompetenzeDirigenteExporter::setUnita(int id)
{
    m_idUnita = id;
}

void CompetenzeDirigenteExporter::setMese(QString id)
{
    m_idMese = id;
}

void CompetenzeDirigenteExporter::setDirigente(int id)
{
    m_idDirigente = id;
}

void CompetenzeDirigenteExporter::run()
{
    QStringList unitaIdList;
    const QString s = m_idMese.split("_").last();
    m_mese = QDate::longMonthName(s.right(2).toInt(), QDate::StandaloneFormat) + " " + s.left(4);
    QString fileName = "Competenze_" + QString(m_mese).replace(" ","_");
    QString dipName;

    if(m_idDirigente != -1) {
        // stampo un singolo report
        QSqlQuery query;
        query.prepare("SELECT matricola,nome FROM medici WHERE id='" + QString::number(m_idDirigente) + "';");
        if(!query.exec()) {
            qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        }
        while(query.next()) {
            dipName = query.value(0).toString() + " - " + query.value(1).toString();
            fileName += "_" + query.value(0).toString() + "_" + query.value(1).toString();
        }
    } else {
        if(m_idUnita != -1) {
            // stampo tutti i medici di una singola unità
            unitaIdList << QString::number(m_idUnita);
            QSqlQuery query;
            query.prepare("SELECT id,nome_mini FROM unita WHERE id='" + QString::number(m_idUnita) + "';");
            if(!query.exec()) {
                qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
            }
            while(query.next()) {
                fileName += "_" + query.value(0).toString() + "_" + query.value(1).toString();
            }
        } else {
            // stampo tutti medici di tutte unità
            unitaIdList << getUnitaIDs(m_idMese);
        }
    }

    fileName += ".pdf";

    QPdfWriter writer(m_path + QDir::separator() + fileName);

    writer.setPageSize(QPagedPaintDevice::A4);
    writer.setPageMargins(QMargins(30, 30, 30, 30));

    QPainter painter(&writer);
    painter.setRenderHint(QPainter::Antialiasing, true);

    bool isFileStart = true;

    if(m_idDirigente != -1) {
        QSqlQuery query;
        query.prepare("SELECT id_unita FROM " + m_idMese + " WHERE id_medico='" + QString::number(m_idDirigente) + "';");
        if(!query.exec()) {
            qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        }
        while(query.next()) {
            m_idUnita = query.value(0).toInt();
        }

        m_unitaName = getUnitaName(QString::number(m_idUnita));
        m_competenza = new Competenza(m_idMese,m_idDirigente);

        // stampo un singolo report
        printDirigente(painter);
    } else {
        foreach (QString unitaId, unitaIdList) {
            m_unitaName = getUnitaName(unitaId);
            QStringList dirigentiIdList = getDirigentiIDs(unitaId);

            foreach (QString dirigenteId, dirigentiIdList) {
                if(!isFileStart)
                    writer.newPage();
                m_competenza = new Competenza(m_idMese,dirigenteId.toInt());
                printDirigente(painter);
                isFileStart = false;
            }
        }
    }

    emit exportFinished();
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
    printAltreAssenze(painter);
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
    QRectF target(0.0, 0.0, m_maxPageWidth, 2000.0);
    QImage image(":/images/sc_header.jpg");
    painter.drawImage(target, image);
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());

    painter.save();
    painter.setPen(QPen(Qt::black, 15, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0,5250,m_maxPageWidth,2300));
    painter.restore();

    painter.setFont(sectionFont());
    painter.drawText(QRect(100,5400,3000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Numero Guardie Effettuate:");

    painter.drawText(QRect(0,5700,6000,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, "Diurne");
    painter.drawText(QRect(0,6600,6000,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, "Notturne");

    painter.setFont(bodyFont());
    painter.drawText(QRect(6000,6000,1700,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Feriali d. .......................");
    painter.drawText(QRect(6000,6300,1700,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Festive d. ........................");
    painter.drawText(QRect(7800,6700,800,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "........................");
    painter.drawText(QRect(6000,6900,1700,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Feriali n. ........................");
    painter.drawText(QRect(6000,7200,1700,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Festive n. ........................");
//    painter.drawText(QRect(5700,5800,300,1200), Qt::AlignLeft | Qt::AlignVCenter, "{");

    painter.save();
    painter.setPen(QPen(Qt::black, 15, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0,7700,m_maxPageWidth,1600));
    painter.restore();

    painter.setFont(sectionFont());
    painter.drawText(QRect(0,8500,6000,m_rowHeight), Qt::AlignCenter | Qt::AlignVCenter, "Reperibilità");

    painter.setFont(bodyFont());
    painter.drawText(QRect(6000,7800,1700,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Feriali d. .......................");
    painter.drawText(QRect(6000,8100,1700,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Festive d. ........................");
    painter.drawText(QRect(7800,8500,800,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "........................");
    painter.drawText(QRect(6000,8700,1700,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Feriali n. ........................");
    painter.drawText(QRect(6000,9000,1700,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Festive n. ........................");
//    painter.drawText(QRect(5700,5800,300,1200), Qt::AlignLeft | Qt::AlignVCenter, "{");

    painter.setFont(sectionFont());
    painter.drawText(QRect(0,9500,5000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Numero di turni di pronta disponibilità effettuati:");
    painter.drawText(QRect(0,9800,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Giorni:");
    painter.setFont(bodyFont());
    painter.drawText(QRect(0,10100,3000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "ore di lavoro dovute......................................................................................................................");
    painter.drawText(QRect(0,10400,3000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "ore di lavoro effettuate......................................................................................................................");
    painter.drawText(QRect(0,10700,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "lavoro straordinario per guardia ore......................................................................................................................");
    painter.drawText(QRect(0,11000,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "lavoro straordinario in pronta disponibilità ore.....................................................................");
    painter.drawText(QRect(0,11300,6200,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "lavoro straordinario per altri motivi documentato ore.....................................................................");
    painter.drawText(QRect(0,11600,2000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Rep.....................................................................");
    painter.drawText(QRect(2100,11600,2000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Deficit orario.....................................................................");
    painter.drawText(QRect(0,11900,2000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Notte.....................................................................");
    painter.drawText(QRect(2100,11900,2000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Festivo.....................................................................");
    painter.drawText(QRect(4200,11900,2000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, "Mensa.....................................................................");
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
//    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeDirigenteExporter::meseFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(180);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeDirigenteExporter::bodyFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(150);
    font.setBold(false);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeDirigenteExporter::bodyFontBold()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(150);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeDirigenteExporter::sectionFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(150);
    font.setBold(true);
    font.setUnderline(true);
    font.setFamily("Sans Serif");
    return font;
}

QFont CompetenzeDirigenteExporter::nameFont()
{
    QFont font;
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(360);
    font.setBold(true);
    font.setFamily("Sans Serif");
    return font;
}


QString CompetenzeDirigenteExporter::getUnitaName(const QString &id)
{
    QString s;
    QSqlQuery query;
    query.prepare("SELECT nome_full FROM unita WHERE id='" + id + "';");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        s = query.value(0).toString();
    }

    return s;
}

QStringList CompetenzeDirigenteExporter::getDirigentiIDs(const QString &id)
{
    QStringList l;
    QSqlQuery query;
    query.prepare("SELECT id_medico FROM " + m_idMese + " WHERE id_unita='" + id + "';");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        l << query.value(0).toString();
    }

    l.removeDuplicates();
    return l;
}

QStringList CompetenzeDirigenteExporter::getUnitaIDs(const QString &id)
{
    QStringList l;
    QSqlQuery query;
    query.prepare("SELECT id_unita FROM " + id + " ORDER BY length(id_unita), id_unita;");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        l << query.value(0).toString();
    }

    l.removeDuplicates();
    return l;
}

void CompetenzeDirigenteExporter::printMonth(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(meseFont());
    painter.drawText(QRect(0, 2100, m_maxPageWidth,160), Qt::AlignRight | Qt::AlignVCenter, text);
    painter.restore();
}

void CompetenzeDirigenteExporter::printUnitaName(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(unitaFont());
    painter.drawText(QRect(650, 2100, 8250,150), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.restore();
}

void CompetenzeDirigenteExporter::printUnitaNumber(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(QPen(Qt::black, 25, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.setFont(numberFont());
    painter.drawText(QRect(0,2100,600,600), Qt::AlignCenter, text);
    painter.drawRect(QRect(0,2100,600,600));
    painter.restore();
}

void CompetenzeDirigenteExporter::printName(QPainter &painter, const QString &text)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(nameFont());
    painter.drawText(QRect(650, 2600,8250,350), Qt::AlignCenter | Qt::AlignVCenter, text);
    painter.restore();
}

void CompetenzeDirigenteExporter::printGiorniLavorati(QPainter &painter)
{
    int vOffset = m_assenzeVOffset+(0*m_lineSpacing);
    QString text = "Giorni Lavorati:";
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());
    QFontMetrics fm(bodyFont());
    int sectionWidth = fm.width(text) + 25;
    painter.drawText(QRect(0,vOffset,4000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.setFont(bodyFontBold());
    painter.drawText(QRect(sectionWidth,vOffset,m_maxPageWidth-sectionWidth,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->giorniLavorativi() + " - " + m_competenza->assenzeTotali() + " = " + m_competenza->giorniLavorati());
    painter.restore();
}

void CompetenzeDirigenteExporter::printFerie(QPainter &painter)
{
    int vOffset = m_assenzeVOffset+(1*m_lineSpacing);
    QString text = "FERIE:";
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());
    QFontMetrics fm(bodyFont());
    int sectionWidth = fm.width(text) + 25;
    painter.drawText(QRect(0,vOffset,4000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.setFont(bodyFontBold());

    QList<QDate> dates = m_competenza->ferieDates();
    QStringList list;

    foreach (QDate d, dates) {
        list << QString::number(d.day());
    }

    painter.drawText(QRect(sectionWidth,vOffset,m_maxPageWidth-sectionWidth,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, list.join(" - "));
    painter.restore();
}

void CompetenzeDirigenteExporter::printCongedi(QPainter &painter)
{
    int vOffset = m_assenzeVOffset+(2*m_lineSpacing);
    QString text = "C.S.:";
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());
    QFontMetrics fm(bodyFont());
    int sectionWidth = fm.width(text) + 25;
    painter.drawText(QRect(0,vOffset,4000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.setFont(bodyFontBold());

    QList<QDate> dates = m_competenza->congediDates();
    QStringList list;

    foreach (QDate d, dates) {
        list << QString::number(d.day());
    }

    painter.drawText(QRect(sectionWidth,vOffset,m_maxPageWidth-sectionWidth,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, list.join(" - "));
    painter.restore();
}

void CompetenzeDirigenteExporter::printMalattia(QPainter &painter)
{
    int vOffset = m_assenzeVOffset+(3*m_lineSpacing);
    QString text = "Malattia:";
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());
    QFontMetrics fm(bodyFont());
    int sectionWidth = fm.width(text) + 25;
    painter.drawText(QRect(0,vOffset,4000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.setFont(bodyFontBold());

    QList<QDate> dates = m_competenza->malattiaDates();
    QStringList list;

    foreach (QDate d, dates) {
        list << QString::number(d.day());
    }

    painter.drawText(QRect(sectionWidth,vOffset,m_maxPageWidth-sectionWidth,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, list.join(" - "));
    painter.restore();
}

void CompetenzeDirigenteExporter::printRmp(QPainter &painter)
{
    int vOffset = m_assenzeVOffset+(4*m_lineSpacing);
    QString text = "Recuperi (m.p.):";
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());
    QFontMetrics fm(bodyFont());
    int sectionWidth = fm.width(text) + 25;
    painter.drawText(QRect(0,vOffset,4000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.setFont(bodyFontBold());

    QList<QDate> dates = m_competenza->rmpDates();
    QStringList list;

    foreach (QDate d, dates) {
        list << QString::number(d.day());
    }

    painter.drawText(QRect(sectionWidth,vOffset,m_maxPageWidth-sectionWidth,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, list.join(" - "));
    painter.restore();
}

void CompetenzeDirigenteExporter::printRmc(QPainter &painter)
{
    int vOffset = m_assenzeVOffset+(5*m_lineSpacing);
    QString text = "Recuperi (m.c.):";
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());
    QFontMetrics fm(bodyFont());
    int sectionWidth = fm.width(text) + 25;
    painter.drawText(QRect(0,vOffset,4000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.setFont(bodyFontBold());

    QList<QDate> dates = m_competenza->rmcDates();
    QStringList list;

    foreach (QDate d, dates) {
        list << QString::number(d.day());
    }

    painter.drawText(QRect(sectionWidth,vOffset,m_maxPageWidth-sectionWidth,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, list.join(" - "));
    painter.restore();
}

void CompetenzeDirigenteExporter::printAltreAssenze(QPainter &painter)
{
    int vOffset = m_assenzeVOffset+(6*m_lineSpacing);
    QString text = "Altre assenze:";
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFont());
    QFontMetrics fm(bodyFont());
    int sectionWidth = fm.width(text) + 25;
    painter.drawText(QRect(0,vOffset,4000,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.setFont(bodyFontBold());

    QList<QDate> dates = m_competenza->altreAssenzeDates();
    QStringList list;

    foreach (QDate d, dates) {
        list << QString::number(d.day());
    }

    painter.drawText(QRect(sectionWidth,vOffset,m_maxPageWidth-sectionWidth,m_rowHeight), Qt::AlignLeft | Qt::AlignVCenter, list.join(" - "));
    painter.restore();
}

/*
 void IndexPage::populateReviewIndex(QPainter *painter,
                                    int &posY,
                                    int &columnCount,
                                    const QString &title,
                                    const QString &page )
{
    const int maxColumnHeight = 125;
    const float columnWidth = 200.0;

    QAbstractTextDocumentLayout::PaintContext context;
    QTextDocument doc;
    doc.setDefaultFont(Fonts::specContentFont());
//	doc.setTextWidth(columnWidth);
    doc.setHtml("<p style=\"font-family:Sans Serif;font-size:80%;font-weight:bold;color:white\">" + title + "</p>");
    if((posY + doc.documentLayout()->documentSize().height()) > maxColumnHeight) {
        if(columnCount != 4) {
            columnCount++;
            posY = 0;
        }
    }

    float tw = doc.documentLayout()->documentSize().width();
    float ratio = tw/columnWidth;
    painter->save();
    painter->translate(22 + (columnCount-1)*225, posY);
    if(ratio > 1.0f) {
        painter->scale(1.0/ratio, 1.0);
    }
    doc.documentLayout()->draw(painter, context);
    painter->restore();

    painter->save();
    painter->setFont(Fonts::proconsFont());
    painter->setPen(Qt::gray);
    painter->drawText((columnCount-1)*225, posY+12, page);
    painter->restore();

    posY += doc.documentLayout()->documentSize().height()-4;
}
*/

void CompetenzeDirigenteExporter::printGuardieDiurne(QPainter &painter)
{
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(15);

    painter.save();

    QFont font = painter.font();
    font.setPixelSize(140);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);

    int cellSize = 300;
    int counter = 0;
    const int hOffset = (6000 - cellSize*m_competenza->guardiaDiurnaMap().count()) / 2;

    QMap<int, GuardiaType>::const_iterator i = m_competenza->guardiaDiurnaMap().constBegin();
    while (i != m_competenza->guardiaDiurnaMap().constEnd()) {
        QRect rect(hOffset+cellSize*counter,6000,cellSize,cellSize);
        painter.drawText(rect,Qt::AlignCenter | Qt::AlignVCenter,QString::number(i.key()));
        painter.drawEllipse(rect.adjusted(15,15,-15,-15));
        counter++;
        i++;
    }
    painter.restore();
}

void CompetenzeDirigenteExporter::printGuardieNotturne(QPainter &painter)
{
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(15);

    painter.save();

    QFont font = painter.font();
    font.setPixelSize(140);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);

    int cellSize = 300;
    int counter = 0;
    const int hOffset = (6000 - cellSize*m_competenza->guardiaNotturnaMap().count()) / 2;

    QMap<int, GuardiaType>::const_iterator i = m_competenza->guardiaNotturnaMap().constBegin();
    while (i != m_competenza->guardiaNotturnaMap().constEnd()) {
        QRect rect(hOffset+cellSize*counter,6900,cellSize,cellSize);
        painter.drawText(rect,Qt::AlignCenter | Qt::AlignVCenter,QString::number(i.key()));
        switch (i.value()) {
        case GuardiaType::Sabato:
            painter.drawText(rect,Qt::AlignHCenter | Qt::AlignTop,"*");
            break;
        case GuardiaType::Domenica:
            painter.drawEllipse(rect.adjusted(15,15,-15,-15));
            break;
        case GuardiaType::GrandeFestivita:
            painter.drawText(QRect(hOffset+cellSize*counter-80,6900-80,cellSize+160,cellSize+160),Qt::AlignHCenter | Qt::AlignTop,"*");
            painter.drawRect(rect.adjusted(25,25,-25,-25));
            break;
        default:
            break;
        }
        counter++;
        i++;
    }
    painter.restore();
}

void CompetenzeDirigenteExporter::printDistribuzioneOreGuardia(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    painter.drawText(QRect(6750,5950,1200,140), Qt::AlignLeft | Qt::AlignVCenter, QString::number(m_competenza->g_d_fer_F()) + " + " + QString::number(m_competenza->g_d_fer_S()) + " + " + QString::number(m_competenza->g_d_fer_D()));
    painter.drawText(QRect(6750,6250,1200,140), Qt::AlignLeft | Qt::AlignVCenter, QString::number(m_competenza->g_d_fes_F()) + " + " + QString::number(m_competenza->g_d_fes_S()) + " + " + QString::number(m_competenza->g_d_fes_D()));
    painter.drawText(QRect(6750,6850,1200,140), Qt::AlignLeft | Qt::AlignVCenter, QString::number(m_competenza->g_n_fer_F()) + " + " + QString::number(m_competenza->g_n_fer_S()) + " + " + QString::number(m_competenza->g_n_fer_D()));
    painter.drawText(QRect(6750,7150,1200,140), Qt::AlignLeft | Qt::AlignVCenter, QString::number(m_competenza->g_n_fes_F()) + " + " + QString::number(m_competenza->g_n_fes_S()) + " + " + QString::number(m_competenza->g_n_fes_D()));
    painter.drawText(QRect(8000,6650,1200,140), Qt::AlignLeft | Qt::AlignVCenter, QString::number(m_competenza->totOreGuardie()));
    painter.restore();
}

void CompetenzeDirigenteExporter::printDistribuzioneOreReperibilita(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    painter.drawText(QRect(6800,7750,1200,140), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->r_d_fer());
    painter.drawText(QRect(6800,8050,1200,140), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->r_d_fes());
    painter.drawText(QRect(6800,8650,1200,140), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->r_n_fer());
    painter.drawText(QRect(6800,8950,1200,140), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->r_n_fes());
    painter.drawText(QRect(8000,8450,1200,140), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->oreGrep());
    painter.restore();
}

void CompetenzeDirigenteExporter::printTurniProntaDisponibilita(QPainter &painter)
{
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(8);

    painter.save();

    QFont font = painter.font();
    font.setPixelSize(130);
    font.setBold(true);
    painter.setFont(font);

    QFont fontApice;
    fontApice.setPixelSize(100);
    fontApice.setBold(false);

    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);

    int cellSize = 240;
    int counter = 0;

    QMap<QDate, ValoreRep>::const_iterator i = m_competenza->rep().constBegin();
    while (i != m_competenza->rep().constEnd()) {
        QRect rect(600+cellSize*counter,9750,cellSize,cellSize);
        QDate d = i.key();
        painter.drawText(rect,Qt::AlignCenter | Qt::AlignVCenter,QString::number(d.day()));
        switch (i.value()) {
        case ValoreRep::Mezzo: {
            painter.save();
            painter.setFont(fontApice);
            QRect rect2(600+cellSize*counter+30,9725,cellSize,cellSize);
            painter.drawText(rect2,Qt::AlignRight | Qt::AlignTop,"½");
            painter.restore();
            break;}
        case ValoreRep::Uno:
            break;
        case ValoreRep::UnoMezzo:
            painter.drawLine(rect.x()+8,rect.y()+rect.height()-8,rect.x()+rect.width()-8,rect.y()+rect.height()-8);
            break;
        case ValoreRep::Due:
            painter.drawEllipse(rect.adjusted(25,25,-25,-25));
            break;
        case ValoreRep::DueMezzo: {
            painter.save();
            painter.setFont(fontApice);
            painter.drawEllipse(rect.adjusted(25,25,-25,-25));
            QRect rect2(600+cellSize*counter+30,9725,cellSize,cellSize);
            painter.drawText(rect2,Qt::AlignRight | Qt::AlignTop,"½");
            painter.restore();
            break;}
        default:
            break;
        }
        counter++;
        i++;
    }
    painter.restore();
}

void CompetenzeDirigenteExporter::printOreLavoroDovute(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    painter.drawText(QRect(2000,10050,6200,150), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->oreDovute());
    painter.restore();
}

void CompetenzeDirigenteExporter::printOreLavoroEffettuate(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    painter.drawText(QRect(2000,10350,6200,150), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->oreEffettuate());
    painter.restore();
}

void CompetenzeDirigenteExporter::printNettoOre(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    QString text = m_competenza->differenzaOreSenzaDmp();

    if(m_competenza->dmp() != QTime(0,0,0)) {
        text += " - d.m.p. " + QString::number(m_competenza->dmp().hour()) + ":" + QString::number(m_competenza->dmp().minute()) + " = " + m_competenza->differenzaOre();
    }

    painter.drawText(QRect(3300,10200,3000,150), Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.restore();
}

void CompetenzeDirigenteExporter::printOreStraordinarioGuardie(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    painter.drawText(QRect(3000,10650,6200,150), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->oreStraordinarioGuardie());
    painter.restore();
}

void CompetenzeDirigenteExporter::printOreProntaDisponibilita(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    painter.drawText(QRect(3600,10950,6200,150), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->oreProntaDisp());
    painter.restore();
}

void CompetenzeDirigenteExporter::printRep(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    painter.drawText(QRect(600,11550,6200,150), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->repCount());
    painter.restore();
}

void CompetenzeDirigenteExporter::printDeficit(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    painter.drawText(QRect(3180,11550,6200,150), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->deficitOrario());
    painter.restore();
}

void CompetenzeDirigenteExporter::printNotte(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    painter.drawText(QRect(600,11850,6200,150), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->notte());
    painter.restore();
}

void CompetenzeDirigenteExporter::printFestivo(QPainter &painter)
{
    painter.save();
    painter.setPen(Qt::black);
    painter.setFont(bodyFontBold());

    painter.drawText(QRect(3180,11850,6200,150), Qt::AlignLeft | Qt::AlignVCenter, m_competenza->festivo());
    painter.restore();
}
