/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef NOMIUNITADIALOG_H
#define NOMIUNITADIALOG_H

#include <QDialog>

namespace Ui {
class NomiUnitaDialog;
}

class NomiUnitaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NomiUnitaDialog(QWidget *parent = nullptr);
    ~NomiUnitaDialog();

    void populateUnits();
    void setUnitaLabel(const QString &name);
    int currentUnit() const;

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::NomiUnitaDialog *ui;
    int m_unitId;
};

#endif // NOMIUNITADIALOG_H
