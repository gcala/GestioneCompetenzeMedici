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

#ifndef NOMIUNITADIALOG_H
#define NOMIUNITADIALOG_H

#include <QDialog>

namespace Ui {
class NomiUnitaDialog;
}

class NomiUnitaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NomiUnitaDialog(QWidget *parent = nullptr);
    ~NomiUnitaDialog();

    void populateUnits();
    void setUnitaLabel(const QString &name);
    int currentUnit() const;

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::NomiUnitaDialog *ui;
    int m_unitId;
};

#endif // NOMIUNITADIALOG_H
