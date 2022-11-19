/*
 *  SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>
 * 
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "causalewidget.h"
#include "ui_causalewidget.h"
#include "utilities.h"
#include "calendarmanager.h"
#include "blinkinglabel.h"

#include <QMenu>
#include <QWidgetAction>

CausaleWidget::CausaleWidget(const QString &causale, const int &minuti, const int &anno, const int &mese, const QVector<int> &giorni, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CausaleWidget)
{
    ui->setupUi(this);
    ui->label->setText(causale + " : " + Utilities::inOrario(minuti));
    m_giorni = giorni;
    m_causale = causale;
    menu = new QMenu(ui->pushButton);
    calendar = new CalendarManager(menu);
    calendarAction = new QWidgetAction(menu);
    calendarAction->setDefaultWidget(calendar);
    menu->addAction(calendarAction);
    ui->pushButton->setMenu(menu);
    calendar->setSelectionMode(QCalendarWidget::NoSelection);
    calendar->setDateRange(QDate(anno, mese, 1), QDate(anno, mese, QDate(anno, mese,1).daysInMonth()));
    QList<QDate> dates;
    for(auto giorno : giorni) {
        dates << QDate(anno, mese, giorno);
    }
    calendar->setDates(dates);
}

CausaleWidget::~CausaleWidget()
{
    delete ui;
}

QVector<int> CausaleWidget::giorni() const
{
    return m_giorni;
}

QString CausaleWidget::causale() const
{
    return m_causale;
}

void CausaleWidget::startBlinking(bool ok)
{
    ui->label->startBlinking(ok);
}
