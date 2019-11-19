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

#include "manageemployee.h"
#include "ui_manageemployee.h"
#include "sqlqueries.h"

#include <QSettings>
#include <QDir>

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

    QSettings settings;

    const QString path = settings.value("photosPath", "").toString();
    if(QFile::exists(path + QDir::separator() + "F" + ui->dirigenteMatricolaSB->text().rightJustified(6, '0') + ".jpg")) {
        QPixmap pix(path + QDir::separator() + "F" + ui->dirigenteMatricolaSB->text().rightJustified(6, '0') + ".jpg");
        if(pix.width() > pix.height())
            pix = pix.scaledToWidth(180);
        else
            pix = pix.scaledToHeight(180);
        ui->photoLabel->setPixmap(pix);
    } else {
        ui->photoLabel->setPixmap(QPixmap(":/images/user-none.png"));
    }
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
