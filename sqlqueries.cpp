/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "sqlqueries.h"
#include "dipendente.h"
#include "competenza.h"
#include "sqldatabasemanager.h"
#include "utilities.h"
#include "reperibilitasemplificata.h"
#include "competenzepagate.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

void SqlQueries::createUnitsTable()
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    if(The::dbManager()->driverName() == "QSQLITE") {
        query.prepare("CREATE TABLE unita "
                      "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                      "raggruppamento TEXT,"
                      "nome TEXT,"
                      "breve TEXT);");
    } else if(The::dbManager()->driverName() == "QMYSQL") {
        query.prepare("CREATE TABLE unita "
                      "(id INT NOT NULL AUTO_INCREMENT,"
                      "raggruppamento varchar(10) NULL,"
                      "nome varchar(128) NULL,"
                      "breve varchar(32) NULL,"
                      "PRIMARY KEY (id));");
    } else {
        qDebug() << Q_FUNC_INFO << "Nessun database configurato. Esco";
        return;
    }

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::createDoctorsTable()
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    if(The::dbManager()->driverName() == "QSQLITE") {
        query.prepare("CREATE TABLE medici "
                      "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                      "matricola INTEGER,"
                      "nome TEXT,"
                      "id_unita INTEGER);");
    } else if(The::dbManager()->driverName() == "QMYSQL") {
        query.prepare("CREATE TABLE competenze.medici "
                      "(id INT NOT NULL AUTO_INCREMENT,"
                      "matricola INT NULL,"
                      "nome varchar(50),"
                      "id_unita INT NULL,"
                      "PRIMARY KEY (id));");
    } else {
        qDebug() << Q_FUNC_INFO << "Nessun database configurato. Esco";
        return;
    }

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::createUnitsPayedHoursTable()
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    if(The::dbManager()->driverName() == "QSQLITE") {
        query.prepare("CREATE TABLE unita_ore_pagate "
                      "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                      "id_unita INTEGER,"
                      "data TEXT DEFAULT (08/2017),"
                      "ore_tot INTEGER NOT NULL DEFAULT (8),"
                      "ore_pagate INTEGER NOT NULL DEFAULT (0));");
    } else if(The::dbManager()->driverName() == "QMYSQL") {
        query.prepare("CREATE TABLE unita_ore_pagate "
                      "(id INT NOT NULL AUTO_INCREMENT,"
                      "id_unita INT NULL,"
                      "data varchar(10) DEFAULT 08/2017 NOT NULL,"
                      "ore_tot INT DEFAULT 8 NOT NULL,"
                      "ore_pagate INT DEFAULT 0 NOT NULL,"
                      "PRIMARY KEY (id));");
    } else {
        qDebug() << Q_FUNC_INFO << "Nessun database configurato. Esco";
        return;
    }

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::createUnitsRepTable()
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    if(The::dbManager()->driverName() == "QSQLITE") {
        query.prepare("CREATE TABLE unita_reperibilita "
                      "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                      "id_unita INTEGER NOT NULL,"
                      "data TEXT NOT NULL DEFAULT (01.2017),"
                      "feriale REAL NOT NULL DEFAULT (0.0),"
                      "sabato REAL NOT NULL DEFAULT (0.0),"
                      "prefestivo REAL NOT NULL DEFAULT (0.0),"
                      "festivo REAL NOT NULL DEFAULT (0.0));");
    } else if(The::dbManager()->driverName() == "QMYSQL") {
        query.prepare("CREATE TABLE unita_reperibilita "
                      "(id INT NOT NULL AUTO_INCREMENT,"
                      "id_unita INT NULL,"
                      "data varchar(10) DEFAULT 01.2017 NOT NULL,"
                      "feriale FLOAT DEFAULT 0.0 NOT NULL,"
                      "sabato FLOAT DEFAULT 0.0 NOT NULL,"
                      "prefestivo FLOAT DEFAULT 0.0 NOT NULL,"
                      "festivo FLOAT DEFAULT 0.0 NOT NULL,"
                      "PRIMARY KEY (id));");
    } else {
        qDebug() << Q_FUNC_INFO << "Nessun database configurato. Esco";
        return;
    }

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::createPagatoTable(int anno, int mese)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    if(The::dbManager()->driverName() == "QSQLITE") {
        query.prepare("CREATE TABLE tcp_" + QString::number(anno) + QString::number(mese).rightJustified(2,'0') +
                      "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                      "ci INTEGER NOT NULL,"
                      "deficit INTEGER NOT NULL,"
                      "ind_not INTEGER NOT NULL DEFAULT (0),"
                      "ind_fes INTEGER NOT NULL DEFAULT (0),"
                      "str_reparto_ord INTEGER NOT NULL DEFAULT (0),"
                      "str_reparto_nof INTEGER NOT NULL DEFAULT (0),"
                      "str_reparto_nef INTEGER NOT NULL DEFAULT (0),"
                      "str_repe_ord INTEGER NOT NULL DEFAULT (0),"
                      "str_repe_nof INTEGER NOT NULL DEFAULT (0),"
                      "str_repe_nef INTEGER NOT NULL DEFAULT (0),"
                      "str_guard_ord INTEGER NOT NULL DEFAULT (0),"
                      "str_guard_nof INTEGER NOT NULL DEFAULT (0),"
                      "str_guard_nef INTEGER NOT NULL DEFAULT (0),"
                      "turni_repe INTEGER NOT NULL DEFAULT (0),"
                      "ore_repe INTEGER NOT NULL DEFAULT (0),"
                      "guard_diu INTEGER NOT NULL DEFAULT (0),"
                      "guard_not INTEGER NOT NULL DEFAULT (0),"
                      "grande_fes INTEGER NOT NULL DEFAULT (0),"
                      "data INTEGER NOT NULL DEFAULT (0));");
    } else if(The::dbManager()->driverName() == "QMYSQL") {
        query.prepare("CREATE TABLE tcp_" + QString::number(anno) + QString::number(mese).rightJustified(2,'0') + " "
                      "(id INT NOT NULL AUTO_INCREMENT,"
                      "ci INT NOT NULL,"
                      "deficit INT DEFAULT 0 NOT NULL,"
                      "ind_not INT DEFAULT 0 NOT NULL,"
                      "ind_fes INT DEFAULT 0 NOT NULL,"
                      "str_reparto_ord INT DEFAULT 0 NOT NULL,"
                      "str_reparto_nof INT DEFAULT 0 NOT NULL,"
                      "str_reparto_nef INT DEFAULT 0 NOT NULL,"
                      "str_repe_ord INT DEFAULT 0 NOT NULL,"
                      "str_repe_nof INT DEFAULT 0 NOT NULL,"
                      "str_repe_nef INT DEFAULT 0 NOT NULL,"
                      "str_guard_ord INT DEFAULT 0 NOT NULL,"
                      "str_guard_nof INT DEFAULT 0 NOT NULL,"
                      "str_guard_nef INT DEFAULT 0 NOT NULL,"
                      "turni_repe INT DEFAULT 0 NOT NULL,"
                      "ore_repe INT DEFAULT 0 NOT NULL,"
                      "guard_diu INT DEFAULT 0 NOT NULL,"
                      "guard_not INT DEFAULT 0 NOT NULL,"
                      "grande_fes INT DEFAULT 0 NOT NULL,"
                      "data INT DEFAULT 0 NOT NULL,"
                      "PRIMARY KEY (id));");
    } else {
        qDebug() << Q_FUNC_INFO << "Nessun database configurato. Esco";
        return;
    }

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::insertUnit(const QString &id,
                            const QString &raggruppamento,
                            const QString &nome,
                            const QString &breve)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("INSERT INTO unita (id,raggruppamento,nome,breve) "
                  "VALUES (:id,:raggruppamento, :nome, :breve);");
    query.bindValue(":id", id);
    query.bindValue(":raggruppamento", raggruppamento);
    query.bindValue(":nome", nome);
    query.bindValue(":breve", breve);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::editUnit(const QString &id,
                          const QString &raggruppamento,
                          const QString &nome,
                          const QString &breve)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("UPDATE unita "
                  "SET raggruppamento=:raggruppamento,nome=:nome,breve=:breve "
                  "WHERE id=" + id + ";");
    query.bindValue(":raggruppamento", raggruppamento);
    query.bindValue(":nome", nome);
    query.bindValue(":breve", breve);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

bool SqlQueries::insertDoctor(const int &matricola,
                              const QString &nome,
                              const QString &id_unita)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
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
                            const QString &nome)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("UPDATE medici "
                  "SET matricola=:matricola,nome=:nome "
                  "WHERE id=" + id + ";");
    query.bindValue(":matricola", matricola);
    query.bindValue(":nome", nome);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::insertPayload(const int &id_unita, const QString &data, const QString &ore_tot, const QString &ore_pagate)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("INSERT INTO unita_ore_pagate (id_unita,data,ore_tot,ore_pagate) "
                  "VALUES (:id_unita,:data,:ore_tot,:ore_pagate);");
    query.bindValue(":id_unita", id_unita);
    query.bindValue(":data", data);
    query.bindValue(":ore_tot", ore_tot);
    query.bindValue(":ore_pagate", ore_pagate);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::insertRep(const QString &id_unita, const QString &data, const QString &feriale, const QString &sabato, const QString &prefestivo, const QString &festivo)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
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
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("DELETE FROM unita WHERE id=" + id + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::removeDoctor(const QString &id)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("DELETE FROM medici WHERE id=" + id + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::removeTimeCard(const QString &tableName, const QString &doctorId)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("DELETE FROM " + tableName + " WHERE id_medico=" + doctorId + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::resetTimeCard(const QString &tableName, const int &doctorId)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("UPDATE " + tableName + " " +
                  "SET riposi=:riposi,minuti_giornalieri=:minuti_giornalieri,"
                  "ferie=:ferie,congedi=:congedi,malattia=:malattia,rmp=:rmp,"
                  "rmc=:rmc,altre_causali=:altre_causali,guardie_diurne=:guardie_diurne,guardie_notturne=:guardie_notturne,"
                  "grep=:grep,congedi_minuti=:congedi_minuti,eccr_minuti=:eccr_minuti,grep_minuti=:grep_minuti,"
                  "guar_minuti=:guar_minuti,rmc_minuti=:rmc_minuti,minuti_fatti=:minuti_fatti,scoperti=:scoperti "
                  "WHERE id_medico=" + QString::number(doctorId) + ";");
    query.bindValue(":riposi", 0);
    query.bindValue(":minuti_giornalieri", 0);
    query.bindValue(":ferie", QString());
    query.bindValue(":congedi", QString());
    query.bindValue(":malattia", QString());
    query.bindValue(":rmp", QString());
    query.bindValue(":rmc", QString());
    query.bindValue(":altre_causali", QString());
    query.bindValue(":guardie_diurne", QString());
    query.bindValue(":guardie_notturne", QString());
    query.bindValue(":grep", QString());
    query.bindValue(":congedi_minuti", 0);
    query.bindValue(":eccr_minuti", 0);
    query.bindValue(":grep_minuti", 0);
    query.bindValue(":guar_minuti", 0);
    query.bindValue(":rmc_minuti", 0);
    query.bindValue(":minuti_fatti", 0);
    query.bindValue(":scoperti", QString());

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

bool SqlQueries::createTimeCardsTable(const QString &tableName)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    if(The::dbManager()->driverName() == "QSQLITE") {
        query.prepare("CREATE TABLE " + tableName + " "
                      "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                      "id_medico INTEGER NOT NULL,"
                      "id_unita INTEGER NOT NULL,"
                      "anno INTEGER NOT NULL,"
                      "mese INTEGER NOT NULL,"
                      "riposi TEXT DEFAULT '',"
                      "minuti_giornalieri INTEGER NOT NULL,"
                      "ferie TEXT DEFAULT '',"
                      "congedi TEXT DEFAULT '',"
                      "malattia TEXT DEFAULT '',"
                      "rmp TEXT DEFAULT '',"
                      "rmc TEXT DEFAULT '',"
                      "altre_causali TEXT DEFAULT '',"
                      "guardie_diurne TEXT DEFAULT '',"
                      "guardie_notturne TEXT DEFAULT '',"
                      "grep TEXT DEFAULT '',"
                      "congedi_minuti INTEGER DEFAULT (0),"
                      "eccr_minuti INTEGER DEFAULT (0),"
                      "grep_minuti INTEGER DEFAULT (0),"
                      "guar_minuti INTEGER DEFAULT (0),"
                      "rmc_minuti INTEGER DEFAULT (0),"
                      "minuti_fatti INTEGER DEFAULT (0),"
                      "scoperti TEXT DEFAULT '');");
    } else if(The::dbManager()->driverName() == "QMYSQL") {
        query.prepare("CREATE TABLE " + tableName + " "
                      "(id INT NOT NULL AUTO_INCREMENT,"
                      "id_medico INT NOT NULL,"
                      "id_unita INT NOT NULL,"
                      "anno INT NOT NULL,"
                      "mese INT NOT NULL,"
                      "riposi varchar(128) DEFAULT '',"
                      "minuti_giornalieri INT NOT NULL,"
                      "ferie varchar(128) DEFAULT '',"
                      "congedi varchar(128) DEFAULT '',"
                      "malattia varchar(128) DEFAULT '',"
                      "rmp varchar(128) DEFAULT '',"
                      "rmc varchar(128) DEFAULT '',"
                      "altre_causali varchar(400) DEFAULT '',"
                      "guardie_diurne varchar(256) DEFAULT '',"
                      "guardie_notturne varchar(128) DEFAULT '',"
                      "grep varchar(512) DEFAULT '',"
                      "congedi_minuti INT DEFAULT 0,"
                      "eccr_minuti INT DEFAULT 0,"
                      "grep_minuti INT DEFAULT 0,"
                      "guar_minuti INT DEFAULT 0,"
                      "rmc_minuti INT DEFAULT 0,"
                      "minuti_fatti INT DEFAULT 0,"
                      "scoperti varchar(128) DEFAULT '',"
                      "PRIMARY KEY (id));");
    } else {
        qDebug() << Q_FUNC_INFO << "Nessun database configurato. Esco";
        return false;
    }

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }

    QString modTableName = tableName;

    if(The::dbManager()->driverName() == "QSQLITE") {
        query.prepare("CREATE TABLE " + modTableName.replace("_","m_") + " "
                      "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                      "id_medico INTEGER NOT NULL,"
                      "guardie_diurne TEXT DEFAULT '',"
                      "guardie_notturne TEXT DEFAULT '',"
                      "turni_reperibilita TEXT DEFAULT '',"
                      "dmp INTEGER DEFAULT (-1),"
                      "dmp_calcolato INTEGER DEFAULT (0),"
                      "altre_assenze TEXT DEFAULT '',"
                      "nota TEXT DEFAULT '',"
                      "altro_str TEXT DEFAULT '',"
                      "mensa TEXT DEFAULT '',"
                      "orario_giornaliero INTEGER DEFAULT (-1),"
                      "pagaStrGuar INTEGER DEFAULT (1) NOT NULL);");
    } else if(The::dbManager()->driverName() == "QMYSQL") {
        query.prepare("CREATE TABLE " + modTableName.replace("_","m_") + " "
                      "(id INT NOT NULL AUTO_INCREMENT,"
                      "id_medico INT NOT NULL,"
                      "guardie_diurne varchar(256) DEFAULT '',"
                      "guardie_notturne varchar(128) DEFAULT '',"
                      "turni_reperibilita varchar(128) DEFAULT '',"
                      "dmp INT DEFAULT -1,"
                      "dmp_calcolato INT DEFAULT 0,"
                      "altre_assenze varchar(128) DEFAULT '',"
                      "nota varchar(512) DEFAULT '',"
                      "altro_str varchar(128) DEFAULT '',"
                      "mensa varchar(128) DEFAULT '',"
                      "orario_giornaliero INT DEFAULT -1,"
                      "pagaStrGuar TINYINT DEFAULT 1 NOT NULL,"
                      "PRIMARY KEY (id));");
    } else {
        qDebug() << Q_FUNC_INFO << "Nessun database configurato. Esco";
        return false;
    }

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }

    return true;
}

bool SqlQueries::tableExists(const QString &tableName)
{
    int count = 0;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    if(The::dbManager()->driverName() == "QSQLITE") {
        query.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='" + tableName + "';");
    } else if(The::dbManager()->driverName() == "QMYSQL") {
        query.prepare("SELECT count(*) FROM information_schema.TABLES WHERE (TABLE_SCHEMA = 'competenze') AND (TABLE_NAME = '" + tableName + "');");
    } else {
        qDebug() << Q_FUNC_INFO << "Nessun database configurato. Esco";
        return false;
    }

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

bool SqlQueries::timeCardExists(const QString &tableName, const int &doctorId)
{
    int count = 0;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id_medico='" + QString::number(doctorId) + "';");
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
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));

    int docId = doctorId(dipendente->matricola());

    if(dipendente->unita() == -1) {
        qDebug() << "Impossibile trovare l'unità" << dipendente->unita();
        return false;
    }

    if(docId == -1) {
//        qDebug() << "---> INSERISCO" << dipendente->matricola() << dipendente->nome() << QString::number(unId);
        if(!insertDoctor(dipendente->matricola(),dipendente->nome(),QString::number(dipendente->unita())))
            return false;
    }

//    docId = doctorId(dipendente->matricola());

    if(timeCardExists(tableName, docId)) {
        resetTimeCard(tableName, docId);
        query.prepare("UPDATE " + tableName + " " +
                      "SET riposi=:riposi,minuti_giornalieri=:minuti_giornalieri,"
                      "ferie=:ferie,congedi=:congedi,malattia=:malattia,rmp=:rmp,"
                      "rmc=:rmc,altre_causali=:altre_causali,guardie_diurne=:guardie_diurne,guardie_notturne=:guardie_notturne,"
                      "grep=:grep,congedi_minuti=:congedi_minuti,eccr_minuti=:eccr_minuti,grep_minuti=:grep_minuti,"
                      "guar_minuti=:guar_minuti,rmc_minuti=:rmc_minuti,minuti_fatti=:minuti_fatti,scoperti=:scoperti "
                      "WHERE id_medico=" + QString::number(docId) + ";");
    } else {
        query.prepare("INSERT INTO " + tableName + " (id_medico, id_unita, anno, mese, riposi, minuti_giornalieri, ferie, congedi, malattia, rmp, rmc, altre_causali, guardie_diurne, guardie_notturne, grep, congedi_minuti, eccr_minuti, grep_minuti, guar_minuti, rmc_minuti, minuti_fatti, scoperti) "
                      "VALUES (:id_medico, :id_unita, :anno, :mese, :riposi, :minuti_giornalieri, :ferie, :congedi, :malattia, :rmp, :rmc, :altre_causali, :guardie_diurne, :guardie_notturne, :grep, :congedi_minuti, :eccr_minuti, :grep_minuti, :guar_minuti, :rmc_minuti, :minuti_fatti, :scoperti);");
        query.bindValue(":id_medico", QString::number(docId));
        query.bindValue(":id_unita", dipendente->unita());
        query.bindValue(":anno", dipendente->anno());
        query.bindValue(":mese", dipendente->mese());
    }

    query.bindValue(":riposi", Utilities::vectorIntToStringlist(dipendente->riposi()).join(","));
    query.bindValue(":minuti_giornalieri", dipendente->minutiGiornalieriVeri());
    if(dipendente->minutiGiornalieriVeri() <= Utilities::m_maxMinutiGiornalieri)
        query.bindValue(":ferie", Utilities::vectorIntToStringlist(dipendente->ferie()).join(","));
    else
        query.bindValue(":ferie", dipendente->numGiorniCartellino());
    query.bindValue(":congedi", Utilities::vectorIntToStringlist(dipendente->congedi()).join(","));
    query.bindValue(":malattia", Utilities::vectorIntToStringlist(dipendente->malattia()).join(","));
    query.bindValue(":rmp", Utilities::vectorIntToStringlist(dipendente->rmp()).join(","));
    query.bindValue(":rmc", Utilities::vectorIntToStringlist(dipendente->rmc()).join(","));

    QString altreCausali = "";
    auto i = dipendente->altreCausali().constBegin();
    while (i != dipendente->altreCausali().constEnd()) {
        altreCausali += i.key() + "," + Utilities::vectorIntToStringlist(i.value().first).join("~") + "," + QString::number(i.value().second) + ";";
        ++i;
    }
    query.bindValue(":altre_causali", altreCausali);

    query.bindValue(":guardie_diurne", Utilities::vectorIntToStringlist(dipendente->guardieDiurne()).join(","));
    query.bindValue(":guardie_notturne", Utilities::vectorIntToStringlist(dipendente->guardieNotturne()).join(","));

    QString grep = "";
    auto it = dipendente->grep().constBegin();
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
    query.bindValue(":scoperti", Utilities::vectorIntToStringlist(dipendente->scoperti()).join(","));

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }

    int diffMinuti = 0;

    QDate d(dipendente->anno(),dipendente->mese(),1);

    QString modTablePrevMonth = "tcm_" + QString::number(d.addDays(-1).year()) + QString::number(d.addDays(-1).month()).rightJustified(2, '0');
    if(tableExists(modTablePrevMonth)) {
        if(timeCardExists(modTablePrevMonth, docId)) {
            Competenza comp(modTablePrevMonth.replace("tcm", "tc"),docId);
            diffMinuti = comp.differenzaMin();
        }
    }

    if(diffMinuti >= 0)
        diffMinuti = 0;
    diffMinuti = abs(diffMinuti);

    QString modTableName = tableName;
    modTableName.replace("_","m_");

    // se la riga del medico nella tabella delle modifiche esiste già non aggiungiamo nuovamente
    if(timeCardExists(modTableName, docId)) {
        query.prepare("UPDATE " + modTableName + " "
                      "SET dmp_calcolato=:dmp_calcolato "
                      "WHERE id_medico=" + QString::number(docId) + ";");
        query.bindValue(":dmp_calcolato", diffMinuti);
        return true;
    }

    query.prepare("INSERT INTO " + modTableName + " (id_medico,dmp_calcolato) "
                  "VALUES (:id_medico,:dmp_calcolato);");
    query.bindValue(":id_medico", docId);
    query.bindValue(":dmp_calcolato", diffMinuti);
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return false;
    }

    return true;
}

int SqlQueries::doctorId(const int &matricola)
{
    int id = -1;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT id FROM medici WHERE matricola=" + QString::number(matricola) + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        id = query.value(0).toInt();
    }
    return id;
}

int SqlQueries::unitId(const int &matricola)
{
    int id = -1;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT id_unita FROM medici WHERE matricola=" + QString::number(matricola) + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        id = query.value(0).toInt();
    }
    return id;
}

QMap<int, QString> SqlQueries::units()
{
    QMap<int, QString> unita;

    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT id,nome FROM unita;");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        unita[query.value(0).toInt()] = query.value(1).toString();
    }

    return unita;
}

void SqlQueries::resetAllDoctorMods(const QString &tableName, const int &id)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("UPDATE " + tableName + " " +
                  "SET guardie_diurne=:guardie_diurne,guardie_notturne=:guardie_notturne,"
                  "turni_reperibilita=:turni_reperibilita,dmp=:dmp,altre_assenze=:altre_assenze,nota=:nota,"
                  "altro_str=:altro_str,mensa=:mensa,orario_giornaliero=:orario_giornaliero "
                  "WHERE id_medico=" + QString::number(id) + ";");
    query.bindValue(":guardie_diurne", QString());
    query.bindValue(":guardie_notturne", QString());
    query.bindValue(":turni_reperibilita", QString());
    query.bindValue(":dmp", -1);
    query.bindValue(":altre_assenze", QString());
    query.bindValue(":nota", QString());
    query.bindValue(":altro_str", QString());
    query.bindValue(":mensa", QString());
    query.bindValue(":orario_giornaliero", -1);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::resetStringValue(const QString &tableName, const QString &columnName, const int &id)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("UPDATE " + tableName + " " +
                  "SET " + columnName + "=:" + columnName + " " +
                  "WHERE id_medico=" + QString::number(id) + ";");
    query.bindValue(":" + columnName, QString());

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

void SqlQueries::resetIntValue(const QString &tableName, const QString &columnName, const int &id)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("UPDATE " + tableName + " " +
                  "SET " + columnName + "=:" + columnName + " " +
                  "WHERE id_medico=" + QString::number(id) + ";");
    query.bindValue(":" + columnName, -1);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

QStringList SqlQueries::timecardsList()
{
    QStringList tables;

    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    if(The::dbManager()->driverName() == "QSQLITE") {
        query.prepare("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;");
    } else if(The::dbManager()->driverName() == "QMYSQL") {
        query.prepare("SELECT table_name FROM information_schema.tables where table_schema='competenze' ORDER BY table_name;");
    } else {
        qDebug() << Q_FUNC_INFO << "Nessun database configurato. Esco";
        return tables;
    }

    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        if(query.value(0).toString().contains("tc_"))
            tables << query.value(0).toString();
    }

    return tables;
}

int SqlQueries::numDoctorsInTimecard(const QString &timecard)
{
    int count = 0;
    QSqlQuery queryCount(QSqlDatabase::database(Utilities::m_connectionName));
    queryCount.prepare("SELECT COUNT(*) FROM " + timecard + ";");
    if(!queryCount.exec()) {
        qDebug() << "ERROR: " << queryCount.lastQuery() << " : " << queryCount.lastError();
    }
    while(queryCount.next()) {
        count = queryCount.value(0).toInt();
    }

    return count;
}

int SqlQueries::numDoctorsFromUnitInTimecard(const QString &timecard, const int &unitId)
{
    int count = 0;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT COUNT(*) FROM " + timecard + " WHERE id_unita='" + QString::number(unitId) + "';");
    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        count = query.value(0).toInt();
    }

    return count;
}

QVariantList SqlQueries::getDoctorTimecard(const QString &tableName, const QString &modTableName, const int &dipendendeId)
{
    QVariantList result;

    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT medici.nome,"
                  "medici.matricola,"
                  "unita.nome,"
                  + tableName + ".riposi,"
                  + tableName + ".minuti_giornalieri,"
                  + tableName + ".ferie,"
                  + tableName + ".congedi,"
                  + tableName + ".malattia,"
                  + tableName + ".rmp,"
                  + tableName + ".rmc,"
                  + tableName + ".altre_causali,"
                  + tableName + ".guardie_diurne,"
                  + tableName + ".guardie_notturne,"
                  + tableName + ".grep,"
                  + tableName + ".congedi_minuti,"
                  + tableName + ".eccr_minuti,"
                  + tableName + ".grep_minuti,"
                  + tableName + ".guar_minuti,"
                  + tableName + ".rmc_minuti,"
                  + tableName + ".minuti_fatti,"
                  + modTableName + ".guardie_diurne,"
                  + modTableName + ".guardie_notturne,"
                  + modTableName + ".turni_reperibilita,"
                  + modTableName + ".dmp,"
                  + modTableName + ".dmp_calcolato,"
                  + modTableName + ".altre_assenze,"
                  + tableName + ".id_unita,"
                  + modTableName + ".nota,"
                  + tableName + ".scoperti, "
                  + modTableName + ".orario_giornaliero, "
                  + modTableName + ".pagaStrGuar "
                  + "FROM " + tableName + " LEFT JOIN medici ON medici.id=" + tableName + ".id_medico "
                  + "LEFT JOIN unita ON unita.id=" + tableName + ".id_unita "
                  + "LEFT JOIN " + modTableName + " ON " + modTableName + ".id_medico=" + tableName + ".id_medico "
                  + "WHERE " + tableName + ".id_medico=" + QString::number(dipendendeId) + ";");
    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return result;
    }

    while(query.next()) {
        result.clear();
        result << query.value(0);  // nome
        result << query.value(1);  // matricola
        result << query.value(2);  // unità
        result << query.value(3);  // riposi
        result << query.value(4);  // orario giornaliero
        result << query.value(5);  // ferie
        result << query.value(6);  // congedi
        result << query.value(7);  // malattia
        result << query.value(8);  // rmp
        result << query.value(9);  // rmc
        result << query.value(10); // altre causali
        result << query.value(11); // guardie diurne
        result << query.value(12); // guardie notturne
        result << query.value(13); // grep
        result << query.value(14); // minuti di congedi
        result << query.value(15); // minuti di eccr
        result << query.value(16); // minuti di grep
        result << query.value(17); // minuti di guar
        result << query.value(18); // minuti di rmc
        result << query.value(19); // minuti fatti
        result << query.value(20); // guardie diurne mod
        result << query.value(21); // guardie notturne mod
        result << query.value(22); // turni reperibilità
        result << query.value(23); // dmp
        result << query.value(24); // dmp calcolato
        result << query.value(25); // altre assenze
        result << query.value(26); // id unità
        result << query.value(27); // nota
        result << query.value(28); // scoperti
        result << query.value(29); // orario_giornaliero
        result << query.value(30); // pagaStrGuar
    }

    return result;
}

void SqlQueries::saveMod(const QString &tableName, const QString &columnName, const int &id, QVariant value)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("UPDATE " + tableName + " " +
                  "SET " + columnName + "=:" + columnName + " " +
                  "WHERE id_medico=" + QString::number(id) + ";");
    query.bindValue(":" + columnName, value);

    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

QMap<QDate, QPair<int, int> > SqlQueries::getOrePagateFromUnit(const int &unitaId)
{
    QMap<QDate, QPair<int,int> > map;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT data,ore_tot,ore_pagate FROM unita_ore_pagate WHERE id_unita=" + QString::number(unitaId) + ";");
    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return map;
    }
    while(query.next()) {
        QStringList meseAnno = query.value(0).toString().split("/");
        QDate date(meseAnno.at(1).toInt(),meseAnno.at(0).toInt(),1);
        QPair<int, int> vals;
        vals.first = query.value(1).toInt();
        vals.second = query.value(2).toInt();
        map[date] = vals;
    }
    return map;
}

QPair<int, QString> SqlQueries::getMatricolaNome(const int &doctorId)
{
    QPair<int, QString> result;

    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT matricola,nome FROM medici WHERE id='" + QString::number(doctorId) + "';");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        result.first = query.value(0).toInt();
        result.second = query.value(1).toString();
    }

    return result;
}

QString SqlQueries::getUnitaNomeBreve(const int &id)
{
    QString nome;

    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT breve FROM unita WHERE id='" + QString::number(id) + "';");
    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        nome = query.value(0).toString();
    }

    return nome;
}

QVector<int> SqlQueries::getUnitaIdsInTimecard(const QString &timecard)
{
    QVector<int> ids;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT id_unita FROM " + timecard + " ORDER BY length(id_unita), id_unita;");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        if(!ids.contains(query.value(0).toInt()))
            ids << query.value(0).toInt();
    }

    return ids;
}

int SqlQueries::getDoctorUnitaIdFromTimecard(const QString &timecard, const int &doctorId)
{
    int id = -1;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT id_unita FROM " + timecard + " WHERE id_medico='" + QString::number(doctorId) + "';");
    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        id = query.value(0).toInt();
    }

    return id;
}

QString SqlQueries::getUnitaNomeCompleto(const int &id)
{
    QString nome;

    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT nome FROM unita WHERE id='" + QString::number(id) + "';");
    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        nome = query.value(0).toString();
    }

    return nome;
}

QVector<int> SqlQueries::getDoctorsIdsFromUnitInTimecard(const QString &timecard, const int &unitId)
{
    QVector<int> ids;
    QString whereClause;
    if(unitId != -1)
        whereClause = "WHERE " + timecard + ".id_unita='" + QString::number(unitId) + "' ";
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT " + timecard + ".id_medico,medici.nome "
                  "FROM " + timecard + " "
                  "LEFT JOIN medici ON "
                  + timecard + ".id_medico=medici.id "
                  + whereClause +
                  "ORDER BY medici.nome;");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        ids << query.value(0).toInt();
    }

    return ids;
}

QVariantList SqlQueries::getOrePagateFromId(const int &id)
{
    QVariantList result;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT data,ore_tot,ore_pagate FROM unita_ore_pagate WHERE id=" + QString::number(id) + ";");
    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return result;
    }
    while(query.next()) {
        result.clear();
        result << query.value(0);
        result << query.value(1);
        result << query.value(2);
    }
    return result;
}

QStringList SqlQueries::getTuttiMatricoleNomi()
{
    QStringList result;

    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT id,matricola,nome FROM medici ORDER BY nome;");
    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        result << query.value(0).toString() + "~" + query.value(1).toString() + "~" + query.value(2).toString();
    }

    return result;
}

void SqlQueries::setUnitaOrePagateModel(QSqlQueryModel *model, const int &idUnita)
{
    model->setQuery("SELECT id,data,ore_tot,ore_pagate FROM unita_ore_pagate WHERE id_unita=" + QString::number(idUnita) + ";", The::dbManager()->currentDatabase());
    if(model->lastError().isValid()) {
        qDebug() << "ERROR: " << model->query().lastQuery() << " : " << model->lastError();
    }
}

void SqlQueries::setUnitaReperibilitaModel(QSqlQueryModel *model, const int &idUnita)
{
    model->setQuery("SELECT * FROM unita_reperibilita WHERE id_unita=" + QString::number(idUnita) + ";");
    if(model->lastError().isValid()) {
        qDebug() << "ERROR: " << model->query().lastQuery() << " : " << model->lastError();
    }
}

QStringList SqlQueries::getUnitaDataFromTimecard(const QString &timecard)
{
    QStringList result;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT " + timecard + ".id_unita,unita.nome,unita.id "
                  "FROM " + timecard + " "
                  "LEFT JOIN unita "
                  "ON " + timecard + ".id_unita=unita.id ORDER BY length(unita.id), unita.id;");
    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return result;
    }

    while(query.next()) {
        result << query.value(0).toString() + "~" + query.value(1).toString() + "~" + query.value(2).toString();
    }

    return result;
}

QStringList SqlQueries::getDoctorDataFromUnitaInTimecard(const QString &timecard, const int &idUnita)
{
    QStringList result;
    QString whereClause;

    if(idUnita != -1)
        whereClause = "WHERE " + timecard + ".id_unita=" + QString::number(idUnita);

    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT medici.id,medici.matricola,medici.nome "
                  "FROM medici "
                  "LEFT JOIN " + timecard + " "
                  "ON " + timecard + ".id_medico=medici.id "
                  + whereClause + "  ORDER BY medici.nome;");
    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return result;
    }

    while(query.next()) {
        result << query.value(0).toString() + "~" + query.value(1).toString() + "~" + query.value(2).toString();
    }

    return result;
}

QVariantList SqlQueries::getUnitaDataById(const int &idUnita)
{
    QVariantList result;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT * FROM unita WHERE id=" + QString::number(idUnita) + ";");
    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return result;
    }
    while(query.next()) {
        result.clear();
        result << query.value(0);
        result << query.value(1);
        result << query.value(2);
        result << query.value(3);
        result << query.value(4);
    }

    return result;
}

QVariantList SqlQueries::getDoctorDataById(const int &idDoctor)
{
    QVariantList result;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT * FROM medici WHERE id=" + QString::number(idDoctor) + ";");
    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return result;
    }
    while(query.next()) {
        result.clear();
        result << query.value(0);
        result << query.value(1);
        result << query.value(2);
        result << query.value(3);
    }

    return result;
}

QPair<int, int> SqlQueries::getRecuperiMeseSuccessivo(const int &anno, const int &mese, const int &doctorId)
{
    QPair<int,int> result;
    result.first = 0;
    result.second = 0;
    const QDate date(anno, mese, 1);

    const QString table = "tc_" + QString::number(date.addMonths(1).year()) + QString::number(date.addMonths(1).month()).rightJustified(2, '0');


    if(!tableExists(table)) {
//        qDebug() << Q_FUNC_INFO << "La tabella " + table + " non esiste";
        return result;
    }

    if(!timeCardExists(table,doctorId)) {
        qDebug() << Q_FUNC_INFO << "La timecard di " + QString::number(doctorId) + " in " + table + " non esiste";
        return result;
    }

    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT minuti_giornalieri,rmp from " + table + " WHERE id_medico=" + QString::number(doctorId) + ";");

    if(!query.exec()) {
        qDebug() << Q_FUNC_INFO << Q_FUNC_INFO << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return result;
    }

    while(query.next()) {
        result.first = query.value(0).toInt(); // minuti_giornalieri
        result.second = query.value(1).toString().trimmed().split(",", Qt::SkipEmptyParts).count(); // rmp
    }

    return result;
}

void SqlQueries::setUnitaMedico(const int &docId, const int &unitId)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("UPDATE medici "
                  "SET id_unita=:id_unita "
                  "WHERE id=" + QString::number(docId) + ";");
    query.bindValue(":id_unita", unitId);

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

bool SqlQueries::noStraordinario(int matricola)
{
    int count = 0;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT COUNT(*) FROM no_straordinario WHERE ci='" + QString::number(matricola) + "';");
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

void SqlQueries::enableDisableStraordinario(int matricola, bool enable)
{
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    if(enable) {
        query.prepare("DELETE FROM 'no_straordinario' WHERE ci=" + QString::number(matricola) + ";");
    } else {
        query.prepare("INSERT INTO no_straordinario (ci) VALUES (:ci);");
        query.bindValue(":ci", matricola);
    }
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

ReperibilitaSemplificata *SqlQueries::reperibilita(int idUnita, int anno, int mese)
{
    if(idUnita == 0 || anno == 0 || mese == 0)
        return new ReperibilitaSemplificata();

    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT * FROM reperibilita_semplificata WHERE id_unita=" + QString::number(idUnita) + " AND decorrenza <= " + QDate(anno, mese, 1).toString("yyyyMMdd") + " ORDER BY decorrenza DESC LIMIT 1;");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        return new ReperibilitaSemplificata(idUnita,
                                        QDate(anno, mese, 1),
                                        query.value(2).toDouble(),
                                        query.value(3).toDouble(),
                                        query.value(4).toDouble());
    }
    return new ReperibilitaSemplificata();
}

CompetenzePagate *SqlQueries::competenzePagate(int ci, int anno, int mese)
{
    if(ci == 0 || anno == 0 || mese == 0)
        return new CompetenzePagate();

    CompetenzePagate *pagato = new CompetenzePagate;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT * FROM tcp_" + QString::number(anno) + QString::number(mese).rightJustified(2, '0') + " WHERE ci=" + QString::number(ci) + " ORDER BY data DESC LIMIT 1;");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
    while(query.next()) {
        pagato->setCi(query.value(1).toInt());
        pagato->setDeficit(query.value(2).toInt());
        pagato->setIndNotturna(query.value(3).toInt());
        pagato->setIndFestiva(query.value(4).toInt());
        pagato->setStr_reparto_ord(query.value(5).toInt());
        pagato->setStr_reparto_nef(query.value(6).toInt());
        pagato->setStr_reparto_nof(query.value(7).toInt());
        pagato->setStr_repe_ord(query.value(8).toInt());
        pagato->setStr_repe_nef(query.value(9).toInt());
        pagato->setStr_repe_nof(query.value(10).toInt());
        pagato->setStr_guard_ord(query.value(11).toInt());
        pagato->setStr_guard_nef(query.value(12).toInt());
        pagato->setStr_guard_nof(query.value(13).toInt());
        pagato->setTurni_repe(query.value(14).toInt());
        pagato->setOre_repe(query.value(15).toInt());
        pagato->setGuard_diu(query.value(16).toInt());
        pagato->setGuard_not(query.value(17).toInt());
        pagato->setGrande_fes(query.value(18).toInt());
        pagato->setData(QDate::fromString(query.value(19).toString(),"yyyyMMdd"));
    }
    return pagato;
}

bool SqlQueries::competenzePagateExists(int ci, int anno, int mese)
{
    int count = 0;
    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("SELECT COUNT(*) FROM tcp_" + QString::number(anno) + QString::number(mese).rightJustified(2, '0') + " WHERE ci='" + QString::number(ci) + "';");
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

void SqlQueries::saveCompetenzePagate(CompetenzePagate *pagato, int anno, int mese)
{
    if(pagato->ci() == 0 || anno == 0 || mese == 0) {
        qDebug() << "ERROR: " << Q_FUNC_INFO << " : " << "ci: " << pagato->ci() << " - anno: " << anno << " - mese: " << mese;
        return;
    }

    QSqlQuery query(QSqlDatabase::database(Utilities::m_connectionName));
    query.prepare("INSERT INTO tcp_" + QString::number(anno) + QString::number(mese).rightJustified(2, '0') + " "
                  "(ci,deficit,ind_not,ind_fes,str_reparto_ord,str_reparto_nof,str_reparto_nef,str_repe_ord,str_repe_nof,str_repe_nef,str_guard_ord,str_guard_nof,str_guard_nef,turni_repe,ore_repe,guard_diu,guard_not,grande_fes,data) "
                  "VALUES (:ci,:deficit,:ind_not,:ind_fes,:str_reparto_ord,:str_reparto_nof,:str_reparto_nef,:str_repe_ord,:str_repe_nof,:str_repe_nef,:str_guard_ord,:str_guard_nof,:str_guard_nef,:turni_repe,:ore_repe,:guard_diu,:guard_not,:grande_fes,:data);");
    query.bindValue(":ci", pagato->ci());
    query.bindValue(":deficit", pagato->deficit());
    query.bindValue(":ind_not", pagato->indNotturna());
    query.bindValue(":ind_fes", pagato->indFestiva());
    query.bindValue(":str_reparto_ord", pagato->str_reparto_ord());
    query.bindValue(":str_reparto_nof", pagato->str_reparto_nof());
    query.bindValue(":str_reparto_nef", pagato->str_reparto_nef());
    query.bindValue(":str_repe_ord", pagato->str_repe_ord());
    query.bindValue(":str_repe_nof", pagato->str_repe_nof());
    query.bindValue(":str_repe_nef", pagato->str_repe_nef());
    query.bindValue(":str_guard_ord", pagato->str_guard_ord());
    query.bindValue(":str_guard_nof", pagato->str_guard_nof());
    query.bindValue(":str_guard_nef", pagato->str_guard_nef());
    query.bindValue(":turni_repe", pagato->turni_repe());
    query.bindValue(":ore_repe", pagato->ore_repe());
    query.bindValue(":guard_diu", pagato->guard_diu());
    query.bindValue(":guard_not", pagato->guard_not());
    query.bindValue(":grande_fes", pagato->grande_fes());
    query.bindValue(":data", pagato->dataElaborazione().toString("yyyyMMdd"));
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }
}

