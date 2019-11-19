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

#ifndef MANAGEUNITS_H
#define MANAGEUNITS_H

#include "dateeditdelegate.h"

#include <QDialog>

class QSqlTableModel;

namespace Ui {
class ManageUnits;
}

class ManageUnits : public QDialog
{
    Q_OBJECT

public:
    explicit ManageUnits(QWidget *parent = nullptr);
    ~ManageUnits();

    void setUnit(const int id);
    bool isChanged() const;

private slots:
    void on_unitaComboBox_currentIndexChanged(int index);
    void on_unitaNomeLE_textChanged(const QString &arg1);
    void on_unitaBreveLE_textChanged(const QString &arg1);
    void on_raggrLE_textChanged(const QString &arg1);
    void on_addUnitaOrePagateButton_clicked();
    void on_removeUnitaOrePagateButton_clicked();
    void on_closeButton_clicked();
    void on_restoreButton_clicked();
    void on_saveButton_clicked();
    void on_tableView_activated(const QModelIndex &index);
    void modelloCambiato(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:
    Ui::ManageUnits *ui;
    bool m_changed;
    bool m_modelChanged;
    QString m_nome;
    QString m_breve;
    QString m_raggruppamento;
    QString m_numero;
    QSqlTableModel *model;
    DateEditDelegate delegate;

    void populateUnita();
    void populateUnitaOrePagate();
    void checkChanged();
};

#endif // MANAGEUNITS_H
