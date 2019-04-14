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

#ifndef SEARCHCOMBOBOX_H
#define SEARCHCOMBOBOX_H

#include <QWidget>
#include <QComboBox>

class QSortFilterProxyModel;
class QCompleter;

class SearchComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit SearchComboBox(QWidget *parent = nullptr);

private slots:
//    void onTextChanged(const QString text);
    void onCompleterActivated(const QString text);

private:
    QSortFilterProxyModel *proxyModel;
    QCompleter *completer;

    void setModel(QAbstractItemModel *model);
    void setModelColumn(int visibleColumn);
};

#endif // SEARCHCOMBOBOX_H
