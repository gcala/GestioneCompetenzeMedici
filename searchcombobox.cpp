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

#include "searchcombobox.h"

#include <QSortFilterProxyModel>
#include <QCompleter>
#include <QLineEdit>

SearchComboBox::SearchComboBox(QWidget *parent) :
    QComboBox(parent)
{
    setEditable(true);

    QFont font;
    font.setPointSize(12);
    lineEdit()->setFont(font);

    proxyModel = new QSortFilterProxyModel;
    proxyModel->setSourceModel(model());
    proxyModel->setFilterCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);

    completer = new QCompleter(proxyModel,this);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    setCompleter(completer);

    connect(this->lineEdit(), SIGNAL(textEdited(const QString &)), proxyModel, SLOT(setFilterFixedString(const QString &)));
    connect(completer, SIGNAL(activated(const QString &)), this, SLOT(onCompleterActivated(const QString &)));
}

void SearchComboBox::onCompleterActivated(const QString &text)
{
    if(text.isEmpty())
        return;
    setCurrentIndex(findText(text));
}

void SearchComboBox::setModel(QAbstractItemModel *model)
{
    QComboBox::setModel(model);
    proxyModel->setSourceModel(model);
    completer->setModel(model);
}

void SearchComboBox::setModelColumn(int visibleColumn)
{
    completer->setCompletionColumn(visibleColumn);
    proxyModel->setFilterKeyColumn(visibleColumn);
    QComboBox::setModelColumn(visibleColumn);
}
