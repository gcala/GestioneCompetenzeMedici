/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "printdialog.h"
#include "differenzedialog.h"
#include "competenzeexporterdialog.h"
#include "calendarmanagerrep.h"
#include "calendarmanagerteleconsulto.h"
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
#include "causalewidget.h"
#include "dipendente.h"
#include "reperibilitasemplificata.h"
#include "differenzeexporter.h"
#include "competenzeexporter.h"
#include "apiservice.h"

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
    connect(tabulaProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(tabulaReadOutput()));
    connect(tabulaProcess, SIGNAL(readyReadStandardError()), this, SLOT(tabulaReadOutput()));

    m_loadingTimeCards = false;
    m_anno = 0;
    m_mese = 0;

    ui->restoreCompetenzeButton->setEnabled(false);
    ui->saveCompetenzeButton->setEnabled(false);
    ui->meseCompetenzeCB->setEnabled(true);
    ui->unitaCompetenzeCB->setEnabled(true);
    ui->dirigentiCompetenzeCB->setEnabled(true);

    rCalendarMenu = new QMenu(ui->rCalendarButton);
    rCalendar = new CalendarManagerRep(rCalendarMenu);
    rCalendarAction = new QWidgetAction(rCalendarMenu);
    rCalendarAction->setDefaultWidget(rCalendar);
    rCalendarMenu->addAction(rCalendarAction);
    ui->rCalendarButton->setMenu(rCalendarMenu);
    connect(rCalendar, SIGNAL(datesChanged()), this, SLOT(rCalendarClicked()));

    tCalendarMenu = new QMenu(ui->tCalendarButton);
    tCalendar = new CalendarManagerTeleconsulto(tCalendarMenu);
    tCalendarAction = new QWidgetAction(tCalendarMenu);
    tCalendarAction->setDefaultWidget(tCalendar);
    tCalendarMenu->addAction(tCalendarAction);
    ui->tCalendarButton->setMenu(tCalendarMenu);
    connect(tCalendar, SIGNAL(datesChanged()), this, SLOT(tCalendarClicked()));

    unitaReadOnlyMode = true;
    dirigenteReadOnlyMode = true;
    unitOp = UndefOp;
    dirigenteOp = UndefOp;

    printDialog = new PrintDialog(this);
    differenzeDialog = new DifferenzeDialog(this);
    competenzeExporterDialog = new CompetenzeExporterDialog(this);
    ui->sommV1->setVisible(false);
    ui->sommV2->setVisible(false);

    causaliLayout = new QHBoxLayout(this);
    ui->causaliGB->setLayout(causaliLayout);
    causaliLayout->setContentsMargins(0,0,0,0);
    causaliLayout->setSpacing(0);

    loadSettings();
    Utilities::m_connectionName = "";

    differenzeExporter = new DifferenzeExporter;
    competenzeExporter = new CompetenzeExporter;

    connect(&cartellinoReader, SIGNAL(timeCardsRead()), this, SLOT(handleResults()));
    connect(&cartellinoReader, SIGNAL(totalRows(int)), this, SLOT(setTotalRows(int)));
    connect(&cartellinoReader, SIGNAL(currentRow(int)), this, SLOT(setCurrentRow(int)));
    connect(&cartellinoReader, SIGNAL( selectUnit(QString, int&) ), this, SLOT( associaUnita(QString, int &) ), Qt::BlockingQueuedConnection ) ;
    connect(&cartellinoReader, SIGNAL( cartellinoInvalido(QString) ), this, SLOT( cartellinoInvalidoMessage(QString) ), Qt::BlockingQueuedConnection ) ;
    connect(&cartellinoReader, &CartellinoCompletoReader::anomalieFound, this, &MainWindow::openFile);
    connect(&unitaCompetenzeExporter, SIGNAL(exportFinished(QString)), this, SLOT(exported(QString)));
    connect(&unitaCompetenzeExporter, SIGNAL(totalRows(int)), this, SLOT(setTotalRows(int)));
    connect(&unitaCompetenzeExporter, SIGNAL(currentRow(int)), this, SLOT(setCurrentRow(int)));
    connect(&deficitRecuperiExporter, SIGNAL(exportFinished(QString)), this, SLOT(exported(QString)));
    connect(&deficitRecuperiExporter, SIGNAL(totalRows(int)), this, SLOT(setTotalRows(int)));
    connect(&deficitRecuperiExporter, SIGNAL(currentRow(int)), this, SLOT(setCurrentRow(int)));
    connect(differenzeExporter, &DifferenzeExporter::exportFinished, this, &MainWindow::exported);
    connect(differenzeExporter, &DifferenzeExporter::totalRows, this, &MainWindow::setTotalRows);
    connect(differenzeExporter, &DifferenzeExporter::currentRow, this, &MainWindow::setCurrentRow);
    connect(competenzeExporter, &CompetenzeExporter::exportFinished, this, &MainWindow::exported);
    connect(competenzeExporter, &CompetenzeExporter::totalRows, this, &MainWindow::setTotalRows);
    connect(competenzeExporter, &CompetenzeExporter::currentRow, this, &MainWindow::setCurrentRow);
    connect(ui->actionGeneraFileModifiche, &QAction::triggered, this, &MainWindow::actionGeneraFileModificheTriggered);
    connect(ui->actionGeneraCompetenze, &QAction::triggered, this, &MainWindow::actionGeneraCompetenzeTriggered);

    m_nomiDialog = new NomiUnitaDialog;

    QTimer::singleShot(500, this, SLOT(delayedSetup()));
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete printDialog;
    delete differenzeDialog;
    delete competenzeExporterDialog;
    delete m_nomiDialog;
    delete ui;
}

void MainWindow::askDbUserPassword()
{
    LoginDialog login(Utilities::m_host, Utilities::m_dbName, this);
    login.setUsername(Utilities::m_lastUsername);
//    login.setPassword(Utilities::m_lastPassword);
//    login.disablePassButton(Utilities::m_lastPassword.isEmpty());
    login.exec();

    if(login.canceled()) {
        return;
    }

    Utilities::m_lastUsername = login.username();
    Utilities::m_lastPassword = login.password();

    Utilities::m_driver = "QMYSQL";

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
//    ui->deficitOrarioLabel->setText(m_competenza->deficitOrario() < 0 ?
//                                    Utilities::inOrario(abs(m_competenza->deficitOrario())) : "//");

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
}

void MainWindow::connectToDatabase()
{
    if(!The::dbManager()->createConnection()) {
        setWindowTitle("Gestione Competenze Medici");
        return;
    }

    QFileInfo fi(The::dbManager()->currentDatabaseName());
    if(Utilities::m_driver == "QMYSQL")
        setWindowTitle("Gestione Competenze Medici - " + Utilities::m_dbName + "@" + Utilities::m_host);
    else
        setWindowTitle("Gestione Competenze Medici - " + fi.completeBaseName());

    populateMeseCompetenzeCB();
    m_nomiDialog->populateUnits();
}

void MainWindow::populateMeseCompetenzeCB()
{
    ui->meseCompetenzeCB->clear();
    printDialog->clearMese();
    differenzeDialog->clearMese();
    competenzeExporterDialog->clearMese();

    QString errorMsg;
    const auto timeCards = ApiService::instance().getTimecardsList(&errorMsg);

    if (!errorMsg.isEmpty()) {
        // Gestisci l'errore se necessario
        qDebug() << "Errore nel recupero delle timecards:" << errorMsg;
        return;
    }

    // Procedi con i dati ottenuti
    QStringList::const_iterator i = timeCards.constEnd();
    while (i != timeCards.constBegin()) {
        --i;
        QString ss = (*i).split("_").last();
        ui->meseCompetenzeCB->addItem(QLocale().monthName((*i).right(2).toInt()) + " " + ss.left(4), *i);
        printDialog->addMese(QLocale().monthName((*i).right(2).toInt()) + " " + ss.left(4), *i);
        differenzeDialog->addMese(QLocale().monthName((*i).right(2).toInt()) + " " + ss.left(4), *i);
        competenzeExporterDialog->addMese(QLocale().monthName((*i).right(2).toInt()) + " " + ss.left(4), *i);
    }
}

void MainWindow::populateUnitaCompetenzeCB()
{
    ui->unitaCompetenzeCB->clear();

    if(ui->meseCompetenzeCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    QString errorMsg;
    const auto unitaList = ApiService::instance().getUnitaDataFromTimecard(
        ui->meseCompetenzeCB->currentData(Qt::UserRole).toString(),
        &errorMsg
        );

    if (!errorMsg.isEmpty()) {
        qDebug() << "Errore nel recupero delle unità:" << errorMsg;
        return;
    }

    for (const UnitaDataTimecard& unita : unitaList) {
        ui->unitaCompetenzeCB->addItem(QString::number(unita.idUnita) + " - " + unita.nome, unita.id);
    }

    rCalendar->setReperibilita(m_reperibilita);
}

void MainWindow::populateDirigentiCompetenzeCB()
{
    ui->dirigentiCompetenzeCB->clear();

    if(ui->unitaCompetenzeCB->currentData(Qt::UserRole).toString().isEmpty())
        return;

    QString errorMsg;
    const auto doctors = ApiService::instance().getDoctorDataFromUnitaInTimecard(
        ui->meseCompetenzeCB->currentData(Qt::UserRole).toString(),
        ui->unitaCompetenzeCB->currentData(Qt::UserRole).toInt(),
        &errorMsg
        );

    if (!errorMsg.isEmpty()) {
        qDebug() << "Errore nel recupero dei medici:" << errorMsg;
        return;
    }

    for (const DoctorData& doctor : doctors) {
        ui->dirigentiCompetenzeCB->addItem(QString::number(doctor.matricola) + " - " + doctor.nome, doctor.id);
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
            Utilities::m_driver = "QSQLITE";
            saveSettings();
            needsBackup();
            setupDbConnectionParameters();
            connectToDatabase();
            populateMeseCompetenzeCB();
            ui->actionBackupDatabase->setEnabled(true);
        }
        return;
    }
    delete databaseWizard;

    // ultima opzione: apri database remoto
    askDbUserPassword();
}

void MainWindow::loadSettings()
{
    QSettings settings;
    Utilities::m_driver = settings.value("lastDriver", "QSQLITE").toString();
    Utilities::m_lastUsername = settings.value("lastUsername", "").toString();
    Utilities::m_importPath = settings.value("importPath", QDir::homePath()).toString();
    Utilities::m_exportPath = settings.value("exportPath", QDir::homePath()).toString();
    Utilities::m_useSSL = settings.value("useSSL", false).toBool();

//    Utilities::m_driver = "QMYSQL";
    Utilities::m_host = settings.value("host", "").toString();
    Utilities::m_dbName = settings.value("dbName", "competenze").toString();
    Utilities::m_certFile = "client-cert.pem";
    Utilities::m_keyFile = "client-key.pem";

    Utilities::m_importPath = settings.value("importPath", QDir::homePath()).toString();
    Utilities::m_exportPath = settings.value("exportPath", QDir::homePath()).toString();


    currentDatabase.setFile(settings.value("lastDatabasePath", "").toString());
    printDialog->setPath(Utilities::m_exportPath);
    differenzeDialog->setPath(Utilities::m_exportPath);
    competenzeExporterDialog->setPath(Utilities::m_exportPath);
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
    settings.setValue("lastDriver", Utilities::m_driver);
    settings.setValue("host", Utilities::m_host);
    settings.setValue("dbName", Utilities::m_dbName);
    settings.setValue("certFile", Utilities::m_certFile);
    settings.setValue("keyFile", Utilities::m_keyFile);
    settings.setValue("importPath", Utilities::m_importPath);
    settings.setValue("exportPath", Utilities::m_exportPath);
    settings.setValue("lastUsername", Utilities::m_lastUsername);
    settings.setValue("useSSL", Utilities::m_useSSL);
    settings.setValue("windowGeometry", geometry());
    settings.setValue("isMaximized", isMaximized());
}

void MainWindow::on_actionCaricaPdf_triggered()
{
    pdfFile.clear();
    pdfFile = QFileDialog::getOpenFileName(this, tr("Seleziona pdf cartellini"),
                                                    Utilities::m_importPath,
                                                    tr("PDF (*.pdf)"));

    if(pdfFile.isEmpty())
        return;

    if(!QFile::exists(pdfFile)) {
        QMessageBox::critical(this, "File non esistente", "Il file selezionato non esiste: " + pdfFile, QMessageBox::Ok);
        return;
    }

    if(!QFile::exists(m_tabulaPath)) {
        QMessageBox::critical(this, "File non esistente", "Il file tabula.jar non esiste: " + m_tabulaPath, QMessageBox::Ok);
        return;
    }

    if(!QFile::exists(m_javaPath)) {
        QMessageBox::critical(this, "File non esistente", "L'eseguibe java non esiste: " + m_javaPath, QMessageBox::Ok);
        return;
    }

    const QFileInfo fi(pdfFile);
    Utilities::m_importPath = fi.absolutePath();

    if(QFile::exists(pdfFile)) {
        m_loadingTimeCards = true;
        progressBar->setVisible(true);
        progressBar->setMinimum(0);
        progressBar->setMaximum(0);
        msgLabel->setText("Converto pdf in csv");
        ui->competenzeWidget->setEnabled(false);

        QStringList arguments;
        arguments << "-jar" << m_tabulaPath;
        arguments << "-n";
        arguments << "-p" << "all";
        arguments << "-a" << "1.0,6.93,606.375,769.23";
        arguments << "-c" << "21.40,28.71,53.46,70.00,78.21,114.84,150.48,187.11,222.75,259.38,296.01,331.65,368.28,401.94,435.6,470.25,504.9,537.00,567.27,594.50,624.69,653.4,682.11,710.21,739.53";
//        arguments << "-c" << "21.978,28.908,52.668,69.498,78.408,114.048,149.688,186.318,221.958,258.588,295.218,330.858,367.488,402.138,434.808,470.448,504.108,537.768,566.478,596.178,624.888,653.598,681.318,711.018,739.728";
        arguments << pdfFile;
        arguments << "-o" << fi.absolutePath() + QDir::separator() + "cartellini.csv";

        QFile::remove(fi.absolutePath() + QDir::separator() + "cartellini.csv");

        m_currentMeseCompetenzeIndex = ui->meseCompetenzeCB->currentIndex();
        m_currentUnitaCompetenzeIndex = ui->unitaCompetenzeCB->currentIndex();
        m_currentDirigenteCompetenzeIndex = ui->dirigentiCompetenzeCB->currentIndex();

#ifdef Q_OS_LINUX
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("LD_LIBRARY_PATH", "/usr/lib/jvm/java-11-openjdk/lib/");
        tabulaProcess->setProcessEnvironment(env);
#endif

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
    ui->mainToolBar->setEnabled(true);
    ui->competenzeWidget->setEnabled(true);
    progressBar->setVisible(false);
    msgLabel->setText("");

//    if(!file.isEmpty())
//        QDesktopServices::openUrl(QUrl::fromLocalFile(file));
}

void MainWindow::openFile(const QString &file)
{
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
    ui->meseCompetenzeCB->setEnabled(true);
    ui->unitaCompetenzeCB->setEnabled(true);
    ui->dirigentiCompetenzeCB->setEnabled(true);
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
    ui->meseCompetenzeCB->setEnabled(true);
    ui->unitaCompetenzeCB->setEnabled(true);
    ui->dirigentiCompetenzeCB->setEnabled(true);
    ui->restoreCompetenzeButton->setEnabled(m_competenza->isRestorable());
}

void MainWindow::on_meseCompetenzeCB_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    m_anno = ui->meseCompetenzeCB->currentData(Qt::UserRole).toString().split("_").last().left(4).toInt();
    m_mese = ui->meseCompetenzeCB->currentData(Qt::UserRole).toString().split("_").last().right(2).toInt();
    m_currDate.setDate(m_anno,
                       m_mese,
                       1);
    const int unitaData = ui->unitaCompetenzeCB->currentData(Qt::UserRole).toInt();
    const int docData = ui->dirigentiCompetenzeCB->currentData(Qt::UserRole).toInt();
    populateUnitaCompetenzeCB();
    const int unitaIndex = ui->unitaCompetenzeCB->findData(unitaData, Qt::UserRole);
    ui->unitaCompetenzeCB->show();
    ui->unitaCompetenzeCB->setCurrentIndex(unitaIndex < 0 ? 0 : unitaIndex);

    const int docIndex = ui->dirigentiCompetenzeCB->findData(docData, Qt::UserRole);
    ui->dirigentiCompetenzeCB->setCurrentIndex(docIndex < 0 ? 0 : docIndex);
    ui->dirigentiCompetenzeCB->show();
//    ui->sommV1->setVisible(date < Utilities::ccnl1618Date);
    ui->sommV1->setVisible(true);
    ui->sommV2->setVisible(m_currDate >= Utilities::ccnl1618Date);
}

void MainWindow::on_unitaCompetenzeCB_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    populateDirigentiCompetenzeCB();

    QString errorMsg;
    m_reperibilita = ApiService::instance().getReperibilitaSemplificata(
        ui->unitaCompetenzeCB->currentData(Qt::UserRole).toInt(),
        m_anno,
        m_mese,
        &errorMsg
        );

    if (!errorMsg.isEmpty()) {
        qDebug() << "Errore nel recupero della reperibilità:" << errorMsg;
        // Gestisci l'errore
    }

    rCalendar->setReperibilita(m_reperibilita);
    QLocale loc;
    ui->pdFeriale->setText(loc.toString(m_reperibilita->feriale(),'f',1));
    ui->pdSabato->setText(loc.toString(m_reperibilita->sabato(),'f',1));
    ui->pdFestivo->setText(loc.toString(m_reperibilita->festivo(),'f',1));
    ui->pdPrefestivo->setText(loc.toString(m_reperibilita->prefestivo() ? m_reperibilita->sabato() : 0.0,'f',1));

    const auto orePagateMap = ApiService::instance().getOrePagateFromUnit(
        ui->unitaCompetenzeCB->currentData(Qt::UserRole).toInt(),
        &errorMsg
        );

    if (!errorMsg.isEmpty()) {
        qDebug() << "Errore nel recupero delle ore pagate:" << errorMsg;
        return;
    }

    QMapIterator<QDate, OrePagate> i(orePagateMap);
    i.toBack();
    while (i.hasPrevious()) {
        i.previous();
        const QDate date(m_anno, m_mese, 1);
        if (i.key() <= date) {
            ui->oreNotturnePagate->setText(QString::number(i.value().orePagate));
            break;
        }
    }
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

    populateCausali();
    populateCompetenzeTab();

    ui->restoreCompetenzeButton->setEnabled(m_competenza->isRestorable());
    ui->saveCompetenzeButton->setEnabled(false);
    ui->meseCompetenzeCB->setEnabled(true);
    ui->unitaCompetenzeCB->setEnabled(true);
    ui->dirigentiCompetenzeCB->setEnabled(true);
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

//    gdCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());
//    gnCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());
    rCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());
    tCalendar->setDateRange(m_competenza->dataIniziale(), m_competenza->dataFinale());

//    gdCalendar->setDates(m_competenza->gdDates());
//    gdCalendar->setMezzeDates(m_competenza->mgdDates());
//    gnCalendar->setDates(m_competenza->gnDates());
    rCalendar->setDates(m_competenza->turniProntaDisponibilita());
    tCalendar->setDates(m_competenza->teleconsulto());

    elaboraSommario();

    ui->noteLine->setText(m_competenza->note());

    connect(ui->dmpHoursEdit, SIGNAL(valueChanged(int)), this, SLOT(oreCambiate(int)));
    connect(ui->dmpMinsEdit, SIGNAL(valueChanged(int)), this, SLOT(minutiCambiati(int)));
//    connect(ui->orarioGiornalieroEdit, SIGNAL(timeChanged(QTime)), this, SLOT(orarioGiornalieroCambiato(QTime)));
    connect(ui->pagaStrGuardiaCB, SIGNAL(toggled(bool)), SLOT(pagaStrGuardiaCambiato(bool)));
}

void MainWindow::populateCausali()
{
    QList<CausaleWidget *> wList = ui->causaliGB->findChildren< CausaleWidget *>();
    foreach( CausaleWidget *l, wList ) {
        l->deleteLater();
    }
    for (int i = 0; i < causaliLayout->count(); ++i) {
        QLayoutItem *layoutItem = causaliLayout->itemAt(i);
        if (layoutItem->spacerItem()) {
            causaliLayout->removeItem(layoutItem);
            // You could also use: layout->takeAt(i);
            delete layoutItem;
            --i;
        }
    }
    const auto map = m_competenza->dipendente()->altreCausali(); // QMap<QString, int>
    auto i = map.constBegin();

    while (i != map.constEnd()) {
        CausaleWidget *cw = new CausaleWidget(i.key(),
                                              i.value().second,
                                              m_competenza->dataIniziale().year(),
                                              m_competenza->dataIniziale().month(),
                                              i.value().first,
                                              ui->causaliGB);

        causaliLayout->addWidget(cw);
        i++;
    }
    causaliLayout->addStretch(10);
}

void MainWindow::elaboraGuardie()
{
    ui->gnPainterWidget->setMeseAnno(m_competenza->dataIniziale().month(), m_competenza->dataIniziale().year());
    ui->gdPainterWidget->setMeseAnno(m_competenza->dataIniziale().month(), m_competenza->dataIniziale().year());
    ui->gnPainterWidget->setGuardiaMap(m_competenza->guardiaNotturnaMap());
    ui->gdPainterWidget->setGuardiaMap(m_competenza->guardiaDiurnaMap());
    ui->gdPainterWidget->setMezzaGuardiaMap(m_competenza->mezzaGuardiaDiurnaMap());

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
//    ui->notteLabel->setText(m_competenza->notte() > 0 ? QString::number(m_competenza->notte()) : "//");
    ui->festivoLabel->setText(m_competenza->numeroGuardieDiurneFatte() > 0 ? QString::number(m_competenza->numeroGuardieDiurneFatte()) : "//");
//    ui->oreStraordinarioGuardieLabel->setText(m_competenza->oreStraordinarioGuardie());
    if(m_competenza->numeroOreGuardieNotturnePagate() == 0 && m_competenza->numeroGrandiFestivitaPagate() == 0) {
        ui->oreStraordinarioGuardieLabel->setText("//");
    } else {
        ui->oreStraordinarioGuardieLabel->setText((m_competenza->numeroGrandiFestivitaPagate() > 0 ? QString::number(m_competenza->numeroGrandiFestivitaPagate() * 480) + " + " : "") + QString::number(m_competenza->numeroOreGuardieNotturnePagate()));
    }

    ui->numGuarDiuPag->setText(m_competenza->numeroGuardieDiurneFatte() > 0 ? QString::number(m_competenza->numeroGuardieDiurneFatte()) : "//");
    ui->numGuarNottPag->setText((m_competenza->numeroGuardieNotturneFatte() + m_competenza->numeroGrandiFestivitaFatte() - m_competenza->numeroGrandiFestivitaPagate()) > 0 ? QString::number(m_competenza->numeroGuardieNotturneFatte() + m_competenza->numeroGrandiFestivitaFatte() - m_competenza->numeroGrandiFestivitaPagate()) : "//");
    ui->numGrFestPag->setText(m_competenza->numeroGrandiFestivitaPagate() > 0 ? QString::number(m_competenza->numeroGrandiFestivitaPagate()) : "//");
}

void MainWindow::elaboraRep()
{
    QLocale loc;
    ui->rPainterWidget->setRepMap(m_competenza->turniProntaDisponibilita());
    double whole = 0.0, fractional = 0.0;
    if(m_competenza->numeroTurniProntaDisponibilita() > 0) {
        fractional = std::modf(m_competenza->numeroTurniProntaDisponibilita(), &whole);
        if(m_currDate >= Utilities::ccnl1921Date) {
            if(whole > 10) {
                ui->repLabel->setText(loc.toString(10.0*12.0,'f',1));
                ui->repOltre10Label->setText(loc.toString((whole - 10 + fractional)*12.0,'f',0));
            } else if(whole > 0) { // 25
                ui->repLabel->setText(loc.toString((whole+fractional)*12.0,'f',1));
                ui->repOltre10Label->setText(loc.toString(0.0,'f',0));
            } else {
                ui->repLabel->setText(loc.toString(0.0,'f',1));
                ui->repOltre10Label->setText(loc.toString(0.0,'f',0));
            }
        } else {
            if(whole > 10) {
                ui->repLabel->setText(loc.toString(10.0+fractional,'f',1));
                ui->repOltre10Label->setText(loc.toString(whole - 10,'f',0));
            } else if(whole > 0) { // 25
                ui->repLabel->setText(loc.toString(whole+fractional,'f',1));
                ui->repOltre10Label->setText(loc.toString(0.0,'f',0));
            } else {
                ui->repLabel->setText(loc.toString(0.0,'f',1));
                ui->repOltre10Label->setText(loc.toString(0.0,'f',0));
            }
        }
    } else {
        ui->repLabel->setText("//");
        ui->repOltre10Label->setText("//");
    }

    ui->r_d_fer->setText(Utilities::inOrario(m_competenza->r_d_fer()));
    ui->r_d_fes->setText(Utilities::inOrario(m_competenza->r_d_fes()));
    ui->r_n_fer->setText(Utilities::inOrario(m_competenza->r_n_fer()));
    ui->r_n_fes->setText(Utilities::inOrario(m_competenza->r_n_fes()));
    ui->totOreRep->setText(Utilities::inOrario(m_competenza->minutiGrep()));
}

void MainWindow::elaboraTeleconsulto()
{
    ui->tPainterWidget->setRepMap(m_competenza->teleconsulto());
    ui->teleconsultoLabel->setText(m_competenza->numeroTurniTeleconsulto() > 0 ? QString::number(m_competenza->numeroTurniTeleconsulto()) : "//");
}

void MainWindow::elaboraSommario()
{
    if(m_competenza->orarioGiornaliero() == "0:00")
        ui->orarioGiornalieroEdit->setTime(QTime(0,0));
    else
        ui->orarioGiornalieroEdit->setTime(QTime::fromString(m_competenza->orarioGiornaliero(),"h:mm"));
    ui->oreDovuteLabel->setText(m_competenza->oreDovute());
    ui->oreEffettuateLabel->setText(m_competenza->oreEffettuate());

//    ui->oreStraordinarioRepLabel->setText(m_competenza->oreRepPagate() > 0 ? QString::number(m_competenza->oreRepPagate()) : "//");
    ui->oreStraordinarioRepLabel->setText(m_competenza->oreReperibilitaPagate() > 0 ? QString::number(m_competenza->oreReperibilitaPagate()) : "//");
    ui->oreStraordinarioAltriMotiviLabel->setText(m_competenza->oreGuardieDiurnePagate() > 0 ? QString::number(m_competenza->oreGuardieDiurnePagate()) : "//");

    mostraDifferenzaOre();
    elaboraGuardie();
    elaboraRep();
    elaboraTeleconsulto();
//    ui->residuoLabel->setText(m_competenza->residuoOreNonPagate() > 0 ? Utilities::inOrario(m_competenza->residuoOreNonPagate()) : "//");
    ui->residuoLabel->setText(m_competenza->saldoMinuti() > 0 ? Utilities::inOrario(m_competenza->saldoMinuti()) : "//");

//    if(m_competenza->numOreRecuperabili() < 0)
//        ui->oreRecLabel->setStyleSheet("color: red;");
//    else
//        ui->oreRecLabel->setStyleSheet("color: green;");

//    if(m_competenza->numOreRecuperabili() == 0 && m_competenza->recuperiMeseSuccessivo().second == 0)
//        ui->oreRecLabel->setText("//");
//    else {
//        ui->oreRecLabel->setText(Utilities::inOrario(m_competenza->numOreRecuperabili()) + " (" + QString::number(m_competenza->recuperiMeseSuccessivo().second) + ")");
//    }

    if(m_competenza->minutiRecuperabili() < 0)
        ui->oreRecLabel->setStyleSheet("color: red;");
    else
        ui->oreRecLabel->setStyleSheet("color: green;");

    if(m_competenza->minutiRecuperabili() == 0 && m_competenza->recuperiMeseSuccessivo().second == 0)
        ui->oreRecLabel->setText("//");
    else {
        ui->oreRecLabel->setText(Utilities::inOrario(m_competenza->minutiRecuperabili()) + " (" + QString::number(m_competenza->recuperiMeseSuccessivo().second) + ")");
    }

    // ui->oreNonRecLabel->setText(m_competenza->minutiNonRecuperabili() > 0 ? Utilities::inOrario(m_competenza->minutiNonRecuperabili()) : "//");
    ui->oreNonRecLabel->setText(m_competenza->minutiNonRecuperabili() > 0 ? Utilities::inOrario(m_competenza->minutiNonRecuperabili()) : "//");
}

void MainWindow::setupDbConnectionParameters()
{
    Utilities::m_localDbFileName = currentDatabase.absoluteFilePath();
}

void MainWindow::on_actionStampaCompetenzeUnita_triggered()
{
    ui->mainToolBar->setEnabled(false);
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

void MainWindow::actionGeneraFileModificheTriggered()
{
    ui->mainToolBar->setEnabled(false);

    differenzeDialog->setCurrentMese(ui->meseCompetenzeCB->currentIndex());
    differenzeDialog->setCurrentUnita(ui->unitaCompetenzeCB->currentIndex() + 1);

    differenzeDialog->exec();

    if(!differenzeDialog->proceed) {
        exported(QStringLiteral());
        return;
    }

//    exported(QStringLiteral());
//    return;

    progressBar->setVisible(true);
    msgLabel->setText("Esportazione modifiche");

    ui->competenzeWidget->setEnabled(false);
    differenzeExporter->setPath(differenzeDialog->path());
    differenzeExporter->setMese(differenzeDialog->currentMeseData());
    differenzeExporter->setUnita(differenzeDialog->currentUnitaData());
    differenzeExporter->setStoricizza(differenzeDialog->storicizzaIsChecked());
    differenzeExporter->setMesePagamento(differenzeDialog->mesePagamento());
    differenzeExporter->setAnnoPagamento(differenzeDialog->annoPagamento());
    differenzeExporter->start();
}

void MainWindow::actionGeneraCompetenzeTriggered()
{
    ui->mainToolBar->setEnabled(false);

    competenzeExporterDialog->setCurrentMese(ui->meseCompetenzeCB->currentIndex());
    competenzeExporterDialog->setCurrentUnita(ui->unitaCompetenzeCB->currentIndex() + 1);

    competenzeExporterDialog->exec();

    if(!competenzeExporterDialog->proceed) {
        exported(QStringLiteral());
        return;
    }

    progressBar->setVisible(true);
    msgLabel->setText("Esportazione competenze");

    ui->competenzeWidget->setEnabled(false);
    competenzeExporter->setPath(competenzeExporterDialog->path());
    competenzeExporter->setMeseDa(competenzeExporterDialog->meseDa());
    competenzeExporter->setMeseA(competenzeExporterDialog->meseA());
    competenzeExporter->setUnita(competenzeExporterDialog->currentUnitaData());
    competenzeExporter->setMatricole(competenzeExporterDialog->matricole());
    competenzeExporter->start();
}

void MainWindow::on_actionPrintDeficit_triggered()
{
    ui->mainToolBar->setEnabled(false);
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
    ui->meseCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->unitaCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->dirigentiCompetenzeCB->setEnabled(!m_competenza->isModded());
}

void MainWindow::gnCalendarClicked(const QDate &date)
{
    m_competenza->addGuardiaNotturnaDay(date.day());
    elaboraGuardie();
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
    ui->meseCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->unitaCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->dirigentiCompetenzeCB->setEnabled(!m_competenza->isModded());
}

void MainWindow::rCalendarClicked()
{
    m_competenza->setTurniProntaDisponibilita(rCalendar->getDates());
    elaboraRep();
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
    ui->meseCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->unitaCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->dirigentiCompetenzeCB->setEnabled(!m_competenza->isModded());
}

void MainWindow::tCalendarClicked()
{
    m_competenza->setTeleconsulto(tCalendar->getDates());
    elaboraTeleconsulto();
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
    ui->meseCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->unitaCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->dirigentiCompetenzeCB->setEnabled(!m_competenza->isModded());
}

void MainWindow::on_actionBackupDatabase_triggered()
{
    backupDatabase(QTime::currentTime().toString("_hhmmss"), false);
}

void MainWindow::backupDatabase(const QString &time, bool quiet)
{
    // curl -k -T Statistica.db -u "H8qM7nBPZkozLyx:" -H 'X-Requested-With: XMLHttpRequest' https://cloud.cuteworks.it/public.php/webdav/Statistica.d
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
        cartellinoReader.setFile(fi.absolutePath() + QDir::separator() + "cartellini.csv");
        cartellinoReader.setDriver(Utilities::m_driver);
        cartellinoReader.start();
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

void MainWindow::tabulaReadOutput()
{
    if(!tabulaProcess->readAllStandardError().isEmpty())
        qDebug() << "StandardError:" << tabulaProcess->readAllStandardError();

    if(!tabulaProcess->readAllStandardOutput().isEmpty())
        qDebug() << "StandardOutput" << tabulaProcess->readAllStandardOutput();
}

void MainWindow::minutiCambiati(int mins)
{
    m_competenza->setDmp(ui->dmpHoursEdit->value()*60+mins);
    elaboraSommario();
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
    ui->meseCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->unitaCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->dirigentiCompetenzeCB->setEnabled(!m_competenza->isModded());
}

void MainWindow::oreCambiate(int ore)
{
    m_competenza->setDmp(ore*60+ui->dmpMinsEdit->value());
    elaboraSommario();
    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
    ui->meseCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->unitaCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->dirigentiCompetenzeCB->setEnabled(!m_competenza->isModded());
}

//void MainWindow::orarioGiornalieroCambiato(QTime orario)
//{
//    m_competenza->setOrarioGiornalieroMod(orario.hour()*60+orario.minute());
//    elaboraSommario();
//    ui->saveCompetenzeButton->setEnabled(m_competenza->isModded());
//    ui->meseCompetenzeCB->setEnabled(!m_competenza->isModded());
//    ui->unitaCompetenzeCB->setEnabled(!m_competenza->isModded());
//    ui->dirigentiCompetenzeCB->setEnabled(!m_competenza->isModded());
//}

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
    ui->meseCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->unitaCompetenzeCB->setEnabled(!m_competenza->isModded());
    ui->dirigentiCompetenzeCB->setEnabled(!m_competenza->isModded());
}

void MainWindow::on_actionConnettiDbRemoto_triggered()
{
    if(Utilities::m_host.isEmpty() || Utilities::m_dbName.isEmpty()) {
        QMessageBox::critical(this, "Errore Connessione", "I parametri per la connessione remota non sono corretti.\n"
                              "Aprire Impostazioni e configurare host e nome database.", QMessageBox::Cancel);
        return;
    }

//    if(Utilities::m_certFile.isEmpty() || Utilities::m_keyFile.isEmpty() || !QFile::exists(Utilities::m_certFile) || !QFile::exists(Utilities::m_keyFile)) {
//        QMessageBox::critical(this, "Errore Connessione", "I file Certificato/Chiave sono necessari per una connessione protetta.\n"
//                                  "Aprire Impostazioni e configurare Certificato e Chiave.", QMessageBox::Cancel);
//        return;
//    }

    askDbUserPassword();
}

void MainWindow::delayedSetup()
{
    if(Utilities::m_driver == "QMYSQL") {
        if(Utilities::m_host.isEmpty() || Utilities::m_dbName.isEmpty()) {
            QMessageBox::warning(this, "Database non configurato",
                                 "Nessun database remoto configurato.\nAprire Impostazioni e indicare Host e Nome Database");
            ui->actionBackupDatabase->setEnabled(false);
            return;
        }
//        if(Utilities::m_certFile.isEmpty() || Utilities::m_keyFile.isEmpty() || !QFile::exists(Utilities::m_certFile) || !QFile::exists(Utilities::m_keyFile)) {
//            QMessageBox::critical(this, "Errore Connessione", "I file Certificato/Chiave sono necessari per una connessione protetta.\n"
//                                      "Aprire Impostazioni e configurare Certificato e Chiave.", QMessageBox::Cancel);
//            ui->actionBackupDatabase->setEnabled(false);
//            return;
//        }
        askDbUserPassword();
        return;
    }

    if(Utilities::m_driver == "QSQLITE") {
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

void MainWindow::associaUnita(const QString &nominativo, int &unitaId)
{
    m_nomiDialog->setNominativoLabel(nominativo);
    m_nomiDialog->exec();
    unitaId = m_nomiDialog->currentUnit();
}

void MainWindow::cartellinoReaderError(const QString &msg)
{
    QMessageBox::critical(this, "Errore", msg, QMessageBox::Ok);
    handleResults();
}

void MainWindow::cartellinoInvalidoMessage(const QString &msg)
{
    QMessageBox::critical(this, "Errore", msg, QMessageBox::Ok);
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
    QString errorMsg;
    bool success = ApiService::instance().saveMod(
        table,
        "pagaStrGuar",
        ui->dirigentiCompetenzeCB->currentData(Qt::UserRole).toInt(),
        value,
        &errorMsg
        );

    if (!success && !errorMsg.isEmpty()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << errorMsg;
    }
    errorMsg.clear();
    success = ApiService::instance().abilitaDisabilitaStraordinario(
        m_competenza->dipendente()->matricola(),
        checked,
        m_anno,
        m_mese,
        &errorMsg
        );

    if (!success && !errorMsg.isEmpty()) {
        qDebug() << "Errore nell'abilitazione/disabilitazione straordinario:" << errorMsg;
        // Opzionale: mostrare un messaggio di errore all'utente
    }
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
