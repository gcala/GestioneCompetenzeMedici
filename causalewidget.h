/*
 *  SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>
 * 
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CAUSALEWIDGET_H
#define CAUSALEWIDGET_H

#include <QWidget>

class QMenu;
class CalendarManager;
class QWidgetAction;

namespace Ui {
class CausaleWidget;
}

class CausaleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CausaleWidget(const QString &causale, const int &minuti, const int &anno, const int &mese, const QVector<int> &giorni, QWidget *parent = nullptr);
    ~CausaleWidget();

    QVector<int> giorni() const;
    QString causale() const;
    void startBlinking(bool ok);

private:
    Ui::CausaleWidget *ui;

    QMenu *menu;
    CalendarManager *calendar;
    QWidgetAction *calendarAction;
    QVector<int> m_giorni;
    QString m_causale;
};

#endif // CAUSALEWIDGET_H
