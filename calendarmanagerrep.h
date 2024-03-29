/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
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
#include <QMap>

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
#if QT_VERSION >= 0x060000
    virtual void paintCell(QPainter *painter, const QRect &rect, QDate date) const;
#else
    virtual void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;
#endif

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
