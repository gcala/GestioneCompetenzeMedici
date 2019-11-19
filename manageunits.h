#ifndef MANAGEUNITS_H
#define MANAGEUNITS_H

#include <QDialog>

class InsertDBValues;

class QSqlQueryModel;

namespace Ui {
class ManageUnits;
}

class ManageUnits : public QDialog
{
    Q_OBJECT

public:
    explicit ManageUnits(QWidget *parent = nullptr);
    ~ManageUnits();

    void setUnit(const int id);
    bool isChanged() const;

private slots:
    void on_unitaComboBox_currentIndexChanged(int index);
    void on_unitaNomeLE_textChanged(const QString &arg1);
    void on_unitaBreveLE_textChanged(const QString &arg1);
    void on_raggrLE_textChanged(const QString &arg1);
    void on_unitaOrePagateTW_activated(const QModelIndex &index);
    void on_addUnitaOrePagateButton_clicked();
    void on_removeUnitaOrePagateButton_clicked();

    void on_closeButton_clicked();

    void on_restoreButton_clicked();

    void on_saveButton_clicked();

private:
    Ui::ManageUnits *ui;
    bool m_changed;
    QString m_nome;
    QString m_breve;
    QString m_raggruppamento;
    QString m_numero;
    QSqlQueryModel *unitaOrePagateModel;
    InsertDBValues *insertDialog;

    void populateUnita();
    void populateUnitaOrePagate();
    void checkChanged();
};

#endif // MANAGEUNITS_H
