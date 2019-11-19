#include "manageemployee.h"
#include "ui_manageemployee.h"
#include "sqlqueries.h"

ManageEmployee::ManageEmployee(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManageEmployee)
{
    ui->setupUi(this);

    populateDirigenti();

    m_changed = false;

    on_dirigentiComboBox_currentIndexChanged(0);
}

ManageEmployee::~ManageEmployee()
{
    delete ui;
}

void ManageEmployee::setDipendente(const int matricola)
{
//    const int index = ui->dirigentiComboBox->findData(matricola, Qt::UserRole);
//    if(index > 0) {
//        ui->dirigentiComboBox->setCurrentIndex(index);
//        ui->dirigentiComboBox->show();
//    }
    for(int i = 0; i < ui->dirigentiComboBox->count(); i++) {
        if(matricola == ui->dirigentiComboBox->itemText(i).split("-").at(0).trimmed().toInt()) {
            ui->dirigentiComboBox->setCurrentIndex(i);
            ui->dirigentiComboBox->show();
            break;
        }
    }
}

bool ManageEmployee::isChanged() const
{
    return m_changed;
}

void ManageEmployee::on_dirigentiComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ui->dirigenteMatricolaSB->setText(ui->dirigentiComboBox->currentText().split("-").at(0).trimmed());
    if(ui->dirigentiComboBox->currentText().split("-").count() == 2)
        ui->dirigenteNomeLE->setText(ui->dirigentiComboBox->currentText().split("-").at(1).trimmed());
    m_nominativo = ui->dirigenteNomeLE->text().trimmed();
    ui->restoreButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->dirigentiComboBox->setEnabled(true);
}

void ManageEmployee::on_dirigenteNomeLE_textChanged(const QString &arg1)
{
    ui->saveButton->setEnabled(m_nominativo != arg1.trimmed());
    ui->restoreButton->setEnabled(m_nominativo != arg1.trimmed());
    ui->dirigentiComboBox->setEnabled(m_nominativo == arg1.trimmed());
}

void ManageEmployee::on_closeButton_clicked()
{
    close();
}

void ManageEmployee::on_restoreButton_clicked()
{
    ui->dirigenteNomeLE->setText(m_nominativo);
    ui->restoreButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
}

void ManageEmployee::on_saveButton_clicked()
{
    SqlQueries::editDoctor(ui->dirigentiComboBox->currentData(Qt::UserRole).toString(),
                           ui->dirigenteMatricolaSB->text().trimmed(),
                           ui->dirigenteNomeLE->text().trimmed());

    m_changed = true;
    populateDirigenti();
    m_nominativo = ui->dirigenteNomeLE->text().trimmed();
    on_dirigenteNomeLE_textChanged(m_nominativo);
}

void ManageEmployee::populateDirigenti()
{
    const int currData = ui->dirigentiComboBox->currentData(Qt::UserRole).toInt();
    ui->dirigentiComboBox->clear();
    QStringList query = SqlQueries::getTuttiMatricoleNomi();
    for(const QString &s : query) {
        QStringList l = s.split("~");
        ui->dirigentiComboBox->addItem(l.at(1) + " - " + l.at(2), l.at(0));
    }
    const int index = ui->dirigentiComboBox->findData(currData, Qt::UserRole);
    if(index > 0) {
        ui->dirigentiComboBox->setCurrentIndex(index);
    } else {
        ui->dirigentiComboBox->setCurrentIndex(0);
    }

    ui->dirigentiComboBox->show();
}
