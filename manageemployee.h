/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef MANAGEEMPLOYEE_H
#define MANAGEEMPLOYEE_H

#include <QDialog>

namespace Ui {
class ManageEmployee;
}

class ManageEmployee : public QDialog
{
    Q_OBJECT

public:
    explicit ManageEmployee(QWidget *parent = nullptr);
    ~ManageEmployee();

    void setDipendente(const int matricola);
    bool isChanged() const;

private slots:
    void on_dirigentiComboBox_currentIndexChanged(int index);
    void on_dirigenteNomeLE_textChanged(const QString &arg1);
    void on_closeButton_clicked();
    void on_restoreButton_clicked();
    void on_saveButton_clicked();

private:
    Ui::ManageEmployee *ui;
    QString m_nominativo;
    bool m_changed;

    void populateDirigenti();
};

#endif // MANAGEEMPLOYEE_H
