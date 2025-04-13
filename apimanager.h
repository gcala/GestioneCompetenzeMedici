#ifndef APIMANAGER_H
#define APIMANAGER_H

#include "doctordata.h"
#include "unitadatatimecard.h"
#include "orepagate.h"

#include "idataprovider.h"
#include <QObject>
#include <memory>

class Indennita;

class ApiManager : public QObject
{
    Q_OBJECT
public:
    static ApiManager* instance();

    void setDriver(const QString& driver);
    QString driver() const { return m_driver; }

    // Metodi delegati all'implementazione corretta
    void getUnits(const std::function<void(const QMap<int, QString>&)>& successCallback,
                  const std::function<void(const QString&)>& errorCallback = nullptr);

    void getDoctorsFromUnit(int unitId,
                           const std::function<void(const QMap<int, QString>&)>& successCallback,
                           const std::function<void(const QString&)>& errorCallback = nullptr);

    void getTimecardsList(const std::function<void(const QStringList&)>& successCallback,
                          const std::function<void(const QString&)>& errorCallback = nullptr);

    void getUnitaDataFromTimecard(const QString& timecard,
                                  const std::function<void(const QVector<UnitaDataTimecard>&)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback = nullptr);

    void getDoctorDataFromUnitaInTimecard(const QString& timecard,
                                          int idUnita,
                                          const std::function<void(const QVector<DoctorData>&)>& successCallback,
                                          const std::function<void(const QString&)>& errorCallback = nullptr);

    void getDoctorTimecard(const QString& tableName,
                         const QString& modTableName,
                         int doctorId,
                         const std::function<void(const QVariantList&)>& successCallback,
                         const std::function<void(const QString&)>& errorCallback = nullptr);

    void getOrePagateFromUnit(int unitaId,
                             const std::function<void(const OrePagateMap&)>& successCallback,
                             const std::function<void(const QString&)>& errorCallback = nullptr);

    void getRecuperiMeseSuccessivo(int anno,
                                  int mese,
                                  int doctorId,
                                  const std::function<void(const QPair<int, int>&)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback = nullptr);

    void isAbilitatoStraordinario(int matricola,
                                 int anno,
                                 int mese,
                                 const std::function<void(bool)>& successCallback,
                                 const std::function<void(const QString&)>& errorCallback = nullptr);

    void getReperibilitaSemplificata(int idUnita,
                                     int anno,
                                     int mese,
                                     const std::function<void(ReperibilitaSemplificata*)>& successCallback,
                                     const std::function<void(const QString&)>& errorCallback = nullptr);

    void abilitaDisabilitaStraordinario(int matricola,
                                        bool enable,
                                        int anno,
                                        int mese,
                                        const std::function<void(bool)>& successCallback,
                                        const std::function<void(const QString&)>& errorCallback = nullptr);

    void idSituazioneSaldo(int anno,
                           int mese,
                           int matricola,
                           const std::function<void(int)>& successCallback,
                           const std::function<void(const QString&)>& errorCallback = nullptr);

    void addSituazioneSaldo(int anno,
                            int mese,
                            int matricola,
                            int saldo,
                            int rmp,
                            int dmp,
                            const std::function<void(bool)>& successCallback,
                            const std::function<void(const QString&)>& errorCallback = nullptr);

    void updateSituazioneSaldo(int id,
                               int saldo,
                               int rmp,
                               int dmp,
                               const std::function<void(bool)>& successCallback,
                               const std::function<void(const QString&)>& errorCallback = nullptr);

    void saveMod(const QString& tableName,
                 const QString& columnName,
                 int id,
                 const QVariant& value,
                 const std::function<void(bool)>& successCallback,
                 const std::function<void(const QString&)>& errorCallback = nullptr);

    void tableExists(const QString& tableName,
                     const std::function<void(bool)>& successCallback,
                     const std::function<void(const QString&)>& errorCallback = nullptr);

    void createTimeCardsTable(const QString& tableName,
                              const std::function<void(bool)>& successCallback,
                              const std::function<void(const QString&)>& errorCallback = nullptr);

    void createPagatoTable(int anno,
                           int mese,
                           const std::function<void(bool)>& successCallback,
                           const std::function<void(const QString&)>& errorCallback = nullptr);

    // In apimanager.h, aggiungi nella dichiarazione della classe
    void unitId(int matricola,
                const std::function<void(int)>& successCallback,
                const std::function<void(const QString&)>& errorCallback = nullptr);

    void addTimeCard(const QString& tableName,
                     const Dipendente* dipendente,
                     const std::function<void(bool)>& successCallback,
                     const std::function<void(const QString&)>& errorCallback = nullptr);

    // In apimanager.h, aggiungi nella dichiarazione della classe
    void doctorId(int matricola,
                  const std::function<void(int)>& successCallback,
                  const std::function<void(const QString&)>& errorCallback = nullptr);

    void editUnit(const QString& id,
                  const QString& raggruppamento,
                  const QString& nome,
                  const QString& breve,
                  const std::function<void(bool)>& successCallback,
                  const std::function<void(const QString&)>& errorCallback = nullptr);

    void editDoctor(const QString& id,
                    const QString& matricola,
                    const QString& nome,
                    const std::function<void(bool)>& successCallback,
                    const std::function<void(const QString&)>& errorCallback = nullptr);

    void insertPayload(int id_unita,
                       const QString& data,
                       int ore_tot,
                       int ore_pagate,
                       int paga_diurno,
                       const std::function<void(bool)>& successCallback,
                       const std::function<void(const QString&)>& errorCallback = nullptr);

    void resetTimeCard(const QString& tableName,
                       int doctorId,
                       const std::function<void(bool)>& successCallback,
                       const std::function<void(const QString&)>& errorCallback = nullptr);

    void resetAllDoctorMods(const QString& tableName,
                            int id,
                            const std::function<void(bool)>& successCallback,
                            const std::function<void(const QString&)>& errorCallback = nullptr);

    void resetStringValue(const QString& tableName,
                          const QString& columnName,
                          int id,
                          const std::function<void(bool)>& successCallback,
                          const std::function<void(const QString&)>& errorCallback = nullptr);

    void numDoctorsFromUnitInTimecard(const QString& timecard,
                                      int unitId,
                                      const std::function<void(int)>& successCallback,
                                      const std::function<void(const QString&)>& errorCallback = nullptr);

    void numDoctorsInTimecard(const QString& timecard,
                              const std::function<void(int)>& successCallback,
                              const std::function<void(const QString&)>& errorCallback = nullptr);

    void getDoctorsIdsFromUnitInTimecard(const QString& timecard,
                                         int unitId,
                                         const std::function<void(const QVector<int>&)>& successCallback,
                                         const std::function<void(const QString&)>& errorCallback = nullptr);

    void getOrePagateFromId(int id,
                            const std::function<void(const QVariantList&)>& successCallback,
                            const std::function<void(const QString&)>& errorCallback = nullptr);

    void getTuttiMatricoleNomi(const std::function<void(const QVector<DoctorData>&)>& successCallback,
                               const std::function<void(const QString&)>& errorCallback = nullptr);

    void getUnitaDataTimecardAll(const std::function<void(const QVector<UnitaDataTimecard>&)>& successCallback,
                                 const std::function<void(const QString&)>& errorCallback = nullptr);

    void getUnitaDataById(int idUnita,
                          const std::function<void(const UnitaData&)>& successCallback,
                          const std::function<void(const QString&)>& errorCallback = nullptr);

    void getCompetenzePagate(int ci,
                             int anno,
                             int mese,
                             const std::function<void(CompetenzePagate*)>& successCallback,
                             const std::function<void(const QString&)>& errorCallback = nullptr);

    void saveCompetenzePagate(CompetenzePagate* pagato,
                              int anno,
                              int mese,
                              const std::function<void(bool)>& successCallback,
                              const std::function<void(const QString&)>& errorCallback = nullptr);

    void getUnitaNomeCompleto(int id,
                              const std::function<void(const QString&)>& successCallback,
                              const std::function<void(const QString&)>& errorCallback = nullptr);

    void getDoctorName(int matricola,
                       const std::function<void(const QString&)>& successCallback,
                       const std::function<void(const QString&)>& errorCallback = nullptr);

    void getDoctorMatricola(int id,
                            const std::function<void(int)>& successCallback,
                            const std::function<void(const QString&)>& errorCallback = nullptr);

    void getIndennita(int anno,
                      int mese,
                      const std::function<void(Indennita*)>& successCallback,
                      const std::function<void(const QString&)>& errorCallback = nullptr);

    void timeCardExists(const QString& tableName,
                        int doctorId,
                        const std::function<void(bool)>& successCallback,
                        const std::function<void(const QString&)>& errorCallback = nullptr);

    void getAllUnitaData(const std::function<void(const QVector<UnitaData>&)>& successCallback,
                         const std::function<void(const QString&)>& errorCallback = nullptr);

    void setUnitaMedico(int docId,
                        int unitId,
                        const std::function<void(bool)>& successCallback,
                        const std::function<void(const QString&)>& errorCallback = nullptr);

private:
    explicit ApiManager(QObject *parent = nullptr);
    ~ApiManager();

    static ApiManager* m_instance;
    QString m_driver;
    std::unique_ptr<IDataProvider> m_provider;

    void createProvider();
};

#endif // APIMANAGER_H
