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
    static void setLocalDbFileName(const QString &fileName);
    static void setDbName(const QString &name);
    static void setHost(const QString &host);
    static void setUser(const QString &user);
    static void setPass(const QString &pass);
    static void setSecure(const bool &secure);
    static void setCert(const QString &cert);
    static void setKey(const QString &key);
    static void setDriver(const QString &driver);

    static QSqlDatabase database(bool &ok, const QString &connectionName = QString());
    static QSqlDatabase currentDatabase();
    QString currentDatabaseName() const;
    QString driverName() const;

signals:

public slots:

private:
    SQLiteDatabaseManager();
//    QString m_currentDatabase;
    static void closeCurrentConnection();
    static bool createLocalConnection();
    static bool createRemoteConnection();

    static QString m_dbFile;
    static QString m_dbName;
    static QString m_host;
    static QString m_user;
    static QString m_password;
    static bool m_secure;
    static QString m_certFile;
    static QString m_keyFile;
    static QString m_driver;
};

#endif // SQLITEDATABASEMANAGER_H
