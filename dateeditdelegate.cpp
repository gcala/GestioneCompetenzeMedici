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

