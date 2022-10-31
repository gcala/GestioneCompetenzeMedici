/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef MANAGEUNITS_H
#define MANAGEUNITS_H

#include "dateeditdelegate.h"

#include <QDialog>

class QSqlTableModel;

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
    void on_addUnitaOrePagateButton_clicked();
    void on_removeUnitaOrePagateButton_clicked();
    void on_closeButton_clicked();
    void on_restoreButton_clicked();
    void on_saveButton_clicked();
    void on_tableView_activated(const QModelIndex &index);
    void modelloCambiato(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:
    Ui::ManageUnits *ui;
    bool m_changed;
    bool m_modelChanged;
    QString m_nome;
    QString m_breve;
    QString m_raggruppamento;
    QString m_numero;
    QSqlTableModel *model;
    DateEditDelegate delegate;

    void populateUnita();
    void populateUnitaOrePagate();
    void checkChanged();
};

#endif // MANAGEUNITS_H
