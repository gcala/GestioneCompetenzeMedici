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

#include "renderarearep.h"

#include <QPainter>
#include <QDebug>

RenderAreaRep::RenderAreaRep(QWidget *parent)
    : QWidget(parent)
    , year(0)
{
//    setBackgroundRole(QPalette::Base);
//    setAutoFillBackground(true);
}

QSize RenderAreaRep::minimumSizeHint() const
{
    return {100, 100};
}

QSize RenderAreaRep::sizeHint() const
{
    return {400, 200};
}

void RenderAreaRep::setRepMap(const QMap<QDate, ValoreRep> &repMap)
{
    m_repMap = repMap;
    update();
}

void RenderAreaRep::paintEvent(QPaintEvent * /* event */)
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

    QFont fontApice;
    fontApice.setPixelSize(13);
    fontApice.setBold(false);

    painter.setPen(blackPen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);

    int cellSize = width() / 5;
    int counter = 0;
    int yCounter = 0;
    QMap<QDate, ValoreRep>::const_iterator i = m_repMap.constBegin();
    while (i != m_repMap.constEnd()) {
        QRect rect(cellSize*(counter%5),cellSize*yCounter,cellSize,cellSize);
        QDate d = i.key();
        if(d.year() != year) {
            year = d.year();
            calcolaFeste();
        }
        if(d.dayOfWeek() == 7 || m_grandiFestivita.contains(d)) {
            painter.setPen(redPen);
        }
        painter.drawText(rect,Qt::AlignCenter | Qt::AlignVCenter,QString::number(d.day()));
        painter.setPen(blackPen);
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

void RenderAreaRep::calcolaFeste()
{
    caricaGrandiFestivita(year);
    caricaPasqua(year);
}

void RenderAreaRep::caricaGrandiFestivita(int anno)
{
    m_grandiFestivita.clear();

    m_grandiFestivita.append(QDate(anno,1,1));
    m_grandiFestivita.append(QDate(anno,1,6));
    m_grandiFestivita.append(QDate(anno,4,25));
    m_grandiFestivita.append(QDate(anno,5,1));
    m_grandiFestivita.append(QDate(anno,5,30));
    m_grandiFestivita.append(QDate(anno,6,2));
    m_grandiFestivita.append(QDate(anno,8,15));
    m_grandiFestivita.append(QDate(anno,11,1));
    m_grandiFestivita.append(QDate(anno,12,8));
    m_grandiFestivita.append(QDate(anno,12,25));
    m_grandiFestivita.append(QDate(anno,12,26));
}

void RenderAreaRep::caricaPasqua(int anno)
{

    int giorno = 0;
    int mese = 0;
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    int e = 0;
    int m = 0;
    int n = 0;

    switch(anno/100)
    {
        case 15:	// 1583 - 1599 (FALL THROUGH)
        case 16:	// 1600 - 1699
            m=22; n=2; 	break;

        case 17:	// 1700 - 1799
            m=23; n=3; break;

        case 18:	// 1800 - 1899
            m=23; n=4; break;

        case 19:	// 1900 - 1999 (FALL THROUGH)
        case 20:	// 2000 - 2099
            m=24; n=5;break;

        case 21:	// 2100 - 2199
            m=24; n=6; break;

        case 22:	// 2200 - 2299
            m=25; n=0; break;

        case 23:	// 2300 - 2399
            m=26; n=1; break;

        case 24:	// 2400 - 2499
            m=25; n=1; break;
    }

    a=anno%19;
    b=anno%4;
    c=anno%7;
    d=(19*a+m)%30;
    e=(2*b+4*c+6*d+n)%7;
    giorno=d+e;

    if (d+e<10)
    {
        giorno+=22;
        mese=3;
    }

    else
    {
        giorno-=9;
        mese=4;

        if ((giorno==26)||((giorno==25)&&(d==28)&&(e==6)&&(a>10)))
        {
            giorno-=7;
        }
    }

    m_grandiFestivita.append(QDate(anno,mese,giorno));
    m_grandiFestivita.append(QDate(anno,mese,giorno).addDays(1));
}
