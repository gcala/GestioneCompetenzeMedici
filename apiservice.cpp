#include "apiservice.h"
#include "apimanager.h"
#include "reperibilitasemplificata.h"
#include "competenzepagate.h"
#include "indennita.h"

#include <QEventLoop>
#include <QDebug>

ApiService& ApiService::instance()
{
    static ApiService instance;
    return instance;
}

// Template helper per eseguire chiamate asincrone in modo sincrono
template<typename ResultType, typename ApiFunction>
ResultType executeSync(ApiFunction apiCall, QString* errorMsg = nullptr) {
    ResultType result{};
    QEventLoop loop;
    bool success = false;
    QString error;
    bool alreadyFinished = false;

    apiCall(
        [&result, &loop, &success, &alreadyFinished](const ResultType& apiResult) {
            result = apiResult;
            success = true;
            alreadyFinished = true;
            loop.quit();
        },
        [&error, &loop, &alreadyFinished](const QString& err) {
            error = err;
            alreadyFinished = true;
            loop.quit();
        }
        );

    if (!alreadyFinished) {
        loop.exec();
    }

    if (!success && errorMsg) {
        *errorMsg = error;
    }

    return result;
}

// Specializzazione per chiamate asincrone che ritornano un bool
template<typename ApiFunction>
bool executeSyncBool(ApiFunction apiCall, QString* errorMsg = nullptr) {
    bool result = false;
    QEventLoop loop;
    bool success = false;
    QString error;
    bool alreadyFinished = false;

    apiCall(
        [&result, &loop, &success, &alreadyFinished](bool apiResult) {
            result = apiResult;
            success = true;
            alreadyFinished = true;
            loop.quit();
        },
        [&error, &loop, &alreadyFinished](const QString& err) {
            error = err;
            alreadyFinished = true;
            loop.quit();
        }
        );

    if (!alreadyFinished) {
        loop.exec();
    }

    if (!success && errorMsg) {
        *errorMsg = error;
    }

    return result;
}

QStringList ApiService::getTimecardsList(QString* errorMsg)
{
    return executeSync<QStringList>(
        [](auto successCb, auto errorCb) {
            ApiManager::instance()->getTimecardsList(successCb, errorCb);
        },
        errorMsg
        );
}

QMap<int, QString> ApiService::getUnits(QString* errorMsg)
{
    return executeSync<QMap<int, QString>>(
        [](auto successCb, auto errorCb) {
            ApiManager::instance()->getUnits(successCb, errorCb);
        },
        errorMsg
        );
}

QMap<int, QString> ApiService::getDoctorsFromUnit(int unitId, QString* errorMsg)
{
    return executeSync<QMap<int, QString>>(
        [unitId](auto successCb, auto errorCb) {
            ApiManager::instance()->getDoctorsFromUnit(unitId, successCb, errorCb);
        },
        errorMsg
        );
}

QVector<UnitaDataTimecard> ApiService::getUnitaDataFromTimecard(const QString& timecard, QString* errorMsg)
{
    return executeSync<QVector<UnitaDataTimecard>>(
        [&timecard](auto successCb, auto errorCb) {
            ApiManager::instance()->getUnitaDataFromTimecard(timecard, successCb, errorCb);
        },
        errorMsg
        );
}

QVector<DoctorData> ApiService::getDoctorDataFromUnitaInTimecard(const QString& timecard, int idUnita, QString* errorMsg)
{
    return executeSync<QVector<DoctorData>>(
        [&timecard, idUnita](auto successCb, auto errorCb) {
            ApiManager::instance()->getDoctorDataFromUnitaInTimecard(timecard, idUnita, successCb, errorCb);
        },
        errorMsg
        );
}

QVariantList ApiService::getDoctorTimecard(const QString& tableName, const QString& modTableName, int doctorId, QString* errorMsg)
{
    return executeSync<QVariantList>(
        [&tableName, &modTableName, doctorId](auto successCb, auto errorCb) {
            ApiManager::instance()->getDoctorTimecard(tableName, modTableName, doctorId, successCb, errorCb);
        },
        errorMsg
        );
}

OrePagateMap ApiService::getOrePagateFromUnit(int unitaId, QString* errorMsg)
{
    return executeSync<OrePagateMap>(
        [unitaId](auto successCb, auto errorCb) {
            ApiManager::instance()->getOrePagateFromUnit(unitaId, successCb, errorCb);
        },
        errorMsg
        );
}

QPair<int, int> ApiService::getRecuperiMeseSuccessivo(int anno, int mese, int doctorId, QString* errorMsg)
{
    return executeSync<QPair<int, int>>(
        [anno, mese, doctorId](auto successCb, auto errorCb) {
            ApiManager::instance()->getRecuperiMeseSuccessivo(anno, mese, doctorId, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::isAbilitatoStraordinario(int matricola, int anno, int mese, QString* errorMsg)
{
    return executeSyncBool(
        [matricola, anno, mese](auto successCb, auto errorCb) {
            ApiManager::instance()->isAbilitatoStraordinario(matricola, anno, mese, successCb, errorCb);
        },
        errorMsg
        );
}

ReperibilitaSemplificata* ApiService::getReperibilitaSemplificata(int idUnita, int anno, int mese, QString* errorMsg)
{
    auto result = executeSync<ReperibilitaSemplificata*>(
        [idUnita, anno, mese](auto successCb, auto errorCb) {
            ApiManager::instance()->getReperibilitaSemplificata(idUnita, anno, mese, successCb, errorCb);
        },
        errorMsg
        );

    // Gestione caso null
    if (!result) {
        result = new ReperibilitaSemplificata();
    }

    return result;
}

bool ApiService::abilitaDisabilitaStraordinario(int matricola, bool enable, int anno, int mese, QString* errorMsg)
{
    return executeSyncBool(
        [matricola, enable, anno, mese](auto successCb, auto errorCb) {
            ApiManager::instance()->abilitaDisabilitaStraordinario(matricola, enable, anno, mese, successCb, errorCb);
        },
        errorMsg
        );
}

int ApiService::idSituazioneSaldo(int anno, int mese, int matricola, QString* errorMsg)
{
    return executeSync<int>(
        [anno, mese, matricola](auto successCb, auto errorCb) {
            ApiManager::instance()->idSituazioneSaldo(anno, mese, matricola, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::addSituazioneSaldo(int anno, int mese, int matricola, int saldo, int rmp, int dmp, QString* errorMsg)
{
    return executeSyncBool(
        [anno, mese, matricola, saldo, rmp, dmp](auto successCb, auto errorCb) {
            ApiManager::instance()->addSituazioneSaldo(anno, mese, matricola, saldo, rmp, dmp, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::updateSituazioneSaldo(int id, int saldo, int rmp, int dmp, QString* errorMsg)
{
    return executeSyncBool(
        [id, saldo, rmp, dmp](auto successCb, auto errorCb) {
            ApiManager::instance()->updateSituazioneSaldo(id, saldo, rmp, dmp, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::saveMod(const QString& tableName, const QString& columnName, int id, const QVariant& value, QString* errorMsg)
{
    return executeSyncBool(
        [&tableName, &columnName, id, &value](auto successCb, auto errorCb) {
            ApiManager::instance()->saveMod(tableName, columnName, id, value, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::tableExists(const QString& tableName, QString* errorMsg)
{
    return executeSyncBool(
        [&tableName](auto successCb, auto errorCb) {
            ApiManager::instance()->tableExists(tableName, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::createTimeCardsTable(const QString& tableName, QString* errorMsg)
{
    return executeSyncBool(
        [&tableName](auto successCb, auto errorCb) {
            ApiManager::instance()->createTimeCardsTable(tableName, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::createPagatoTable(int anno, int mese, QString* errorMsg)
{
    return executeSyncBool(
        [anno, mese](auto successCb, auto errorCb) {
            ApiManager::instance()->createPagatoTable(anno, mese, successCb, errorCb);
        },
        errorMsg
        );
}

int ApiService::unitId(int matricola, QString* errorMsg)
{
    return executeSync<int>(
        [matricola](auto successCb, auto errorCb) {
            ApiManager::instance()->unitId(matricola, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::addTimeCard(const QString& tableName, const Dipendente* dipendente, QString* errorMsg)
{
    return executeSyncBool(
        [&tableName, dipendente](auto successCb, auto errorCb) {
            ApiManager::instance()->addTimeCard(tableName, dipendente, successCb, errorCb);
        },
        errorMsg
        );
}

int ApiService::doctorId(int matricola, QString* errorMsg)
{
    return executeSync<int>(
        [matricola](auto successCb, auto errorCb) {
            ApiManager::instance()->doctorId(matricola, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::editUnit(const QString& id,
                          const QString& raggruppamento,
                          const QString& nome,
                          const QString& breve,
                          QString* errorMsg)
{
    return executeSyncBool(
        [&id, &raggruppamento, &nome, &breve](auto successCb, auto errorCb) {
            ApiManager::instance()->editUnit(id, raggruppamento, nome, breve, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::editDoctor(const QString& id, const QString& matricola, const QString& nome, QString* errorMsg)
{
    return executeSyncBool(
        [&id, &matricola, &nome](auto successCb, auto errorCb) {
            ApiManager::instance()->editDoctor(id, matricola, nome, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::insertPayload(int id_unita,
                               const QString& data,
                               int ore_tot,
                               int ore_pagate,
                               int paga_diurno,
                               QString* errorMsg)
{
    return executeSyncBool(
        [id_unita, &data, ore_tot, ore_pagate, paga_diurno](auto successCb, auto errorCb) {
            ApiManager::instance()->insertPayload(id_unita, data, ore_tot, ore_pagate, paga_diurno,
                                                  successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::resetTimeCard(const QString& tableName, int doctorId, QString* errorMsg)
{
    return executeSyncBool(
        [&tableName, doctorId](auto successCb, auto errorCb) {
            ApiManager::instance()->resetTimeCard(tableName, doctorId, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::resetAllDoctorMods(const QString& tableName, int id, QString* errorMsg)
{
    return executeSyncBool(
        [&tableName, id](auto successCb, auto errorCb) {
            ApiManager::instance()->resetAllDoctorMods(tableName, id, successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::resetStringValue(const QString& tableName, const QString& columnName, int id, QString* errorMsg)
{
    return executeSyncBool(
        [&tableName, &columnName, id](auto successCb, auto errorCb) {
            ApiManager::instance()->resetStringValue(tableName, columnName, id, successCb, errorCb);
        },
        errorMsg
        );
}

int ApiService::numDoctorsFromUnitInTimecard(const QString& timecard, int unitId, QString* errorMsg)
{
    return executeSync<int>(
        [&timecard, unitId](auto successCb, auto errorCb) {
            ApiManager::instance()->numDoctorsFromUnitInTimecard(timecard, unitId, successCb, errorCb);
        },
        errorMsg
        );
}

int ApiService::numDoctorsInTimecard(const QString& timecard, QString* errorMsg)
{
    return executeSync<int>(
        [&timecard](auto successCb, auto errorCb) {
            ApiManager::instance()->numDoctorsInTimecard(timecard, successCb, errorCb);
        },
        errorMsg
        );
}

QVector<int> ApiService::getDoctorsIdsFromUnitInTimecard(const QString& timecard, int unitId, QString* errorMsg)
{
    return executeSync<QVector<int>>(
        [&timecard, unitId](auto successCb, auto errorCb) {
            ApiManager::instance()->getDoctorsIdsFromUnitInTimecard(timecard, unitId, successCb, errorCb);
        },
        errorMsg
        );
}

QVariantList ApiService::getOrePagateFromId(int id, QString* errorMsg)
{
    return executeSync<QVariantList>(
        [id](auto successCb, auto errorCb) {
            ApiManager::instance()->getOrePagateFromId(id, successCb, errorCb);
        },
        errorMsg
        );
}

QVector<DoctorData> ApiService::getTuttiMatricoleNomi(QString* errorMsg)
{
    return executeSync<QVector<DoctorData>>(
        [](auto successCb, auto errorCb) {
            ApiManager::instance()->getTuttiMatricoleNomi(successCb, errorCb);
        },
        errorMsg
        );
}

QVector<UnitaDataTimecard> ApiService::getUnitaDataTimecardAll(QString* errorMsg)
{
    return executeSync<QVector<UnitaDataTimecard>>(
        [](auto successCb, auto errorCb) {
            ApiManager::instance()->getUnitaDataTimecardAll(successCb, errorCb);
        },
        errorMsg
        );
}

UnitaData ApiService::getUnitaDataById(int idUnita, QString* errorMsg)
{
    return executeSync<UnitaData>(
        [idUnita](auto successCb, auto errorCb) {
            ApiManager::instance()->getUnitaDataById(idUnita, successCb, errorCb);
        },
        errorMsg
        );
}

CompetenzePagate* ApiService::getCompetenzePagate(int ci, int anno, int mese, QString* errorMsg)
{
    auto result = executeSync<CompetenzePagate*>(
        [ci, anno, mese](auto successCb, auto errorCb) {
            ApiManager::instance()->getCompetenzePagate(ci, anno, mese, successCb, errorCb);
        },
        errorMsg
        );

    // Gestione caso null
    if (!result) {
        result = new CompetenzePagate();
    }

    return result;
}

bool ApiService::saveCompetenzePagate(CompetenzePagate* pagato, int anno, int mese, QString* errorMsg)
{
    return executeSyncBool(
        [pagato, anno, mese](auto successCb, auto errorCb) {
            ApiManager::instance()->saveCompetenzePagate(pagato, anno, mese, successCb, errorCb);
        },
        errorMsg
        );
}

QString ApiService::getUnitaNomeCompleto(int id, QString* errorMsg)
{
    return executeSync<QString>(
        [id](auto successCb, auto errorCb) {
            ApiManager::instance()->getUnitaNomeCompleto(id, successCb, errorCb);
        },
        errorMsg
        );
}

QString ApiService::getDoctorName(int matricola, QString* errorMsg)
{
    return executeSync<QString>(
        [matricola](auto successCb, auto errorCb) {
            ApiManager::instance()->getDoctorName(matricola, successCb, errorCb);
        },
        errorMsg
        );
}

int ApiService::getDoctorMatricola(int id, QString* errorMsg)
{
    return executeSync<int>(
        [id](auto successCb, auto errorCb) {
            ApiManager::instance()->getDoctorMatricola(id, successCb, errorCb);
        },
        errorMsg
        );
}

Indennita* ApiService::getIndennita(int anno, int mese, QString* errorMsg)
{
    auto result = executeSync<Indennita*>(
        [anno, mese](auto successCb, auto errorCb) {
            ApiManager::instance()->getIndennita(anno, mese, successCb, errorCb);
        },
        errorMsg
        );

    // Gestione caso null
    if (!result) {
        result = new Indennita();
    }

    return result;
}

bool ApiService::timeCardExists(const QString& tableName, int doctorId, QString* errorMsg)
{
    return executeSyncBool(
        [&tableName, doctorId](auto successCb, auto errorCb) {
            ApiManager::instance()->timeCardExists(tableName, doctorId, successCb, errorCb);
        },
        errorMsg
        );
}

QVector<UnitaData> ApiService::getAllUnitaData(QString* errorMsg)
{
    return executeSync<QVector<UnitaData>>(
        [](auto successCb, auto errorCb) {
            ApiManager::instance()->getAllUnitaData(successCb, errorCb);
        },
        errorMsg
        );
}

bool ApiService::setUnitaMedico(int docId, int unitId, QString* errorMsg)
{
    return executeSyncBool(
        [docId, unitId](auto successCb, auto errorCb) {
            ApiManager::instance()->setUnitaMedico(docId, unitId, successCb, errorCb);
        },
        errorMsg
        );
}
