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

#ifndef SQLQUERIES_H
#define SQLQUERIES_H

#include <QString>
#include <QVariantList>
#include <QSqlQueryModel>

class Dipendente;

class SqlQueries
{
private:
    SqlQueries(){}
    static QMap<int, QStringList> m_unitsMap;

public:
    static void createUnitsTable();
    static void createDoctorsTable();
    static void createUnitsPayedHoursTable();
    static void createUnitsRepTable();
    static void insertUnit(const QString &id, const QString &raggruppamento, const QString &nome, const QString &breve, const QString &pseudo);
    static void editUnit(const QString &id, const QString &raggruppamento, const QString &nome, const QString &breve, const QString &pseudo);
    static bool insertDoctor(const QString &matricola, const QString &nome, const QString &id_unita);
    static void editDoctor(const QString &id, const QString &matricola, const QString &nome, const QString &id_unita);
    static void insertPayload(const QString &id_unita, const QString &data, const QString &ore_tot, const QString &ore_pagate);
    static void insertRep(const QString &id_unita, const QString &data, const QString &feriale, const QString &sabato, const QString &prefestivo, const QString &festivo);
    static void removeUnit(const QString &id);
    static void removeDoctor(const QString &id);
    static void removeTimeCard(const QString &tableName, const QString &doctorId);
    static void resetTimeCard(const QString &tableName, const int &doctorId);
    static bool createTimeCardsTable(const QString &tableName);
    static bool tableExists(const QString &tableName);
    static bool timeCardExists(const QString &tableName, const int &doctorId);
    static bool addTimeCard(const QString &tableName, const Dipendente *dipendente);
    static int doctorId(const QString &matricola);
    static int unitId(const QString &matricola);
    static void buildUnitsMap();
    static QMap<int, QString> units();
    static void appendPseudoUnitaName(const int id, const QString &nome);
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
    static int getDoctorUnitaIdFromTimecard(const QString &timecard, const int &doctorId);
    static QString getUnitaNomeCompleto(const int &id);
    static QVector<int> getDoctorsIdsFromUnitInTimecard(const QString &timecard, const int &unitId);
    static QVariantList getOrePagateFromId(const int &unitaId);
    static QStringList getTuttiMatricoleNomi();
    static void setUnitaOrePagateModel(QSqlQueryModel *model, const int &idUnita);
    static void setUnitaReperibilitaModel(QSqlQueryModel *model, const int &idUnita);
    static QStringList getUnitaDataFromTimecard(const QString &timecard);
    static QStringList getDoctorDataFromUnitaInTimecard(const QString &timecard, const int &idUnita);
    static QVariantList getUnitaDataById(const int &idUnita);
    static QVariantList getDoctorDataById(const int &idDoctor);
    static int getRecuperiMeseSuccessivo(const int &anno, const int &mese, const int &doctorId);
};

#endif // SQLQUERIES_H
