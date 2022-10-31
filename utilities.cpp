/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "utilities.h"

QString Utilities::m_connectionName;
QDate Utilities::ccnl1618Date(2020,1,1);

QString Utilities::inOrario(int value )
{
    QString sign;
    if(value < 0) {
        sign = "-";
        value *= -1;
    }
    int ore = value / 60;
    int mins = value - ore * 60;

    return sign + QString::number(ore) + ":" + QString::number(mins).rightJustified(2, '0');
}
