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

#include "renderarearep.h"

#include <QPainter>
#include <QDebug>

RenderAreaRep::RenderAreaRep(QWidget *parent)
    : QWidget(parent)
{
//    setBackgroundRole(QPalette::Base);
//    setAutoFillBackground(true);
}

QSize RenderAreaRep::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderAreaRep::sizeHint() const
{
    return QSize(400, 200);
}

void RenderAreaRep::setRepMap(const QMap<QDate, ValoreRep> &repMap)
{
    m_repMap = repMap;
    update();
}

void RenderAreaRep::paintEvent(QPaintEvent * /* event */)
{
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(2);

    QPainter painter(this);
    QFont font = painter.font();
    font.setPixelSize(18);
    font.setBold(true);
    painter.setFont(font);

    QFont fontApice;
    fontApice.setPixelSize(13);
    fontApice.setBold(false);

    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);

    int cellSize = width() / 5;
    int counter = 0;
    int yCounter = 0;
    QMap<QDate, ValoreRep>::const_iterator i = m_repMap.constBegin();
    while (i != m_repMap.constEnd()) {
        QRect rect(cellSize*(counter%5),cellSize*yCounter,cellSize,cellSize);
        QDate d = i.key();
        painter.drawText(rect,Qt::AlignCenter | Qt::AlignVCenter,QString::number(d.day()));
        switch (i.value()) {
        case ValoreRep::Mezzo:
            painter.save();
            painter.setFont(fontApice);
            painter.drawText(rect.adjusted(0,0,0,0),Qt::AlignRight | Qt::AlignTop,"½");
            painter.restore();
            break;
        case ValoreRep::Uno:
            break;
        case ValoreRep::UnoMezzo:
            painter.drawLine(rect.x()+8,rect.y()+rect.height()-8,rect.x()+rect.width()-8,rect.y()+rect.height()-8);
            break;
        case ValoreRep::Due:
            painter.drawEllipse(rect.adjusted(8,8,-8,-8));
            break;
        case ValoreRep::DueMezzo:
            painter.save();
            painter.setFont(fontApice);
            painter.drawEllipse(rect.adjusted(8,8,-8,-8));
            painter.drawText(rect.adjusted(0,0,0,0),Qt::AlignRight | Qt::AlignTop,"½");
            painter.restore();
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
