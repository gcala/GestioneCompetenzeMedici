/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/


#include "settingsitem.h"

#include <utility>
#include "ui_settingsitem.h"

/*!
  \brief SettingsItem: this is the SettingsItem constructor
  \param name: setting name
  \param icon: setting icon
  \param parent: The Parent Widget
*/
SettingsItem::SettingsItem(QString name, QString icon, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsItem),
    m_settingName(std::move(name)),
    m_settingIcon(std::move(icon))
{
    ui->setupUi(this);
    if(m_settingIcon.isEmpty()) {
        ui->settingIconLabel->setPixmap(QPixmap(QLatin1String(":/images/logo.png")));
    }
    else {
        ui->settingIconLabel->setPixmap(QPixmap(QLatin1String(":/icons/" + m_settingIcon.toLatin1() + ".svg")));
    }
    ui->settingIconLabel->setScaledContents(true);
    // fix small icons in gnome
//    ui->settingIconLabel->setScaledContents(true);

    ui->settingNameLabel->setText(m_settingName);
}


