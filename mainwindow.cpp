/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "printdialog.h"
#include "sqlqueries.h"
#include "calendarmanager.h"
#include "calendarmanagerrep.h"
#include "competenza.h"
#include "utilities.h"
#include "databasewizard.h"
#include "sqldatabasemanager.h"
#include "dmpcompute.h"
#include "aboutdialog.h"
#include "configdialog.h"
#include "resetdialog.h"
#include "logindialog.h"
#include "nomiunitadialog.h"
#include "switchunitdialog.h"
#include "manageemployee.h"
#include "manageunits.h"

#include <QtWidgets>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    progressBar = new QProgressBar(this);
    progressBar->setMinimumWidth(200);
    msgLabel = new QLabel(this);
    msgLabel->setAlignment(Qt::AlignRight);
    statusBar()->addWidget(msgLabel,1);
    statusBar()->addWidget(progressBar);
    progressBar->setVisible(false);

    ui->gdPainterWidget->setDiurna(true);

    tabulaProcess = new QProcess;
    connect(tabulaProcess, SIGNAL(finished(int , QProcess::ExitStatus)), this, SLOT(tabulaFinished(int , QProcess::ExitStatus)));
    connect(tabulaProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(tabulaError(QProcess::ProcessError)));

    m_loadingTimeCards = false;

    ui->restoreCompetenzeButton->setEnabled(false);
    ui->saveCompetenzeButton->setEnabled(false);

    gdCalendarMenu = new QMenu(ui->gdCalendarButton);
    gdCalendar = new CalendarManager(gdCalendarMenu);
    gdCalendarAction = new QWidgetAction(gdCalendarMenu);
    gdCalendarAction->setDefaultWidget(gdCalendar);
    gdCalendarMenu->addAction(gdCalendarAction);
    ui->gdCalendarButton->setMenu(gdCalendarMenu);
    connect(gdCalendar, SIGNAL(clicked(QDate)), this, SLOT(gdCalendarClicked(QDate)));

    gnCalendarMenu = new QMenu(ui->gnCalendarButton);
    gnCalendar = new CalendarManager(gnCalendarMenu);
    gnCalendarAction = new QWidgetAction(gnCalendarMenu);
    gnCalendarAction->setDefaultWidget(gnCalendar);
    gnCalendarMenu->addAction(gnCalendarAction);
    ui->gnCalendarButton->setMenu(gnCalendarMenu);
    connect(gnCalendar, SIGNAL(clicked(QDate)), this, SLOT(gnCalendarClicked(QDate)));

    rCalendarMenu = new QMenu(ui->rCalendarButton);
    rCalendar = new CalendarManagerRep(rCalendarMenu);
    rCalendarAction = new QWidgetAction(rCalendarMenu);
    rCalendarAction->setDefaultWidget(rCalendar);
    rCalendarMenu->addAction(rCalendarAction);
    ui->rCalendarButton->setMenu(rCalendarMenu);
    connect(rCalendar, SIGNAL(clicked(QDate)), this, SLOT(rCalendarClicked(QDate)));

    ferieCalendarMenu = new QMenu(ui->ferieCalendarButton);
    ferieCalendar = new CalendarManager(ferieCalendarMenu);
    ferieCalendarAction = new QWidgetAction(ferieCalendarMenu);
    ferieCalendarAction->setDefaultWidget(ferieCalendar);
    ferieCalendarMenu->addAction(ferieCalendarAction);
    ui->ferieCalendarButton->setMenu(ferieCalendarMenu);
    ferieCalendar->setSelectionMode(QCalendarWidget::NoSelection);

    congediCalendarMenu = new QMenu(ui->congediCalendarButton);
    congediCalendar = new CalendarManager(congediCalendarMenu);
    congediCalendarAction = new QWidgetAction(congediCalendarMenu);
    congediCalendarAction->setDefaultWidget(congediCalendar);
    congediCalendarMenu->addAction(congediCalendarAction);
    ui->congediCalendarButton->setMenu(congediCalendarMenu);
    congediCalendar->setSelectionMode(QCalendarWidget::NoSelection);

    malattiaCalendarMenu = new QMenu(ui->malattiaCalendarButton);
    malattiaCalendar = new CalendarManager(malattiaCalendarMenu);
    malattiaCalendarAction = new QWidgetAction(malattiaCalendarMenu);
    malattiaCalendarAction->setDefaultWidget(malattiaCalendar);
    malattiaCalendarMenu->addAction(malattiaCalendarAction);
    ui->malattiaCalendarButton->setMenu(malattiaCalendarMenu);
    malattiaCalendar->setSelectionMode(QCalendarWidget::NoSelection);

    rmpCalendarMenu = new QMenu(ui->rmpCalendarButton);
    rmpCalendar = new CalendarManager(rmpCalendarMenu);
    rmpCalendarAction = new QWidgetAction(rmpCalendarMenu);
    rmpCalendarAction->setDefaultWidget(rmpCalendar);
    rmpCalendarMenu->addAction(rmpCalendarAction);
    ui->rmpCalendarButton->setMenu(rmpCalendarMenu);
    rmpCalendar->setSelectionMode(QCalendarWidget::NoSelection);

    rmcCalendarMenu = new QMenu(ui->rmcCalendarButton);
    rmcCalendar = new CalendarManager(rmcCalendarMenu);
    rmcCalendarAction = new QWidgetAction(rmcCalendarMenu);
    rmcCalendarAction->setDefaultWidget(rmcCalendar);
    rmcCalendarMenu->addAction(rmcCalendarAction);
    ui->rmcCalendarButton->setMenu(rmcCalendarMenu);
    rmcCalendar->setSelectionMode(QCalendarWidget::NoSelection);

    altreCalendarMenu = new QMenu(ui->altreCalendarButton);
    altreCalendar = new CalendarManager(altreCalendarMenu);
    altreCalendarAction = new QWidgetAction(altreCalendarMenu);
    altreCalendarAction->setDefaultWidget(altreCalendar);
    altreCalendarMenu->addAction(altreCalendarAction);
    ui->altreCalendarButton->setMenu(altreCalendarMenu);
    connect(altreCalendar, SIGNAL(clicked(QDate)), this, SLOT(altreCalendarClicked(QDate)));

    unitaReadOnlyMode = true;
    dirigenteReadOnlyMode = true;
    unitOp = UndefOp;
    dirigenteOp = UndefOp;

    printDialog = new PrintDialog(this);
    ui->sommV1->setVisible(false);
    ui->sommV2->setVisible(false);

    loadSettings();
    Utilities::m_connectionName = "";

    connect(&tabulaReader, SIGNAL(timeCardsRead()), this, SLOT(handleResults()));
    connect(&tabulaReader, SIGNAL(totalRows(int)), this, SLOT(setTotalRows(int)));
    connect(&tabulaReader, SIGNAL(currentRow(int)), this, SLOT(setCurrentRow(int)));
    connect(&okularReader, SIGNAL(timeCardsRead()), this, SLOT(handleResults()));
    connect(&okularReader, SIGNAL(totalRows(int)), this, SLOT(setTotalRows(int)));
    connect(&okularReader, SIGNAL(currentRow(int)), this, SLOT(setCurrentRow(int)));
    connect(&unitaCompetenzeExporter, SIGNAL(exportFinished(QString)), this, SLOT(exported(QString)));
    connect(&unitaCompetenzeExporter, SIGNAL(totalRows(int)), this, SLOT(setTotalRows(int)));
    connect(&unitaCompetenzeExporter, SIGNAL(currentRow(int)), this, SLOT(setCurrentRow(int)));
    connect(&deficitRecuperiExporter, SIGNAL(exportFinished(QString)), this, SLOT(exported(QString)));
    connect(&deficitRecuperiExporter, SIGNAL(totalRows(int)), this, SLOT(setTotalRows(int)));
    connect(&deficitRecuperiExporter, SIGNAL(currentRow(int)), this, SLOT(setCurrentRow(int)));
    connect(&dirigenteCompetenzeExporter, SIGNAL(exportFinished(QString)), this, SLOT(exported(QString)));
    connect(&dirigenteCompetenzeExporter, SIGNAL(totalRows(int)), this, SLOT(setTotalRows(int)));
    connect(&dirigenteCompetenzeExporter, SIGNAL(currentRow(int)), this, SLOT(setCurrentRow(int)));

    m_nomiDialog = new NomiUnitaDialog;
    connect(&tabulaReader, SIGNAL( selectUnit(QString, int&) ), this, SLOT( associaUnita(QString, int &) ), Qt::BlockingQueuedConnection ) ;

//    ui->unitaComboBox->setCurrentIndex(0);

    QTimer::singleShot(500, this, SLOT(delayedSetup()));
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete printDialog;
    delete m_nomiDialog;
    delete ui;
}

void MainWindow::askDbUserPassword()
{
    LoginDialog login(m_host, m_dbName, this);
    login.setUsername(m_lastUsername);
    login.setPassword(m_lastPassword);
    login.disablePassButton(m_lastPassword.isEmpty());
    login.exec();

    if(login.canceled()) {
        return;
    }

    m_lastUsername = login.username();
    m_lastPassword = login.password();

    m_driver = "QMYSQL";

    setupDbConnectionParameters();

    saveSettings();

    connectToDatabase();
}

void MainWindow::mostraDifferenzaOre()
{
    if(m_competenza->differenzaOre().startsWith("-"))
        ui->oreDifferenzaLabel->setStyleSheet("color:red;");
    else if(m_competenza->differenzaOre() == "0:00")
        ui->oreDifferenzaLabel->setStyleSheet("");
    else
        ui->oreDifferenzaLabel->setStyleSheet("color:green;");

    ui->oreDifferenzaLabel->setText(m_competenza->differenzaOre());
    ui->deficitOrarioLabel->setText(m_competenza->deficitOrario());

    if(m_competenza->dmp() == 0) {
        ui->dmpHoursEdit->setValue(0);
        ui->dmpMinsEdit->setValue(0);
    } else {
        ui->dmpHoursEdit->setValue(m_competenza->dmp()/60);
        ui->dmpMinsEdit->setValue(m_competenza->dmp()%60);
    }
}

void MainWindow::on_actionNuovoDatabase_triggered()
{
    auto databaseWizard = new DatabaseWizard(2, this);
    databaseWizard->exec();

    if(databaseWizard->canceled()) {
        delete databaseWizard;
        return;
    }

    currentDatabase.setFile(The::dbManager()->currentDatabaseName());

    clearWidgets();
    populateMeseCompetenzeCB();
    ui->actionBackupDatabase->setEnabled(true);

    delete databaseWizard;
}

void MainWindow::clearWidgets()
{
    ui->meseCompetenzeCB->clear();
    ui->unitaCompetenzeCB->clear();
    ui->dirigentiCompetenzeCB->clear();
    ui->workDaysLabel->clear();
    ui->ferieLabel->clear();
    ui->congediLabel->clear();
    ui->malattiaLabel->clear();
    ui->rmpLabel->clear();
    ui->altreLabel->clear();
}

void MainWindow::connectToDatabase()
{
    if(!The::dbManager()->createConnection()) {
        setWindowTitle("Gestione Competenze Medici");
        return;
    }

    QFileInfo fi(The::dbManager()->currentDatabaseName());

    setWindowTitle("Gestione Competenze Medici - " + fi.completeBaseName());

    populateMeseCompetenzeCB();
    m_nomiDialog->populateUnits();
}

void MainWindow::populateMeseCompetenzeCB()
{
    ui->meseCompetenzeCB->clear();
    printDialog->clearMese();

    const QStringList timeCards = SqlQueries::timecardsList();

    QStringList::const_iterator i = timeCards.constEnd();

    while(i != timeCards.constBegin()) {
        --i;
        QString ss = (*i).split("_").last();
        ui->meseCompetenzeCB->addItem(QLocale().monthName((*i).right(2).toInt()) + " " + ss.left(4), *i);
        printDialog->addMese(QLocale().monthName((*i).right(2).toInt()) + " " + ss.left(4), *i);
    }
}

void MainWindow::populateUnitaCompetenzeCB()
{
    ui->unitaCompetenzeCB->clear();

    if(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    QStringList query = SqlQueries::getUnitaDataFromTimecard(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString());

    QStringList list;

    for(const QString &s : query) {
        QStringList l = s.split("~");
        if(!list.contains(l.at(1))) {
            ui->unitaCompetenzeCB->addItem(l.at(2) + " - " + l.at(1), l.at(0));
            list << l.at(1);
        }
    }
}

void MainWindow::populateDirigentiCompetenzeCB()
{
    ui->dirigentiCompetenzeCB->clear();

    if(ui->unitaCompetenzeCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    QStringList query = SqlQueries::getDoctorDataFromUnitaInTimecard(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString(), ui->unitaCompetenzeCB->currentData(Qt::UserRole).toInt());

    for(const QString &s : query) {
        QStringList l = s.split("~");
        ui->dirigentiCompetenzeCB->addItem(l.at(1) + " - " + l.at(2), l.at(0));
    }
}

void MainWindow::on_actionApriDatabase_triggered()
{
    auto databaseWizard = new DatabaseWizard(0, this);
    databaseWizard->exec();

    if(databaseWizard->canceled()) {
        delete databaseWizard;
        return;
    }

    if(databaseWizard->openDb()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Seleziona file database"),
                                                        currentDatabase.absolutePath().isEmpty() ? QDir::homePath() : currentDatabase.absolutePath(),
                                                        tr("Database (*.db)"));

        // be sure that a valid path was selected
        if( QFile::exists( fileName ) ) {
            currentDatabase.setFile(fileName);
            m_driver = "QSQLITE";
            saveSettings();
            needsBackup();
            setupDbConnectionParameters();
            connectToDatabase();
            populateMeseCompetenzeCB();
            ui->actionBackupDatabase->setEnabled(true);
        }
        return;
    }

    // ultima opzione: apri database remoto
    // salviamo innanzitutto host e database
    m_host = databaseWizard->host();
    m_dbName = databaseWizard->database();
    m_driver = "QMYSQL";
    m_useSSL = databaseWizard->useSSL();
    m_certFile = databaseWizard->certFile();
    m_keyFile = databaseWizard->keyFile();
    m_lastUsername = databaseWizard->user();
    m_lastPassword = databaseWizard->password();
    saveSettings();

    setupDbConnectionParameters();
    connectToDatabase();

    delete databaseWizard;
}

void MainWindow::loadSettings()
{
    QSettings settings;
    m_driver = settings.value("lastDriver", "QSQLITE").toString();
    m_host = settings.value("host", "").toString();
    m_dbName = settings.value("dbName", "").toString();
    m_certFile = settings.value("certFile", "").toString();
    m_keyFile = settings.value("keyFile", "").toString();
    m_useSSL = settings.value("useSSL", false).toBool();
    m_lastUsername = settings.value("lastUsername", "").toString();
    currentDatabase.setFile(settings.value("lastDatabasePath", "").toString());
    printDialog->setPath(settings.value("exportPath", QDir::homePath()).toString());
    m_photosPath = settings.value("photosPath", "").toString();
    m_tabulaPath = settings.value("tabulaPath", QApplication::applicationDirPath() + QDir::separator() + "tabula.jar").toString();
#ifdef _WIN32
    m_javaPath = settings.value("javaPath", "C:\\ProgramData\\Oracle\\Java\\javapath\\java.exe").toString();
#else
    m_javaPath = settings.value("javaPath", "/usr/bin/java").toString();
#endif

    if(settings.value("isMaximized", false).toBool())
        showMaximized();
    else
        setGeometry(settings.value("windowGeometry", QRect(100,100,800,600)).toRect());

    setupDbConnectionParameters();
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("lastDatabasePath", currentDatabase.absoluteFilePath());
    settings.setValue("exportPath", printDialog->path());
    settings.setValue("photosPath", m_photosPath);
    settings.setValue("tabulaPath", m_tabulaPath);
    settings.setValue("javaPath", m_javaPath);
    settings.setValue("lastDriver", m_driver);
    settings.setValue("host", m_host);
    settings.setValue("dbName", m_dbName);
    settings.setValue("certFile", m_certFile);
    settings.setValue("keyFile", m_keyFile);
    settings.setValue("useSSL", m_useSSL);
    settings.setValue("lastUsername", m_lastUsername);
    settings.setValue("windowGeometry", geometry());
    settings.setValue("isMaximized", isMaximized());
}

void MainWindow::on_actionCaricaPdf_triggered()
{
    pdfFile.clear();
    pdfFile = QFileDialog::getOpenFileName(this, tr("Seleziona pdf cartellini"),
                                                    currentDatabase.absolutePath().isEmpty() ? QDir::homePath() : currentDatabase.absolutePath(),
                                                    tr("PDF (*.pdf)"));

    if(pdfFile.isEmpty())
        return;

    if(QFile::exists(pdfFile)) {
        m_loadingTimeCards = true;
        progressBar->setVisible(true);
        progressBar->setMinimum(0);
        progressBar->setMaximum(0);
        msgLabel->setText("Converto pdf in csv");
        ui->competenzeWidget->setEnabled(false);

        QFileInfo fi(pdfFile);

        QStringList arguments;
        arguments << "-jar" << m_tabulaPath;
        arguments << "-n";
        arguments << "-p" << "all";
        arguments << "-a" << "6.93,2.475,470.0,772.695";
        arguments << "-c" << "23,32,72,114,155,196,237,277,318,357,395,430,466,501,529,570,597,638,665,704,731";
        arguments << pdfFile;
        arguments << "-o" << fi.absolutePath() + QDir::separator() + "cartellini.csv";

        QFile::remove(fi.absolutePath() + QDir::separator() + "cartellini.csv");

        m_currentMeseCompetenzeIndex = ui->meseCompetenzeCB->currentIndex();
        m_currentUnitaCompetenzeIndex = ui->unitaCompetenzeCB->currentIndex();
        m_currentDirigenteCompetenzeIndex = ui->dirigentiCompetenzeCB->currentIndex();

        tabulaProcess->start(m_javaPath, arguments);
    }
}

void MainWindow::handleResults()
{
    Utilities::m_connectionName = "";

    progressBar->setVisible(false);
    msgLabel->setText("");
    ui->competenzeWidget->setEnabled(true);

    populateMeseCompetenzeCB();
    m_loadingTimeCards = false;
    ui->meseCompetenzeCB->setCurrentIndex(m_currentMeseCompetenzeIndex);
    ui->unitaCompetenzeCB->setCurrentIndex(m_currentUnitaCompetenzeIndex);
    ui->dirigentiCompetenzeCB->setCurrentIndex(m_currentDirigenteCompetenzeIndex);
}

void MainWindow::exported(const QString &file)
{
    Utilities::m_connectionName = "";
    ui->actionStampaCompetenzeUnita->setEnabled(true);
    ui->actionStampaCompetenzeDirigenti->setEnabled(true);
    ui->actionPrintDeficit->setEnabled(true);
    ui->competenzeWidget->setEnabled(true);
    progressBar->setVisible(false);
    msgLabel->setText("");

    if(!file.isEmpty())
        QDesktopServices::openUrl(QUrl::fromLocalFile(file));
}

void MainWindow::computed()
{
    Utilities::m_connectionName = "";

    disconnect(The::dmpCompute(), SIGNAL(computeFinished()), nullptr, nullptr);
    disconnect(The::dmpCompute(), SIGNAL(currentItem(int)), nullptr, nullptr);
    disconnect(The::dmpCompute(), SIGNAL(totalItems(int)), nullptr, nullptr);

    ui->actionRicalcolaDeficit->setEnabled(true);
    progressBar->setVisible(false);
    msgLabel->setText("");
}

void MainWindow::setTotalRows(int value)
{
    progressBar->setMaximum(value);
}

void MainWindow::setCurrentRow(int value)
{
    progressBar->setValue(value);
}

void MainWindow::on_saveCompetenzeButton_clicked()
{
    ui->saveCompetenzeButton->setEnabled(false);
    m_competenza->saveMods();

    // per aggiornare la situazione cambiare l'indice corrente e torniamo subito dopo su quello originale
    int idx = ui->dirigentiCompetenzeCB->currentIndex();
    ui->dirigentiCompetenzeCB->setCurrentIndex(-1);
    ui->dirigentiCompetenzeCB->setCurrentIndex(idx);
    ui->dirigentiCompetenzeCB->show();

    ui->restoreCompetenzeButton->setEnabled(m_competenza->isRestorable());
}

void MainWindow::on_restoreCompetenzeButton_clicked()
{
//    ResetDialog *resetDialog = new ResetDialog(m_competenza->modTableName(), m_competenza->doctorId(), this);
    auto resetDialog = new ResetDialog(m_competenza, this);
    resetDialog->exec();
    // per aggiornare la situazione cambiare l'indice corrente e torniamo subito dopo su quello originale
    int idx = ui->dirigentiCompetenzeCB->currentIndex();
    ui->dirigentiCompetenzeCB->setCurrentIndex(-1);
    ui->dirigentiCompetenzeCB->setCurrentIndex(idx);
    ui->dirigentiCompetenzeCB->show();

    ui->saveCompetenzeButton->setEnabled(false);
    ui->restoreCompetenzeButton->setEnabled(m_competenza->isRestorable());
}

void MainWindow::on_meseCompetenzeCB_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    int docIndex = ui->dirigentiCompetenzeCB->currentIndex();
    int unitaIndex = ui->unitaCompetenzeCB->currentIndex();
    populateUnitaCompetenzeCB();
    ui->unitaCompetenzeCB->setCurrentIndex(unitaIndex < 0 ? 0 : unitaIndex);
    ui->unitaCompetenzeCB->show();
    ui->dirigentiCompetenzeCB->setCurrentIndex(docIndex < 0 ? 0 : docIndex);
    ui->dirigentiCompetenzeCB->show();
    const QDate date(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString().split("_").last().left(4).toInt(),
                     ui->meseCompetenzeCB->currentData(Qt::UserRole).toString().split("_").last().right(2).toInt(),
                     1);
//    ui->sommV1->setVisible(date < Utilities::ccnl1618Date);
    ui->sommV1->setVisible(true);
    ui->sommV2->setVisible(date >= Utilities::ccnl1618Date);
}

void MainWindow::on_unitaCompetenzeCB_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    populateDirigentiCompetenzeCB();
}

void MainWindow::on_dirigentiCompetenzeCB_currentIndexChanged(int index)
{
    Q_UNUSED(index)

    // TODO: controllo necessario altrimenti l'applicazione crasha durante il caricamento dei cartellini
    // Da investigare
    if(m_loadingTimeCards)
        return;

    ui->restoreCompetenzeButton->setEnabled(false);

    if(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    m_competenza = new Competenza(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString(), ui->dirigentiCompetenzeCB->currentData(Qt::UserRole).toInt());

    populateCompetenzeTab();

    ui->restoreCompetenzeButton->setEnabled(m_competenza->isRestorable());
    ui->saveCompetenzeButton->setEnabled(false);
}

void MainWindow::populateCompetenzeTab()
{
    disconnect(ui->dmpHoursEdit, SIGNAL(valueChanged(int)), nullptr, nullptr);
    disconnect(ui->dmpMinsEdit, SIGNAL(valueChanged(int)), nullptr, nullptr);
    disconnect(ui->orarioGiornalieroEdit, SIGNAL(dateChanged(QDate)), nullptr, nullptr);
    disconnect(ui->pagaStrGuardiaCB, SIGNAL(toggled(bool)), nullptr, nullptr);

    ui->pagaStrGuardiaCB->setChecked(m_competenza->pagaStrGuardia());
    QFont font = ui->oreStraordinarioGuardieLabel->font();
    font.setStrikeOut(!ui->pagaStrGuardiaCB->isChecked());
    ui->oreStraordinarioGuardieLabel->setFont(font);

    ferieCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());
    congediCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());
    malattiaCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());
    rmcCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());
    rmpCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());
    gdCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());
    gnCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());
    rCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());
    altreCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());

    ui->workDaysLabel->setText(m_competenza->giorniLavorati());

    ui->ferieLabel->setText(m_competenza->ferieCount());
    ferieCalendar->setDates(m_competenza->ferieDates());

    ui->congediLabel->setText(QString::number(m_competenza->altreCausaliDates().count()));
    congediCalendar->setDates(m_competenza->altreCausaliDates());

    ui->malattiaLabel->setText(m_competenza->malattiaCount());
    malattiaCalendar->setDates(m_competenza->malattiaDates());

    ui->rmpLabel->setText(m_competenza->rmpCount());
    rmpCalendar->setDates(m_competenza->rmpDates());

    ui->rmcLabel->setText(m_competenza->rmcCount());
    rmcCalendar->setDates(m_competenza->rmcDates());

    gdCalendar->setDates(m_competenza->gdDates());
    gnCalendar->setDates(m_competenza->gnDates());
    rCalendar->setDates(m_competenza->rep());

    if(m_competenza->altreAssenzeDates().count() > 0)
        ui->altreLabel->setStyleSheet("color: red;");
    else
        ui->altreLabel->setStyleSheet("color: black;");
    ui->altreLabel->setText(QString::number(m_competenza->altreAssenzeDates().count()));
    altreCalendar->setDates(m_competenza->altreAssenzeDates());
    altreCalendar->setScopertiDates(m_competenza->scopertiDates());

    elaboraSommario();

    ui->noteLine->setText(m_competenza->note());

    connect(ui->dmpHoursEdit, SIGNAL(valueChanged(int)), this, SLOT(oreCambiate(int)));
    connect(ui->dmpMinsEdit, SIGNAL(valueChanged(int)), this, SLOT(minutiCambiati(int)));
    connect(ui->orarioGiornalieroEdit, SIGNAL(timeChanged(QTime)), this, SLOT(orarioGiornalieroCambiato(QTime)));
    connect(ui->pagaStrGuardiaCB, SIGNAL(toggled(bool)), SLOT(pagaStrGuardiaCambiato(bool)));
}

void MainWindow::elaboraGuardie()
{
    ui->gnPainterWidget->setMeseAnno(m_competenza->dataIniziale().month(), m_competenza->dataIniziale().year());
    ui->gdPainterWidget->setMeseAnno(m_competenza->dataIniziale().month(), m_competenza->dataIniziale().year());
    ui->gnPainterWidget->setGuardiaMap(m_competenza->guardiaNotturnaMap());
    ui->gdPainterWidget->setGuardiaMap(m_competenza->guardiaDiurnaMap());

    ui->g_d_fer_F->setText(QString::number(m_competenza->g_d_fer_F()));
    ui->g_d_fer_S->setText(QString::number(m_competenza->g_d_fer_S()));
    ui->g_d_fer_D->setText(QString::number(m_competenza->g_d_fer_D()));
    ui->g_d_fes_F->setText(QString::number(m_competenza->g_d_fes_F()));
    ui->g_d_fes_S->setText(QString::number(m_competenza->g_d_fes_S()));
    ui->g_d_fes_D->setText(QString::number(m_competenza->g_d_fes_D()));
    ui->g_n_fer_F->setText(QString::number(m_competenza->g_n_fer_F()));
    ui->g_n_fer_S->setText(QString::number(m_competenza->g_n_fer_S()));
    ui->g_n_fer_D->setText(QString::number(m_competenza->g_n_fer_D()));
    ui->g_n_fes_F->setText(QString::number(m_competenza->g_n_fes_F()));
    ui->g_n_fes_S->setText(QString::number(m_competenza->g_n_fes_S()));
    ui->g_n_fes_D->setText(QString::number(m_competenza->g_n_fes_D()));

    ui->totOreGuardie->setText(QString::number(m_competenza->totOreGuardie()));
    ui->notteLabel->setText(m_competenza->notte() > 0 ? QString::number(m_competenza->notte()) : "//");
    ui->festivoLabel->setText(m_competenza->numGuarDiurne() > 0 ? QString::number(m_competenza->numGuarDiurne()) : "//");
    ui->oreStraordinarioGuardieLabel->setText(m_competenza->oreStraordinarioGuardie());

    ui->numGuarDiuPag->setText(QString::number(m_competenza->numGuarDiurne()));
    ui->numGuarNottPag->setText(QString::number(m_competenza->numGuarNottPag()));
    ui->numGrFestPag->setText(QString::number(m_competenza->numGrFestPagabili()));
    ui->oreRepPag->setText(QString::number(m_competenza->oreRepPagate()));
}

void MainWindow::elaboraRep()
{
    ui->rPainterWidget->setRepMap(m_competenza->rep());
    ui->repLabel->setText(m_competenza->repCount());

    ui->r_d_fer->setText(Utilities::inOrario(m_competenza->r_d_fer()));
    ui->r_d_fes->setText(Utilities::inOrario(m_competenza->r_d_fes()));
    ui->r_n_fer->setText(Utilities::inOrario(m_competenza->r_n_fer()));
    ui->r_n_fes->setText(Utilities::inOrario(m_competenza->r_n_fes()));
    ui->totOreRep->setText(m_competenza->oreGrep());
}

void MainWindow::elaboraSommario()
{
    if(m_competenza->orarioGiornaliero() == "0:00")
        ui->orarioGiornalieroEdit->setTime(QTime(0,0));
    else
        ui->orarioGiornalieroEdit->setTime(QTime::fromString(m_competenza->orarioGiornaliero(),"h:mm"));
    ui->oreDovuteLabel->setText(m_competenza->oreDovute());
    ui->oreEffettuateLabel->setText(m_competenza->oreEffettuate());

    ui->oreStraordinarioRepLabel->setText(m_competenza->oreRepPagate() > 0 ? QString::number(m_competenza->oreRepPagate()) : "//");

    mostraDifferenzaOre();
    elaboraGuardie();
    elaboraRep();
    ui->residuoLabel->setText(m_competenza->residuoOreNonPagate() > 0 ? Utilities::inOrario(m_competenza->residuoOreNonPagate()) : "//");

    if(m_competenza->numOreRecuperabili() < 0)
        ui->oreRecLabel->setStyleSheet("color: red;");
    else
        ui->oreRecLabel->setStyleSheet("color: green;");

    if(m_competenza->numOreRecuperabili() == 0 && m_competenza->recuperiMesiSuccessivo().second == 0)
        ui->oreRecLabel->setText("//");
    else {
        ui->oreRecLabel->setText(Utilities::inOrario(m_competenza->numOreRecuperabili()) + " (" + QString::number(m_competenza->recuperiMesiSuccessivo().second) + ")");
    }

    ui->oreNonRecLabel->setText(m_competenza->residuoOreNonRecuperabili());
}

void MainWindow::setupDbConnectionParameters()
{
    The::dbManager()->setDriver(m_driver);
    The::dbManager()->setHost(m_host);
    The::dbManager()->setDbName(m_dbName);
    The::dbManager()->setCert(m_certFile);
    The::dbManager()->setKey(m_keyFile);
    The::dbManager()->setSecure(m_useSSL);
    The::dbManager()->setLocalDbFileName(currentDatabase.absoluteFilePath());
    The::dbManager()->setUser(m_lastUsername);
    The::dbManager()->setPass(m_lastPassword);
}

void MainWindow::on_actionStampaCompetenzeDirigenti_triggered()
{
    ui->actionStampaCompetenzeDirigenti->setEnabled(false);
    ui->actionStampaCompetenzeUnita->setEnabled(false);
    ui->actionPrintDeficit->setEnabled(false);
    printDialog->setCurrentOp(PrintDialog::ToolOps::PrintDoctors);

    printDialog->setCurrentMese(ui->meseCompetenzeCB->currentIndex());
    printDialog->setCurrentUnita(ui->unitaCompetenzeCB->currentIndex() + 1);
    printDialog->setCurrentDirigente(ui->dirigentiCompetenzeCB->currentIndex() + 1);

    printDialog->exec();

    if(!printDialog->proceed) {
        exported(QString());
        return;
    }

    progressBar->setVisible(true);
    msgLabel->setText("Esportazione competenze dirigenti");

    ui->competenzeWidget->setEnabled(false);
    dirigenteCompetenzeExporter.setPath(printDialog->path());
    dirigenteCompetenzeExporter.setTable(printDialog->currentMeseData());
    dirigenteCompetenzeExporter.setUnita(printDialog->currentUnitaData());
    dirigenteCompetenzeExporter.setDirigente(printDialog->currentDirigenteData());
    dirigenteCompetenzeExporter.start();
}

void MainWindow::on_actionStampaCompetenzeUnita_triggered()
{
    ui->actionStampaCompetenzeDirigenti->setEnabled(false);
    ui->actionStampaCompetenzeUnita->setEnabled(false);
    ui->actionPrintDeficit->setEnabled(false);
    printDialog->setCurrentOp(PrintDialog::ToolOps::PrintUnits);

    printDialog->setCurrentMese(ui->meseCompetenzeCB->currentIndex());
    printDialog->setCurrentUnita(ui->unitaCompetenzeCB->currentIndex() + 1);

    printDialog->exec();

    if(!printDialog->proceed) {
        exported(QString());
        return;
    }

    progressBar->setVisible(true);
    msgLabel->setText("Esportazione competenze unità");

    ui->competenzeWidget->setEnabled(false);
    unitaCompetenzeExporter.setPath(printDialog->path());
    unitaCompetenzeExporter.setMese(printDialog->currentMeseData());
    unitaCompetenzeExporter.setUnita(printDialog->currentUnitaData());
    unitaCompetenzeExporter.setPrintCasi(printDialog->casiIsChecked());
    unitaCompetenzeExporter.setPrintData(printDialog->dataIsChecked());
    unitaCompetenzeExporter.start();
}

void MainWindow::on_actionPrintDeficit_triggered()
{
    ui->actionStampaCompetenzeDirigenti->setEnabled(false);
    ui->actionStampaCompetenzeUnita->setEnabled(false);
    ui->actionPrintDeficit->setEnabled(false);
    printDialog->setCurrentOp(PrintDialog::ToolOps::PrintDeficit);

    printDialog->setCurrentMese(ui->meseCompetenzeCB->currentIndex());
    printDialog->setCurrentUnita(ui->unitaCompetenzeCB->currentIndex() + 1);

    printDialog->exec();

    if(!printDialog->proceed) {
        exported(QString());
        return;
    }

    progressBar->setVisible(true);
    msgLabel->setText("Esportazione deficit");

    ui->competenzeWidget->setEnabled(false);
    deficitRecuperiExporter.setPath(printDialog->path());
    deficitRecuperiExporter.setMese(printDialog->currentMeseData());
    deficitRecuperiExporter.setUnita(printDialog->currentUnitaData());
    deficitRecuperiExporter.setType(printDialog->type);
    deficitRecuperiExporter.start();
}

void MainWindow::gdCalendarClicked(const QDate &date)
{
    m_competenza->addGuardiaDiurnaDay(date.day());
    elaboraGuardie();
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
}

void MainWindow::gnCalendarClicked(const QDate &date)
{
    m_competenza->addGuardiaNotturnaDay(date.day());
    elaboraGuardie();
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
}

void MainWindow::rCalendarClicked(const QDate &date)
{
    Q_UNUSED(date)
    m_competenza->setRep(rCalendar->getDates());
    elaboraRep();
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
}

void MainWindow::altreCalendarClicked(const QDate &date)
{
    Q_UNUSED(date)
    m_competenza->setAltreAssenze(altreCalendar->getDates());
    populateCompetenzeTab();
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
}

void MainWindow::on_actionBackupDatabase_triggered()
{
    backupDatabase(QTime::currentTime().toString("_hhmmss"), false);
}

void MainWindow::backupDatabase(const QString &time, bool quiet)
{
    const QString fileName = backupFileName(time);
    QDir backupDir(currentDatabase.absolutePath() + QDir::separator() + "backups");

    if(!backupDir.exists()) {
        if(!backupDir.mkdir(backupDir.absolutePath())) {
            QMessageBox::critical(this, "Errore Backup", "Impossibile creare la cartella backups in " + currentDatabase.absolutePath(), QMessageBox::Ok);
            return;
        }
    }

    if(!QFile::copy(currentDatabase.absoluteFilePath(), fileName)) {
        QMessageBox::critical(this, "Errore Backup", "Impossibile eseguire la copia di backup\n" + fileName, QMessageBox::Ok);
    } else {
        if(!quiet)
            QMessageBox::information(this, "Backup Eseguito", "La copia di backup è stata eseguita:\n" + fileName, QMessageBox::Ok);
    }
}

QString MainWindow::backupFileName(const QString &time) const
{
    QString fileName = currentDatabase.completeBaseName();

    QDir backupDir(currentDatabase.absolutePath() + QDir::separator() + "backups");

    return backupDir.absolutePath() + QDir::separator() + fileName + "_backup_" + QDate::currentDate().toString("yyyyMMdd") + time + ".db";
}

void MainWindow::needsBackup()
{
    if(!QFile::exists(backupFileName(""))) {
        backupDatabase("", true);
    }
}

void MainWindow::tabulaFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::CrashExit) {
        QMessageBox::critical(this, "Errore Tabula", "Si è verificato un errore con Tabula.\n"
                              "Codice dell'errore: " + QString::number(exitCode), QMessageBox::Ok);
        handleResults();
        return;
    }

    QFileInfo fi(pdfFile);

    // be sure that a valid path was selected
    if( QFile::exists( fi.absolutePath() + QDir::separator() + "cartellini.csv" ) ) {
        // leggi cartellini
        tabulaReader.setFile(fi.absolutePath() + QDir::separator() + "cartellini.csv");
        tabulaReader.start();
        msgLabel->setText("Importo i cartellini");
    } else {
        handleResults();
    }
}

void MainWindow::tabulaError(QProcess::ProcessError error)
{
    QMessageBox::critical(nullptr, "Errore generazione file csv", "Si è verificato un errore durante la crezione del file CSV.\n"
                          "Codice dell'errore: " + QString::number(error), QMessageBox::Ok);
    handleResults();
}

void MainWindow::on_actionCaricaCsv_triggered()
{
    pdfFile.clear();
    pdfFile = QFileDialog::getOpenFileName(this, tr("Seleziona file cartellini"),
                                                        currentDatabase.absolutePath().isEmpty() ? QDir::homePath() : currentDatabase.absolutePath(),
                                                        tr("CSV (*.csv)"));

    if(pdfFile.isEmpty())
        return;

    // be sure that a valid path was selected
    if( QFile::exists( pdfFile ) ) {
        // leggi cartellini
        m_currentMeseCompetenzeIndex = ui->meseCompetenzeCB->currentIndex();
        m_currentUnitaCompetenzeIndex = ui->unitaCompetenzeCB->currentIndex();
        m_currentDirigenteCompetenzeIndex = ui->dirigentiCompetenzeCB->currentIndex();

        m_loadingTimeCards = true;
        okularReader.setFile(pdfFile);
        okularReader.start();
        progressBar->setVisible(true);
        msgLabel->setText("Importo i cartellini");
        ui->competenzeWidget->setEnabled(false);
    }
}

void MainWindow::minutiCambiati(int mins)
{
    m_competenza->setDmp(ui->dmpHoursEdit->value()*60+mins);
    mostraDifferenzaOre();
    elaboraGuardie();
    elaboraRep();
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
}

void MainWindow::oreCambiate(int ore)
{
    m_competenza->setDmp(ore*60+ui->dmpMinsEdit->value());
    mostraDifferenzaOre();
    elaboraGuardie();
    elaboraRep();
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
}

void MainWindow::orarioGiornalieroCambiato(QTime orario)
{
    m_competenza->setOrarioGiornalieroMod(orario.hour()*60+orario.minute());
    elaboraSommario();
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
}

void MainWindow::on_actionInformazioni_triggered()
{
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

void MainWindow::on_actionDonazione_triggered()
{
    QDesktopServices::openUrl( QUrl( QLatin1String( "https://paypal.me/GCala" ) ) );
}

void MainWindow::on_actionConfigura_triggered()
{
    ConfigDialog config;
    config.exec();

    loadSettings();
}

void MainWindow::on_actionRicalcolaDeficit_triggered()
{
    ui->actionRicalcolaDeficit->setEnabled(false);
    printDialog->setCurrentOp(PrintDialog::ToolOps::CalcDpm);

    printDialog->setCurrentMese(ui->meseCompetenzeCB->currentIndex());
    printDialog->setCurrentUnita(ui->unitaCompetenzeCB->currentIndex() + 1);
    printDialog->setCurrentDirigente(ui->dirigentiCompetenzeCB->currentIndex() + 1);

    printDialog->exec();

    if(!printDialog->proceed) {
        computed();
        return;
    }

    progressBar->setVisible(true);
    msgLabel->setText("Ricalcolo deficit...");

    connect(The::dmpCompute(), SIGNAL(computeFinished()), this, SLOT(computed()));
    connect(The::dmpCompute(), SIGNAL(currentItem(int)), this, SLOT(setCurrentRow(int)));
    connect(The::dmpCompute(), SIGNAL(totalItems(int)), this, SLOT(setTotalRows(int)));

    The::dmpCompute()->setTable(printDialog->currentMeseData());
    The::dmpCompute()->setUnita(printDialog->currentUnitaData());
    The::dmpCompute()->setDirigente(printDialog->currentDirigenteData());
    The::dmpCompute()->start();
}

void MainWindow::on_noteLine_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1)
    m_competenza->setNote(ui->noteLine->text().trimmed());
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
}

void MainWindow::on_actionConnettiDbRemoto_triggered()
{
    if(m_host.isEmpty() || m_dbName.isEmpty()) {
        QMessageBox::critical(this, "Errore Connessione", "I parametri per la connessione remota non sono corretti.\n"
                              "Aprire Impostazioni e configurare host e nome database.", QMessageBox::Cancel);
        return;
    }

    if(m_useSSL) {
        if(m_certFile.isEmpty() || m_keyFile.isEmpty() || !QFile::exists(m_certFile) || !QFile::exists(m_keyFile)) {
            QMessageBox::critical(this, "Errore Connessione", "I file Certificato/Chiave sono necessari per una connessione protetta.\n"
                                  "Aprire Impostazioni e configurare Certificato e Chiave.", QMessageBox::Cancel);
            return;
        }
    }

    askDbUserPassword();
}

void MainWindow::delayedSetup()
{
    if(m_driver == "QMYSQL") {
        if(m_host.isEmpty() || m_dbName.isEmpty()) {
            QMessageBox::warning(this, "Database non configurato",
                                 "Nessun database remoto configurato.\nAprire Impostazioni e indicare Host e Nome Database");
            ui->actionBackupDatabase->setEnabled(false);
            return;
        }
        if(m_useSSL) {
            if(m_certFile.isEmpty() || m_keyFile.isEmpty() || !QFile::exists(m_certFile) || !QFile::exists(m_keyFile)) {
                QMessageBox::critical(this, "Errore Connessione", "I file Certificato/Chiave sono necessari per una connessione protetta.\n"
                                      "Aprire Impostazioni e configurare Certificato e Chiave.", QMessageBox::Cancel);
                ui->actionBackupDatabase->setEnabled(false);
                return;
            }
        }
        askDbUserPassword();
        return;
    }

    if(m_driver == "QSQLITE") {
        if(currentDatabase.fileName().isEmpty()) {
            QMessageBox::warning(this, "Nessun Database", "Nessun database configurato (primo avvio?)\nUsare il menu File per aprire un database esistente \no per crearne uno nuovo.");
            ui->actionBackupDatabase->setEnabled(false);
            return;
        }

        if(!currentDatabase.exists()) {
            QMessageBox::warning(this, "Il database non esiste", "L'ultimo file database usato non è stato trovato:\n" + currentDatabase.absoluteFilePath());
            ui->actionBackupDatabase->setEnabled(false);
            return;
        }

        needsBackup();
    }

    connectToDatabase();
}

void MainWindow::associaUnita(const QString &nome, int &unitaId)
{
    m_nomiDialog->setUnitaLabel(nome);
    m_nomiDialog->exec();
    unitaId = m_nomiDialog->currentUnit();
}

void MainWindow::pagaStrGuardiaCambiato(bool checked)
{
    QFont font = ui->oreStraordinarioGuardieLabel->font();
    font.setStrikeOut(!checked);
    ui->oreStraordinarioGuardieLabel->setFont(font);
    m_competenza->setPagaStrGuardia(checked);
    QString table = ui->meseCompetenzeCB->currentData(Qt::UserRole).toString();
    table.replace("_","m_");
    int value = checked ? 1 : 0;
    SqlQueries::saveMod(table, "pagaStrGuar", ui->dirigentiCompetenzeCB->currentData(Qt::UserRole).toInt(), value);
    elaboraSommario();
}

void MainWindow::on_actionTakeScreenshot_triggered()
{

    QScreen *screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();
    if (!screen)
        return;

    ui->actionTakeScreenshot->setEnabled(false);
    QPixmap originalPixmap = screen->grabWindow(0,this->geometry().x(),this->geometry().y(),this->geometry().width(),this->geometry().height());

//    qDebug() << QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    const QString fileName = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + QDir::separator() + ui->dirigentiCompetenzeCB->currentText() + " - " + ui->meseCompetenzeCB->currentText() + ".png";

    QFile outPix(fileName);

    if(!outPix.open(QIODevice::WriteOnly)) {
        ui->actionTakeScreenshot->setEnabled(true);
        return;
    }

    originalPixmap.save(&outPix, "PNG");

    outPix.close();

    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    ui->actionTakeScreenshot->setEnabled(true);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(index == 2) {
        ui->actionTakeScreenshot->setEnabled(true);
    } else {
        ui->actionTakeScreenshot->setEnabled(false);
    }
}

void MainWindow::on_actionCambiaUnit_triggered()
{
    SwitchUnitDialog dialog;
    dialog.exec();
}

void MainWindow::on_actionManageDirigenti_triggered()
{
    ManageEmployee dialog;
    dialog.exec();
    if(dialog.isChanged()) {
        populateDirigentiCompetenzeCB();
    }
}

void MainWindow::on_editEmployeeButton_clicked()
{
    ManageEmployee dialog;
    dialog.setDipendente(ui->dirigentiCompetenzeCB->currentText().split("-").at(0).trimmed().toInt());
    dialog.exec();
    if(dialog.isChanged()) {
        populateDirigentiCompetenzeCB();
    }
}

void MainWindow::on_editUnitButton_clicked()
{
    ManageUnits dialog;
    dialog.setUnit(ui->unitaCompetenzeCB->currentData(Qt::UserRole).toInt());
    dialog.exec();
    if(dialog.isChanged()) {
        populateUnitaCompetenzeCB();
    }
}

void MainWindow::on_actionManageUnits_triggered()
{
    ManageUnits dialog;
    dialog.exec();
    if(dialog.isChanged()) {
        populateUnitaCompetenzeCB();
    }
}
