/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017-2018 Giuseppe Calà <giuseppe.cala@mailbox.org>
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

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(const QString &host, const QString &db, QWidget *parent = 0);
    ~LoginDialog();

    bool canceled() const;
    QString username() const;
    void setUsername(const QString &user);
    QString password() const;
    void setPassword(const QString &pass);
    void disablePassButton(bool ok);

private slots:
    void on_userLine_textChanged(const QString &arg1);
    void on_passLine_textChanged(const QString &arg1);
    void on_passButton_clicked();
    void on_cancelButton_clicked();
    void on_connectButton_clicked();

private:
    Ui::LoginDialog *ui;
    bool m_revealPass;
    bool m_canceled;
};

#endif // LOGINDIALOG_H
