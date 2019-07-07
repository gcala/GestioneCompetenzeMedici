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

#include "nomiunitadialog.h"
#include "ui_nomiunitadialog.h"
#include "sqlqueries.h"

#include <QtWidgets>

NomiUnitaDialog::NomiUnitaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NomiUnitaDialog)
{
    ui->setupUi(this);
    populateUnits();
}

NomiUnitaDialog::~NomiUnitaDialog()
{
    delete ui;
}

void NomiUnitaDialog::populateUnits()
{
    ui->unitaCB->clear();
    const QMap<int, QString> map = SqlQueries::units();
    QMap<int, QString>::const_iterator i = map.constBegin();
    while (i != map.constEnd()) {
        ui->unitaCB->addItem(QString::number(i.key()) + " - " + i.value(), i.key());
        ++i;
    }
    ui->unitaCB->setCurrentIndex(0);
    ui->unitaCB->show();
}

void NomiUnitaDialog::setUnitaLabel(const QString &name)
{
    ui->unitaLabel->setText(name);
    m_unitId = -1;
}

int NomiUnitaDialog::currentUnit() const
{
    return m_unitId;
}

void NomiUnitaDialog::on_okButton_clicked()
{
    m_unitId = ui->unitaCB->currentData(Qt::UserRole).toInt();
    close();
}

void NomiUnitaDialog::on_cancelButton_clicked()
{
    m_unitId = -1;
    close();
}
