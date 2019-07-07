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

#ifndef CALENDARMANAGERREP_H
#define CALENDARMANAGERREP_H

#include "defines.h"

#include <QCalendarWidget>

#include <QStringList>
#include <QBrush>
#include <QColor>
#include <QFile>
#include <QList>
#include <QDate>
#include <QPen>

class CalendarManagerRep : public QCalendarWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ getColor WRITE setColor)

public:
    CalendarManagerRep(QWidget *parent = nullptr);
    ~CalendarManagerRep();

//    Q_ENUM(ValoreRep)

    void setColor(const QColor &color);
    QColor getColor() const;

    QMap<QDate, ValoreRep> getDates() const;
    void setDates(const QMap<QDate, ValoreRep> &dates);

protected:
    virtual void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;

private slots:
    void dataSelezionata(const QDate &date);
    void noSelected();
    void mezzoSelected();
    void unoSelected();
    void unomezzoSelected();
    void dueSelected();

signals:
    void datesChanged();

private:
    QMap<QDate, ValoreRep> m_dates;
    QDate m_selectedDate;

    QPen m_outlinePen;
    QBrush m_transparentBrush;
};

#endif // CALENDARMANAGERREP_H
