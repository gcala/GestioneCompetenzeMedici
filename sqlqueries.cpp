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

#include "sqlqueries.h"
#include "dipendente.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QMap<int, QStringList> SqlQueries::m_unitsMap;

void SqlQueries::createUnitsTable()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE unita "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                  "raggruppamento TEXT,"
                  "nome_full TEXT,"
                  "nome_mini TEXT,"
                  "altri_nomi TEXT);");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::createDoctorsTable()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE medici "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                  "matricola INTEGER,"
                  "nome TEXT,"
                  "id_unita INTEGER);");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::createUnitsPayedHoursTable()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE unita_ore_pagate "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                  "id_unita INTEGER,"
                  "data TEXT DEFAULT (01.2017),"
                  "ore INTEGER NOT NULL DEFAULT (0));");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::createUnitsRepTable()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE unita_reperibilita "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                  "id_unita INTEGER NOT NULL,"
                  "data TEXT NOT NULL DEFAULT (01.2017),"
                  "feriale REAL NOT NULL DEFAULT (0.0),"
                  "sabato REAL NOT NULL DEFAULT (0.0),"
                  "prefestivo REAL NOT NULL DEFAULT (0.0),"
                  "festivo REAL NOT NULL DEFAULT (0.0));");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::insertUnit(const QString &id,
                            const QString &raggruppamento,
                            const QString &nome_full,
                            const QString &nome_mini,
                            const QString &altri_nomi)
{
    QSqlQuery query;
    query.prepare("INSERT INTO unita (id,raggruppamento,nome_full,nome_mini,altri_nomi) "
                  "VALUES (:id,:raggruppamento, :nome_full, :nome_mini, :altri_nomi);");
    query.bindValue(":id", id);
    query.bindValue(":raggruppamento", raggruppamento);
    query.bindValue(":nome_full", nome_full);
    query.bindValue(":nome_mini", nome_mini);
    query.bindValue(":altri_nomi", altri_nomi);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::editUnit(const QString &id,
                          const QString &raggruppamento,
                          const QString &nome_full,
                          const QString &nome_mini,
                          const QString &altri_nomi)
{
    QSqlQuery query;
    query.prepare("UPDATE unita "
                  "SET raggruppamento=:raggruppamento,nome_full=:nome_full,nome_mini=:nome_mini,altri_nomi=:altri_nomi "
                  "WHERE id=" + id + ";");
    query.bindValue(":raggruppamento", raggruppamento);
    query.bindValue(":nome_full", nome_full);
    query.bindValue(":nome_mini", nome_mini);
    query.bindValue(":altri_nomi", altri_nomi);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

bool SqlQueries::insertDoctor(const QString &matricola,
                              const QString &nome,
                              const QString &id_unita)
{
    QSqlQuery query;
    query.prepare("INSERT INTO medici (matricola,nome,id_unita  ) "
                  "VALUES (:matricola, :nome, :id_unita);");
    query.bindValue(":matricola", matricola);
    query.bindValue(":nome", nome);
    query.bindValue(":id_unita", id_unita);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }
    return true;
}

void SqlQueries::editDoctor(const QString &id,
                            const QString &matricola,
                            const QString &nome,
                            const QString &id_unita)
{
    QSqlQuery query;
    query.prepare("UPDATE medici "
                  "SET matricola=:matricola,nome=:nome,id_unita=:id_unita "
                  "WHERE id=" + id + ";");
    query.bindValue(":matricola", matricola);
    query.bindValue(":nome", QString(nome).replace("''","'"));
    query.bindValue(":id_unita", id_unita);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::insertPayload(const QString &id_unita, const QString &data, const QString &ore)
{
    QSqlQuery query;
    query.prepare("INSERT INTO unita_ore_pagate (id_unita,data,ore) "
                  "VALUES (:id_unita,:data,:ore);");
    query.bindValue(":id_unita", id_unita);
    query.bindValue(":data", data);
    query.bindValue(":ore", ore);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::insertRep(const QString &id_unita, const QString &data, const QString &feriale, const QString &sabato, const QString &prefestivo, const QString &festivo)
{
    QSqlQuery query;
    query.prepare("INSERT INTO unita_reperibilita (id_unita,data,feriale,sabato,prefestivo,festivo) "
                  "VALUES (:id_unita,:data, :feriale, :sabato, :prefestivo, :festivo);");
    query.bindValue(":id_unita", id_unita);
    query.bindValue(":data", data);
    query.bindValue(":feriale", feriale);
    query.bindValue(":sabato", sabato);
    query.bindValue(":prefestivo", prefestivo);
    query.bindValue(":festivo", festivo);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::removeUnit(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM unita WHERE id=" + id + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::removeDoctor(const QString &id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM medici WHERE id=" + id + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::removeTimeCard(const QString &tableName, const QString &doctorId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM " + tableName + " WHERE id_medico=" + doctorId + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

bool SqlQueries::createTimeCardsTable(const QString &tableName)
{
    QSqlQuery query;
    query.prepare("CREATE TABLE " + tableName + " "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                  "id_medico INTEGER NOT NULL,"
                  "id_unita INTEGER NOT NULL,"
                  "anno INTEGER NOT NULL,"
                  "mese INTEGER NOT NULL,"
                  "riposi INTEGER NOT NULL,"
                  "minuti_giornalieri INTEGER NOT NULL,"
                  "ferie TEXT DEFAULT '',"
                  "congedi TEXT DEFAULT '',"
                  "malattia TEXT DEFAULT '',"
                  "rmp TEXT DEFAULT '',"
                  "rmc TEXT DEFAULT '',"
                  "altre_assenze TEXT DEFAULT '',"
                  "guardie_diurne TEXT DEFAULT '',"
                  "guardie_notturne TEXT DEFAULT '',"
                  "grep TEXT DEFAULT '',"
                  "congedi_minuti INTEGER DEFAULT (0),"
                  "eccr_minuti INTEGER DEFAULT (0),"
                  "grep_minuti INTEGER DEFAULT (0),"
                  "guar_minuti INTEGER DEFAULT (0),"
                  "rmc_minuti INTEGER DEFAULT (0),"
                  "minuti_fatti INTEGER DEFAULT (0));");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }

    QString modTableName = tableName;

    query.prepare("CREATE TABLE " + modTableName.replace("_","m_") + " "
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                  "id_medico INTEGER NOT NULL,"
                  "guardie_diurne TEXT DEFAULT '',"
                  "guardie_notturne TEXT DEFAULT '',"
                  "turni_reperibilita TEXT DEFAULT '',"
                  "dmp INTEGER DEFAULT (0),"
                  "altre_assenze TEXT DEFAULT '');");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }

    return true;
}

bool SqlQueries::tableExists(const QString &tableName)
{
    int count = 0;
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='" + tableName + "';");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        count = query.value(0).toInt();
    }

    if(count <= 0)
        return false;

    return true;
}

bool SqlQueries::timeCardExists(const QString &tableName, const QString &doctorId)
{
    int count = 0;
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id_medico='" + doctorId + "';");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        count = query.value(0).toInt();
    }

    if(count <= 0)
        return false;

    return true;
}

bool SqlQueries::addTimeCard(const QString &tableName, const Dipendente *dipendente)
{
    QSqlQuery query;

    int docId = doctorId(dipendente->matricola());
    int unId = unitId(dipendente->unita());

    if(unId == -1) {
        qDebug() << "Impossibile trovare l'unità" << dipendente->unita();
        return false;
    }

    if(docId == -1) {
//        qDebug() << "---> INSERISCO" << dipendente->matricola() << dipendente->nome() << QString::number(unId);
        if(!insertDoctor(dipendente->matricola(),dipendente->nome(),QString::number(unId)))
            return false;
    }

    docId = doctorId(dipendente->matricola());

    if(timeCardExists(tableName, QString::number(docId))) {
//        qDebug() << "---> RIMUOVO " << dipendente->nome() << "da" << tableName;
        removeTimeCard(tableName, QString::number(docId));
    }

    query.prepare("INSERT INTO " + tableName + " (id_medico, id_unita, anno, mese, riposi, minuti_giornalieri, ferie, congedi, malattia, rmp, rmc, altre_assenze, guardie_diurne, guardie_notturne, grep, congedi_minuti, eccr_minuti, grep_minuti, guar_minuti, rmc_minuti, minuti_fatti) "
                  "VALUES (:id_medico, :id_unita, :anno, :mese, :riposi, :minuti_giornalieri, :ferie, :congedi, :malattia, :rmp, :rmc, :altre_assenze, :guardie_diurne, :guardie_notturne, :grep, :congedi_minuti, :eccr_minuti, :grep_minuti, :guar_minuti, :rmc_minuti, :minuti_fatti);");
    query.bindValue(":id_medico", QString::number(docId));
    query.bindValue(":id_unita", unId);
    query.bindValue(":anno", dipendente->anno());
    query.bindValue(":mese", dipendente->mese());
    query.bindValue(":riposi", dipendente->riposi());
    query.bindValue(":minuti_giornalieri", dipendente->minutiGiornalieri());
    query.bindValue(":ferie", dipendente->ferie().join(","));
    query.bindValue(":congedi", dipendente->congedi().join(","));
    query.bindValue(":malattia", dipendente->malattia().join(","));
    query.bindValue(":rmp", dipendente->rmp().join(","));
    query.bindValue(":rmc", dipendente->rmc().join(","));

    QString altreAssenze = "";
    QMap<QString, QPair<QStringList, int> >::const_iterator i = dipendente->altreAssenze().constBegin();
    while (i != dipendente->altreAssenze().constEnd()) {
        altreAssenze += i.key() + "," + i.value().first.join("~") + "," + QString::number(i.value().second) + ";";
        ++i;
    }
    query.bindValue(":altre_assenze", altreAssenze);

    query.bindValue(":guardie_diurne", dipendente->guardieDiurne().join(","));
    query.bindValue(":guardie_notturne", dipendente->guardieNotturne().join(","));

    QString grep = "";
    QMap<int, QPair<int, int>>::const_iterator it = dipendente->grep().constBegin();
    while (it != dipendente->grep().constEnd()) {
        grep += QString::number(it.key()) + "," + QString::number(it.value().first) + "," + QString::number(it.value().second) + ";";
        ++it;
    }
    query.bindValue(":grep", grep);

    query.bindValue(":congedi_minuti", dipendente->minutiCongedi());
    query.bindValue(":eccr_minuti", dipendente->minutiEccr());
    query.bindValue(":grep_minuti", dipendente->minutiGrep());
    query.bindValue(":guar_minuti", dipendente->minutiGuar());
    query.bindValue(":rmc_minuti", dipendente->minutiRmc());
    query.bindValue(":minuti_fatti", dipendente->minutiFatti());

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }

    QString modTableName = tableName;
    modTableName.replace("_","m_");

    // se la riga del medico nella tabella delle modifiche esiste già non aggiungiamo nuovamente
    if(timeCardExists(modTableName, QString::number(docId))) {
        return true;
    }

    query.prepare("INSERT INTO " + modTableName + " (id_medico) "
                  "VALUES (:id_medico);");
    query.bindValue(":id_medico", QString::number(docId));
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }

    return true;
}

int SqlQueries::doctorId(const QString &matricola)
{
    int id = -1;
    QSqlQuery query;
    query.prepare("SELECT id FROM medici WHERE matricola='" + matricola + "';");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        id = query.value(0).toInt();
    }
    return id;
}

int SqlQueries::unitId(const QString &unit)
{
    int id = -1;
    QMap<int, QStringList>::const_iterator i = m_unitsMap.constBegin();
    while (i != m_unitsMap.constEnd()) {
        if(i.value().contains(unit)) {
            id = i.key();
            break;
        }
        ++i;
    }
    return id;
}

void SqlQueries::buildUnitsMap()
{
    m_unitsMap.clear();
    QSqlQuery query;
    query.prepare("SELECT id,altri_nomi FROM unita;");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }

    while(query.next()) {
        m_unitsMap[query.value(0).toInt()] = query.value(1).toString().split(";");
    }
}

QMap<int, QString> SqlQueries::units()
{
    QMap<int, QString> unita;

    QSqlQuery query;
    query.prepare("SELECT id,nome_full FROM unita;");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        unita[query.value(0).toInt()] = query.value(1).toString();
    }

    return unita;
}

void SqlQueries::appendOtherUnitaName(const int id, const QString &nome)
{
    QSqlQuery query;
    query.prepare("SELECT altri_nomi FROM unita WHERE id=" + QString::number(id) + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return;
    }

    QString nomi;

    while(query.next()) {
        nomi = query.value(0).toString();
    }

    nomi += ";" + nome;

    query.prepare("UPDATE unita "
                  "SET altri_nomi=:altri_nomi "
                  "WHERE id=" + QString::number(id) + ";");
    query.bindValue(":altri_nomi", nomi);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }

    buildUnitsMap();
}
