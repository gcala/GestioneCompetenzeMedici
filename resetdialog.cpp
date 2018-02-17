/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017-2018 Giuseppe Calà <giuseppe.cala@mailbox.org>
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

#include "resetdialog.h"
#include "ui_resetdialog.h"
#include "sqlqueries.h"
#include "competenza.h"

#include <QtWidgets>
#include <QDebug>

ResetDialog::ResetDialog(const QString &tableName, const int &id, QWidget *parent)
    : QDialog(parent)
    , m_tableName(tableName)
    , m_id(id)
    , ui(new Ui::ResetDialog)
{
    ui->setupUi(this);
}

ResetDialog::ResetDialog(Competenza *competenza, QWidget *parent)
    : QDialog(parent)
    , m_competenza(competenza)
    , ui(new Ui::ResetDialog)
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
        SqlQueries::resetAllDoctorMods(m_tableName,m_id);
    } else {
        if(ui->gdiurneCB->isChecked()) {
            SqlQueries::resetStringValue(m_tableName, "guardie_diurne", m_id);
        }

        if(ui->orarioGiornoCB->isChecked()) {
            SqlQueries::resetIntValue(m_tableName, "orario_giornaliero", m_id);
        }

        if(ui->gnotturneCB->isChecked()) {
            SqlQueries::resetStringValue(m_tableName, "guardie_notturne", m_id);
        }

        if(ui->repCB->isChecked()) {
            SqlQueries::resetStringValue(m_tableName, "turni_reperibilita", m_id);
        }

        if(ui->dmpCB->isChecked()) {
            SqlQueries::resetIntValue(m_tableName, "dmp", m_id);
        }

        if(ui->altreCB->isChecked()) {
            SqlQueries::resetStringValue(m_tableName, "altre_assenze", m_id);
        }

        if(ui->noteCB->isChecked()) {
            SqlQueries::resetStringValue(m_tableName, "nota", m_id);
        }
    }

    close();
}

void ResetDialog::on_tuttoCB_toggled(bool checked)
{
    if(!checked) {
        enableItems();
    } else {
        ui->gdiurneCB->setEnabled(false);
        ui->gnotturneCB->setEnabled(false);
        ui->repCB->setEnabled(false);
        ui->dmpCB->setEnabled(false);
        ui->altreCB->setEnabled(false);
        ui->noteCB->setEnabled(false);
        ui->orarioGiornoCB->setEnabled(false);
    }
}

void ResetDialog::enableItems()
{
    ui->gdiurneCB->setEnabled(m_competenza->isGuardieDiurneModded());
    ui->gnotturneCB->setEnabled(m_competenza->isGuardieNotturneModded());
    ui->repCB->setEnabled(m_competenza->isReperibilitaModded());
    ui->dmpCB->setEnabled(m_competenza->isDmpModded());
    ui->altreCB->setEnabled(m_competenza->isAltreModded());
    ui->noteCB->setEnabled(m_competenza->isNoteModded());
    ui->orarioGiornoCB->setEnabled(m_competenza->isOrarioGiornalieroModded());
}
