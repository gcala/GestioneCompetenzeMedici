/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef SETTINGSITEM_H
#define SETTINGSITEM_H

#include <QWidget>

namespace Ui {
class SettingsItem;
}

class SettingsItem : public QWidget
{
    Q_OBJECT

public:
    SettingsItem(QString name, QString icon, QWidget *parent = nullptr);

private:
    Ui::SettingsItem *ui;
    QString m_settingName;
    QString m_settingIcon;
};

#endif // SETTINGSITEM_H
