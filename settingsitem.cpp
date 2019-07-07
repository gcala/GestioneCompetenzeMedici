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


