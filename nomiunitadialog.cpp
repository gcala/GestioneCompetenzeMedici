#include "nomiunitadialog.h"
#include "ui_nomiunitadialog.h"
#include "sqlqueries.h"

#include <QtWidgets>

NomiUnitaDialog::NomiUnitaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NomiUnitaDialog)
{
    ui->setupUi(this);
    populateUnits();
}

NomiUnitaDialog::~NomiUnitaDialog()
{
    delete ui;
}

void NomiUnitaDialog::populateUnits()
{
    ui->unitaCB->clear();
    const QMap<int, QString> map = SqlQueries::units();
    QMap<int, QString>::const_iterator i = map.constBegin();
    while (i != map.constEnd()) {
        ui->unitaCB->addItem(QString::number(i.key()) + " - " + i.value(), i.key());
        ++i;
    }
}

void NomiUnitaDialog::setUnitaLabel(const QString &name)
{
    ui->unitaLabel->setText(name);
}

void NomiUnitaDialog::on_okButton_clicked()
{
    SqlQueries::appendOtherUnitaName(ui->unitaCB->currentData(Qt::UserRole).toInt(), ui->unitaLabel->text());
    close();
}
