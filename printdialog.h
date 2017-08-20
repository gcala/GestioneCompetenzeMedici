/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017 Giuseppe Calà <giuseppe.cala@mailbox.org>
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

#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>

namespace Ui {
class PrintDialog;
}

class PrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintDialog(QWidget *parent = 0);
    ~PrintDialog();

    void addUnita(const QString &unita, const QVariant &id);
    void addMese(const QString &mese, const QVariant &id);
    void addDirigente(const QString &dirigente, const QVariant &id);
    void clearUnita();
    void clearMese();
    void clearDirigente();
    void mostraDirigenti(bool ok);
    QString currentMeseData() const;
    int currentUnitaData() const;
    int currentDirigenteData() const;
    void setCurrentMese(int index);
    void setCurrentUnita(int index);
    void setCurrentDirigente(int index);
    bool proceed;
    QString path() const;
    void setPath(const QString &path);

private slots:
    void on_cancelButton_clicked();
    void on_saveButton_clicked();
    void on_unitaCB_currentIndexChanged(int index);
    void on_meseCB_currentIndexChanged(int index);
    void on_browseButton_clicked();

private:
    Ui::PrintDialog *ui;
};

#endif // PRINTDIALOG_H
