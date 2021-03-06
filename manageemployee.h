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

#ifndef MANAGEEMPLOYEE_H
#define MANAGEEMPLOYEE_H

#include <QDialog>

namespace Ui {
class ManageEmployee;
}

class ManageEmployee : public QDialog
{
    Q_OBJECT

public:
    explicit ManageEmployee(QWidget *parent = nullptr);
    ~ManageEmployee();

    void setDipendente(const int matricola);
    bool isChanged() const;

private slots:
    void on_dirigentiComboBox_currentIndexChanged(int index);
    void on_dirigenteNomeLE_textChanged(const QString &arg1);
    void on_closeButton_clicked();
    void on_restoreButton_clicked();
    void on_saveButton_clicked();

private:
    Ui::ManageEmployee *ui;
    QString m_nominativo;
    bool m_changed;

    void populateDirigenti();
};

#endif // MANAGEEMPLOYEE_H
