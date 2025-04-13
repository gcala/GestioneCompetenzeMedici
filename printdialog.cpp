/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "printdialog.h"
#include "ui_printdialog.h"
//#include "sqlqueries.h"
#include "utilities.h"
#include "apiservice.h"

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

    const auto doctorData = ApiService::instance().getDoctorDataFromUnitaInTimecard(ui->meseCB->currentData(Qt::UserRole).toString(), ui->unitaCB->currentData(Qt::UserRole).toInt());

    for(const auto &doctor : doctorData) {
        ui->dirigenteCB->addItem(QString::number(doctor.matricola) + " - " + doctor.nome, doctor.id);
    }
}

void PrintDialog::on_meseCB_currentIndexChanged(int index)
{
    Q_UNUSED(index)

    ui->unitaCB->clear();
    ui->unitaCB->addItem("Tutte", -1);

    if(ui->meseCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    const auto unitaData = ApiService::instance().getUnitaDataFromTimecard(ui->meseCB->currentData(Qt::UserRole).toString());

    QStringList list;

    for(const auto unita : unitaData) {
        if(!list.contains(unita.nome)) {
            ui->unitaCB->addItem(QString::number(unita.idUnita) + " - " + unita.nome, unita.id);
            list << unita.nome;
        }
    }
}

void PrintDialog::on_browseButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory( this,
                                                     tr( "Seleziona destinazione" ),
                                                     Utilities::m_exportPath,
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

    // be sure that a valid path was selected
    const QFileInfo fi(dir);
    if( fi.exists() ) {
        Utilities::m_exportPath = fi.absoluteFilePath();
        ui->path->setText( dir );
    }
}
