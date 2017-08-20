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

#include "printdialog.h"
#include "ui_printdialog.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
#include <QDebug>

PrintDialog::PrintDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintDialog)
{
    ui->setupUi(this);
}

PrintDialog::~PrintDialog()
{
    delete ui;
}

void PrintDialog::addUnita(const QString &unita, const QVariant &id)
{
    ui->unitaCB->addItem(unita, id);
}

void PrintDialog::addMese(const QString &mese, const QVariant &id)
{
    ui->meseCB->addItem(mese, id);
}

void PrintDialog::addDirigente(const QString &dirigente, const QVariant &id)
{
    ui->dirigenteCB->addItem(dirigente, id);
}

void PrintDialog::clearUnita()
{
    ui->unitaCB->clear();
}

void PrintDialog::clearMese()
{
    ui->meseCB->clear();
}

void PrintDialog::clearDirigente()
{
    ui->dirigenteCB->clear();
}

void PrintDialog::mostraDirigenti(bool ok)
{
    proceed = false;
    ui->dirigenteCB->setVisible(ok);
    ui->dirigenteLabel->setVisible(ok);
}

QString PrintDialog::currentMeseData() const
{
    return ui->meseCB->currentData().toString();
}

int PrintDialog::currentUnitaData() const
{
    return ui->unitaCB->currentData().toInt();
}

int PrintDialog::currentDirigenteData() const
{
    return ui->dirigenteCB->currentData().toInt();
}

void PrintDialog::setCurrentMese(int index)
{
    ui->meseCB->setCurrentIndex(index);
}

void PrintDialog::setCurrentUnita(int index)
{
    ui->unitaCB->setCurrentIndex(index);
}

void PrintDialog::setCurrentDirigente(int index)
{
    ui->dirigenteCB->setCurrentIndex(index);
}

QString PrintDialog::path() const
{
    return ui->path->text();
}

void PrintDialog::setPath(const QString &path)
{
    ui->path->setText(path);
}

void PrintDialog::on_cancelButton_clicked()
{
    proceed = false;
    close();
}

void PrintDialog::on_saveButton_clicked()
{
    proceed = true;
    close();
}

void PrintDialog::on_unitaCB_currentIndexChanged(int index)
{
    Q_UNUSED(index)

    if(ui->unitaCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    ui->dirigenteCB->clear();
    ui->dirigenteCB->addItem("Tutti", -1);

    QString whereClause;

    if(ui->unitaCB->currentData(Qt::UserRole).toInt() != -1)
        whereClause = "WHERE " + ui->meseCB->currentData(Qt::UserRole).toString() + ".id_unita=" + ui->unitaCB->currentData(Qt::UserRole).toString();

    QSqlQuery query;
    query.prepare("SELECT medici.id,medici.matricola,medici.nome "
                  "FROM " + ui->meseCB->currentData(Qt::UserRole).toString() + " " +
                  "LEFT JOIN medici " +
                  "ON " + ui->meseCB->currentData(Qt::UserRole).toString() + ".id_medico=medici.id "
                  + whereClause + "  ORDER BY medici.nome;");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return;
    }

    while(query.next()) {
        ui->dirigenteCB->addItem(query.value(1).toString() + " - " + query.value(2).toString(), query.value(0).toString());
    }

//    ui->dirigenteCB->show();
}

void PrintDialog::on_meseCB_currentIndexChanged(int index)
{
    Q_UNUSED(index)

    ui->unitaCB->clear();
    ui->unitaCB->addItem("Tutte", -1);

    if(ui->meseCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    QSqlQuery query;
    query.prepare("SELECT " + ui->meseCB->currentData(Qt::UserRole).toString() + ".id_unita,unita.nome_full,unita.numero "
                  "FROM " + ui->meseCB->currentData(Qt::UserRole).toString() + " "
                  "LEFT JOIN unita "
                  "ON " + ui->meseCB->currentData(Qt::UserRole).toString() + ".id_unita=unita.id ORDER BY length(unita.numero), unita.numero;");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return;
    }

    QStringList list;

    while(query.next()) {
        if(!list.contains(query.value(1).toString())) {
            ui->unitaCB->addItem(query.value(2).toString() + " - " + query.value(1).toString(), query.value(0).toString());
            list << query.value(1).toString();
        }
    }
}

void PrintDialog::on_browseButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory( this,
                                                     trUtf8( "Seleziona destinazione" ),
                                                     ui->path->text().isEmpty() ? QDir::homePath() : ui->path->text(),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

    // be sure that a valid path was selected
    if( QFile::exists( dir ) )
        ui->path->setText( dir );
}
