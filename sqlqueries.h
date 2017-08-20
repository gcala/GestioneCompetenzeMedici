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
    static void populateUnitsTable();
    static void populateUnitsPayedHoursTable();
    static void populateUnitsRepTable();
    static void insertUnit(const QString &raggruppamento, const QString &nome_full, const QString &nome_mini, const QString &numero, const QString &altri_nomi);
    static void editUnit(const QString &id, const QString &raggruppamento, const QString &nome_full, const QString &nome_mini, const QString &numero, const QString &altri_nomi);
    static bool insertDoctor(const QString &matricola, const QString &nome, const QString &id_unita);
    static void editDoctor(const QString &id, const QString &matricola, const QString &nome, const QString &id_unita);
    static void removeUnit(const QString &id);
    static void removeDoctor(const QString &id);
    static void removeTimeCard(const QString &tableName, const QString &doctorId);
    static bool createTimeCardsTable(const QString &tableName);
    static bool tableExists(const QString &tableName);
    static bool timeCardExists(const QString &tableName, const QString &doctorId);
    static bool addTimeCard(const QString &tableName, const Dipendente *dipendente);
    static int doctorId(const QString &matricola);
    static int unitId(const QString &unit);
    static void buildUnitsMap();
};

#endif // SQLQUERIES_H
