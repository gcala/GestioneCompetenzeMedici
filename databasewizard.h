/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017-2019 Giuseppe Calà <giuseppe.cala@mailbox.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

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
    explicit DatabaseWizard(int page = 0, QWidget *parent = nullptr);
    ~DatabaseWizard();

    bool openDb() const;
    bool canceled() const;
    QString host() const;
    QString database() const;
    QString user() const;
    QString password() const;
    bool useSSL() const;
    QString certFile() const;
    QString keyFile() const;

private slots:
    void on_destBrowse_clicked();
    void on_unitsBrowse_clicked();
    void on_payloadBrowse_clicked();
    void on_repsBrowse_clicked();
    void on_cancelNewButton_clicked();
    void on_createButton_clicked();
    void on_dbName_textChanged(const QString &arg1);
    void on_dbDest_textChanged(const QString &arg1);
    void on_openLocalDbButton_clicked();
    void on_openRemoteDbButton_clicked();
    void on_hostLine_textChanged(const QString &arg1);
    void on_dbLine_textChanged(const QString &arg1);
    void on_remoteDbCancel_clicked();
    void on_remoteDbBack_clicked();
    void on_remoteDbSave_clicked();
    void on_cancelOpenButton_clicked();
    void on_userLine_textChanged(const QString &arg1);
    void on_passLine_textChanged(const QString &arg1);
    void on_revealButton_clicked();
    void on_useSSL_toggled(bool checked);
    void on_certButton_clicked();
    void on_keyButton_clicked();

private:
    Ui::DatabaseWizard *ui;
    int m_startPage;
    bool m_openDb;
    bool m_canceled;
    bool m_revealPass;
};

#endif // DATABASEWIZARD_H
