/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
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

void NomiUnitaDialog::setNominativoLabel(const QString &name)
{
    ui->nominativoLabel->setText(name);
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
