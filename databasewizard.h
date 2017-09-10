#ifndef DATABASEWIZARD_H
#define DATABASEWIZARD_H

#include <QDialog>

namespace Ui {
class DatabaseWizard;
}

class DatabaseWizard : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseWizard(QWidget *parent = 0);
    ~DatabaseWizard();

private slots:
    void on_destBrowse_clicked();
    void on_unitsBrowse_clicked();
    void on_payloadBrowse_clicked();
    void on_repsBrowse_clicked();
    void on_cancelButton_clicked();
    void on_createButton_clicked();
    void on_dbName_textChanged(const QString &arg1);
    void on_dbDest_textChanged(const QString &arg1);

private:
    Ui::DatabaseWizard *ui;

    bool insertUnitsFromFile(const QString &file);
    bool insertPayloadFromFile(const QString &file);
    bool insertRepsFromFile(const QString &file);
};

#endif // DATABASEWIZARD_H
