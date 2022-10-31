/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "insertdbvalues.h"
#include "ui_insertdbvalues.h"
#include "sqlqueries.h"

#include <QtWidgets>

InsertDBValues::InsertDBValues(QWidget *parent):
    QDialog(parent),
    ui(new Ui::InsertDBValues)
{
    ui->setupUi(this);

    ui->unitaData->setFixedWidth(ui->unitaData->width());
}

InsertDBValues::~InsertDBValues()
{
    delete ui;
}

void InsertDBValues::unitaAddOreSetup(const int &unitaId)
{
    currentOp = AddOre;
    ui->stackedWidget->setCurrentIndex(0);
    ui->unitaData->setDate(QDate::currentDate());
    ui->unitaOrePagate->setValue(0);
    ui->unitaOreTot->setValue(8);
    tableName = "unita_ore_pagate";
    idName = "id_unita";
    currentId = unitaId;
    ui->unitaSave->setText("Salva");
    ui->unitaMsg->setText("Inserire Ore di Straordinario Pagate");
}

void InsertDBValues::unitaRemoveOreSetup(const int &id)
{
    currentOp = RemoveOre;
    ui->stackedWidget->setCurrentIndex(0);
    ui->unitaSave->setText("Elimina");
    ui->unitaMsg->setText("Sicuri di voler eliminare il seguente dato?");
    currentId = id;
    tableName = "unita_ore_pagate";
    QVariantList query = SqlQueries::getOrePagateFromId(id);

    if(query.isEmpty()) {
        qDebug() << Q_FUNC_INFO << ":: ERRORE :: ore pagate non trovate";
        return;
    }

    ui->unitaData->setDate(QDate::fromString(query.at(0).toString(), "MM/yyyy"));
    ui->unitaOreTot->setValue(query.at(1).toInt());
    ui->unitaOrePagate->setValue(query.at(2).toInt());
}

void InsertDBValues::on_unitaCancel_clicked()
{
    this->close();
}

void InsertDBValues::on_unitaSave_clicked()
{
    if(!ui->unitaData->date().isValid())
        return;

    switch(currentOp) {
    case AddOre:
        SqlQueries::insertPayload(currentId,
                                  ui->unitaData->date().toString("MM/yyyy"),
                                  QString::number(ui->unitaOreTot->value()),
                                  QString::number(ui->unitaOrePagate->value()));
        break;
    default:
        break;
    }

    this->close();
}
