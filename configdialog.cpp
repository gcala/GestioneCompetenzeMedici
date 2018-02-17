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

#include "configdialog.h"
#include "ui_configdialog.h"
#include "settingsitem.h"

#include <QtWidgets>

ConfigDialog::ConfigDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConfigDialog)
    , configChanged(false)
{
    ui->setupUi(this);
    setupSettingsCategories();
    loadSettings();
    ui->applyButton->setEnabled( false );
    ui->stackedWidget->setCurrentIndex( 0 );
    connect( ui->contentsWidget, SIGNAL(currentRowChanged(int)), SLOT(switchPage(int)) );
    connect( ui->applyButton, SIGNAL(clicked()), SLOT(saveSettings()) );
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::setupSettingsCategories()
{
    QString generalString = trUtf8( "Generale" );
    QString pathsString = trUtf8( "Percorsi" );

    QFont boldFont;
    boldFont.setBold( true );
    QFontMetrics fmText( boldFont );

    // adapat column width to translated strings
    int maxWidth = qMax(fmText.width( generalString ), fmText.width( pathsString ));

    ui->contentsWidget->setFixedWidth( maxWidth + 20 /*margin*/ );

    SettingsItem *generalItem = new SettingsItem( generalString, "", this ); // empty icon name is default for application own icon
    SettingsItem *pathsItem = new SettingsItem( pathsString, QLatin1String( "stock_folder" ), this );

    QListWidgetItem *generalWidget = new QListWidgetItem();
    ui->contentsWidget->addItem( generalWidget );
    ui->contentsWidget->setItemWidget( generalWidget, generalItem );
    generalWidget->setSizeHint( QSize( maxWidth,80 ) );

    QListWidgetItem *pathsWidget = new QListWidgetItem();
    ui->contentsWidget->addItem( pathsWidget );
    ui->contentsWidget->setItemWidget( pathsWidget, pathsItem );
    pathsWidget->setSizeHint( QSize(maxWidth,80 ) );

    ui->contentsWidget->setCurrentItem( generalWidget );
}

void ConfigDialog::loadSettings()
{
    QSettings settings;
    ui->photosPath->setText(settings.value("photosPath", "").toString());
#ifdef _WIN32
    ui->javaPath->setText(settings.value("javaPath", "C:\\ProgramData\\Oracle\\Java\\javapath\\java.exe").toString());
#else
    ui->javaPath->setText(settings.value("javaPath", "/usr/bin/java").toString());
#endif
    ui->tabulaPath->setText(settings.value("tabulaPath", QApplication::applicationDirPath() + QDir::separator() + "tabula.jar").toString());
}

void ConfigDialog::on_photoBrowser_clicked()
{
    QString dir = QFileDialog::getExistingDirectory( this,
                                                     trUtf8( "Seleziona cartella con le foto" ),
                                                     ui->photosPath->text().isEmpty() ? QDir::homePath() : ui->photosPath->text(),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

    // be sure that a valid path was selected
    if( QFile::exists( dir ) ) {
        ui->photosPath->setText( dir );
        configChanged = true;
        ui->applyButton->setEnabled( true );
    }
}

void ConfigDialog::on_javaBrowser_clicked()
{
    QFileInfo fi(ui->javaPath->text());
    QString fileName;
#ifdef _WIN32
    fileName = QFileDialog::getOpenFileName(this, tr("Seleziona Java"),
                                            fi.absolutePath().isEmpty() ? QDir::homePath() : fi.absolutePath(),
                                            tr("File eseguibili (*.exe)"));
#else
    fileName = QFileDialog::getOpenFileName(this, tr("Seleziona Java"),
                                            fi.absolutePath().isEmpty() ? QDir::homePath() : fi.absolutePath(),
                                            tr("Tutti i file (*.*)"));
#endif
    // be sure that a valid path was selected
    if( QFile::exists( fileName ) ) {
        ui->javaPath->setText( fileName );
        configChanged = true;
        ui->applyButton->setEnabled( true );
    }
}

void ConfigDialog::on_tabulaBrowser_clicked()
{
    QFileInfo fi(ui->tabulaPath->text());
    QString fileName = QFileDialog::getOpenFileName(this, tr("Seleziona Tabula"),
                                                        fi.absolutePath().isEmpty() ? QDir::homePath() : fi.absolutePath(),
                                                        tr("JAR file (*.jar)"));
    // be sure that a valid path was selected
    if( QFile::exists( fileName ) ) {
        ui->tabulaPath->setText( fileName );
        configChanged = true;
        ui->applyButton->setEnabled( true );
    }
}

void ConfigDialog::on_restoreButton_clicked()
{
    configChanged = true;
    ui->applyButton->setEnabled( true );
    ui->photosPath->setText("");
    ui->tabulaPath->setText(QApplication::applicationDirPath() + QDir::separator() + "tabula.jar");
#ifdef _WIN32
    ui->javaPath->setText("C:\\ProgramData\\Oracle\\Java\\javapath\\java.exe");
#else
    ui->javaPath->setText("/usr/bin/java");
#endif
}

void ConfigDialog::on_okButton_clicked()
{
    if( configChanged )
        saveSettings();
    close();
}

void ConfigDialog::on_cancelButton_clicked()
{
    close();
}

void ConfigDialog::saveSettings()
{
    configChanged = false;
    ui->applyButton->setEnabled( false );
    QSettings settings;
    settings.setValue("photosPath", ui->photosPath->text());
    settings.setValue("tabulaPath", ui->tabulaPath->text());
    settings.setValue("javaPath", ui->javaPath->text());
}

void ConfigDialog::switchPage(int page)
{
    ui->stackedWidget->setCurrentIndex( page );
}
