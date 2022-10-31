/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();

private slots:
    void on_photoBrowser_clicked();
    void on_javaBrowser_clicked();
    void on_tabulaBrowser_clicked();
    void on_restoreButton_clicked();
    void on_okButton_clicked();
    void on_cancelButton_clicked();
    void saveSettings();
    void switchPage( int );

private:
    Ui::ConfigDialog *ui;
    bool configChanged;

    void setupSettingsCategories();
    void loadSettings();
};

#endif // CONFIGDIALOG_H
