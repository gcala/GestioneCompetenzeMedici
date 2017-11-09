#ifndef SQLITEDATABASEMANAGER_H
#define SQLITEDATABASEMANAGER_H

#include <QObject>

class SQLiteDatabaseManager;

namespace The {
    SQLiteDatabaseManager* dbManager();
}

class SQLiteDatabaseManager : public QObject
{
    Q_OBJECT
public:
    friend SQLiteDatabaseManager* The::dbManager();
    static bool createLocalConnection(const QString &fileName);
    static bool createRemoteConnection(const QString &host,
                                       const QString &dbName,
                                       const QString &user,
                                       const QString &pass,
                                       const bool &secure,
                                       const QString &cert,
                                       const QString &key);
    QString currentDatabase() const;
    QString driverName() const;

signals:

public slots:

private:
    SQLiteDatabaseManager();
    QString m_currentDatabase;
    static void closeCurrentConnection();
};

#endif // SQLITEDATABASEMANAGER_H
