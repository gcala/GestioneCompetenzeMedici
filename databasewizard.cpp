#include "databasewizard.h"
#include "ui_databasewizard.h"
#include "sqldatabasemanager.h"
#include "sqlqueries.h"

#include <QtWidgets>
#include <QDebug>

DatabaseWizard::DatabaseWizard(int page, QWidget *parent)
    : m_startPage(page)
    , QDialog(parent)
    , ui(new Ui::DatabaseWizard)
{
    ui->setupUi(this);
    ui->createButton->setEnabled(false);
    ui->remoteDbSave->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(m_startPage);
    m_openDb = false;
    m_canceled = true;
    m_revealPass = false;
}

DatabaseWizard::~DatabaseWizard()
{
    delete ui;
}

bool DatabaseWizard::openDb() const
{
    return m_openDb;
}

bool DatabaseWizard::canceled() const
{
    return m_canceled;
}

QString DatabaseWizard::host() const
{
    return ui->hostLine->text().trimmed();
}

QString DatabaseWizard::database() const
{
    return ui->dbLine->text().trimmed();
}

QString DatabaseWizard::user() const
{
    return ui->userLine->text().trimmed();
}

QString DatabaseWizard::password() const
{
    return ui->passLine->text().trimmed();
}

void DatabaseWizard::on_destBrowse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Scegli destinazione database"),
                                                     QDir::homePath(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);

    if(QFile::exists(dir)) {
        ui->dbDest->setText(dir);
    } else {
        ui->dbDest->clear();
    }
}

void DatabaseWizard::on_unitsBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Seleziona file delle Unità Operative"),
                                                   QApplication::applicationDirPath(), tr("File CSV (*.csv)"));

    // be sure that a valid path was selected
    if( QFile::exists( fileName ) ) {
        ui->unitsCSV->setText(fileName);
    } else {
        ui->unitsCSV->clear();
    }
}

void DatabaseWizard::on_payloadBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Seleziona file delle Ore Pagate"),
                                                   QApplication::applicationDirPath(), tr("File CSV (*.csv)"));

    // be sure that a valid path was selected
    if( QFile::exists( fileName ) ) {
        ui->payloadCSV->setText(fileName);
    } else {
        ui->payloadCSV->clear();
    }
}

void DatabaseWizard::on_repsBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Seleziona file delle Reperibilità"),
                                                   QApplication::applicationDirPath(), tr("File CSV (*.csv)"));

    // be sure that a valid path was selected
    if( QFile::exists( fileName ) ) {
        ui->repsCSV->setText(fileName);
    } else {
        ui->repsCSV->clear();
    }
}

void DatabaseWizard::on_cancelNewButton_clicked()
{
    m_canceled = true;
    close();
}

void DatabaseWizard::on_createButton_clicked()
{
    QString file = ui->dbDest->text() + QDir::separator() + ui->dbName->text().trimmed() + ".db";

    if(file == The::dbManager()->currentDatabase()) {
        QMessageBox::critical(this, "Database in uso", "Si sta tentando di sovrascrivere un database aperto. "
                                                       "Si prega di selezionare un altro nome per il nuovo database, "
                                                       "oppure chiudere l'applicazione ed eliminare "
                                                       "manualmente il vecchio database!",QMessageBox::Cancel);
        return;
    }

    if(QFile::exists(file)) {
        QMessageBox msgBox;
        msgBox.setText("Il database \n" + file + "\nesiste.");
        msgBox.setInformativeText("Sicuri di volerlo sovrascivere?");
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Save );
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Save) {
            QFile::remove(file);
        } else {
            return;
        }
    }

    qDebug() << "Creo un nuovo database" << file;
    if(!The::dbManager()->createLocalConnection(file))
        return;

    SqlQueries::createUnitsTable();
    SqlQueries::createUnitsPayedHoursTable();
    SqlQueries::createDoctorsTable();
    SqlQueries::createUnitsRepTable();

    if(insertUnitsFromFile(ui->unitsCSV->text())) {
        insertPayloadFromFile(ui->payloadCSV->text());
        insertRepsFromFile(ui->repsCSV->text());
    }

    close();
}

void DatabaseWizard::on_dbName_textChanged(const QString &arg1)
{
    if(!arg1.isEmpty() && !ui->dbDest->text().isEmpty())
        ui->createButton->setEnabled(true);
    else
        ui->createButton->setEnabled(false);
}

void DatabaseWizard::on_dbDest_textChanged(const QString &arg1)
{
    if(!arg1.isEmpty() && !ui->dbName->text().isEmpty())
        ui->createButton->setEnabled(true);
    else
        ui->createButton->setEnabled(false);
}

bool DatabaseWizard::insertUnitsFromFile(const QString &file)
{
    if(file.isEmpty() || !QFile::exists(file))
        return false;

    QFile f(file);

    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    while(!f.atEnd()) {
        QString line = f.readLine().trimmed();
        if(line.trimmed().isEmpty())
            continue;
        QStringList list = line.split("|");
        if(list.size() != 5) {
            qDebug() << "ERRORE alla riga" << line;
            continue;
        }

        SqlQueries::insertUnit(list.at(0), list.at(1), list.at(2), list.at(3), list.at(4));
    }

    f.close();
    return true;
}

bool DatabaseWizard::insertPayloadFromFile(const QString &file)
{
    if(file.isEmpty() || !QFile::exists(file))
        return false;

    QFile f(file);

    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    while(!f.atEnd()) {
        QString line = f.readLine().trimmed();
        if(line.trimmed().isEmpty())
            continue;
        QStringList list = line.split("|");
        if(list.size() != 4) {
            qDebug() << "ERRORE alla riga" << line;
            continue;
        }

        SqlQueries::insertPayload(list.at(0), list.at(1), list.at(2), list.at(3));
    }

    f.close();
    return true;
}

bool DatabaseWizard::insertRepsFromFile(const QString &file)
{
    if(file.isEmpty() || !QFile::exists(file))
        return false;

    QFile f(file);

    if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    while(!f.atEnd()) {
        QString line = f.readLine().trimmed();
        if(line.trimmed().isEmpty())
            continue;
        QStringList list = line.split("|");
        if(list.size() != 6) {
            qDebug() << "ERRORE alla riga" << line;
            continue;
        }

        SqlQueries::insertRep(list.at(0), list.at(1), list.at(2), list.at(3), list.at(4), list.at(5));
    }

    f.close();
    return true;
}

void DatabaseWizard::on_openLocalDbButton_clicked()
{
    m_openDb = true;
    m_canceled = false;
    close();
}

void DatabaseWizard::on_openRemoteDbButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void DatabaseWizard::on_hostLine_textChanged(const QString &arg1)
{
    if(!arg1.trimmed().isEmpty() && !ui->dbLine->text().trimmed().isEmpty() && !ui->userLine->text().trimmed().isEmpty() && !ui->passLine->text().trimmed().isEmpty()) {
        ui->remoteDbSave->setEnabled(true);
    } else {
        ui->remoteDbSave->setEnabled(false);
    }
}

void DatabaseWizard::on_dbLine_textChanged(const QString &arg1)
{
    if(!arg1.trimmed().isEmpty() && !ui->hostLine->text().trimmed().isEmpty() && !ui->userLine->text().trimmed().isEmpty() && !ui->passLine->text().trimmed().isEmpty()) {
        ui->remoteDbSave->setEnabled(true);
    } else {
        ui->remoteDbSave->setEnabled(false);
    }
}

void DatabaseWizard::on_userLine_textChanged(const QString &arg1)
{
    if(!arg1.trimmed().isEmpty() && !ui->hostLine->text().trimmed().isEmpty() && !ui->dbLine->text().trimmed().isEmpty() && !ui->passLine->text().trimmed().isEmpty()) {
        ui->remoteDbSave->setEnabled(true);
    } else {
        ui->remoteDbSave->setEnabled(false);
    }
}

void DatabaseWizard::on_passLine_textChanged(const QString &arg1)
{
    if(!arg1.trimmed().isEmpty() && !ui->hostLine->text().trimmed().isEmpty() && !ui->userLine->text().trimmed().isEmpty() && !ui->dbLine->text().trimmed().isEmpty()) {
        ui->remoteDbSave->setEnabled(true);
    } else {
        ui->remoteDbSave->setEnabled(false);
    }
}

void DatabaseWizard::on_remoteDbCancel_clicked()
{
    m_canceled = true;
    close();
}

void DatabaseWizard::on_remoteDbBack_clicked()
{
    ui->stackedWidget->setCurrentIndex(m_startPage);
}

void DatabaseWizard::on_remoteDbSave_clicked()
{
    m_canceled = false;
    close();
}

void DatabaseWizard::on_cancelOpenButton_clicked()
{
    m_canceled = true;
    close();
}

void DatabaseWizard::on_revealButton_clicked()
{
    m_revealPass = !m_revealPass;
    if(m_revealPass) {
        ui->revealButton->setIcon(QIcon(":/icons/password-show-on.svg"));
        ui->passLine->setEchoMode(QLineEdit::Normal);
    } else {
        ui->revealButton->setIcon(QIcon(":/icons/password-show-off.svg"));
        ui->passLine->setEchoMode(QLineEdit::Password);
    }
}
