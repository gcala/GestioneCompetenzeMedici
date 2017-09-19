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

#include <QtWidgets>
#include <QSqlQuery>
#include <QSqlError>

InsertDBValues::InsertDBValues(QWidget *parent):
    QDialog(parent),
    ui(new Ui::InsertDBValues)
{
    ui->setupUi(this);

    ui->oreEdit->setFixedWidth(ui->dateEdit->width());
    ui->notteCB->setFixedWidth(ui->dateEdit->width());

    ui->notteCB->insertItem(0, "NO", 0);
    ui->notteCB->insertItem(1, "SI", 1);
}

InsertDBValues::~InsertDBValues()
{
    delete ui;
}

void InsertDBValues::unitaAddOreSetup(const QString &unitaId)
{
    qDebug() << unitaId;
    currentOp = AddOre;
    ui->dateEdit->setDate(QDate::currentDate());
    ui->oreEdit->setValue(0);
    ui->notteWidget->setVisible(false);
    ui->oreWidget->setVisible(true);
    tableName = "unita_ore_pagate";
    idName = "id_unita";
    currentId = unitaId;
    ui->saveButton->setText("Salva");
    ui->msgLabel->setText("Inserire Ore di Straordinario Pagate");
}

void InsertDBValues::unitaRemoveOreSetup(const QString &id)
{
    currentOp = RemoveOre;
    ui->notteWidget->setVisible(false);
    ui->oreWidget->setVisible(true);
    ui->saveButton->setText("Elimina");
    ui->msgLabel->setText("Sicuri di voler eliminare il seguente dato?");
    currentId = id;
    tableName = "unita_ore_pagate";
    QSqlQuery query;
    query.prepare("SELECT ore,data FROM " + tableName + " WHERE id=" + currentId + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return;
    }

    while(query.next()) {
        ui->dateEdit->setDate(QDate::fromString(query.value(1).toString(), "MM.yyyy"));
        ui->oreEdit->setValue(query.value(0).toInt());
    }
}

void InsertDBValues::unitaAddNotteSetup(const QString &unitaId)
{
    currentOp = AddNotte;
    ui->dateEdit->setDate(QDate::currentDate());
    ui->notteWidget->setVisible(true);
    ui->oreWidget->setVisible(false);
    tableName = "unita_notte";
    idName = "id_unita";
    currentId = unitaId;
    ui->saveButton->setText("Salva");
    ui->msgLabel->setText("Inserire variazione Notte?");
}

void InsertDBValues::unitaRemoveNotteSetup(const QString &id)
{
    currentOp = RemoveNotte;
    ui->notteWidget->setVisible(true);
    ui->oreWidget->setVisible(false);
    currentId = id;
    tableName = "unita_notte";
    idName = "id_unita";
    ui->saveButton->setText("Elimina");
    ui->msgLabel->setText("Sicuri di voler eliminare il seguente dato?");
    QSqlQuery query;
    query.prepare("SELECT notte,data FROM " + tableName + " WHERE id=" + currentId + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return;
    }

    while(query.next()) {
        ui->dateEdit->setDate(QDate::fromString(query.value(1).toString(), "MM.yyyy"));
        ui->notteCB->setCurrentText(query.value(0).toString());
        ui->notteCB->show();
    }
}

void InsertDBValues::dirigenteAddNotteSetup(const QString &unitaId)
{
    currentOp = AddNotte;
    ui->dateEdit->setDate(QDate::currentDate());
    ui->notteWidget->setVisible(true);
    ui->oreWidget->setVisible(false);
    tableName = "medici_notte";
    idName = "id_medico";
    currentId = unitaId;
    ui->saveButton->setText("Salva");
    ui->msgLabel->setText("Inserire variazione Notte?");
}

void InsertDBValues::dirigenteRemoveNotteSetup(const QString &id)
{
    currentOp = RemoveNotte;
    ui->notteWidget->setVisible(true);
    ui->oreWidget->setVisible(false);
    currentId = id;
    tableName = "medici_notte";
    idName = "id_medico";
    ui->saveButton->setText("Elimina");
    ui->msgLabel->setText("Sicuri di voler eliminare il seguente dato?");
    QSqlQuery query;
    query.prepare("SELECT notte,data FROM " + tableName + " WHERE id=" + currentId + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return;
    }

    while(query.next()) {
        ui->dateEdit->setDate(QDate::fromString(query.value(1).toString(), "MM.yyyy"));
        ui->notteCB->setCurrentText(query.value(0).toString());
        ui->notteCB->show();
    }
}

void InsertDBValues::on_cancelButton_clicked()
{
    this->close();
}

void InsertDBValues::on_saveButton_clicked()
{
    if(!ui->dateEdit->date().isValid())
        return;

    QSqlQuery query;

    switch(currentOp) {
    case AddOre:
        query.prepare("INSERT INTO " + tableName + " (" + idName + ",ore,data) "
                      "VALUES (:" + idName + ",:ore,:data);");
        query.bindValue(":" + idName, currentId);
        query.bindValue(":ore", ui->oreEdit->value());
        query.bindValue(":data", ui->dateEdit->date().toString("MM.yyyy"));
        break;
    case AddNotte:
        query.prepare("INSERT INTO " + tableName + " (" + idName + ",notte,data) "
                      "VALUES (:" + idName + ",:notte,:data);");
        query.bindValue(":" + idName, currentId);
        query.bindValue(":notte", ui->notteCB->currentText());
        query.bindValue(":data", ui->dateEdit->date().toString("MM.yyyy"));
        break;
    default: // RemoveNotte
        query.prepare("DELETE FROM " + tableName + " WHERE id=" + currentId + ";");
    }

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }

    this->close();
}
