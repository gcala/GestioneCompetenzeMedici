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

#include <timecardsreader.h>
#include <competenzeunitaexporter.h>
#include <competenzedirigenteexporter.h>

#include <QMainWindow>
#include <QFileInfo>
#include <QTime>

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
    void exported();
    void setTotalRows(int);
    void setCurrentRow(int);

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
    void on_actionCaricaCartellini_triggered();
    void on_actionModificaDirigente_triggered();
    void on_actionAggiungiDirigente_triggered();
    void on_actionRimuoviDirigente_triggered();
    void on_actionNuovoDatabase_triggered();
    void on_unitaReperibilitaTW_activated(const QModelIndex &index);
    void on_addUnitaReperibilitaButton_clicked();
    void on_removeUnitaReperibilitaButton_clicked();
    void on_saveCompetenzeButton_clicked();
    void on_restoreCompetenzeButton_clicked();
    void on_dmpEdit_timeChanged(const QTime &time);
    void on_meseCompetenzeCB_currentIndexChanged(int index);
    void on_unitaCompetenzeCB_currentIndexChanged(int index);
    void on_dirigentiCompetenzeCB_currentIndexChanged(int index);
    void on_actionStampaCompetenzeDirigenti_triggered();
    void on_actionStampaCompetenzeUnita_triggered();
    void gdCalendarClicked(const QDate &date);
    void gnCalendarClicked(const QDate &date);
    void rCalendarClicked(const QDate &date);
    void altreCalendarClicked(const QDate &date);

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
    TimeCardsReader worker;
    CompetenzeUnitaExporter unitaCompetenzeExporter;
    CompetenzeDirigenteExporter dirigenteCompetenzeExporter;

    void loadSettings();
    void saveSettings();
    void toggleUnitaEditMode();
    void toggleDirigenteEditMode();
    void populateUnitaCB();
    void populateUnitaOrePagate();
    void populateUnitaReperibilita();
    void populateDirigentiCB();
    void populateDirigenteCronoNotti();
    void saveCurrentUnitaValues();
    void saveCurrentDirigenteValues();
    void restoreUnitaValues();
    void restoreDirigenteValues();
    void connectToDatabase();

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
};

#endif // MAINWINDOW_H
