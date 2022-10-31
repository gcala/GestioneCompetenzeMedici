/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>
#include <QDate>

class Utilities
{
private:
    Utilities(){}

public:
    static QString inOrario(int value);
    static QString m_connectionName;
    static QDate ccnl1618Date;
};

#endif // UTILITIES_H
