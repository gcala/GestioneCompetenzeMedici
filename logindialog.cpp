/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "logindialog.h"
#include "ui_logindialog.h"

#include <QtWidgets>

LoginDialog::LoginDialog(const QString &host, const QString &db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->connectButton->setEnabled(false);
    m_revealPass = false;
    m_canceled = true;
    ui->dbLabel->setText("Connessione a " + db);
    ui->hostLabel->setText("su " + host);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

bool LoginDialog::canceled() const
{
    return m_canceled;
}

QString LoginDialog::username() const
{
    return ui->userLine->text().trimmed().toLower();
}

void LoginDialog::setUsername(const QString &user)
{
    ui->userLine->setText(user);
    if(!user.isEmpty())
        ui->passLine->setFocus();
}

QString LoginDialog::password() const
{
    return ui->passLine->text().trimmed();
}

void LoginDialog::setPassword(const QString &pass)
{
    ui->passLine->setText(pass);
}

void LoginDialog::disablePassButton(bool ok)
{
    ui->passButton->setEnabled(ok);
}

void LoginDialog::on_userLine_textChanged(const QString &arg1)
{
    if(!arg1.trimmed().isEmpty() && !ui->passLine->text().trimmed().isEmpty())
        ui->connectButton->setEnabled(true);
    else
        ui->connectButton->setEnabled(false);
}

void LoginDialog::on_passLine_textChanged(const QString &arg1)
{
    if(!arg1.trimmed().isEmpty() && !ui->userLine->text().trimmed().isEmpty())
        ui->connectButton->setEnabled(true);
    else
        ui->connectButton->setEnabled(false);
}

void LoginDialog::on_passButton_clicked()
{
    m_revealPass = !m_revealPass;
    if(m_revealPass) {
        ui->passButton->setIcon(QIcon(":/icons/password-show-off.svg"));
        ui->passLine->setEchoMode(QLineEdit::Normal);
    } else {
        ui->passButton->setIcon(QIcon(":/icons/password-show-on.svg"));
        ui->passLine->setEchoMode(QLineEdit::Password);
    }
}

void LoginDialog::on_cancelButton_clicked()
{
    m_canceled = true;
    close();
}

void LoginDialog::on_connectButton_clicked()
{
    m_canceled = false;
    close();
}
