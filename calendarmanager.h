/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CALENDARMANAGER_H
#define CALENDARMANAGER_H

#include <QCalendarWidget>

#include <QStringList>
#include <QBrush>
#include <QColor>
#include <QFile>
#include <QList>
#include <QDate>
#include <QPen>

class CalendarManager : public QCalendarWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ getColor WRITE setColor)

public:
    CalendarManager(QWidget *parent = nullptr);
    ~CalendarManager();

    enum ValoreRep { Mezzo, Uno, UnoMezzo, Due, DueMezzo };
    Q_ENUM(ValoreRep)

    void setColor(const QColor &color);
    QColor getColor() const;

    QList<QDate> getDates() const;
    void setDates(const QList<QDate> &dates);

    QList<QDate> getScopertiDates() const;
    void setScopertiDates(const QList<QDate> &dates);

protected:
#if QT_VERSION >= 0x060000
    virtual void paintCell(QPainter *painter, const QRect &rect, QDate date) const;
#else
    virtual void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;
#endif

private slots:
    void dataSelezionata(const QDate &date);

signals:
    void datesChanged();

private:
    QPen m_outlinePen;
    QBrush m_transparentBrush;
    QList<QDate> m_dates;
    QList<QDate> m_scoperti;
};

#endif // CALENDARMANAGER_H 
