/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>

namespace Ui {
class ResetDialog;
}

class Competenza;

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QString tableName = QString(), const int &id = -1, QWidget *parent = nullptr);
    explicit ResetDialog(Competenza *competenza, QWidget *parent = nullptr);
    ~ResetDialog();

private slots:
    void on_cancelButton_clicked();
    void on_restoreButton_clicked();
    void on_tuttoCB_toggled(bool checked);

private:
    Ui::ResetDialog *ui;
    QString m_tableName;
    int m_id;
    Competenza *m_competenza{};

    void enableItems();

};

#endif // RESETDIALOG_H
