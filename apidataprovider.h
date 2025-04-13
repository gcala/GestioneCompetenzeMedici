#ifndef APIDATAPROVIDER_H
#define APIDATAPROVIDER_H

#include "idataprovider.h"
#include <QNetworkAccessManager>

class ApiDataProvider : public IDataProvider
{
public:
    explicit ApiDataProvider(const QString& baseUrl, QObject* parent = nullptr);

    void getUnits(const std::function<void(const QMap<int, QString>&)>& successCallback,
                  const std::function<void(const QString&)>& errorCallback) override;

    void getDoctorsFromUnit(int unitId,
                           const std::function<void(const QMap<int, QString>&)>& successCallback,
                           const std::function<void(const QString&)>& errorCallback) override;

    void getTimecardsList(const std::function<void(const QStringList&)>& successCallback,
                          const std::function<void(const QString&)>& errorCallback) override;

    void getUnitaDataFromTimecard(const QString& timecard,
                                  const std::function<void(const QVector<UnitaDataTimecard>&)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback) override;

    void getDoctorDataFromUnitaInTimecard(const QString& timecard,
                                          int idUnita,
                                          const std::function<void(const QVector<DoctorData>&)>& successCallback,
                                          const std::function<void(const QString&)>& errorCallback) override;

    void getDoctorTimecard(const QString& tableName,
                         const QString& modTableName,
                         int doctorId,
                         const std::function<void(const QVariantList&)>& successCallback,
                         const std::function<void(const QString&)>& errorCallback) override;

    void getOrePagateFromUnit(int unitaId,
                             const std::function<void(const OrePagateMap&)>& successCallback,
                             const std::function<void(const QString&)>& errorCallback) override;

    void getRecuperiMeseSuccessivo(int anno,
                                  int mese,
                                  int doctorId,
                                  const std::function<void(const QPair<int, int>&)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback) override;

    void isAbilitatoStraordinario(int matricola,
                                 int anno,
                                 int mese,
                                 const std::function<void(bool)>& successCallback,
                                 const std::function<void(const QString&)>& errorCallback) override;

    void getReperibilitaSemplificata(int idUnita,
                                     int anno,
                                     int mese,
                                     const std::function<void(ReperibilitaSemplificata*)>& successCallback,
                                     const std::function<void(const QString&)>& errorCallback) override;

    void abilitaDisabilitaStraordinario(int matricola,
                                        bool enable,
                                        int anno,
                                        int mese,
                                        const std::function<void(bool)>& successCallback,
                                        const std::function<void(const QString&)>& errorCallback) override;

    void idSituazioneSaldo(int anno,
                           int mese,
                           int matricola,
                           const std::function<void(int)>& successCallback,
                           const std::function<void(const QString&)>& errorCallback) override;

    void addSituazioneSaldo(int anno,
                            int mese,
                            int matricola,
                            int saldo,
                            int rmp,
                            int dmp,
                            const std::function<void(bool)>& successCallback,
                            const std::function<void(const QString&)>& errorCallback) override;

    void updateSituazioneSaldo(int id,
                               int saldo,
                               int rmp,
                               int dmp,
                               const std::function<void(bool)>& successCallback,
                               const std::function<void(const QString&)>& errorCallback) override;

    void saveMod(const QString& tableName,
                 const QString& columnName,
                 int id,
                 const QVariant& value,
                 const std::function<void(bool)>& successCallback,
                 const std::function<void(const QString&)>& errorCallback) override;

    void tableExists(const QString& tableName,
                     const std::function<void(bool)>& successCallback,
                     const std::function<void(const QString&)>& errorCallback) override;

    void createTimeCardsTable(const QString& tableName,
                              const std::function<void(bool)>& successCallback,
                              const std::function<void(const QString&)>& errorCallback) override;

    void createPagatoTable(int anno,
                           int mese,
                           const std::function<void(bool)>& successCallback,
                           const std::function<void(const QString&)>& errorCallback) override;

    // In apidataprovider.h, aggiungi nella dichiarazione della classe
    void unitId(int matricola,
                const std::function<void(int)>& successCallback,
                const std::function<void(const QString&)>& errorCallback) override;

    void addTimeCard(const QString& tableName,
                     const Dipendente* dipendente,
                     const std::function<void(bool)>& successCallback,
                     const std::function<void(const QString&)>& errorCallback) override;

    // In apidataprovider.h, aggiungi nella dichiarazione della classe
    void doctorId(int matricola,
                  const std::function<void(int)>& successCallback,
                  const std::function<void(const QString&)>& errorCallback) override;

    void editUnit(const QString& id,
                  const QString& raggruppamento,
                  const QString& nome,
                  const QString& breve,
                  const std::function<void(bool)>& successCallback,
                  const std::function<void(const QString&)>& errorCallback) override;

    void editDoctor(const QString& id,
                    const QString& matricola,
                    const QString& nome,
                    const std::function<void(bool)>& successCallback,
                    const std::function<void(const QString&)>& errorCallback) override;

    void insertPayload(int id_unita,
                       const QString& data,
                       int ore_tot,
                       int ore_pagate,
                       int paga_diurno,
                       const std::function<void(bool)>& successCallback,
                       const std::function<void(const QString&)>& errorCallback) override;

    void resetTimeCard(const QString& tableName,
                       int doctorId,
                       const std::function<void(bool)>& successCallback,
                       const std::function<void(const QString&)>& errorCallback) override;

    void resetAllDoctorMods(const QString& tableName,
                            int id,
                            const std::function<void(bool)>& successCallback,
                            const std::function<void(const QString&)>& errorCallback) override;

    void resetStringValue(const QString& tableName,
                          const QString& columnName,
                          int id,
                          const std::function<void(bool)>& successCallback,
                          const std::function<void(const QString&)>& errorCallback) override;

    void numDoctorsFromUnitInTimecard(const QString& timecard,
                                      int unitId,
                                      const std::function<void(int)>& successCallback,
                                      const std::function<void(const QString&)>& errorCallback) override;

    void numDoctorsInTimecard(const QString& timecard,
                              const std::function<void(int)>& successCallback,
                              const std::function<void(const QString&)>& errorCallback) override;

    void getDoctorsIdsFromUnitInTimecard(const QString& timecard,
                                         int unitId,
                                         const std::function<void(const QVector<int>&)>& successCallback,
                                         const std::function<void(const QString&)>& errorCallback) override;

    void getOrePagateFromId(int id,
                            const std::function<void(const QVariantList&)>& successCallback,
                            const std::function<void(const QString&)>& errorCallback) override;

    void getTuttiMatricoleNomi(const std::function<void(const QVector<DoctorData>&)>& successCallback,
                               const std::function<void(const QString&)>& errorCallback) override;

    void getUnitaDataTimecardAll(const std::function<void(const QVector<UnitaDataTimecard>&)>& successCallback,
                                 const std::function<void(const QString&)>& errorCallback) override;

    void getUnitaDataById(int idUnita,
                          const std::function<void(const UnitaData&)>& successCallback,
                          const std::function<void(const QString&)>& errorCallback) override;

    // In apidataprovider.h
    void getCompetenzePagate(int ci,
                             int anno,
                             int mese,
                             const std::function<void(CompetenzePagate*)>& successCallback,
                             const std::function<void(const QString&)>& errorCallback) override;

    void saveCompetenzePagate(CompetenzePagate* pagato,
                              int anno,
                              int mese,
                              const std::function<void(bool)>& successCallback,
                              const std::function<void(const QString&)>& errorCallback) override;

    void getUnitaNomeCompleto(int id,
                              const std::function<void(const QString&)>& successCallback,
                              const std::function<void(const QString&)>& errorCallback) override;

    void getDoctorName(int matricola,
                       const std::function<void(const QString&)>& successCallback,
                       const std::function<void(const QString&)>& errorCallback) override;

    void getDoctorMatricola(int id,
                            const std::function<void(int)>& successCallback,
                            const std::function<void(const QString&)>& errorCallback) override;

    void getIndennita(int anno,
                      int mese,
                      const std::function<void(Indennita*)>& successCallback,
                      const std::function<void(const QString&)>& errorCallback) override;

    void timeCardExists(const QString& tableName,
                        int doctorId,
                        const std::function<void(bool)>& successCallback,
                        const std::function<void(const QString&)>& errorCallback) override;

    void getAllUnitaData(const std::function<void(const QVector<UnitaData>&)>& successCallback,
                         const std::function<void(const QString&)>& errorCallback) override;

    void setUnitaMedico(int docId,
                        int unitId,
                        const std::function<void(bool)>& successCallback,
                        const std::function<void(const QString&)>& errorCallback) override;



private:
    QNetworkAccessManager* m_networkManager;
    QString m_baseUrl;

    void makeRequest(const QString& endpoint,
                    const std::function<void(const QJsonDocument&)>& successCallback,
                    const std::function<void(const QString&)>& errorCallback);
    void handleNetworkError(QNetworkReply* reply,
                           const std::function<void(const QString&)>& errorCallback);
};

#endif // APIDATAPROVIDER_H
