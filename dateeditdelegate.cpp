/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "dateeditdelegate.h"

#include <QDateEdit>

DateEditDelegate::DateEditDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *DateEditDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/* option */,
                                       const QModelIndex &/* index */) const
{    
    QDateEdit *editor = new QDateEdit(parent);
    editor->setFrame(false);
    editor->setDisplayFormat("MM/yyyy");
    editor->setCalendarPopup(true);

    return editor;
}

void DateEditDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    const QString value = index.model()->data(index, Qt::EditRole).toString();
    QDate date;
    if(value.trimmed() == "")
        date = QDate::currentDate();
    else
        date = QDate::fromString(value,"MM/yyyy");

    QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
    dateEdit->setDate(date);
}

void DateEditDelegate::setModelData(QWidget *editor, 
                                   QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
    dateEdit->interpretText();
    QDate value = dateEdit->date();

    model->setData(index, value.toString("MM/yyyy"), Qt::EditRole);
}

void DateEditDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

