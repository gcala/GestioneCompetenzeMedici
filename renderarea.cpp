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

#include "renderarea.h"
#include "almanac.h"

#include <QPainter>
#include <QDebug>

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
    , m_diurna(false)
{
//    setBackgroundRole(QPalette::Base);
//    setAutoFillBackground(true);
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderArea::sizeHint() const
{
    return QSize(400, 200);
}

void RenderArea::setGuardiaMap(const QMap<int, GuardiaType> &guardiaMap)
{
    m_guardiaMap = guardiaMap;
}

void RenderArea::setDiurna(const bool ok)
{
    m_diurna = ok;
}

void RenderArea::setMeseAnno(const int &mese, const int &anno)
{
    m_mese = mese;
    m_anno = anno;
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPen blackPen;
    blackPen.setColor(Qt::black);
    blackPen.setWidth(2);

    QPen redPen;
    redPen.setColor(Qt::red);
    redPen.setWidth(2);

    QPainter painter(this);
    QFont font = painter.font();
    font.setPixelSize(18);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(blackPen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);

    int cellSize = width() / 5;
    int counter = 0;
    int yCounter = 0;
    QMap<int, GuardiaType>::const_iterator i = m_guardiaMap.constBegin();
    while (i != m_guardiaMap.constEnd()) {
        QRect rect(cellSize*(counter%5),cellSize*yCounter,cellSize,cellSize);
        if(The::almanac()->isGrandeFestivita(QDate(m_anno, m_mese, i.key())) || QDate(m_anno, m_mese, i.key()).dayOfWeek() == 7 ) {
            painter.setPen(redPen);
        }
        painter.drawText(rect,Qt::AlignCenter | Qt::AlignVCenter,QString::number(i.key()));
        painter.setPen(blackPen);
        switch (i.value()) {
        case GuardiaType::Sabato:
            if(m_diurna)
                painter.drawEllipse(rect.adjusted(8,8,-8,-8));
            else
                painter.drawText(rect.adjusted(0,0,0,-30),Qt::AlignCenter | Qt::AlignTop,"*");
            break;
        case GuardiaType::Domenica:
            painter.drawEllipse(rect.adjusted(8,8,-8,-8));
            break;
        case GuardiaType::GrandeFestivita:
            if(m_diurna)
                painter.drawEllipse(rect.adjusted(8,8,-8,-8));
            else {
                painter.drawText(rect.adjusted(0,0,0,-30),Qt::AlignCenter | Qt::AlignTop,"*");
                painter.drawRect(cellSize*(counter%5)+10,cellSize*yCounter+10,cellSize-20,cellSize-20);
            }
            break;
        default:
            break;
        }
        i++;
        counter++;
        if(counter < 5)
            yCounter = 0;
        else if(counter < 10)
            yCounter = 1;
        else if(counter < 15)
            yCounter = 2;
        else
            yCounter = 3;
    }
}
