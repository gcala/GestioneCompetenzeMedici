/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef SQLITEDATABASEMANAGER_H
#define SQLITEDATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>

class SQLiteDatabaseManager;

namespace The {
    SQLiteDatabaseManager* dbManager();
}

class SQLiteDatabaseManager : public QObject
{
    Q_OBJECT
public:
    friend SQLiteDatabaseManager* The::dbManager();
    static bool createConnection();
    static bool createLocalConnection(const QString &fileName);
    static QSqlDatabase database(bool &ok, const QString &connectionName = QString());
    static QSqlDatabase currentDatabase();
    QString currentDatabaseName() const;
    QString driverName() const;

signals:

public slots:

private:
    SQLiteDatabaseManager();
    static void closeCurrentConnection();
    static bool createLocalConnection();
    static bool createRemoteConnection();
};

#endif // SQLITEDATABASEMANAGER_H
