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
}
