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

#include "manageunits.h"
#include "ui_manageunits.h"
#include "sqlqueries.h"
#include "sqldatabasemanager.h"
#include "insertdbvalues.h"

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
    const QMap<int, QString> map = SqlQueries::units();
    QMap<int, QString>::const_iterator i = map.constBegin();
    while (i != map.constEnd()) {
        ui->unitaComboBox->addItem(QString::number(i.key()) + " - " + i.value(), i.key());
        ++i;
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

    const QVariantList query = SqlQueries::getUnitaDataById(ui->unitaComboBox->currentData(Qt::UserRole).toInt());

    if(!query.isEmpty() && query.size() == 5 ) {
        ui->unitaNomeLE->setText(query.at(2).toString());
        ui->unitaBreveLE->setText(query.at(3).toString());
        ui->raggrLE->setText(query.at(1).toString());
        ui->unitaNumLE->setText(query.at(0).toString());
        m_nome = query.at(2).toString();
        m_breve = query.at(3).toString();
        m_raggruppamento = query.at(1).toString();
        m_numero = query.at(0).toString();
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
    SqlQueries::editUnit(m_numero,
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
