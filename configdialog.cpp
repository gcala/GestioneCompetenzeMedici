/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
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
    connect(ui->certButton, &QPushButton::clicked, this, &ConfigDialog::certButtonClicked);
    connect(ui->keyButton, &QPushButton::clicked, this, &ConfigDialog::keyButtonClicked);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::setupSettingsCategories()
{
    QString generalString = tr( "Generale" );
    QString pathsString = tr( "Percorsi" );
    QString databaseString = tr( "Database Remoto" );

    QFont boldFont;
    boldFont.setBold( true );
    QFontMetrics fmText( boldFont );

    // adapat column width to translated strings
    int maxWidth = qMax(fmText.horizontalAdvance( generalString ), fmText.horizontalAdvance( pathsString ));

    ui->contentsWidget->setFixedWidth( maxWidth + 20 /*margin*/ );

    SettingsItem *generalItem = new SettingsItem( generalString, "", this ); // empty icon name is default for application own icon
    SettingsItem *pathsItem = new SettingsItem( pathsString, QLatin1String( "stock_folder" ), this );
    SettingsItem *databaseItem = new SettingsItem( databaseString, QLatin1String( "network-server-database" ), this );

    auto generalWidget = new QListWidgetItem();
    ui->contentsWidget->addItem( generalWidget );
    ui->contentsWidget->setItemWidget( generalWidget, generalItem );
    generalWidget->setSizeHint( QSize( maxWidth,80 ) );

    auto pathsWidget = new QListWidgetItem();
    ui->contentsWidget->addItem( pathsWidget );
    ui->contentsWidget->setItemWidget( pathsWidget, pathsItem );
    pathsWidget->setSizeHint( QSize(maxWidth,80 ) );

    auto databaseWidget = new QListWidgetItem();
    ui->contentsWidget->addItem( databaseWidget );
    ui->contentsWidget->setItemWidget( databaseWidget, databaseItem );
    databaseWidget->setSizeHint( QSize(maxWidth,80 ) );

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

    ui->hostLine->setText(settings.value("host", "").toString());
    ui->dbLine->setText(settings.value("dbName", "").toString());
    ui->certLine->setText(settings.value("certFile", "").toString());
    ui->keyLine->setText(settings.value("keyFile", "").toString());
    ui->userLine->setText(settings.value("lastUsername", "").toString());
}

QString ConfigDialog::selectFile(const QString &title)
{
    return QFileDialog::getOpenFileName(this, title, QApplication::applicationDirPath(), tr("File PEM (*.pem)"));
}

void ConfigDialog::on_photoBrowser_clicked()
{
    QString dir = QFileDialog::getExistingDirectory( this,
                                                     tr( "Seleziona cartella con le foto" ),
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
    settings.setValue("host", ui->hostLine->text());
    settings.setValue("dbName", ui->dbLine->text());
    settings.setValue("certFile", ui->certLine->text());
    settings.setValue("keyFile", ui->keyLine->text());
    settings.setValue("lastUsername", ui->userLine->text());
}

void ConfigDialog::switchPage(int page)
{
    ui->stackedWidget->setCurrentIndex( page );
}

void ConfigDialog::certButtonClicked()
{
    QString fileName = selectFile("Seleziona file Certificato");

    // be sure that a valid path was selected
    if( QFile::exists( fileName ) ) {
        ui->certLine->setText(fileName);
    } else {
        ui->certLine->clear();
    }
}

void ConfigDialog::keyButtonClicked()
{
    QString fileName = selectFile("Seleziona file Chiave");

    // be sure that a valid path was selected
    if( QFile::exists( fileName ) ) {
        ui->keyLine->setText(fileName);
    } else {
        ui->keyLine->clear();
    }
}
