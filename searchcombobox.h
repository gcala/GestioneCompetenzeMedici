/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
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
    void onCompleterActivated(const QString &text);

private:
    QSortFilterProxyModel *proxyModel;
    QCompleter *completer;

    void setModel(QAbstractItemModel *model);
    void setModelColumn(int visibleColumn);
};

#endif // SEARCHCOMBOBOX_H
