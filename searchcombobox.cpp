/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
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
