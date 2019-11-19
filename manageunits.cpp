#include "manageunits.h"
#include "ui_manageunits.h"
#include "sqlqueries.h"
#include "sqldatabasemanager.h"
#include "insertdbvalues.h"

#include <QSqlQueryModel>
#include <QDebug>

ManageUnits::ManageUnits(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManageUnits)
{
    ui->setupUi(this);
    m_changed = false;

    insertDialog = new InsertDBValues(this);

    unitaOrePagateModel = new QSqlQueryModel;
    ui->unitaOrePagateTW->setModel(unitaOrePagateModel);

    populateUnita();
}

ManageUnits::~ManageUnits()
{
    delete insertDialog;
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
    SqlQueries::setUnitaOrePagateModel(unitaOrePagateModel, ui->unitaComboBox->currentData(Qt::UserRole).toInt());

    unitaOrePagateModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Da Mese"));
    unitaOrePagateModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Ore totali"));
    unitaOrePagateModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Ore pagate"));
    ui->unitaOrePagateTW->hideColumn(0);
    ui->removeUnitaOrePagateButton->setEnabled(false);
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

void ManageUnits::on_unitaOrePagateTW_activated(const QModelIndex &index)
{
    Q_UNUSED(index)
    ui->removeUnitaOrePagateButton->setEnabled(true);
}

void ManageUnits::on_addUnitaOrePagateButton_clicked()
{
    insertDialog->unitaAddOreSetup(ui->unitaComboBox->currentData(Qt::UserRole).toString());
    insertDialog->exec();
    populateUnitaOrePagate();
}

void ManageUnits::on_removeUnitaOrePagateButton_clicked()
{
    const QAbstractItemModel * model = ui->unitaOrePagateTW->currentIndex().model();
    int id = model->data(model->index(ui->unitaOrePagateTW->currentIndex().row(), 0), Qt::DisplayRole).toInt();
    insertDialog->unitaRemoveOreSetup(id);
    insertDialog->exec();
    populateUnitaOrePagate();
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
    checkChanged();
}

void ManageUnits::on_saveButton_clicked()
{
    SqlQueries::editUnit(m_numero,
                         ui->raggrLE->text().trimmed(),
                         ui->unitaNomeLE->text().trimmed(),
                         ui->unitaBreveLE->text().trimmed());
    m_changed = true;
    m_nome = ui->unitaNomeLE->text().trimmed();
    m_breve = ui->unitaBreveLE->text().trimmed();
    m_raggruppamento = ui->raggrLE->text().trimmed();
    populateUnita();
    checkChanged();
}
