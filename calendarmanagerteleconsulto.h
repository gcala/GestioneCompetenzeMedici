/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CALENDARMANAGERTELECONSULTO_H
#define CALENDARMANAGERTELECONSULTO_H

#include "utilities.h"

#include <QCalendarWidget>
#include <QStringList>
#include <QBrush>
#include <QColor>
#include <QFile>
#include <QList>
#include <QDate>
#include <QPen>
#include <QMap>

class CalendarManagerTeleconsulto : public QCalendarWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ getColor WRITE setColor)

public:
    CalendarManagerTeleconsulto(QWidget *parent = nullptr);
    ~CalendarManagerTeleconsulto();

//    Q_ENUM(ValoreRep)

    void setColor(const QColor &color);
    QColor getColor() const;

    QMap<QDate, Utilities::ValoreRep> getDates() const;
    void setDates(const QMap<QDate, Utilities::ValoreRep> &dates);

protected:
#if QT_VERSION >= 0x060000
    virtual void paintCell(QPainter *painter, const QRect &rect, QDate date) const;
#else
    virtual void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;
#endif

private slots:
    void dataRightClicked(const QPoint &pos);
    void dataLeftClicked(const QDate &date);
    void noSelected();
    void t6Selected();
    void t12Selected();
    void t18Selected();
    void t24Selected();

signals:
    void datesChanged();

private:
    QMap<QDate, Utilities::ValoreRep> m_dates;
    QDate m_selectedDate;

    QPen m_outlinePen;
    QBrush m_transparentBrush;
    Utilities::ValoreRep teleconsultoConvert(const double value);
};

#endif // CALENDARMANAGERTELECONSULTO_H
