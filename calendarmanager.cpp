/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
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

//    connect(this, SIGNAL(clicked(QDate)), this, SLOT(dataSelezionata(QDate)));

    setMinimumSize(400,400);
}

CalendarManager::~CalendarManager()
= default;

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

void CalendarManager::setMezzeDates(const QList<QDate> &dates)
{
     m_mezzeDates = dates;
}

QList<QDate> CalendarManager::getScopertiDates() const
{
    return m_scoperti;
}

void CalendarManager::setScopertiDates(const QList<QDate> &dates)
{
    m_scoperti = dates;
}

#if QT_VERSION >= 0x060000
    void CalendarManager::paintCell(QPainter *painter, const QRect &rect, QDate date) const
#else
    void CalendarManager::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
#endif
{
    QCalendarWidget::paintCell(painter, rect, date);

    if( m_dates.contains(date) ) {
        painter->save();
        painter->setPen(m_outlinePen);
        painter->setBrush(m_transparentBrush);
        painter->drawRect(rect.adjusted(4,4,-5,-5));
        painter->restore();
    }

    if( m_mezzeDates.contains(date) ) {
        QPen outlinePen;
        outlinePen.setColor(Qt::darkGreen);
        outlinePen.setWidth(2);
        painter->save();
        painter->setPen(outlinePen);
        painter->setBrush(m_transparentBrush);
        painter->drawEllipse(rect.adjusted(4,4,-5,-5));
        painter->restore();
    }

    if( m_scoperti.contains(date) ) {
        painter->save();
        painter->setPen(m_outlinePen);
        painter->drawText(rect.adjusted(6,6,-7,-7), Qt::AlignRight | Qt::AlignTop,"A");
        painter->restore();
    }
}

void CalendarManager::dataSelezionata(const QDate &date)
{
    if(m_dates.contains(date))
        m_dates.removeOne(date);
    else
        m_dates.append(date);

    std::sort(m_dates.begin(),m_dates.end());

    auto view = this->findChild<QAbstractItemView*>();
    if(view){
        view->viewport()->update();
    } else update(); // fallback

    emit datesChanged();
}
