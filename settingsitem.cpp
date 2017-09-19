/*
    GrooveOff - Offline Grooveshark.com music
    Copyright (C) 2013-2015  Giuseppe Calà <jiveaxe@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "settingsitem.h"
#include "ui_settingsitem.h"

/*!
  \brief SettingsItem: this is the SettingsItem constructor
  \param name: setting name
  \param icon: setting icon
  \param parent: The Parent Widget
*/
SettingsItem::SettingsItem(const QString &name, const QString &icon, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsItem),
    m_settingName(name),
    m_settingIcon(icon)
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


