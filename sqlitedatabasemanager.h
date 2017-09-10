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
    static bool createConnection(const QString &fileName);
    QString currentDatabase() const;

signals:

public slots:

private:
    SQLiteDatabaseManager();
    QString m_currentDatabase;
};

#endif // SQLITEDATABASEMANAGER_H
