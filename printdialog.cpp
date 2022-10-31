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

#include "printdialog.h"
#include "ui_printdialog.h"
#include "sqlqueries.h"

#include <QtWidgets>
#include <QFileDialog>
#include <QDebug>

PrintDialog::PrintDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrintDialog)
{
    ui->setupUi(this);
    ui->casiCB->setChecked(true);
    ui->saveButton->addAction(ui->actionGeneraCSV);
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

void PrintDialog::setCurrentOp(const PrintDialog::ToolOps &op)
{
    m_currOp = op;
    proceed = false;

    switch (m_currOp) {
    case PrintUnits:
        ui->titleLabel->setText("Genera competenze Unità Operativa");
        setWindowTitle("Stampa competenze");
        ui->saveButton->setIcon(QIcon(":/icons/pdflatex.svg"));
        ui->saveButton->setText("Genera PDF");
        ui->destWidget->setVisible(true);
        ui->optnsWidget->setVisible(true);
        ui->dirigenteCB->setVisible(false);
        ui->dirigenteLabel->setVisible(false);
        adjustSize();
        break;
    case PrintDoctors:
        ui->titleLabel->setText("Genera competenze Dirigenti");
        setWindowTitle("Stampa competenze");
        ui->saveButton->setIcon(QIcon(":/icons/pdflatex.svg"));
        ui->saveButton->setText("Genera PDF");
        ui->destWidget->setVisible(true);
        ui->optnsWidget->setVisible(false);
        ui->dirigenteCB->setVisible(true);
        ui->dirigenteLabel->setVisible(true);
        adjustSize();
        break;
    case PrintDeficit:
        ui->titleLabel->setText("Stampa Deficit");
        setWindowTitle("Stampa Deficit");
        ui->saveButton->setText("Genera PDF");
        ui->saveButton->setIcon(QIcon(":/icons/pdflatex.svg"));
        ui->destWidget->setVisible(true);
        ui->optnsWidget->setVisible(false);
        ui->dirigenteCB->setVisible(false);
        ui->dirigenteLabel->setVisible(false);
        adjustSize();
        break;
    default:
        ui->titleLabel->setText("Ricalcola Deficit");
        setWindowTitle("Ricalcola Deficit");
        ui->saveButton->setText("Ricalcola");
        ui->saveButton->setIcon(QIcon(":/icons/view-refresh.svg"));
        ui->destWidget->setVisible(false);
        ui->optnsWidget->setVisible(false);
        ui->dirigenteCB->setVisible(true);
        ui->dirigenteLabel->setVisible(true);
        adjustSize();
        break;
    }
}

PrintDialog::ToolOps PrintDialog::currentOp() const
{
    return m_currOp;
}

bool PrintDialog::casiIsChecked() const
{
    return ui->casiCB->isChecked();
}

bool PrintDialog::dataIsChecked() const
{
    return ui->dataCB->isChecked();
}

void PrintDialog::on_cancelButton_clicked()
{
    proceed = false;
    close();
}

void PrintDialog::on_saveButton_clicked()
{
    proceed = true;
    type = "pdf";
    close();
}

void PrintDialog::on_actionGeneraCSV_triggered()
{
    proceed = true;
    type = "csv";
    close();
}

void PrintDialog::on_unitaCB_currentIndexChanged(int index)
{
    Q_UNUSED(index)

    if(ui->unitaCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    ui->dirigenteCB->clear();
    ui->dirigenteCB->addItem("Tutti", -1);

    QStringList query = SqlQueries::getDoctorDataFromUnitaInTimecard(ui->meseCB->currentData(Qt::UserRole).toString(), ui->unitaCB->currentData(Qt::UserRole).toInt());

    for(const QString &s : query) {
        QStringList l = s.split("~");
        ui->dirigenteCB->addItem(l.at(1) + " - " + l.at(2), l.at(0));
    }
}

void PrintDialog::on_meseCB_currentIndexChanged(int index)
{
    Q_UNUSED(index)

    ui->unitaCB->clear();
    ui->unitaCB->addItem("Tutte", -1);

    if(ui->meseCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    QStringList query = SqlQueries::getUnitaDataFromTimecard(ui->meseCB->currentData(Qt::UserRole).toString());

    QStringList list;

    for(const QString &s : query) {
        QStringList l = s.split("~");
        if(!list.contains(l.at(1))) {
            ui->unitaCB->addItem(l.at(2) + " - " + l.at(1), l.at(0));
            list << l.at(1);
        }
    }
}

void PrintDialog::on_browseButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory( this,
                                                     tr( "Seleziona destinazione" ),
                                                     ui->path->text().isEmpty() ? QDir::homePath() : ui->path->text(),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

    // be sure that a valid path was selected
    if( QFile::exists( dir ) )
        ui->path->setText( dir );
}
