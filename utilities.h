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
    static int inMinuti(QString orario);
    static int mese2Int(const QString &mese);
    static QString int2MeseBreve(const int &mese);
    static bool timbraturaValida(QString text);

    static QString m_driver;
    static QString m_host;
    static QString m_dbName;
    static QString m_certFile;
    static QString m_keyFile;
    static bool m_useSSL;
    static QString m_localDbFileName;
    static QString m_lastUsername;
    static QString m_lastPassword;
};

#endif // UTILITIES_H
