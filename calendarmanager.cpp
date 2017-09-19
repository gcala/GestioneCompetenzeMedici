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

#include "calendarmanager.h"

#include <algorithm>
#include <QPainter>
#include <QAbstractItemView>

CalendarManager::CalendarManager(QWidget *parent)
    : QCalendarWidget(parent)
{
    m_outlinePen.setColor(Qt::darkRed);
    m_outlinePen.setWidth(2);
    m_transparentBrush.setColor(Qt::transparent);
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    connect(this, SIGNAL(clicked(QDate)), this, SLOT(dataSelezionata(QDate)));

    setMinimumSize(400,400);
}

CalendarManager::~CalendarManager()
{

}

void CalendarManager::setColor(const QColor &color)
{
    m_outlinePen.setColor(color);
}

QColor CalendarManager::getColor() const
{
    return ( m_outlinePen.color() );
}

QList<QDate> CalendarManager::getDates() const
{
    return m_dates;
}

void CalendarManager::setDates(const QList<QDate> &dates)
{
    m_dates = dates;
}

void CalendarManager::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    QCalendarWidget::paintCell(painter, rect, date);

    if( m_dates.contains(date) ) {
        painter->setPen(m_outlinePen);
        painter->setBrush(m_transparentBrush);
        painter->drawRect(rect.adjusted(4,4,-5,-5));
    }
}

void CalendarManager::dataSelezionata(const QDate &date)
{
    if(m_dates.contains(date))
        m_dates.removeOne(date);
    else
        m_dates.append(date);

    std::sort(m_dates.begin(),m_dates.end());

    QAbstractItemView *view = this->findChild<QAbstractItemView*>();
    if(view){
        view->viewport()->update();
    } else update(); // fallback

    emit datesChanged();
}
