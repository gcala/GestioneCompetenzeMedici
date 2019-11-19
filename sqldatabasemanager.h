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
