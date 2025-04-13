/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "resetdialog.h"
#include "ui_resetdialog.h"
//#include "sqlqueries.h"
#include "competenza.h"
#include "apiservice.h"

#include <QtWidgets>
#include <QDebug>
#include <utility>

ResetDialog::ResetDialog(QString tableName, const int &id, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
    , m_tableName(std::move(tableName))
    , m_id(id)
{
    ui->setupUi(this);
}

ResetDialog::ResetDialog(Competenza *competenza, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
    , m_competenza(competenza)
{
    ui->setupUi(this);

    m_tableName = m_competenza->modTableName();
    m_id = m_competenza->doctorId();
    enableItems();
}

ResetDialog::~ResetDialog()
{
    delete ui;
}

void ResetDialog::on_cancelButton_clicked()
{
    close();
}

void ResetDialog::on_restoreButton_clicked()
{
    if(m_tableName.isEmpty() || m_id < 0) {
        qDebug() << "ERRORE :: dati mancanti" << m_tableName << m_id;
        close();
    }

    if(ui->tuttoCB->isChecked()) {
        ApiService::instance().resetAllDoctorMods(m_tableName,m_id);
    } else {
        if(ui->repCB->isChecked()) {
            ApiService::instance().resetStringValue(m_tableName, "turni_reperibilita", m_id);
        }

        if(ui->dmpCB->isChecked()) {
            ApiService::instance().resetStringValue(m_tableName, "dmp", m_id);
        }

        if(ui->noteCB->isChecked()) {
            ApiService::instance().resetStringValue(m_tableName, "nota", m_id);
        }

        if(ui->teleconsultoCB->isChecked()) {
            ApiService::instance().resetStringValue(m_tableName, "turni_teleconsulto", m_id);
        }
    }

    close();
}

void ResetDialog::on_tuttoCB_toggled(bool checked)
{
    if(!checked) {
        enableItems();
    } else {
//        ui->gdiurneCB->setEnabled(false);
//        ui->gnotturneCB->setEnabled(false);
        ui->repCB->setEnabled(false);
        ui->dmpCB->setEnabled(false);
        ui->teleconsultoCB->setEnabled(false);
        // ui->altreCB->setEnabled(false);
        ui->noteCB->setEnabled(false);
//        ui->orarioGiornoCB->setEnabled(false);
    }
}

void ResetDialog::enableItems()
{
//    ui->gdiurneCB->setEnabled(m_competenza->isGuardieDiurneModded());
//    ui->gnotturneCB->setEnabled(m_competenza->isGuardieNotturneModded());
    ui->repCB->setEnabled(m_competenza->isReperibilitaModded());
    ui->dmpCB->setEnabled(m_competenza->isDmpModded());
    // ui->altreCB->setEnabled(m_competenza->isAltreModded());
    ui->noteCB->setEnabled(m_competenza->isNoteModded());
    ui->teleconsultoCB->setEnabled(m_competenza->isTeleconsultoModded());
//    ui->orarioGiornoCB->setEnabled(m_competenza->isOrarioGiornalieroModded());
}
