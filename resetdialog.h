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

#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>

namespace Ui {
class ResetDialog;
}

class Competenza;

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(const QString &tableName = QString(), const int &id = -1, QWidget *parent = 0);
    explicit ResetDialog(Competenza *competenza, QWidget *parent = 0);
    ~ResetDialog();

private slots:
    void on_cancelButton_clicked();
    void on_restoreButton_clicked();
    void on_tuttoCB_toggled(bool checked);

private:
    Ui::ResetDialog *ui;
    QString m_tableName;
    int m_id;
    Competenza *m_competenza;

    void enableItems();

};

#endif // RESETDIALOG_H
