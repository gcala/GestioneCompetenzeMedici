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

#include "sqldatabasemanager.h"
#include "utilities.h"

#include <QMessageBox>
#include <QSqlError>
#include <QApplication>
#include <QFile>
#include <QDebug>

QString SQLiteDatabaseManager::m_dbFile;
QString SQLiteDatabaseManager::m_dbName;
QString SQLiteDatabaseManager::m_host;
QString SQLiteDatabaseManager::m_user;
QString SQLiteDatabaseManager::m_password;
bool SQLiteDatabaseManager::m_secure;
QString SQLiteDatabaseManager::m_certFile;
QString SQLiteDatabaseManager::m_keyFile;
QString SQLiteDatabaseManager::m_driver;

namespace The {
    static SQLiteDatabaseManager* s_SQLiteDatabaseManager_instance = nullptr;

    SQLiteDatabaseManager* dbManager()
    {
        if( !s_SQLiteDatabaseManager_instance ) {
            s_SQLiteDatabaseManager_instance = new SQLiteDatabaseManager();
            s_SQLiteDatabaseManager_instance->m_secure = false;
        }

        return s_SQLiteDatabaseManager_instance;
    }
}

SQLiteDatabaseManager::SQLiteDatabaseManager()
= default;

void SQLiteDatabaseManager::closeCurrentConnection()
{
    QSqlDatabase db = QSqlDatabase::database();

    if(!db.databaseName().isEmpty()) {
        db.close();
        db.removeDatabase(db.databaseName());
    }
}

bool SQLiteDatabaseManager::createLocalConnection()
{
//    closeCurrentConnection();

    QSqlDatabase db = QSqlDatabase::addDatabase(m_driver);

    if(!QSqlDatabase::connectionNames().contains(Utilities::m_connectionName)) {
        QSqlDatabase db = QSqlDatabase::addDatabase(m_driver, Utilities::m_connectionName);
        db.setDatabaseName(m_dbFile);
        if (!db.open()) {
            QMessageBox::critical(nullptr, qApp->tr("Errore apertura database"),
                                  "Impossibile connettersi al database.\n" + db.lastError().text(), QMessageBox::Cancel);
            return false;
        }
    }

    return true;
}

bool SQLiteDatabaseManager::createRemoteConnection()
{
//    closeCurrentConnection();

    if(!QSqlDatabase::connectionNames().contains(Utilities::m_connectionName)) {
        QString opts = "MYSQL_OPT_RECONNECT=1;";

        if(m_secure) {
            if(m_certFile.isEmpty() || m_keyFile.isEmpty() || !QFile::exists(m_certFile) || !QFile::exists(m_keyFile)) {
                QMessageBox::critical(nullptr, "Errore Connessione", "I file Certificato/Chiave sono necessari per una connessione protetta.\n"
                                                               "Aprire Impostazioni e configurare Certificato e Chiave.", QMessageBox::Cancel);
                return false;
            }
            opts += "SSL_KEY=" + m_keyFile + ";";
            opts += "SSL_CERT=" + m_certFile + ";";
        }

        QSqlDatabase db = QSqlDatabase::addDatabase(m_driver, Utilities::m_connectionName);
        db.setConnectOptions(opts);
        db.setHostName(m_host);
        db.setDatabaseName(m_dbName);
        db.setUserName(m_user);
        db.setPassword(m_password);
        if (!db.open()) {
            QMessageBox::critical(nullptr, qApp->tr("Errore apertura database"),
                                  "Impossibile connettersi al database.\n" + db.lastError().text(), QMessageBox::Cancel);
            return false;
        }
    }

    return true;
}

/*
    This file defines a helper function to open a connection to an
    in-memory SQLITE database and to create a test table.

    If you want to use another database, simply modify the code
    below. All the examples in this directory use this function to
    connect to a database.
*/
bool SQLiteDatabaseManager::createLocalConnection(const QString &fileName)
{
//    closeCurrentConnection();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName(fileName);

    if (!db.open()) {
        QMessageBox::critical(nullptr, qApp->tr("Errore apertura database"),
            "Impossibile connettersi al database.\n" + db.lastError().text(), QMessageBox::Cancel);
        return false;
    }

    return true;
}

QString SQLiteDatabaseManager::currentDatabase() const
{
    QSqlDatabase db = QSqlDatabase::database(Utilities::m_connectionName);
    return db.databaseName();
}

bool SQLiteDatabaseManager::createConnection()
{
    if(m_driver.trimmed().isEmpty()) {
        QMessageBox::critical(nullptr, qApp->tr("Errore connessione"),
                              "Non è stato indicato il driver da usare. Connessione fallita!", QMessageBox::Cancel);
        return false;
    }

    if(m_driver == "QMYSQL") {
        return createRemoteConnection();
    } else if(m_driver == "QSQLITE") {
        return createLocalConnection();
    }

    return false;
}

void SQLiteDatabaseManager::setLocalDbFileName(const QString &fileName)
{
    m_dbFile = fileName;
}

void SQLiteDatabaseManager::setDbName(const QString &name)
{
    m_dbName = name;
}

void SQLiteDatabaseManager::setHost(const QString &host)
{
    m_host = host;
}

void SQLiteDatabaseManager::setUser(const QString &user)
{
    m_user = user;
}

void SQLiteDatabaseManager::setPass(const QString &pass)
{
    m_password = pass;
}

void SQLiteDatabaseManager::setSecure(const bool &secure)
{
    m_secure = secure;
}

void SQLiteDatabaseManager::setCert(const QString &cert)
{
    m_certFile = cert;
}

void SQLiteDatabaseManager::setKey(const QString &key)
{
    m_keyFile = key;
}

void SQLiteDatabaseManager::setDriver(const QString &driver)
{
    m_driver = driver;
}

QSqlDatabase SQLiteDatabaseManager::database(bool &ok, const QString &connectionName)
{
    QSqlDatabase db;
    ok = true;

    if(m_driver == "QMYSQL") {
        QString opts = "MYSQL_OPT_RECONNECT=1;";

        if(m_secure) {
            if(m_certFile.isEmpty() || m_keyFile.isEmpty() || !QFile::exists(m_certFile) || !QFile::exists(m_keyFile)) {
                qDebug() << "I file Certificato/Chiave sono necessari per una connessione protetta.\nAprire Impostazioni e configurare Certificato e Chiave.";
                ok = false;
            } else {
                opts += "SSL_KEY=" + m_keyFile + ";";
                opts += "SSL_CERT=" + m_certFile + ";";
            }
        }

        db = QSqlDatabase::addDatabase(m_driver, connectionName);
        db.setConnectOptions(opts);
        db.setHostName(m_host);
        db.setDatabaseName(m_dbName);
        db.setUserName(m_user);
        db.setPassword(m_password);
//        return db;
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(m_dbFile);
    }

    return db;
}

QString SQLiteDatabaseManager::driverName() const
{
    QSqlDatabase db = QSqlDatabase::database(Utilities::m_connectionName);
    return db.driverName();
}
