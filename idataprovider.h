#ifndef IDATAPROVIDER_H
#define IDATAPROVIDER_H

#include "doctordata.h"
#include "unitadatatimecard.h"
#include "orepagate.h"
#include "unitadata.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <functional>

class ReperibilitaSemplificata;
class Dipendente;
class CompetenzePagate;
class Indennita;

class IDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit IDataProvider(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IDataProvider() = default;

    virtual void getUnits(const std::function<void(const QMap<int, QString>&)>& successCallback,
                         const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getDoctorsFromUnit(int unitId,
                                  const std::function<void(const QMap<int, QString>&)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getTimecardsList(const std::function<void(const QStringList&)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getUnitaDataFromTimecard(const QString& timecard,
                                          const std::function<void(const QVector<UnitaDataTimecard>&)>& successCallback,
                                          const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getDoctorDataFromUnitaInTimecard(const QString& timecard,
                                                  int idUnita,
                                                  const std::function<void(const QVector<DoctorData>&)>& successCallback,
                                                  const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getDoctorTimecard(const QString& tableName,
                                 const QString& modTableName,
                                 int doctorId,
                                 const std::function<void(const QVariantList&)>& successCallback,
                                 const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getOrePagateFromUnit(int unitaId,
                                     const std::function<void(const OrePagateMap&)>& successCallback,
                                     const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getRecuperiMeseSuccessivo(int anno,
                                         int mese,
                                         int doctorId,
                                         const std::function<void(const QPair<int, int>&)>& successCallback,
                                         const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void isAbilitatoStraordinario(int matricola,
                                         int anno,
                                         int mese,
                                         const std::function<void(bool)>& successCallback,
                                         const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getReperibilitaSemplificata(int idUnita,
                                             int anno,
                                             int mese,
                                             const std::function<void(ReperibilitaSemplificata*)>& successCallback,
                                             const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void abilitaDisabilitaStraordinario(int matricola,
                                                bool enable,
                                                int anno,
                                                int mese,
                                                const std::function<void(bool)>& successCallback,
                                                const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void idSituazioneSaldo(int anno,
                                   int mese,
                                   int matricola,
                                   const std::function<void(int)>& successCallback,
                                   const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void addSituazioneSaldo(int anno,
                                    int mese,
                                    int matricola,
                                    int saldo,
                                    int rmp,
                                    int dmp,
                                    const std::function<void(bool)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void updateSituazioneSaldo(int id,
                                       int saldo,
                                       int rmp,
                                       int dmp,
                                       const std::function<void(bool)>& successCallback,
                                       const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void saveMod(const QString& tableName,
                         const QString& columnName,
                         int id,
                         const QVariant& value,
                         const std::function<void(bool)>& successCallback,
                         const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void tableExists(const QString& tableName,
                             const std::function<void(bool)>& successCallback,
                             const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void createTimeCardsTable(const QString& tableName,
                                      const std::function<void(bool)>& successCallback,
                                      const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void createPagatoTable(int anno,
                                   int mese,
                                   const std::function<void(bool)>& successCallback,
                                   const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void unitId(int matricola,
                        const std::function<void(int)>& successCallback,
                        const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void addTimeCard(const QString& tableName,
                             const Dipendente* dipendente,
                             const std::function<void(bool)>& successCallback,
                             const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void doctorId(int matricola,
                          const std::function<void(int)>& successCallback,
                          const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void editUnit(const QString& id,
                          const QString& raggruppamento,
                          const QString& nome,
                          const QString& breve,
                          const std::function<void(bool)>& successCallback,
                          const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void editDoctor(const QString& id,
                            const QString& matricola,
                            const QString& nome,
                            const std::function<void(bool)>& successCallback,
                            const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void insertPayload(int id_unita,
                               const QString& data,
                               int ore_tot,
                               int ore_pagate,
                               int paga_diurno,
                               const std::function<void(bool)>& successCallback,
                               const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void resetTimeCard(const QString& tableName,
                               int doctorId,
                               const std::function<void(bool)>& successCallback,
                               const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void resetAllDoctorMods(const QString& tableName,
                                    int id,
                                    const std::function<void(bool)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void resetStringValue(const QString& tableName,
                                  const QString& columnName,
                                  int id,
                                  const std::function<void(bool)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void numDoctorsFromUnitInTimecard(const QString& timecard,
                                              int unitId,
                                              const std::function<void(int)>& successCallback,
                                              const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void numDoctorsInTimecard(const QString& timecard,
                                      const std::function<void(int)>& successCallback,
                                      const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getDoctorsIdsFromUnitInTimecard(const QString& timecard,
                                                 int unitId,
                                                 const std::function<void(const QVector<int>&)>& successCallback,
                                                 const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getOrePagateFromId(int id,
                                    const std::function<void(const QVariantList&)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getTuttiMatricoleNomi(const std::function<void(const QVector<DoctorData>&)>& successCallback,
                                       const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getUnitaDataTimecardAll(const std::function<void(const QVector<UnitaDataTimecard>&)>& successCallback,
                                         const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getUnitaDataById(int idUnita,
                                  const std::function<void(const UnitaData&)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getCompetenzePagate(int ci,
                                     int anno,
                                     int mese,
                                     const std::function<void(CompetenzePagate*)>& successCallback,
                                     const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void saveCompetenzePagate(CompetenzePagate* pagato,
                                      int anno,
                                      int mese,
                                      const std::function<void(bool)>& successCallback,
                                      const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getUnitaNomeCompleto(int id,
                                      const std::function<void(const QString&)>& successCallback,
                                      const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getDoctorName(int matricola,
                               const std::function<void(const QString&)>& successCallback,
                               const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getDoctorMatricola(int id,
                                    const std::function<void(int)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getIndennita(int anno,
                              int mese,
                              const std::function<void(Indennita*)>& successCallback,
                              const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void timeCardExists(const QString& tableName,
                                int doctorId,
                                const std::function<void(bool)>& successCallback,
                                const std::function<void(const QString&)>& errorCallback) = 0;

    virtual void getAllUnitaData(const std::function<void(const QVector<UnitaData>&)>& successCallback,
                                 const std::function<void(const QString&)>& errorCallback) = 0;

    // Aggiungiamo questa dichiarazione alla classe IDataProvider
    virtual void setUnitaMedico(int docId,
                                int unitId,
                                const std::function<void(bool)>& successCallback,
                                const std::function<void(const QString&)>& errorCallback) = 0;
};

#endif // IDATAPROVIDER_H
