/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cartellinocompletoreader.h"
#include "competenzeunitaexporter.h"
#include "deficitrecuperiexporter.h"

#include <QMainWindow>
#include <QFileInfo>
#include <QTime>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class QLabel;
class QProgressBar;
class QSqlQueryModel;
class PrintDialog;
class CalendarManager;
class CalendarManagerRep;
class QWidgetAction;
class Competenza;
class NomiUnitaDialog;
class QHBoxLayout;
class ReperibilitaSemplificata;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum UnitsOps { UndefOp, AddUnit, RemoveUnit, EditUnit };
    Q_ENUM(UnitsOps)

private slots:
    void handleResults();
    void exported(const QString &file);
    void computed();
    void setTotalRows(int);
    void setCurrentRow(int);
    void tabulaFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void tabulaError(QProcess::ProcessError error);
    void delayedSetup();
    void associaUnita(const QString &nominativo, int &unitaId);

    void on_actionApriDatabase_triggered();
    void on_actionCaricaPdf_triggered();
    void on_actionNuovoDatabase_triggered();
    void on_saveCompetenzeButton_clicked();
    void on_restoreCompetenzeButton_clicked();
    void on_meseCompetenzeCB_currentIndexChanged(int index);
    void on_unitaCompetenzeCB_currentIndexChanged(int index);
    void on_dirigentiCompetenzeCB_currentIndexChanged(int index);
    void on_actionStampaCompetenzeUnita_triggered();
    void gdCalendarClicked(const QDate &date);
    void gnCalendarClicked(const QDate &date);
    void rCalendarClicked();
    void on_actionBackupDatabase_triggered();
    void minutiCambiati(int mins);
    void oreCambiate(int ore);
    void orarioGiornalieroCambiato(QTime orario);
    void on_actionInformazioni_triggered();
    void on_actionDonazione_triggered();
    void on_actionConfigura_triggered();
    void on_actionRicalcolaDeficit_triggered();
    void on_noteLine_textEdited(const QString &arg1);
    void on_actionConnettiDbRemoto_triggered();
    void pagaStrGuardiaCambiato(bool checked);
    void on_actionTakeScreenshot_triggered();
    void on_tabWidget_currentChanged(int index);
    void on_actionPrintDeficit_triggered();
    void on_actionCambiaUnit_triggered();
    void on_actionManageDirigenti_triggered();
    void on_editEmployeeButton_clicked();
    void on_editUnitButton_clicked();
    void on_actionManageUnits_triggered();

private:
    Ui::MainWindow *ui;
    QHBoxLayout *causaliLayout;
    bool unitaReadOnlyMode;
    bool dirigenteReadOnlyMode;
    int currentUnitaIndex;
    int currentDirigenteIndex;
    int currentDirigenteUnitaIndex;
    UnitsOps unitOp;
    UnitsOps dirigenteOp;
    Competenza *m_competenza;
    bool m_loadingTimeCards;
    QProgressBar *progressBar;
    QLabel *msgLabel;
    bool m_isScheduledBackup;
    QProcess *tabulaProcess;
    QString pdfFile;
    QString m_photosPath;
    QString m_javaPath;
    QString m_tabulaPath;
    int m_currentMeseCompetenzeIndex;
    int m_currentUnitaCompetenzeIndex;
    int m_currentDirigenteCompetenzeIndex;

    QMenu *gdCalendarMenu;
    CalendarManager *gdCalendar;
    QWidgetAction *gdCalendarAction;

    QMenu *gnCalendarMenu;
    CalendarManager *gnCalendar;
    QWidgetAction *gnCalendarAction;

    QMenu *rCalendarMenu;
    CalendarManagerRep *rCalendar;
    QWidgetAction *rCalendarAction;

    NomiUnitaDialog *m_nomiDialog;

    QString lastUnitaNum;
    QString lastUnitaRaggr;
    QString lastUnitaNome;
    QString lastUnitaBreve;
    QString lastOtherNames;
    QString lastDirigenteNome;
    int lastDirigenteMatricola;
    int lastDirigenteUnitaIndex;
    QString lastDirigenteUnitaNome;
    QFileInfo currentDatabase;
    PrintDialog *printDialog;
    CartellinoCompletoReader cartellinoReader;
    CompetenzeUnitaExporter unitaCompetenzeExporter;
    DeficitRecuperiExporter deficitRecuperiExporter;
    ReperibilitaSemplificata *m_reperibilita;

    QString backupFileName(const QString &time) const;
    void backupDatabase(const QString &time, bool quiet);
    void needsBackup();
    void loadSettings();
    void saveSettings();
    void clearWidgets();
    void populateDirigenteCronoNotti();
    void connectToDatabase();
    void askDbUserPassword();

    void createUnitsTable();
    void createUnitsNightTable();
    void createUnitsPayedHoursTable();
    void createDoctorsTable();
    void createDoctorsNightTable();
    void createUnitsRepTable();
    void populateUnitsTable();
    void populateUnitsPayedHoursTable();
    void populateUnitsRepTable();
    void populateMeseCompetenzeCB();
    void populateUnitaCompetenzeCB();
    void populateDirigentiCompetenzeCB();
    void populateCompetenzeTab();
    void populateCausali();
    void mostraDifferenzaOre();
    void elaboraGuardie();
    void elaboraRep();
    void elaboraSommario();

    void setupDbConnectionParameters();
};

#endif // MAINWINDOW_H
