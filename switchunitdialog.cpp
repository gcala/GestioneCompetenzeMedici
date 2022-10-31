#include "switchunitdialog.h"
#include "ui_switchunitdialog.h"
#include "sqlqueries.h"

#include <algorithm>
#include <QtWidgets>
#include <QLocale>

SwitchUnitDialog::SwitchUnitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SwitchUnitDialog)
{
    ui->setupUi(this);

    m_dirigenti1Model = ui->dirigenti1List->model();
    m_dirigenti2Model = ui->dirigenti2List->model();

    connect(m_dirigenti1Model, &QAbstractItemModel::rowsRemoved, this, &SwitchUnitDialog::listsChanged);
    connect(m_dirigenti1Model, &QAbstractItemModel::rowsInserted, this, &SwitchUnitDialog::listsChanged);

    connect(m_dirigenti2Model, &QAbstractItemModel::rowsRemoved, this, &SwitchUnitDialog::listsChanged);
    connect(m_dirigenti2Model, &QAbstractItemModel::rowsInserted, this, &SwitchUnitDialog::listsChanged);

    populateMeseCompetenzeCB();
    populateUnitaCompetenzeCB();

    enableButtons();
    enableLists();
    enableComboBoxes();
}

SwitchUnitDialog::~SwitchUnitDialog()
{
    delete m_dirigenti1Model;
    delete m_dirigenti2Model;
    delete ui;
}

void SwitchUnitDialog::closeEvent(QCloseEvent *event)
{
    if(ui->saveButton->isEnabled()) {
        QMessageBox::StandardButton answer = QMessageBox::question(this, "Cambia Unità - Modifche non salvate", "Alcune modifiche non sono state salvate. Decidendo di chiudere andranno perse.\nProcedo alla chiusura?", QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
        if(answer == QMessageBox::No) {
            event->ignore();
        } else {
            event->accept();
        }
    }
}

void SwitchUnitDialog::populateMeseCompetenzeCB()
{
    ui->meseCompetenzeCB->clear();

    const QStringList timeCards = SqlQueries::timecardsList();

    QStringList::const_iterator i = timeCards.constEnd();

    while(i != timeCards.constBegin()) {
        --i;
        QString ss = (*i).split("_").last();
        QLocale locale;
        ui->meseCompetenzeCB->addItem(locale.monthName((*i).right(2).toInt(), QLocale::LongFormat) + " " + ss.left(4), *i);
    }
}

void SwitchUnitDialog::populateUnitaCompetenzeCB()
{
    ui->unitaCompetenze1CB->clear();
    ui->unitaCompetenze2CB->clear();

    if(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    QStringList query = SqlQueries::getUnitaDataFromTimecard(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString());

    QStringList list;

    for(const QString &s : query) {
        QStringList l = s.split("~");
        if(!list.contains(l.at(1))) {
            ui->unitaCompetenze1CB->addItem(l.at(2) + " - " + l.at(1), l.at(0));
            ui->unitaCompetenze2CB->addItem(l.at(2) + " - " + l.at(1), l.at(0));
            list << l.at(1);
        }
    }
}

void SwitchUnitDialog::enableButtons()
{
    if(m_dirigenti1Current != m_dirigenti1Initial || m_dirigenti2Current != m_dirigenti2Initial) {
        ui->saveButton->setEnabled(true);
        ui->restoreButton->setEnabled(true);
    } else {
        ui->saveButton->setEnabled(false);
        ui->restoreButton->setEnabled(false);
    }
}

void SwitchUnitDialog::enableComboBoxes()
{
    if(m_dirigenti1Current != m_dirigenti1Initial || m_dirigenti2Current != m_dirigenti2Initial) {
        ui->meseCompetenzeCB->setEnabled(false);
        ui->unitaCompetenze1CB->setEnabled(false);
        ui->unitaCompetenze2CB->setEnabled(false);
    } else {
        ui->meseCompetenzeCB->setEnabled(true);
        ui->unitaCompetenze1CB->setEnabled(true);
        ui->unitaCompetenze2CB->setEnabled(true);
    }
}

void SwitchUnitDialog::enableLists()
{
    if(ui->unitaCompetenze1CB->currentData(Qt::UserRole).toString() == ui->unitaCompetenze2CB->currentData(Qt::UserRole).toString()) {
        ui->dirigenti1List->setEnabled(false);
        ui->dirigenti2List->setEnabled(false);
    } else {
        ui->dirigenti1List->setEnabled(true);
        ui->dirigenti2List->setEnabled(true);
    }
}

void SwitchUnitDialog::on_meseCompetenzeCB_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    const QString unita1Data = ui->unitaCompetenze1CB->currentData(Qt::UserRole).toString();
    const QString unita2Data = ui->unitaCompetenze2CB->currentData(Qt::UserRole).toString();

    populateUnitaCompetenzeCB();

    const int unita1Index = ui->unitaCompetenze1CB->findData(unita1Data, Qt::UserRole);
    const int unita2Index = ui->unitaCompetenze2CB->findData(unita2Data, Qt::UserRole);

    if(unita1Index >= 0) {
        ui->unitaCompetenze1CB->setCurrentIndex(unita1Index);
        ui->unitaCompetenze1CB->show();
    }

    if(unita2Index >= 0) {
        ui->unitaCompetenze2CB->setCurrentIndex(unita2Index);
        ui->unitaCompetenze2CB->show();
    }

    enableButtons();
    enableLists();
}

void SwitchUnitDialog::on_unitaCompetenze1CB_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ui->dirigenti1List->clear();
    m_dirigenti1Initial.clear();
    m_dirigenti1Current.clear();

    if(ui->unitaCompetenze1CB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    QStringList query = SqlQueries::getDoctorDataFromUnitaInTimecard(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString(), ui->unitaCompetenze1CB->currentData(Qt::UserRole).toInt());

    for(const QString &s : query) {
        QStringList l = s.split("~");
        m_dirigenti1Initial << l.at(1);
        m_dirigenti1Current << l.at(1);
        ui->dirigenti1List->addItem(l.at(1) + " - " + l.at(2));
    }

    std::sort(m_dirigenti1Initial.begin(), m_dirigenti1Initial.end());
    std::sort(m_dirigenti1Current.begin(), m_dirigenti1Current.end());

    enableButtons();
    enableLists();
    enableComboBoxes();
}

void SwitchUnitDialog::on_unitaCompetenze2CB_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ui->dirigenti2List->clear();
    m_dirigenti2Initial.clear();
    m_dirigenti2Current.clear();

    if(ui->unitaCompetenze1CB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    QStringList query = SqlQueries::getDoctorDataFromUnitaInTimecard(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString(), ui->unitaCompetenze2CB->currentData(Qt::UserRole).toInt());

    for(const QString &s : query) {
        QStringList l = s.split("~");
        m_dirigenti2Initial << l.at(1);
        m_dirigenti2Current << l.at(1);
        ui->dirigenti2List->addItem(l.at(1) + " - " + l.at(2));
    }

    std::sort(m_dirigenti2Initial.begin(), m_dirigenti2Initial.end());
    std::sort(m_dirigenti2Current.begin(), m_dirigenti2Current.end());

    enableButtons();
    enableLists();
    enableComboBoxes();
}

void SwitchUnitDialog::listsChanged()
{
    m_dirigenti1Current.clear();
    m_dirigenti2Current.clear();
    for(int i = 0; i < ui->dirigenti1List->count(); i++) {
        m_dirigenti1Current << ui->dirigenti1List->item(i)->text().split("-").first().trimmed();
    }
    for(int i = 0; i < ui->dirigenti2List->count(); i++) {
        m_dirigenti2Current << ui->dirigenti2List->item(i)->text().split("-").first().trimmed();
    }

    std::sort(m_dirigenti1Current.begin(), m_dirigenti1Current.end());
    std::sort(m_dirigenti2Current.begin(), m_dirigenti2Current.end());

    enableButtons();
    enableComboBoxes();
}

void SwitchUnitDialog::on_restoreButton_clicked()
{
    on_unitaCompetenze1CB_currentIndexChanged(0);
    on_unitaCompetenze2CB_currentIndexChanged(0);
}

void SwitchUnitDialog::on_saveButton_clicked()
{
    Q_FOREACH(const QString &s, m_dirigenti1Initial) {
        if(m_dirigenti1Current.contains(s))
            continue;
        SqlQueries::saveMod(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString(), "id_unita", SqlQueries::doctorId(s), ui->unitaCompetenze2CB->currentData());
        SqlQueries::setUnitaMedico(SqlQueries::doctorId(s), ui->unitaCompetenze2CB->currentData().toInt());
    }

    Q_FOREACH(const QString &s, m_dirigenti2Initial) {
        if(m_dirigenti2Current.contains(s))
            continue;
        SqlQueries::saveMod(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString(), "id_unita", SqlQueries::doctorId(s), ui->unitaCompetenze1CB->currentData());
        SqlQueries::setUnitaMedico(SqlQueries::doctorId(s), ui->unitaCompetenze1CB->currentData().toInt());
    }

    on_unitaCompetenze1CB_currentIndexChanged(0);
    on_unitaCompetenze2CB_currentIndexChanged(0);
}
