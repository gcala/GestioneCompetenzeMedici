#include "sqlitedatabasemanager.h"

#include <QMessageBox>
#include <QSqlDatabase>
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
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
}

/*
    This file defines a helper function to open a connection to an
    in-memory SQLITE database and to create a test table.

    If you want to use another database, simply modify the code
    below. All the examples in this directory use this function to
    connect to a database.
*/
bool SQLiteDatabaseManager::createConnection(const QString &fileName)
{
    QSqlDatabase db = QSqlDatabase::database();

    if(!db.databaseName().isEmpty()) {
        db.close();
        db.removeDatabase(db.databaseName());
    }

    db.setDatabaseName(fileName);

    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Errore apertura database"),
            qApp->tr("Impossibile connettersi al database.\n"
                     "This program needs SQLite support. Please read "
                     "the Qt SQL driver documentation for information how "
                     "to build it.\n\n"
                     "Premere annulla per uscire."), QMessageBox::Cancel);
        return false;
    }

    return true;
}

QString SQLiteDatabaseManager::currentDatabase() const
{
    QSqlDatabase db = QSqlDatabase::database();
    return db.databaseName();
}
