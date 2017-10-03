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

#include "insertdbvalues.h"
#include "ui_insertdbvalues.h"
#include "sqlqueries.h"

#include <QtWidgets>
#include <QSqlQuery>
#include <QSqlError>

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

void InsertDBValues::unitaAddOreSetup(const QString &unitaId)
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

void InsertDBValues::unitaRemoveOreSetup(const QString &id)
{
    currentOp = RemoveOre;
    ui->stackedWidget->setCurrentIndex(0);
    ui->unitaSave->setText("Elimina");
    ui->unitaMsg->setText("Sicuri di voler eliminare il seguente dato?");
    currentId = id;
    tableName = "unita_ore_pagate";
    QSqlQuery query;
    query.prepare("SELECT ore_pagate,ore_tot,data FROM " + tableName + " WHERE id=" + currentId + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return;
    }

    while(query.next()) {
        ui->unitaData->setDate(QDate::fromString(query.value(2).toString(), "MM.yyyy"));
        ui->unitaOrePagate->setValue(query.value(0).toInt());
        ui->unitaOreTot->setValue(query.value(1).toInt());
    }
}

void InsertDBValues::on_unitaCancel_clicked()
{
    this->close();
}

void InsertDBValues::on_unitaSave_clicked()
{
    if(!ui->unitaData->date().isValid())
        return;
    QSqlQuery query;

    switch(currentOp) {
    case AddOre:
        SqlQueries::insertPayload(idName,
                                  ui->unitaData->date().toString("MM.yyyy"),
                                  QString::number(ui->unitaOreTot->value()),
                                  QString::number(ui->unitaOrePagate->value()));
        break;
    default:
        break;
    }

    this->close();
}
