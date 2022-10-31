/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef ALMANAC_H
#define ALMANAC_H

#include <QObject>
#include <QVector>
#include <QDate>

class Almanac;

namespace The {
    Almanac* almanac();
}

class Almanac : public QObject
{
    Q_OBJECT
public:
    friend Almanac* The::almanac();

    bool isGrandeFestivita(const QDate &date);

signals:

public slots:

private:
    Almanac();
    QList<QDate> m_grandiFestivita;
    QVector<int> m_years;

    void caricaGrandiFestivita(const int &anno);
    void caricaPasqua(const int &anno);
};

#endif // ALMANAC_H
