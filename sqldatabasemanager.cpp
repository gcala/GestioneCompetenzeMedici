/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "sqldatabasemanager.h"
#include "utilities.h"

#include <QMessageBox>
#include <QSqlError>
#include <QApplication>
#include <QFile>
#include <QDebug>

namespace The {
    static SQLiteDatabaseManager* s_SQLiteDatabaseManager_instance = nullptr;

    SQLiteDatabaseManager* dbManager()
    {
        if( !s_SQLiteDatabaseManager_instance ) {
            s_SQLiteDatabaseManager_instance = new SQLiteDatabaseManager();
            Utilities::m_useSSL = false;
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

    QSqlDatabase db = QSqlDatabase::addDatabase(Utilities::m_driver);

    if(!QSqlDatabase::connectionNames().contains(Utilities::m_connectionName)) {
        QSqlDatabase db = QSqlDatabase::addDatabase(Utilities::m_driver, Utilities::m_connectionName);
        db.setDatabaseName(Utilities::m_localDbFileName);
        if (!db.open()) {
            QMessageBox::critical(nullptr, qApp->tr("Errore apertura database"),
                                  "Impossibile connettersi al database.\n" + db.lastError().text(), QMessageBox::Cancel);
            return false;
        }
    }

    qDebug() << Utilities::m_localDbFileName;

    return true;
}

bool SQLiteDatabaseManager::createRemoteConnection()
{
//    closeCurrentConnection();

    if(!QSqlDatabase::connectionNames().contains(Utilities::m_connectionName)) {
        QString opts = "MYSQL_OPT_RECONNECT=1;";

        if(Utilities::m_useSSL) {
            if(Utilities::m_certFile.isEmpty() || Utilities::m_keyFile.isEmpty() || !QFile::exists(Utilities::m_certFile) || !QFile::exists(Utilities::m_keyFile)) {
                QMessageBox::critical(nullptr, "Errore Connessione", "I file Certificato/Chiave sono necessari per una connessione protetta.\n"
                                                               "Aprire Impostazioni e configurare Certificato e Chiave.", QMessageBox::Cancel);
                return false;
            }
            opts += "SSL_KEY=" + Utilities::m_keyFile + ";";
            opts += "SSL_CERT=" + Utilities::m_certFile + ";";
        }

        QSqlDatabase db = QSqlDatabase::addDatabase(Utilities::m_driver, Utilities::m_connectionName);
        db.setConnectOptions(opts);
        db.setHostName(Utilities::m_host);
        db.setDatabaseName(Utilities::m_dbName);
        db.setUserName(Utilities::m_lastUsername);
        db.setPassword(Utilities::m_lastPassword);
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

QString SQLiteDatabaseManager::currentDatabaseName() const
{
    QSqlDatabase db = QSqlDatabase::database(Utilities::m_connectionName);
    return db.databaseName();
}

bool SQLiteDatabaseManager::createConnection()
{
    if(Utilities::m_driver.trimmed().isEmpty()) {
        QMessageBox::critical(nullptr, qApp->tr("Errore connessione"),
                              "Non è stato indicato il driver da usare. Connessione fallita!", QMessageBox::Cancel);
        return false;
    }

    if(Utilities::m_driver == "QMYSQL") {
        return createRemoteConnection();
    } else if(Utilities::m_driver == "QSQLITE") {
        return createLocalConnection();
    }

    return false;
}

QSqlDatabase SQLiteDatabaseManager::database(bool &ok, const QString &connectionName)
{
    QSqlDatabase db;
    ok = true;

    if(Utilities::m_driver == "QMYSQL") {
        QString opts = "MYSQL_OPT_RECONNECT=1;";

        if(Utilities::m_useSSL) {
            if(Utilities::m_certFile.isEmpty() || Utilities::m_keyFile.isEmpty() || !QFile::exists(Utilities::m_certFile) || !QFile::exists(Utilities::m_keyFile)) {
                qDebug() << "I file Certificato/Chiave sono necessari per una connessione protetta.\nAprire Impostazioni e configurare Certificato e Chiave.";
                ok = false;
            } else {
                opts += "SSL_KEY=" + Utilities::m_keyFile + ";";
                opts += "SSL_CERT=" + Utilities::m_certFile + ";";
            }
        }

        db = QSqlDatabase::addDatabase(Utilities::m_driver, connectionName);
        db.setConnectOptions(opts);
        db.setHostName(Utilities::m_host);
        db.setDatabaseName(Utilities::m_dbName);
        db.setUserName(Utilities::m_lastUsername);
        db.setPassword(Utilities::m_lastPassword);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(Utilities::m_localDbFileName);
    }

    return db;
}

QSqlDatabase SQLiteDatabaseManager::currentDatabase()
{
    return QSqlDatabase::database(Utilities::m_connectionName);
}

QString SQLiteDatabaseManager::driverName() const
{
    return Utilities::m_driver;
}
