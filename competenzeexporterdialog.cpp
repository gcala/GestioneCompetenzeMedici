/*
    SPDX-FileCopyrightText: 2023 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "competenzeexporterdialog.h"
#include "ui_competenzeexporterdialog.h"
#include "utilities.h"
#include "sqlqueries.h"

#include <QFileDialog>

CompetenzeExporterDialog::CompetenzeExporterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompetenzeExporterDialog)
{
    ui->setupUi(this);

    connect(ui->saveButton, &QPushButton::clicked, [=] () {
        proceed = true;
        close();
    });
    connect(ui->cancelButton, &QPushButton::clicked, [=] () {
        proceed = false;
        close();
    });

    ui->unitaCB->addItem("Tutte", -1);
}

CompetenzeExporterDialog::~CompetenzeExporterDialog()
{
    delete ui;
}

void CompetenzeExporterDialog::clearMese()
{
    ui->daMeseCB->clear();
    ui->aMeseCB->clear();
}

void CompetenzeExporterDialog::addMese(const QString &mese, const QVariant &id)
{
    ui->daMeseCB->addItem(mese, id);
    ui->aMeseCB->addItem(mese, id);
}

void CompetenzeExporterDialog::setCurrentMese(int index)
{
    ui->daMeseCB->setCurrentIndex(index);
    ui->aMeseCB->setCurrentIndex(index);
}

void CompetenzeExporterDialog::setCurrentUnita(int index)
{
    ui->unitaCB->setCurrentIndex(index);
    QStringList query = SqlQueries::getUnitaDataAll();

    QStringList list;

    for(const QString &s : query) {
        QStringList l = s.split("~");
        if(!list.contains(l.at(1))) {
            ui->unitaCB->addItem(l.at(0) + " - " + l.at(1), l.at(0));
            list << l.at(1);
        }
    }
}

void CompetenzeExporterDialog::setPath(const QString &path)
{
    ui->path->setText(path);
}

QString CompetenzeExporterDialog::path() const
{
    return ui->path->text();
}

int CompetenzeExporterDialog::currentUnitaData() const
{
    return ui->unitaCB->currentData().toInt();
}

QString CompetenzeExporterDialog::meseDa() const
{
    return ui->daMeseCB->currentData().toString();
}

QString CompetenzeExporterDialog::meseA() const
{
    return ui->aMeseCB->currentData().toString();
}

QVector<int> CompetenzeExporterDialog::matricole() const
{
    return Utilities::stringlistToVectorInt(ui->matricoleLine->text().trimmed().split(' ', Qt::SkipEmptyParts));
}

void CompetenzeExporterDialog::browseButtonClicked()
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
