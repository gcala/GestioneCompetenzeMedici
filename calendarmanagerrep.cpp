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

#include "calendarmanagerrep.h"

#include <QPainter>
#include <QAbstractItemView>
#include <QDebug>

CalendarManagerRep::CalendarManagerRep(QWidget *parent)
    : QCalendarWidget(parent)
{
    m_outlinePen.setColor(Qt::darkRed);
    m_outlinePen.setWidth(2);
    m_transparentBrush.setColor(Qt::transparent);
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    connect(this, SIGNAL(clicked(QDate)), this, SLOT(dataSelezionata(QDate)));

    setMinimumSize(400,400);
}

CalendarManagerRep::~CalendarManagerRep()
{

}

void CalendarManagerRep::setColor(const QColor &color)
{
    m_outlinePen.setColor(color);
}

QColor CalendarManagerRep::getColor() const
{
    return ( m_outlinePen.color() );
}

QMap<QDate, ValoreRep> CalendarManagerRep::getDates() const
{
    return m_dates;
}

void CalendarManagerRep::setDates(const QMap<QDate, ValoreRep> &dates)
{
    m_dates = dates;
}

void CalendarManagerRep::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    QCalendarWidget::paintCell(painter, rect, date);

    if( m_dates.keys().contains(date) ) {
        QFont font = painter->font();
        font.setPixelSize(18);
        font.setBold(true);
        painter->setFont(font);
        painter->setPen(m_outlinePen);
        painter->setBrush(m_transparentBrush);
        painter->drawRect(rect.adjusted(0,0,-1,-1));
        switch(m_dates[date]) {
        case Mezzo:
            painter->drawText(rect.adjusted(0,0,-1,-1), Qt::AlignRight | Qt::AlignTop,"½");
            break;
        case Uno:
            painter->drawText(rect.adjusted(0,0,-1,-1), Qt::AlignRight | Qt::AlignTop,"1");
            break;
        case UnoMezzo:
            painter->drawText(rect.adjusted(0,0,-1,-1), Qt::AlignRight | Qt::AlignTop,"1½");
            break;
        case Due:
            painter->drawText(rect.adjusted(0,0,-1,-1), Qt::AlignRight | Qt::AlignTop,"2");
            break;
        default:
            painter->drawText(rect.adjusted(0,0,-1,-1), Qt::AlignRight | Qt::AlignTop,"2½");
        }
    }
}

void CalendarManagerRep::dataSelezionata(const QDate &date)
{
    if(m_dates.keys().contains(date)) {
        switch(m_dates[date]) {
        case Mezzo:
            m_dates[date] = Uno;
            break;
        case Uno:
            m_dates[date] = UnoMezzo;
            break;
        case UnoMezzo:
            m_dates[date] = Due;
            break;
        case Due:
            m_dates[date] = DueMezzo;
            break;
        default:
            m_dates.remove(date);
        }
    } else {
        m_dates[date] = Mezzo;
    }

    QAbstractItemView *view = this->findChild<QAbstractItemView*>();
    if(view){
        view->viewport()->update();
    } else update(); // fallback

    emit datesChanged();
}
