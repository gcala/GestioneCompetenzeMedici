#include "apimanager.h"
#include "sqlitedataprovider.h"
#include "apidataprovider.h"
#include "utilities.h"
#include "reperibilitasemplificata.h"
#include "competenzepagate.h"
#include "indennita.h"

ApiManager* ApiManager::m_instance = nullptr;

ApiManager* ApiManager::instance()
{
    if (!m_instance) {
        m_instance = new ApiManager();
    }
    return m_instance;
}

ApiManager::ApiManager(QObject *parent)
    : QObject(parent)
    , m_driver("QMYSQL") // Default driver
{
    createProvider();
}

ApiManager::~ApiManager()
{

}

void ApiManager::setDriver(const QString& driver)
{
    if (m_driver != driver) {
        m_driver = driver;
        createProvider();
    }
}

void ApiManager::createProvider()
{
    if (m_driver == "QSQLITE") {
        m_provider = std::make_unique<SqliteDataProvider>(Utilities::m_connectionName, this);
    } else if (m_driver == "QMYSQL") {
        m_provider = std::make_unique<ApiDataProvider>("https://cuteworks.it/GestioneCompetenzeMediciAPI", this);
    }
}

void ApiManager::getUnits(const std::function<void(const QMap<int, QString>&)>& successCallback,
                         const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getUnits(successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::getDoctorsFromUnit(int unitId,
                                  const std::function<void(const QMap<int, QString>&)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getDoctorsFromUnit(unitId, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::getTimecardsList(const std::function<void(const QStringList&)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getTimecardsList(successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::getUnitaDataFromTimecard(const QString& timecard,
                                          const std::function<void(const QVector<UnitaDataTimecard>&)>& successCallback,
                                          const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getUnitaDataFromTimecard(timecard, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::getDoctorDataFromUnitaInTimecard(const QString& timecard,
                                                  int idUnita,
                                                  const std::function<void(const QVector<DoctorData>&)>& successCallback,
                                                  const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getDoctorDataFromUnitaInTimecard(timecard, idUnita, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::getDoctorTimecard(const QString& tableName,
                                const QString& modTableName,
                                int doctorId,
                                const std::function<void(const QVariantList&)>& successCallback,
                                const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getDoctorTimecard(tableName, modTableName, doctorId, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::getOrePagateFromUnit(int unitaId,
                                    const std::function<void(const OrePagateMap&)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getOrePagateFromUnit(unitaId, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::getRecuperiMeseSuccessivo(int anno,
                                         int mese,
                                         int doctorId,
                                         const std::function<void(const QPair<int, int>&)>& successCallback,
                                         const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getRecuperiMeseSuccessivo(anno, mese, doctorId, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::isAbilitatoStraordinario(int matricola,
                                        int anno,
                                        int mese,
                                        const std::function<void(bool)>& successCallback,
                                        const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->isAbilitatoStraordinario(matricola, anno, mese, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::getReperibilitaSemplificata(int idUnita,
                                             int anno,
                                             int mese,
                                             const std::function<void(ReperibilitaSemplificata*)>& successCallback,
                                             const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getReperibilitaSemplificata(idUnita, anno, mese, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else if (successCallback) {
        // Se non c'è un provider e non c'è un error callback, restituiamo un oggetto vuoto
        successCallback(new ReperibilitaSemplificata());
    }
}

void ApiManager::abilitaDisabilitaStraordinario(int matricola,
                                                bool enable,
                                                int anno,
                                                int mese,
                                                const std::function<void(bool)>& successCallback,
                                                const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->abilitaDisabilitaStraordinario(matricola, enable, anno, mese, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::idSituazioneSaldo(int anno,
                                   int mese,
                                   int matricola,
                                   const std::function<void(int)>& successCallback,
                                   const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->idSituazioneSaldo(anno, mese, matricola, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::addSituazioneSaldo(int anno,
                                    int mese,
                                    int matricola,
                                    int saldo,
                                    int rmp,
                                    int dmp,
                                    const std::function<void(bool)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->addSituazioneSaldo(anno, mese, matricola, saldo, rmp, dmp, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::updateSituazioneSaldo(int id,
                                       int saldo,
                                       int rmp,
                                       int dmp,
                                       const std::function<void(bool)>& successCallback,
                                       const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->updateSituazioneSaldo(id, saldo, rmp, dmp, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::saveMod(const QString& tableName,
                         const QString& columnName,
                         int id,
                         const QVariant& value,
                         const std::function<void(bool)>& successCallback,
                         const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->saveMod(tableName, columnName, id, value, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::tableExists(const QString& tableName,
                             const std::function<void(bool)>& successCallback,
                             const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->tableExists(tableName, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::createTimeCardsTable(const QString& tableName,
                                      const std::function<void(bool)>& successCallback,
                                      const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->createTimeCardsTable(tableName, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::createPagatoTable(int anno,
                                   int mese,
                                   const std::function<void(bool)>& successCallback,
                                   const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->createPagatoTable(anno, mese, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::unitId(int matricola,
                        const std::function<void(int)>& successCallback,
                        const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->unitId(matricola, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::addTimeCard(const QString& tableName,
                             const Dipendente* dipendente,
                             const std::function<void(bool)>& successCallback,
                             const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->addTimeCard(tableName, dipendente, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::doctorId(int matricola,
                          const std::function<void(int)>& successCallback,
                          const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->doctorId(matricola, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::editUnit(const QString& id,
                          const QString& raggruppamento,
                          const QString& nome,
                          const QString& breve,
                          const std::function<void(bool)>& successCallback,
                          const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->editUnit(id, raggruppamento, nome, breve, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::editDoctor(const QString& id,
                            const QString& matricola,
                            const QString& nome,
                            const std::function<void(bool)>& successCallback,
                            const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->editDoctor(id, matricola, nome, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::insertPayload(int id_unita,
                               const QString& data,
                               int ore_tot,
                               int ore_pagate,
                               int paga_diurno,
                               const std::function<void(bool)>& successCallback,
                               const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->insertPayload(id_unita, data, ore_tot, ore_pagate, paga_diurno,
                                  successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::resetTimeCard(const QString& tableName,
                               int doctorId,
                               const std::function<void(bool)>& successCallback,
                               const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->resetTimeCard(tableName, doctorId, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::resetAllDoctorMods(const QString& tableName,
                                    int id,
                                    const std::function<void(bool)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->resetAllDoctorMods(tableName, id, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::resetStringValue(const QString& tableName,
                                  const QString& columnName,
                                  int id,
                                  const std::function<void(bool)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->resetStringValue(tableName, columnName, id, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}

void ApiManager::numDoctorsFromUnitInTimecard(const QString& timecard,
                                              int unitId,
                                              const std::function<void(int)>& successCallback,
                                              const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->numDoctorsFromUnitInTimecard(timecard, unitId, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else {
        successCallback(0);
    }
}

void ApiManager::numDoctorsInTimecard(const QString& timecard,
                                      const std::function<void(int)>& successCallback,
                                      const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->numDoctorsInTimecard(timecard, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else {
        successCallback(0);
    }
}

void ApiManager::getDoctorsIdsFromUnitInTimecard(const QString& timecard,
                                                 int unitId,
                                                 const std::function<void(const QVector<int>&)>& successCallback,
                                                 const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getDoctorsIdsFromUnitInTimecard(timecard, unitId, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else {
        successCallback(QVector<int>());
    }
}

void ApiManager::getOrePagateFromId(int id,
                                    const std::function<void(const QVariantList&)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getOrePagateFromId(id, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else {
        successCallback(QVariantList());
    }
}

void ApiManager::getTuttiMatricoleNomi(const std::function<void(const QVector<DoctorData>&)>& successCallback,
                                       const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getTuttiMatricoleNomi(successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else {
        successCallback(QVector<DoctorData>());
    }
}

void ApiManager::getUnitaDataTimecardAll(const std::function<void(const QVector<UnitaDataTimecard>&)>& successCallback,
                                         const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getUnitaDataTimecardAll(successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else {
        successCallback(QVector<UnitaDataTimecard>());
    }
}

void ApiManager::getUnitaDataById(int idUnita,
                                  const std::function<void(const UnitaData&)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getUnitaDataById(idUnita, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else {
        successCallback(UnitaData());
    }
}

void ApiManager::getCompetenzePagate(int ci,
                                     int anno,
                                     int mese,
                                     const std::function<void(CompetenzePagate*)>& successCallback,
                                     const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getCompetenzePagate(ci, anno, mese, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else if (successCallback) {
        // Se non c'è un provider e non c'è un error callback, restituiamo un oggetto vuoto
        successCallback(new CompetenzePagate());
    }
}

void ApiManager::saveCompetenzePagate(CompetenzePagate* pagato,
                                      int anno,
                                      int mese,
                                      const std::function<void(bool)>& successCallback,
                                      const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->saveCompetenzePagate(pagato, anno, mese, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else {
        successCallback(false);
    }
}

void ApiManager::getUnitaNomeCompleto(int id,
                                      const std::function<void(const QString&)>& successCallback,
                                      const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getUnitaNomeCompleto(id, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else {
        successCallback(QString());
    }
}

void ApiManager::getDoctorName(int matricola,
                               const std::function<void(const QString&)>& successCallback,
                               const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getDoctorName(matricola, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else {
        successCallback(QString());
    }
}

void ApiManager::getDoctorMatricola(int id,
                                    const std::function<void(int)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getDoctorMatricola(id, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else {
        successCallback(0);
    }
}

void ApiManager::getIndennita(int anno,
                              int mese,
                              const std::function<void(Indennita*)>& successCallback,
                              const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getIndennita(anno, mese, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else if (successCallback) {
        // Se non c'è un provider e non c'è un error callback, restituiamo un oggetto vuoto
        successCallback(new Indennita());
    }
}

void ApiManager::timeCardExists(const QString& tableName,
                                int doctorId,
                                const std::function<void(bool)>& successCallback,
                                const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->timeCardExists(tableName, doctorId, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else if (successCallback) {
        // Se non c'è un provider e non c'è un error callback, restituiamo un oggetto vuoto
        successCallback(false);
    }
}

void ApiManager::getAllUnitaData(const std::function<void(const QVector<UnitaData>&)>& successCallback,
                                 const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->getAllUnitaData(successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    } else {
        successCallback(QVector<UnitaData>());
    }
}

void ApiManager::setUnitaMedico(int docId,
                                int unitId,
                                const std::function<void(bool)>& successCallback,
                                const std::function<void(const QString&)>& errorCallback)
{
    if (m_provider) {
        m_provider->setUnitaMedico(docId, unitId, successCallback, errorCallback);
    } else if (errorCallback) {
        errorCallback("No data provider available");
    }
}
