#include "sqldatabasemanager.h"

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QApplication>
#include <QDebug>

namespace The {
    static SQLiteDatabaseManager* s_SQLiteDatabaseManager_instance = 0;

    SQLiteDatabaseManager* dbManager()
    {
        if( !s_SQLiteDatabaseManager_instance )
            s_SQLiteDatabaseManager_instance = new SQLiteDatabaseManager();

        return s_SQLiteDatabaseManager_instance;
    }
}

SQLiteDatabaseManager::SQLiteDatabaseManager()
{

}

void SQLiteDatabaseManager::closeCurrentConnection()
{
    QSqlDatabase db = QSqlDatabase::database();

    if(!db.databaseName().isEmpty()) {
        db.close();
        db.removeDatabase(db.databaseName());
    }
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
    closeCurrentConnection();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName(fileName);

    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Errore apertura database"),
            "Impossibile connettersi al database.\n" + db.lastError().text(), QMessageBox::Cancel);
        return false;
    }

    return true;
}

bool SQLiteDatabaseManager::createRemoteConnection(const QString &host, const QString &dbName, const QString &user, const QString &pass)
{
    closeCurrentConnection();

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(host);
    db.setDatabaseName(dbName);
    db.setUserName(user);
    db.setPassword(pass);
    // bool ok = db.open();

    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Errore apertura database"),
            "Impossibile connettersi al database.\n" + db.lastError().text(), QMessageBox::Cancel);
        return false;
    }

    return true;
}

QString SQLiteDatabaseManager::currentDatabase() const
{
    QSqlDatabase db = QSqlDatabase::database();
    return db.databaseName();
}

QString SQLiteDatabaseManager::driverName() const
{
    QSqlDatabase db = QSqlDatabase::database();
    return db.driverName();
}