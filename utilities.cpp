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
