/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "manageunits.h"
#include "ui_manageunits.h"
//#include "sqlqueries.h"
#include "sqldatabasemanager.h"
#include "insertdbvalues.h"
#include "apiservice.h"

#include <QSqlTableModel>
#include <QDebug>

ManageUnits::ManageUnits(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManageUnits)
{
    ui->setupUi(this);
    m_changed = false;
    m_modelChanged = false;

    QSqlTableModel *model = new QSqlTableModel(this, The::dbManager()->currentDatabase());
    QObject::connect(model,
                     SIGNAL(dataChanged(const QModelIndex, const QModelIndex, const QVector<int>)),
                     this, SLOT(modelloCambiato(const QModelIndex, const QModelIndex, const QVector<int>)));
    ui->tableView->setModel(model);
    ui->tableView->show();
    static_cast <QSqlTableModel*>(ui->tableView->model())->setTable("unita_ore_pagate");
    static_cast <QSqlTableModel*>(ui->tableView->model())->setEditStrategy(QSqlTableModel::OnManualSubmit);
    static_cast <QSqlTableModel*>(ui->tableView->model())->setHeaderData(2, Qt::Horizontal, QObject::tr("Da Mese"));
    static_cast <QSqlTableModel*>(ui->tableView->model())->setHeaderData(3, Qt::Horizontal, QObject::tr("Ore Totali"));
    static_cast <QSqlTableModel*>(ui->tableView->model())->setHeaderData(4, Qt::Horizontal, QObject::tr("Ore Pagate"));
    ui->tableView->setItemDelegateForColumn(2,&delegate);

    populateUnita();
}

ManageUnits::~ManageUnits()
{
    delete ui;
}

void ManageUnits::setUnit(const int id)
{
    const int index = ui->unitaComboBox->findData(id, Qt::UserRole);
    if(index > 0) {
        ui->unitaComboBox->setCurrentIndex(index);
        ui->unitaComboBox->show();
    }
}

bool ManageUnits::isChanged() const
{
    return m_changed;
}

void ManageUnits::populateUnita()
{
    const int currData = ui->unitaComboBox->currentData(Qt::UserRole).toInt();
    ui->unitaComboBox->clear();
    // const QMap<int, QString> map = SqlQueries::units();
    // QMap<int, QString>::const_iterator i = map.constBegin();
    // while (i != map.constEnd()) {
    //     ui->unitaComboBox->addItem(QString::number(i.key()) + " - " + i.value(), i.key());
    //     ++i;
    // }

    QString errorMsg;
    const auto units = ApiService::instance().getUnits(&errorMsg);

    if (!errorMsg.isEmpty()) {
        qDebug() << "Errore nel recupero delle unità:" << errorMsg;
        return;
    }

    // Ordiniamo le chiavi per avere un ordine consistente
    QList<int> keys = units.keys();
    std::sort(keys.begin(), keys.end());

    for (const int& key : keys) {
        ui->unitaComboBox->addItem(QString::number(key) + " - " + units.value(key), key);
    }
    const int index = ui->unitaComboBox->findData(currData, Qt::UserRole);
    if(index > 0)
        ui->unitaComboBox->setCurrentIndex(index);
    else
        ui->unitaComboBox->setCurrentIndex(0);
    ui->unitaComboBox->show();
}

void ManageUnits::populateUnitaOrePagate()
{
    ui->removeUnitaOrePagateButton->setEnabled(false);

    static_cast <QSqlTableModel*>(ui->tableView->model())->setFilter("id_unita=" + ui->unitaComboBox->currentData(Qt::UserRole).toString());
    static_cast <QSqlTableModel*>(ui->tableView->model())->select();
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(1);
    ui->tableView->show();
}

void ManageUnits::checkChanged()
{
    bool ok = false;
    if(m_nome != ui->unitaNomeLE->text().trimmed())
        ok = true;

    if(m_breve != ui->unitaBreveLE->text().trimmed())
        ok = true;

    if(m_raggruppamento != ui->raggrLE->text().trimmed())
        ok = true;

    if(m_modelChanged)
        ok = true;

    ui->saveButton->setEnabled(ok);
    ui->restoreButton->setEnabled(ok);
    ui->unitaComboBox->setEnabled(!ok);
}

void ManageUnits::on_unitaComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index)

    if(ui->unitaComboBox->currentData(Qt::UserRole).toString().isEmpty())
        return;

    const auto unita = ApiService::instance().getUnitaDataById(ui->unitaComboBox->currentData(Qt::UserRole).toInt());
    if(unita.id > 0 ) {
        ui->unitaNomeLE->setText(unita.nome);
        ui->unitaBreveLE->setText(unita.breve);
        ui->raggrLE->setText(unita.raggruppamento);
        ui->unitaNumLE->setText(QString::number(unita.id));
        m_nome = unita.nome;
        m_breve = unita.breve;
        m_raggruppamento = unita.raggruppamento;
        m_numero = QString::number(unita.id);
    }

    populateUnitaOrePagate();
    checkChanged();
}

void ManageUnits::on_unitaNomeLE_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    checkChanged();
}

void ManageUnits::on_unitaBreveLE_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    checkChanged();
}

void ManageUnits::on_raggrLE_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    checkChanged();
}

void ManageUnits::on_tableView_activated(const QModelIndex &index)
{
    Q_UNUSED(index)
    ui->removeUnitaOrePagateButton->setEnabled(true);
}

void ManageUnits::modelloCambiato(const QModelIndex &topLeft,
                                  const QModelIndex &bottomRight,
                                  const QVector<int> &roles)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    Q_UNUSED(roles)
    m_modelChanged = true;
    checkChanged();
}

void ManageUnits::on_addUnitaOrePagateButton_clicked()
{
    static_cast <QSqlTableModel*>(ui->tableView->model())->insertRows(static_cast <QSqlTableModel*>(ui->tableView->model())->rowCount(), 1);
    QModelIndex index = static_cast <QSqlTableModel*>(ui->tableView->model())->index(static_cast <QSqlTableModel*>(ui->tableView->model())->rowCount()-1, 1, QModelIndex());
    static_cast <QSqlTableModel*>(ui->tableView->model())->setData(index, ui->unitaComboBox->currentData(Qt::UserRole).toInt());
}

void ManageUnits::on_removeUnitaOrePagateButton_clicked()
{
    static_cast <QSqlTableModel*>(ui->tableView->model())->removeRows(ui->tableView->currentIndex().row(), 1);
}

void ManageUnits::on_closeButton_clicked()
{
    close();
}

void ManageUnits::on_restoreButton_clicked()
{
    ui->unitaNomeLE->setText(m_nome);
    ui->unitaBreveLE->setText(m_breve);
    ui->raggrLE->setText(m_raggruppamento);
    populateUnitaOrePagate();
    m_modelChanged = false;
    checkChanged();
}

void ManageUnits::on_saveButton_clicked()
{
    ApiService::instance().editUnit(m_numero,
                                    ui->raggrLE->text().trimmed(),
                                    ui->unitaNomeLE->text().trimmed(),
                                    ui->unitaBreveLE->text().trimmed());

    ui->tableView->selectRow(ui->tableView->currentIndex().row());
    static_cast <QSqlTableModel*>(ui->tableView->model())->submitAll();

    m_changed = true;
    m_modelChanged = false;
    m_nome = ui->unitaNomeLE->text().trimmed();
    m_breve = ui->unitaBreveLE->text().trimmed();
    m_raggruppamento = ui->raggrLE->text().trimmed();
    populateUnita();
    checkChanged();
}
