/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef SQLQUERIES_H
#define SQLQUERIES_H

#include <QString>
#include <QVariantList>
#include <QSqlQueryModel>

class Dipendente;
class ReperibilitaSemplificata;
class CompetenzePagate;

class SqlQueries
{
private:
    SqlQueries(){}

public:
    static void createUnitsTable();
    static void createDoctorsTable();
    static void createUnitsPayedHoursTable();
    static void createUnitsRepTable();
    static void createPagatoTable(int anno, int mese);
    static void insertUnit(const QString &id, const QString &raggruppamento, const QString &nome, const QString &breve);
    static void editUnit(const QString &id, const QString &raggruppamento, const QString &nome, const QString &breve);
    static int insertDoctor(const int &matricola, const QString &nome, const QString &id_unita);
    static void editDoctor(const QString &id, const QString &matricola, const QString &nome);
    static void insertPayload(const int &id_unita, const QString &data, const QString &ore_tot, const QString &ore_pagate);
    static void insertRep(const QString &id_unita, const QString &data, const QString &feriale, const QString &sabato, const QString &prefestivo, const QString &festivo);
    static void removeUnit(const QString &id);
    static void removeDoctor(const QString &id);
    static void removeTimeCard(const QString &tableName, const QString &doctorId);
    static void resetTimeCard(const QString &tableName, const int &doctorId);
    static bool createTimeCardsTable(const QString &tableName);
    static bool tableExists(const QString &tableName);
    static bool timeCardExists(const QString &tableName, const int &doctorId);
    static bool addTimeCard(const QString &tableName, const Dipendente *dipendente);
    static int doctorId(const int &matricola);
    static int unitId(const int &matricola);
    static void buildUnitsMap();
    static QMap<int, QString> units();
    static void resetAllDoctorMods(const QString &tableName, const int &id);
    static void resetStringValue(const QString &tableName, const QString &columnName, const int &id);
    static void resetIntValue(const QString &tableName, const QString &columnName, const int &id);
    static QStringList timecardsList();
    static int numDoctorsFromUnitInTimecard(const QString &timecard, const int &unitId);
    static int numDoctorsInTimecard(const QString &timecard);
    static QVariantList getDoctorTimecard(const QString &tableName, const QString &modTableName, const int &dipendendeId);
    static void saveMod(const QString &tableName, const QString &columnName, const int &id, QVariant value);
    static QMap<QDate, QPair<int,int> > getOrePagateFromUnit(const int &unitaId);
    static QPair<int, QString> getMatricolaNome(const int &doctorId);
    static QString getUnitaNomeBreve(const int &id);
    static QVector<int> getUnitaIdsInTimecard(const QString &timecard);
    static QVector<int> getUnitaIdsAll();
    static int getDoctorUnitaIdFromTimecard(const QString &timecard, const int &doctorId);
    static QString getUnitaNomeCompleto(const int &id);
    static QVector<int> getDoctorsIdsFromUnitInTimecard(const QString &timecard, const int &unitId);
    static QVariantList getOrePagateFromId(const int &unitaId);
    static QStringList getTuttiMatricoleNomi();
    static void setUnitaOrePagateModel(QSqlQueryModel *model, const int &idUnita);
    static void setUnitaReperibilitaModel(QSqlQueryModel *model, const int &idUnita);
    static QStringList getUnitaDataFromTimecard(const QString &timecard);
    static QStringList getUnitaDataAll();
    static QStringList getDoctorDataFromUnitaInTimecard(const QString &timecard, const int &idUnita);
    static QVariantList getUnitaDataById(const int &idUnita);
    static QVariantList getDoctorDataById(const int &idDoctor);
    static QPair<int,int> getRecuperiMeseSuccessivo(const int &anno, const int &mese, const int &doctorId);
    static void setUnitaMedico(const int &docId, const int &unitId);
    static bool noStraordinario(int matricola);
    static void enableDisableStraordinario(int matricola, bool enable);
    static ReperibilitaSemplificata * reperibilita(int idUnita, int anno, int mese);
    static CompetenzePagate * competenzePagate(int ci, int anno, int mese);
    static bool competenzePagateExists(int ci, int anno, int mese);
    static void saveCompetenzePagate(CompetenzePagate *pagato, int anno, int mese);
    static QString doctorName(int matricola);
    static int doctorMatricola(int id);
};

#endif // SQLQUERIES_H
