/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INSERTDBVALUES_H
#define INSERTDBVALUES_H

#include <QDialog>

namespace Ui {
class InsertDBValues;
}

class InsertDBValues : public QDialog
{
    Q_OBJECT

public:
    explicit InsertDBValues(QWidget *parent = nullptr);
    ~InsertDBValues();

    enum Operation { AddOre, RemoveOre, AddNotte, RemoveNotte };
    Q_ENUM(Operation)

    void unitaAddOreSetup(const int &unitaId);
    void unitaRemoveOreSetup(const int &id);

private slots:
    void on_unitaCancel_clicked();
    void on_unitaSave_clicked();

private:
    Ui::InsertDBValues *ui;
    Operation currentOp;
    int currentId;
    QString tableName;
    QString idName;
};

#endif // INSERTDBVALUES_H
