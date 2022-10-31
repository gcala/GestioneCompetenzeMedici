/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef DATEEDITDELEGATE_H
#define DATEEDITDELEGATE_H

#include <QStyledItemDelegate>

class DateEditDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    DateEditDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, 
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, 
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, 
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;
};

#endif 
