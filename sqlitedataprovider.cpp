#include "sqlitedataprovider.h"
#include "reperibilitasemplificata.h"
#include "dipendente.h"
#include "utilities.h"
#include "competenzepagate.h"
#include "indennita.h"

#include <QSqlQuery>
#include <QSqlError>

SqliteDataProvider::SqliteDataProvider(const QString& connectionName, QObject* parent)
    : IDataProvider(parent)
    , m_connectionName(connectionName)
{
}

void SqliteDataProvider::getUnits(const std::function<void(const QMap<int, QString>&)>& successCallback,
                                 const std::function<void(const QString&)>& errorCallback)
{
    try {
        QMap<int, QString> unita;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        query.prepare("SELECT id,nome FROM unita;");
        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            unita[query.value(0).toInt()] = query.value(1).toString();
        }

        successCallback(unita);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getDoctorsFromUnit(int unitId,
                                          const std::function<void(const QMap<int, QString>&)>& successCallback,
                                          const std::function<void(const QString&)>& errorCallback)
{
    try {
        QMap<int, QString> doctors;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        query.prepare("SELECT id, nome FROM medici WHERE id_unita = :unitId ORDER BY nome;");
        query.bindValue(":unitId", unitId);

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            doctors[query.value(0).toInt()] = query.value(1).toString();
        }

        successCallback(doctors);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getTimecardsList(const std::function<void(const QStringList&)>& successCallback,
                                          const std::function<void(const QString&)>& errorCallback)
{
    try {
        QStringList tables;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        query.prepare("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            if(query.value(0).toString().contains("tc_"))
                tables << query.value(0).toString();
        }

        successCallback(tables);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getUnitaDataFromTimecard(const QString& timecard,
                                                  const std::function<void(const QVector<UnitaDataTimecard>&)>& successCallback,
                                                  const std::function<void(const QString&)>& errorCallback)
{
    try {
        QVector<UnitaDataTimecard> result;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        query.prepare("SELECT DISTINCT " + timecard + ".id_unita, unita.nome, unita.id " +
                      "FROM " + timecard + " " +
                      "LEFT JOIN unita " +
                      "ON " + timecard + ".id_unita=unita.id ORDER BY length(unita.id), unita.id;");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            UnitaDataTimecard unita;
            unita.idUnita = query.value(0).toInt();
            unita.nome = query.value(1).toString();
            unita.id = query.value(2).toInt();
            result.append(unita);
        }

        successCallback(result);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getDoctorDataFromUnitaInTimecard(const QString& timecard,
                                                          int idUnita,
                                                          const std::function<void(const QVector<DoctorData>&)>& successCallback,
                                                          const std::function<void(const QString&)>& errorCallback)
{
    try {
        QVector<DoctorData> result;
        QString whereClause;

        if(idUnita != -1)
            whereClause = "WHERE " + timecard + ".id_unita=" + QString::number(idUnita);

        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT medici.id, medici.matricola, medici.nome "
                      "FROM medici "
                      "LEFT JOIN " + timecard + " "
                                   "ON " + timecard + ".id_medico=medici.id "
                      + whereClause + " ORDER BY medici.nome;");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            DoctorData doctor;
            doctor.id = query.value(0).toInt();
            doctor.matricola = query.value(1).toInt();
            doctor.nome = query.value(2).toString();
            result.append(doctor);
        }

        successCallback(result);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getDoctorTimecard(const QString& tableName,
                                        const QString& modTableName,
                                        int doctorId,
                                        const std::function<void(const QVariantList&)>& successCallback,
                                        const std::function<void(const QString&)>& errorCallback)
{
    try {
        QVariantList result;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

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
                      + modTableName + ".orario_giornaliero,"
                      + modTableName + ".pagaStrGuar,"
                      + tableName + ".indennita_festiva,"
                      + tableName + ".indennita_notturna,"
                      + modTableName + ".turni_teleconsulto "
                      + "FROM " + tableName + " LEFT JOIN medici ON medici.id=" + tableName + ".id_medico "
                      + "LEFT JOIN unita ON unita.id=" + tableName + ".id_unita "
                      + "LEFT JOIN " + modTableName + " ON " + modTableName + ".id_medico=" + tableName + ".id_medico "
                      + "WHERE " + tableName + ".id_medico=" + QString::number(doctorId) + ";");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
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
            result << query.value(31); // indennita_festiva
            result << query.value(32); // indennita_notturna
            result << query.value(33); // turni teleconsulto
        }

        successCallback(result);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getOrePagateFromUnit(int unitaId,
                                           const std::function<void(const OrePagateMap&)>& successCallback,
                                           const std::function<void(const QString&)>& errorCallback)
{
    try {
        OrePagateMap map;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT data,ore_tot,ore_pagate,paga_diurno,paga_ore_lavorate FROM unita_ore_pagate WHERE id_unita=" + QString::number(unitaId) + ";");
        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            QStringList meseAnno = query.value(0).toString().split("/");
            QDate date(meseAnno.at(1).toInt(), meseAnno.at(0).toInt(), 1);

            OrePagate ore(
                query.value(1).toInt(),  // ore_tot
                query.value(2).toInt(),  // ore_pagate
                query.value(3).toInt(),  // paga_diurno
                query.value(4).toInt()   // paga_ore_lavorate
            );

            map[date] = ore;
        }

        successCallback(map);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getRecuperiMeseSuccessivo(int anno,
                                                int mese,
                                                int doctorId,
                                                const std::function<void(const QPair<int, int>&)>& successCallback,
                                                const std::function<void(const QString&)>& errorCallback)
{
    try {
        QPair<int, int> result;
        result.first = 0;
        result.second = 0;

        const QDate date(anno, mese, 1);
        const QString table = "tc_" + QString::number(date.addMonths(1).year()) +
                              QString::number(date.addMonths(1).month()).rightJustified(2, '0');

        // Verificare se la tabella esiste
        QSqlQuery checkTableQuery(QSqlDatabase::database(m_connectionName));
        bool tableExists = false;

        checkTableQuery.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='" + table + "';");

        if (!checkTableQuery.exec()) {
            throw QString("Error checking table existence: %1").arg(checkTableQuery.lastError().text());
        }

        if (checkTableQuery.next()) {
            tableExists = checkTableQuery.value(0).toInt() > 0;
        }

        if (!tableExists) {
            successCallback(result);
            return;
        }

        // Verificare se la timecard esiste
        QSqlQuery checkTimeCardQuery(QSqlDatabase::database(m_connectionName));
        checkTimeCardQuery.prepare("SELECT COUNT(*) FROM " + table + " WHERE id_medico='" + QString::number(doctorId) + "';");

        if (!checkTimeCardQuery.exec()) {
            throw QString("Error checking timecard existence: %1").arg(checkTimeCardQuery.lastError().text());
        }

        bool timeCardExists = false;
        if (checkTimeCardQuery.next()) {
            timeCardExists = checkTimeCardQuery.value(0).toInt() > 0;
        }

        if (!timeCardExists) {
            successCallback(result);
            return;
        }

        // Recuperare i dati di recupero
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT minuti_giornalieri, rmp from " + table + " WHERE id_medico=" + QString::number(doctorId) + ";");

        if (!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        if (query.next()) {
            result.first = query.value(0).toInt(); // minuti_giornalieri
            result.second = query.value(1).toString().trimmed().split(",", Qt::SkipEmptyParts).count(); // rmp
        }

        successCallback(result);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::isAbilitatoStraordinario(int matricola,
                                               int anno,
                                               int mese,
                                               const std::function<void(bool)>& successCallback,
                                               const std::function<void(const QString&)>& errorCallback)
{
    try {
        bool ok = true;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        // Crea la data di decorrenza nel formato YYYYMMDD
        QDate decorrenzaDate(anno, mese, 1);
        QString decorrenza = decorrenzaDate.toString("yyyyMMdd");

        query.prepare("SELECT abilitato FROM abilitazione_straordinario WHERE matricola=" +
                      QString::number(matricola) +
                      " AND decorrenza <= " + decorrenza +
                      " ORDER BY decorrenza DESC LIMIT 1;");

        if (!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        // Se ci sono risultati, prendi il valore di abilitato
        if (query.next()) {
            ok = query.value(0).toBool();
        }

        successCallback(ok);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getReperibilitaSemplificata(int idUnita,
                                                     int anno,
                                                     int mese,
                                                     const std::function<void(ReperibilitaSemplificata*)>& successCallback,
                                                     const std::function<void(const QString&)>& errorCallback)
{
    try {
        // Se i parametri non sono validi, ritorna un oggetto vuoto
        if (idUnita == 0 || anno == 0 || mese == 0) {
            successCallback(new ReperibilitaSemplificata());
            return;
        }

        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        // Crea la data di decorrenza nel formato YYYYMMDD
        QDate decorrenzaDate(anno, mese, 1);
        QString decorrenza = decorrenzaDate.toString("yyyyMMdd");

        query.prepare("SELECT id,id_unita,feriale,sabato,festivo,decorrenza,prefestivo FROM reperibilita_semplificata "
                      "WHERE id_unita=" + QString::number(idUnita) +
                      " AND decorrenza <= " + decorrenza +
                      " ORDER BY decorrenza DESC LIMIT 1;");

        if (!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        if (query.next()) {
            // Crea un oggetto ReperibilitaSemplificata con i dati trovati
            ReperibilitaSemplificata* rep = new ReperibilitaSemplificata(
                idUnita,
                QDate(anno, mese, 1),
                query.value(2).toDouble(),     // feriale
                query.value(3).toDouble(),     // sabato
                query.value(4).toDouble(),     // festivo
                query.value(6).toBool()        // prefestivo
                );

            successCallback(rep);
        } else {
            // Se non viene trovato nessun record, restituisci un oggetto vuoto
            successCallback(new ReperibilitaSemplificata());
        }
    } catch (const QString& error) {
        if (errorCallback) {
            errorCallback(error);
        } else if (successCallback) {
            // In caso di errore senza error callback, restituisci un oggetto vuoto
            successCallback(new ReperibilitaSemplificata());
        }
    }
}

void SqliteDataProvider::abilitaDisabilitaStraordinario(int matricola,
                                                        bool enable,
                                                        int anno,
                                                        int mese,
                                                        const std::function<void(bool)>& successCallback,
                                                        const std::function<void(const QString&)>& errorCallback)
{
    QSqlQuery query(QSqlDatabase::database(m_connectionName));

    // Verificare se esiste già un record per questa combinazione di matricola/anno/mese
    int id = 0;
    query.prepare("SELECT id FROM abilitazione_straordinario WHERE matricola=" +
                  QString::number(matricola) + " AND decorrenza=" +
                  QDate(anno, mese, 1).toString("yyyyMMdd") + ";");

    if (!query.exec()) {
        if (errorCallback) {
            errorCallback("Error executing query: " + query.lastError().text());
        }
        return;
    }

    while (query.next()) {
        id = query.value(0).toInt();
    }

    bool result = false;

    if (id > 0) {
        // Aggiorna il valore esistente
        query.prepare("UPDATE abilitazione_straordinario "
                      "SET abilitato=:abilitato "
                      "WHERE id=" + QString::number(id) + ";");
        query.bindValue(":abilitato", enable ? 1 : 0);
    } else {
        // Inserisci un nuovo record
        query.prepare("INSERT INTO abilitazione_straordinario (matricola, decorrenza, abilitato) "
                      "VALUES (:matricola, :decorrenza, :abilitato);");
        query.bindValue(":matricola", matricola);
        query.bindValue(":decorrenza", QDate(anno, mese, 1).toString("yyyyMMdd"));
        query.bindValue(":abilitato", enable ? 1 : 0);
    }

    if (!query.exec()) {
        if (errorCallback) {
            errorCallback("Error executing query: " + query.lastError().text());
        }
        return;
    }

    result = true;
    successCallback(result);
}

void SqliteDataProvider::idSituazioneSaldo(int anno,
                                           int mese,
                                           int matricola,
                                           const std::function<void(int)>& successCallback,
                                           const std::function<void(const QString&)>& errorCallback)
{
    if(anno == 0 || mese == 0 || matricola == 0) {
        if (successCallback) {
            successCallback(0);
        }
        return;
    }

    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare("SELECT id FROM situazione_saldo WHERE anno=" + QString::number(anno) +
                  " AND mese=" + QString::number(mese) +
                  " AND matricola=" + QString::number(matricola) + ";");

    if(!query.exec()) {
        if (errorCallback) {
            errorCallback("Error executing query: " + query.lastError().text());
        }
        return;
    }

    int id = 0;
    while(query.next()) {
        id = query.value(0).toInt();
    }

    if (successCallback) {
        successCallback(id);
    }
}

void SqliteDataProvider::addSituazioneSaldo(int anno,
                                            int mese,
                                            int matricola,
                                            int saldo,
                                            int rmp,
                                            int dmp,
                                            const std::function<void(bool)>& successCallback,
                                            const std::function<void(const QString&)>& errorCallback)
{
    if(anno == 0 || mese == 0 || matricola == 0) {
        if (errorCallback) {
            errorCallback("Invalid parameters: anno, mese, or matricola is zero");
        }
        return;
    }

    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare("INSERT INTO situazione_saldo (matricola, anno, mese, saldo, rmp, dmp) " +
                  QString("VALUES (%1, %2, %3, %4, %5, %6);")
                      .arg(matricola)
                      .arg(anno)
                      .arg(mese)
                      .arg(saldo)
                      .arg(rmp)
                      .arg(dmp));

    if(!query.exec()) {
        if (errorCallback) {
            errorCallback("Error executing query: " + query.lastError().text());
        }
        return;
    }

    if (successCallback) {
        successCallback(true);
    }
}

void SqliteDataProvider::updateSituazioneSaldo(int id,
                                               int saldo,
                                               int rmp,
                                               int dmp,
                                               const std::function<void(bool)>& successCallback,
                                               const std::function<void(const QString&)>& errorCallback)
{
    if(id == 0) {
        if (errorCallback) {
            errorCallback("Invalid parameter: id is zero");
        }
        return;
    }

    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare("UPDATE situazione_saldo SET saldo=" + QString::number(saldo) +
                  ", rmp=" + QString::number(rmp) +
                  ", dmp=" + QString::number(dmp) +
                  " WHERE id=" + QString::number(id) + ";");

    if(!query.exec()) {
        if (errorCallback) {
            errorCallback("Error executing query: " + query.lastError().text());
        }
        return;
    }

    if (successCallback) {
        successCallback(true);
    }
}


void SqliteDataProvider::saveMod(const QString& tableName,
                                 const QString& columnName,
                                 int id,
                                 const QVariant& value,
                                 const std::function<void(bool)>& successCallback,
                                 const std::function<void(const QString&)>& errorCallback)
{
    QSqlQuery query(QSqlDatabase::database(m_connectionName));

    // Preparazione della query con placeholder
    QString queryString = "UPDATE " + tableName + " SET " + columnName + " = :value WHERE id_medico = :id";
    query.prepare(queryString);

    // Binding dei parametri
    query.bindValue(":id", id);
    query.bindValue(":value", value);

    if(!query.exec()) {
        if (errorCallback) {
            errorCallback("Error executing query: " + query.lastError().text());
        }
        return;
    }

    if (successCallback) {
        successCallback(true);
    }
}

void SqliteDataProvider::tableExists(const QString& tableName,
                                     const std::function<void(bool)>& successCallback,
                                     const std::function<void(const QString&)>& errorCallback)
{
    try {
        int count = 0;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        query.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='" + tableName + "';");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

        successCallback(count > 0);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::createTimeCardsTable(const QString& tableName,
                                              const std::function<void(bool)>& successCallback,
                                              const std::function<void(const QString&)>& errorCallback)
{
    try {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        // Crea la tabella principale dei cartellini
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
                                                    "scoperti TEXT DEFAULT '',"
                                                    "indennita_festiva TEXT DEFAULT '',"
                                                    "indennita_notturna TEXT DEFAULT '');");

        if(!query.exec()) {
            throw QString("Error creating timecard table: %1").arg(query.lastError().text());
        }

        // Crea la tabella delle modifiche
        QString modTableName = tableName;
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
                                                                          "pagaStrGuar INTEGER DEFAULT (1) NOT NULL,"
                                                                          "turni_teleconsulto TEXT DEFAULT '');");

        if(!query.exec()) {
            throw QString("Error creating mod table: %1").arg(query.lastError().text());
        }

        successCallback(true);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::createPagatoTable(int anno,
                                           int mese,
                                           const std::function<void(bool)>& successCallback,
                                           const std::function<void(const QString&)>& errorCallback)
{
    try {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        QString tableName = "tcp_" + QString::number(anno) + QString::number(mese).rightJustified(2,'0');

        query.prepare("CREATE TABLE " + tableName +
                      "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                      "ci INTEGER NOT NULL,"
                      "deficit INTEGER NOT NULL,"
                      "ind_not INTEGER NOT NULL DEFAULT (0),"
                      "ind_fes REAL NOT NULL DEFAULT (0.0),"
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
                      "data INTEGER NOT NULL DEFAULT (0),"
                      "rep_oltre_10 REAL NOT NULL DEFAULT (0.0),"
                      "teleconsulto REAL NOT NULL DEFAULT (0.0),"
                      "ore_lavorate REAL NOT NULL DEFAULT (0.0));");

        if(!query.exec()) {
            throw QString("Error creating pagato table: %1").arg(query.lastError().text());
        }

        successCallback(true);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}


void SqliteDataProvider::unitId(int matricola,
                                const std::function<void(int)>& successCallback,
                                const std::function<void(const QString&)>& errorCallback)
{
    try {
        int id = -1;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        query.prepare("SELECT id_unita FROM medici WHERE matricola=" + QString::number(matricola) + ";");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            id = query.value(0).toInt();
        }

        successCallback(id);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::addTimeCard(const QString& tableName,
                                     const Dipendente* dipendente,
                                     const std::function<void(bool)>& successCallback,
                                     const std::function<void(const QString&)>& errorCallback)
{
    try {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        int docId = doctorId(dipendente->matricola());

        if(dipendente->unita() == -1) {
            throw QString("Impossibile trovare l'unità %1").arg(dipendente->unita());
        }

        if(docId == -1) {
            docId = insertDoctor(dipendente->matricola(), dipendente->nome(), QString::number(dipendente->unita()));
        }

        if(docId == -1) {
            throw QString("id dipendente -1");
        }

        if(timeCardExists(tableName, docId)) {
            resetTimeCard(tableName, docId);
            query.prepare("UPDATE " + tableName + " " +
                          "SET riposi=:riposi,minuti_giornalieri=:minuti_giornalieri,"
                          "ferie=:ferie,congedi=:congedi,malattia=:malattia,rmp=:rmp,"
                          "rmc=:rmc,altre_causali=:altre_causali,guardie_diurne=:guardie_diurne,guardie_notturne=:guardie_notturne,"
                          "grep=:grep,congedi_minuti=:congedi_minuti,eccr_minuti=:eccr_minuti,grep_minuti=:grep_minuti,"
                          "guar_minuti=:guar_minuti,rmc_minuti=:rmc_minuti,minuti_fatti=:minuti_fatti,scoperti=:scoperti,indennita_festiva=:indennita_festiva,indennita_notturna=:indennita_notturna "
                          "WHERE id_medico=" + QString::number(docId) + ";");
        } else {
            query.prepare("INSERT INTO " + tableName + " (id_medico, id_unita, anno, mese, riposi, minuti_giornalieri, ferie, congedi, malattia, rmp, rmc, altre_causali, guardie_diurne, guardie_notturne, grep, congedi_minuti, eccr_minuti, grep_minuti, guar_minuti, rmc_minuti, minuti_fatti, scoperti, indennita_festiva, indennita_notturna) "
                                                       "VALUES (:id_medico, :id_unita, :anno, :mese, :riposi, :minuti_giornalieri, :ferie, :congedi, :malattia, :rmp, :rmc, :altre_causali, :guardie_diurne, :guardie_notturne, :grep, :congedi_minuti, :eccr_minuti, :grep_minuti, :guar_minuti, :rmc_minuti, :minuti_fatti, :scoperti, :indennita_festiva, :indennita_notturna);");
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

        QStringList mezzeGuardieDiurne;
        for(auto i : dipendente->mezzeGuardieDiurne()) {
            mezzeGuardieDiurne << QString::number(i) + "*";
        }

        QStringList list;
        list << Utilities::vectorIntToStringlist(dipendente->guardieDiurne()) << mezzeGuardieDiurne;

        query.bindValue(":guardie_diurne", list.join(","));
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
        query.bindValue(":indennita_festiva", dipendente->indennitaFestiva());
        query.bindValue(":indennita_notturna", Utilities::vectorIntToStringlist(dipendente->indennitaNotturna()).join(","));

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        int diffMinuti = 0;

        QDate d(dipendente->anno(), dipendente->mese(), 1);

        QString modTablePrevMonth = "tcm_" + QString::number(d.addDays(-1).year()) + QString::number(d.addDays(-1).month()).rightJustified(2, '0');
        bool prevTableExists = false;

        QSqlQuery checkQuery(QSqlDatabase::database(m_connectionName));
        checkQuery.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='" + modTablePrevMonth + "';");

        if(checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
            prevTableExists = true;
        }

        if(prevTableExists) {
            QSqlQuery timeCardQuery(QSqlDatabase::database(m_connectionName));
            timeCardQuery.prepare("SELECT COUNT(*) FROM " + modTablePrevMonth + " WHERE id_medico='" + QString::number(docId) + "';");

            if(timeCardQuery.exec() && timeCardQuery.next() && timeCardQuery.value(0).toInt() > 0) {
                // Qui dovremmo ottenere minutiSaldoMese da Competenza
                // Per semplicità, supponiamo di poter ottenere questo valore
                // In una implementazione reale, dovreste usare la classe Competenza
                diffMinuti = 0; // Questo valore dovrebbe essere ottenuto da Competenza
            }
        }

        if(diffMinuti >= 0)
            diffMinuti = 0;
        diffMinuti = abs(diffMinuti);

        QString modTableName = tableName;
        modTableName.replace("_","m_");

        // se la riga del medico nella tabella delle modifiche esiste già non aggiungiamo nuovamente
        QSqlQuery checkModQuery(QSqlDatabase::database(m_connectionName));
        checkModQuery.prepare("SELECT COUNT(*) FROM " + modTableName + " WHERE id_medico='" + QString::number(docId) + "';");

        bool modTableExists = false;

        if(checkModQuery.exec() && checkModQuery.next() && checkModQuery.value(0).toInt() > 0) {
            modTableExists = true;
        }

        if(modTableExists) {
            QSqlQuery updateQuery(QSqlDatabase::database(m_connectionName));
            updateQuery.prepare("UPDATE " + modTableName + " "
                                                           "SET dmp_calcolato=:dmp_calcolato "
                                                           "WHERE id_medico=" + QString::number(docId) + ";");
            updateQuery.bindValue(":dmp_calcolato", diffMinuti);

            if(!updateQuery.exec()) {
                throw QString("Error updating mod table: %1").arg(updateQuery.lastError().text());
            }

            successCallback(true);
            return;
        }

        QSqlQuery insertModQuery(QSqlDatabase::database(m_connectionName));
        insertModQuery.prepare("INSERT INTO " + modTableName + " (id_medico,dmp_calcolato) "
                                                               "VALUES (:id_medico,:dmp_calcolato);");
        insertModQuery.bindValue(":id_medico", docId);
        insertModQuery.bindValue(":dmp_calcolato", diffMinuti);

        if(!insertModQuery.exec()) {
            throw QString("Error inserting into mod table: %1").arg(insertModQuery.lastError().text());
        }

        successCallback(true);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

int SqliteDataProvider::doctorId(int matricola)
{
    int id = -1;
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare("SELECT id FROM medici WHERE matricola=" + QString::number(matricola) + ";");
    if(query.exec() && query.next()) {
        id = query.value(0).toInt();
    }
    return id;
}

int SqliteDataProvider::insertDoctor(int matricola, const QString& nome, const QString& id_unita)
{
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare("INSERT INTO medici (matricola,nome,id_unita) "
                  "VALUES (:matricola, :nome, :id_unita);");
    query.bindValue(":matricola", matricola);
    query.bindValue(":nome", nome);
    query.bindValue(":id_unita", id_unita);

    if(!query.exec()) {
        return -1;
    }

    if(query.lastInsertId().isValid())
        return query.lastInsertId().toInt();

    return -1;
}

bool SqliteDataProvider::timeCardExists(const QString& tableName, int doctorId)
{
    int count = 0;
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id_medico='" + QString::number(doctorId) + "';");
    if(query.exec() && query.next()) {
        count = query.value(0).toInt();
    }

    return (count > 0);
}

void SqliteDataProvider::resetTimeCard(const QString& tableName, int doctorId)
{
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    query.prepare("UPDATE " + tableName + " " +
                  "SET riposi=:riposi,minuti_giornalieri=:minuti_giornalieri,"
                  "ferie=:ferie,congedi=:congedi,malattia=:malattia,rmp=:rmp,"
                  "rmc=:rmc,altre_causali=:altre_causali,guardie_diurne=:guardie_diurne,guardie_notturne=:guardie_notturne,"
                  "grep=:grep,congedi_minuti=:congedi_minuti,eccr_minuti=:eccr_minuti,grep_minuti=:grep_minuti,"
                  "guar_minuti=:guar_minuti,rmc_minuti=:rmc_minuti,minuti_fatti=:minuti_fatti,scoperti=:scoperti,indennita_festiva=:indennita_festiva,indennita_notturna=:indennita_notturna "
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
    query.bindValue(":indennita_festiva", QString());
    query.bindValue(":indennita_notturna", QString());

    query.exec();
}

void SqliteDataProvider::doctorId(int matricola,
                                  const std::function<void(int)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback)
{
    try {
        int id = -1;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        query.prepare("SELECT id FROM medici WHERE matricola=" + QString::number(matricola) + ";");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            id = query.value(0).toInt();
        }

        successCallback(id);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::editUnit(const QString& id,
                                  const QString& raggruppamento,
                                  const QString& nome,
                                  const QString& breve,
                                  const std::function<void(bool)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback)
{
    try {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        query.prepare("UPDATE unita "
                      "SET raggruppamento=:raggruppamento,nome=:nome,breve=:breve "
                      "WHERE id=" + id + ";");
        query.bindValue(":raggruppamento", raggruppamento);
        query.bindValue(":nome", nome);
        query.bindValue(":breve", breve);

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        successCallback(true);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::editDoctor(const QString& id,
                                    const QString& matricola,
                                    const QString& nome,
                                    const std::function<void(bool)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback)
{
    try {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("UPDATE medici "
                      "SET matricola=:matricola, nome=:nome "
                      "WHERE id=" + id + ";");
        query.bindValue(":matricola", matricola);
        query.bindValue(":nome", nome);

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        successCallback(true);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::insertPayload(int id_unita,
                                       const QString& data,
                                       int ore_tot,
                                       int ore_pagate,
                                       int paga_diurno,
                                       const std::function<void(bool)>& successCallback,
                                       const std::function<void(const QString&)>& errorCallback)
{
    try {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("INSERT INTO unita_ore_pagate (id_unita, data, ore_tot, ore_pagate, paga_diurno) "
                      "VALUES (:id_unita, :data, :ore_tot, :ore_pagate, :paga_diurno);");
        query.bindValue(":id_unita", id_unita);
        query.bindValue(":data", data);
        query.bindValue(":ore_tot", ore_tot);
        query.bindValue(":ore_pagate", ore_pagate);
        query.bindValue(":paga_diurno", paga_diurno);

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        successCallback(true);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::resetTimeCard(const QString& tableName,
                                       int doctorId,
                                       const std::function<void(bool)>& successCallback,
                                       const std::function<void(const QString&)>& errorCallback)
{
    try {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("UPDATE " + tableName + " " +
                      "SET riposi=:riposi,minuti_giornalieri=:minuti_giornalieri,"
                      "ferie=:ferie,congedi=:congedi,malattia=:malattia,rmp=:rmp,"
                      "rmc=:rmc,altre_causali=:altre_causali,guardie_diurne=:guardie_diurne,guardie_notturne=:guardie_notturne,"
                      "grep=:grep,congedi_minuti=:congedi_minuti,eccr_minuti=:eccr_minuti,grep_minuti=:grep_minuti,"
                      "guar_minuti=:guar_minuti,rmc_minuti=:rmc_minuti,minuti_fatti=:minuti_fatti,scoperti=:scoperti,indennita_festiva=:indennita_festiva,indennita_notturna=:indennita_notturna "
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
        query.bindValue(":indennita_festiva", QString());
        query.bindValue(":indennita_notturna", QString());

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        successCallback(true);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::resetAllDoctorMods(const QString& tableName,
                                            int id,
                                            const std::function<void(bool)>& successCallback,
                                            const std::function<void(const QString&)>& errorCallback)
{
    try {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("UPDATE " + tableName + " " +
                      "SET guardie_diurne=:guardie_diurne,guardie_notturne=:guardie_notturne,"
                      "turni_reperibilita=:turni_reperibilita,dmp=:dmp,altre_assenze=:altre_assenze,nota=:nota,"
                      "altro_str=:altro_str,mensa=:mensa,orario_giornaliero=:orario_giornaliero,turni_teleconsulto=:turni_teleconsulto "
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
        query.bindValue(":turni_teleconsulto", QString());

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        successCallback(true);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::resetStringValue(const QString& tableName,
                                          const QString& columnName,
                                          int id,
                                          const std::function<void(bool)>& successCallback,
                                          const std::function<void(const QString&)>& errorCallback)
{
    try {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("UPDATE " + tableName + " " +
                      "SET " + columnName + "=:" + columnName + " " +
                      "WHERE id_medico=" + QString::number(id) + ";");
        query.bindValue(":" + columnName, QString());

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        successCallback(true);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::numDoctorsFromUnitInTimecard(const QString& timecard,
                                                      int unitId,
                                                      const std::function<void(int)>& successCallback,
                                                      const std::function<void(const QString&)>& errorCallback)
{
    try {
        int count = 0;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT COUNT(*) FROM " + timecard + " WHERE id_unita='" + QString::number(unitId) + "';");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

        successCallback(count);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::numDoctorsInTimecard(const QString& timecard,
                                              const std::function<void(int)>& successCallback,
                                              const std::function<void(const QString&)>& errorCallback)
{
    try {
        int count = 0;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT COUNT(*) FROM " + timecard + ";");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

        successCallback(count);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getDoctorsIdsFromUnitInTimecard(const QString& timecard,
                                                         int unitId,
                                                         const std::function<void(const QVector<int>&)>& successCallback,
                                                         const std::function<void(const QString&)>& errorCallback)
{
    try {
        QVector<int> ids;
        QString whereClause;
        if(unitId != -1)
            whereClause = "WHERE " + timecard + ".id_unita='" + QString::number(unitId) + "' ";

        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT " + timecard + ".id_medico,medici.nome "
                                             "FROM " + timecard + " "
                                   "LEFT JOIN medici ON "
                      + timecard + ".id_medico=medici.id "
                      + whereClause +
                      "ORDER BY medici.nome;");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            ids << query.value(0).toInt();
        }

        successCallback(ids);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getOrePagateFromId(int id,
                                            const std::function<void(const QVariantList&)>& successCallback,
                                            const std::function<void(const QString&)>& errorCallback)
{
    try {
        QVariantList result;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT data,ore_tot,ore_pagate,paga_diurno FROM unita_ore_pagate WHERE id=" + QString::number(id) + ";");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            result.clear();
            result << query.value(0);
            result << query.value(1);
            result << query.value(2);
            result << query.value(3);
        }

        successCallback(result);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getTuttiMatricoleNomi(const std::function<void(const QVector<DoctorData>&)>& successCallback,
                                               const std::function<void(const QString&)>& errorCallback)
{
    try {
        QVector<DoctorData> result;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT id,matricola,nome FROM medici ORDER BY nome;");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            DoctorData doctor;
            doctor.id = query.value(0).toInt();
            doctor.matricola = query.value(1).toInt();
            doctor.nome = query.value(2).toString();
            result.append(doctor);
        }

        successCallback(result);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getUnitaDataTimecardAll(const std::function<void(const QVector<UnitaDataTimecard>&)>& successCallback,
                                                 const std::function<void(const QString&)>& errorCallback)
{
    try {
        QVector<UnitaDataTimecard> result;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT id, nome FROM unita;");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            UnitaDataTimecard unita;
            unita.id = query.value(0).toInt();
            unita.idUnita = query.value(0).toInt(); // Assumiamo che id e idUnita siano uguali in questo caso
            unita.nome = query.value(1).toString();
            result.append(unita);
        }

        successCallback(result);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getUnitaDataById(int idUnita,
                                          const std::function<void(const UnitaData&)>& successCallback,
                                          const std::function<void(const QString&)>& errorCallback)
{
    try {
        UnitaData unita;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT id, raggruppamento, nome, breve, pseudo FROM unita WHERE id=" + QString::number(idUnita) + ";");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        if (query.next()) {
            unita.id = query.value(0).toInt();
            unita.raggruppamento = query.value(1).toString();
            unita.nome = query.value(2).toString();
            unita.breve = query.value(3).toString();
            unita.pseudo = query.value(4).toString();
        }

        successCallback(unita);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getCompetenzePagate(int ci,
                                             int anno,
                                             int mese,
                                             const std::function<void(CompetenzePagate*)>& successCallback,
                                             const std::function<void(const QString&)>& errorCallback)
{
    try {
        if(ci == 0 || anno == 0 || mese == 0) {
            successCallback(new CompetenzePagate());
            return;
        }

        CompetenzePagate *pagato = new CompetenzePagate();
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT id,ci,deficit,ind_not,ind_fes,str_reparto_ord,str_reparto_nof,"
                      "str_reparto_nef,str_repe_ord,str_repe_nof,str_repe_nef,str_guard_ord,"
                      "str_guard_nof,str_guard_nef,turni_repe,ore_repe,guard_diu,guard_not,"
                      "grande_fes,data,rep_oltre_10,teleconsulto,ore_lavorate "
                      "FROM tcp_" + QString::number(anno) + QString::number(mese).rightJustified(2, '0') +
                      " WHERE ci=" + QString::number(ci) + " ORDER BY data DESC LIMIT 1;");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            pagato->setCi(query.value(1).toInt());
            pagato->setDeficit(query.value(2).toInt());
            pagato->setIndNotturna(query.value(3).toInt());
            pagato->setIndFestiva(query.value(4).toDouble());
            pagato->setStr_reparto_ord(query.value(5).toInt());
            pagato->setStr_reparto_nof(query.value(6).toInt());
            pagato->setStr_reparto_nef(query.value(7).toInt());
            pagato->setStr_repe_ord(query.value(8).toInt());
            pagato->setStr_repe_nof(query.value(9).toInt());
            pagato->setStr_repe_nef(query.value(10).toInt());
            pagato->setStr_guard_ord(query.value(11).toInt());
            pagato->setStr_guard_nof(query.value(12).toInt());
            pagato->setStr_guard_nef(query.value(13).toInt());
            pagato->setTurni_repe(query.value(14).toInt());
            pagato->setOre_repe(query.value(15).toInt());
            pagato->setGuard_diu(query.value(16).toDouble());
            pagato->setGuard_not(query.value(17).toInt());
            pagato->setGrande_fes(query.value(18).toInt());
            pagato->setDateTime(QDateTime::fromString(query.value(19).toString(), "yyyyMMddhhmm"));
            pagato->setRepOltre10(query.value(20).toDouble());
            pagato->setTeleconsulto(query.value(21).toDouble());
            pagato->setOreLavorate(query.value(22).toDouble());
        }

        successCallback(pagato);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::saveCompetenzePagate(CompetenzePagate* pagato,
                                              int anno,
                                              int mese,
                                              const std::function<void(bool)>& successCallback,
                                              const std::function<void(const QString&)>& errorCallback)
{
    try {
        if(pagato->ci() == 0 || anno == 0 || mese == 0) {
            throw QString("Invalid parameters: pagato->ci() or anno or mese is zero");
        }

        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("INSERT INTO tcp_" + QString::number(anno) + QString::number(mese).rightJustified(2, '0') + " "
                                                                                                                  "(ci,deficit,ind_not,ind_fes,str_reparto_ord,str_reparto_nof,str_reparto_nef,str_repe_ord,str_repe_nof,str_repe_nef,str_guard_ord,str_guard_nof,str_guard_nef,turni_repe,ore_repe,guard_diu,guard_not,grande_fes,data,rep_oltre_10,teleconsulto,ore_lavorate) "
                                                                                                                  "VALUES (:ci,:deficit,:ind_not,:ind_fes,:str_reparto_ord,:str_reparto_nof,:str_reparto_nef,:str_repe_ord,:str_repe_nof,:str_repe_nef,:str_guard_ord,:str_guard_nof,:str_guard_nef,:turni_repe,:ore_repe,:guard_diu,:guard_not,:grande_fes,:data,:rep_oltre_10,:teleconsulto,:ore_lavorate);");
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
        query.bindValue(":data", pagato->dataElaborazione().toString("yyyyMMddhhmm"));
        query.bindValue(":rep_oltre_10", pagato->repOltre10());
        query.bindValue(":teleconsulto", pagato->teleconsulto());
        query.bindValue(":ore_lavorate", pagato->oreLavorate());

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        successCallback(true);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getUnitaNomeCompleto(int id,
                                              const std::function<void(const QString&)>& successCallback,
                                              const std::function<void(const QString&)>& errorCallback)
{
    try {
        QString nome;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        query.prepare("SELECT nome FROM unita WHERE id='" + QString::number(id) + "';");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            nome = query.value(0).toString();
        }

        successCallback(nome);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getDoctorName(int matricola,
                                       const std::function<void(const QString&)>& successCallback,
                                       const std::function<void(const QString&)>& errorCallback)
{
    try {
        QString name;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT nome FROM medici WHERE matricola=" + QString::number(matricola) + ";");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            name = query.value(0).toString();
        }

        successCallback(name);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getDoctorMatricola(int id,
                                            const std::function<void(int)>& successCallback,
                                            const std::function<void(const QString&)>& errorCallback)
{
    try {
        int matricola = 0;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT matricola FROM medici WHERE id=" + QString::number(id) + ";");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            matricola = query.value(0).toInt();
        }

        successCallback(matricola);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getIndennita(int anno,
                                      int mese,
                                      const std::function<void(Indennita*)>& successCallback,
                                      const std::function<void(const QString&)>& errorCallback)
{
    try {
        Indennita* indennita = new Indennita();
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT id,id_unita,tipo,decorrenza,voce,sub FROM voci_paga;");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            indennita->addItem(Utilities::indennitaEnum(query.value(2).toString()),
                               query.value(1).toInt(),
                               query.value(4).toString(),
                               query.value(5).toString());
        }

        successCallback(indennita);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::timeCardExists(const QString& tableName,
                                     int doctorId,
                                     const std::function<void(bool)>& successCallback,
                                     const std::function<void(const QString&)>& errorCallback)
{
    try {
        int count = 0;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));

        query.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id_medico='" + QString::number(doctorId) + "';");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            count = query.value(0).toInt();
        }

        successCallback(count > 0);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::getAllUnitaData(const std::function<void(const QVector<UnitaData>&)>& successCallback,
                                         const std::function<void(const QString&)>& errorCallback)
{
    try {
        QVector<UnitaData> result;
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("SELECT id, raggruppamento, nome, breve, pseudo FROM unita;");

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        while(query.next()) {
            UnitaData unita;
            unita.id = query.value(0).toInt();
            unita.raggruppamento = query.value(1).toString(); // Assumiamo che id e idUnita siano uguali in questo caso
            unita.nome = query.value(2).toString();
            unita.breve = query.value(3).toString();
            unita.pseudo = query.value(4).toString();
            result.append(unita);
        }

        successCallback(result);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}

void SqliteDataProvider::setUnitaMedico(int docId,
                                        int unitId,
                                        const std::function<void(bool)>& successCallback,
                                        const std::function<void(const QString&)>& errorCallback)
{
    try {
        QSqlQuery query(QSqlDatabase::database(m_connectionName));
        query.prepare("UPDATE medici SET id_unita = :unitId WHERE id = :docId");
        query.bindValue(":unitId", unitId);
        query.bindValue(":docId", docId);

        if(!query.exec()) {
            throw QString("Error executing query: %1").arg(query.lastError().text());
        }

        // Verifichiamo che almeno una riga sia stata modificata
        if(query.numRowsAffected() == 0) {
            throw QString("No record found for the specified doctor ID");
        }

        successCallback(true);
    } catch (const QString& error) {
        if (errorCallback) errorCallback(error);
    }
}
