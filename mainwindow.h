/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017 Giuseppe Calà <giuseppe.cala@mailbox.org>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tabulacsvtimecardsreader.h"
#include "okularcsvtimecardsreader.h"
#include "competenzeunitaexporter.h"
#include "competenzedirigenteexporter.h"

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
class InsertDBValues;
class PrintDialog;
class CalendarManager;
class CalendarManagerRep;
class QWidgetAction;
class Competenza;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum UnitsOps { UndefOp, AddUnit, RemoveUnit, EditUnit };
    Q_ENUM(UnitsOps)

private slots:
    void handleResults();
    void exported(QString file);
    void computed();
    void setTotalRows(int);
    void setCurrentRow(int);
    void tabulaFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void tabulaError(QProcess::ProcessError error);
    void delayedSetup();

    void on_editUnitaSaveButton_clicked();
    void on_editUnitaCancelButton_clicked();
    void on_editDirigenteSaveButton_clicked();
    void on_editDirigenteCancelButton_clicked();
    void on_actionModificaUnita_triggered();
    void on_actionAggiungiUnita_triggered();
    void on_actionRimuoviUnita_triggered();
    void on_actionApriDatabase_triggered();
    void on_unitaComboBox_currentIndexChanged(int index);
    void on_dirigentiComboBox_currentIndexChanged(int index);
    void on_unitaOrePagateTW_activated(const QModelIndex &index);
    void on_addUnitaOrePagateButton_clicked();
    void on_removeUnitaOrePagateButton_clicked();
    void on_actionCaricaPdf_triggered();
    void on_actionModificaDirigente_triggered();
    void on_actionAggiungiDirigente_triggered();
    void on_actionRimuoviDirigente_triggered();
    void on_actionNuovoDatabase_triggered();
    void on_unitaReperibilitaTW_activated(const QModelIndex &index);
    void on_addUnitaReperibilitaButton_clicked();
    void on_removeUnitaReperibilitaButton_clicked();
    void on_saveCompetenzeButton_clicked();
    void on_restoreCompetenzeButton_clicked();
    void on_meseCompetenzeCB_currentIndexChanged(int index);
    void on_unitaCompetenzeCB_currentIndexChanged(int index);
    void on_dirigentiCompetenzeCB_currentIndexChanged(int index);
    void on_actionStampaCompetenzeDirigenti_triggered();
    void on_actionStampaCompetenzeUnita_triggered();
    void gdCalendarClicked(const QDate &date);
    void gnCalendarClicked(const QDate &date);
    void rCalendarClicked(const QDate &date);
    void altreCalendarClicked(const QDate &date);
    void on_actionBackupDatabase_triggered();
    void on_actionCaricaCsv_triggered();
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

private:
    Ui::MainWindow *ui;
    bool unitaReadOnlyMode;
    bool dirigenteReadOnlyMode;
    int currentUnitaIndex;
    int currentDirigenteIndex;
    int currentDirigenteUnitaIndex;
    QSqlQueryModel *unitaOrePagateModel;
    QSqlQueryModel *unitaReperibilitaModel;
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
    QString m_driver;
    QString m_host;
    QString m_dbName;
    QString m_certFile;
    QString m_keyFile;
    bool m_useSSL;
    QString m_lastUsername;
    QString m_lastPassword;
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

    QMenu *ferieCalendarMenu;
    CalendarManager *ferieCalendar;
    QWidgetAction *ferieCalendarAction;

    QMenu *congediCalendarMenu;
    CalendarManager *congediCalendar;
    QWidgetAction *congediCalendarAction;

    QMenu *malattiaCalendarMenu;
    CalendarManager *malattiaCalendar;
    QWidgetAction *malattiaCalendarAction;

    QMenu *rmpCalendarMenu;
    CalendarManager *rmpCalendar;
    QWidgetAction *rmpCalendarAction;

    QMenu *rmcCalendarMenu;
    CalendarManager *rmcCalendar;
    QWidgetAction *rmcCalendarAction;

    QMenu *altreCalendarMenu;
    CalendarManager *altreCalendar;
    QWidgetAction *altreCalendarAction;

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
    InsertDBValues *insertDialog;
    PrintDialog *printDialog;
    TabulaCsvTimeCardsReader tabulaReader;
    OkularCsvTimeCardsReader okularReader;
    CompetenzeUnitaExporter unitaCompetenzeExporter;
    CompetenzeDirigenteExporter dirigenteCompetenzeExporter;

    QString backupFileName(const QString &time) const;
    void backupDatabase(const QString &time, bool quiet);
    void needsBackup();
    void loadSettings();
    void saveSettings();
    void toggleUnitaEditMode();
    void toggleDirigenteEditMode();
    void clearWidgets();
    void populateUnitaCB();
    void populateUnitaOrePagate();
    void populateUnitaReperibilita();
    void populateDirigentiCB();
    void populateDirigenteCronoNotti();
    void saveCurrentUnitaValues();
    void saveCurrentDirigenteValues();
    void restoreUnitaValues();
    void restoreDirigenteValues();
    void connectToLocalDatabase();
    void connectToRemoteDatabase(const QString &user, const QString &pass);
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
    void mostraDifferenzaOre();
    void elaboraGuardie();
    void elaboraRep();
    void elaboraSommario();
};

#endif // MAINWINDOW_H
