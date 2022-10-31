/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
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
    explicit LoginDialog(const QString &host, const QString &db, QWidget *parent = nullptr);
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
