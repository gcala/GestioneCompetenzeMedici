#ifndef APISERVICE_H
#define APISERVICE_H

#include "doctordata.h"
#include "unitadatatimecard.h"
#include "orepagate.h"
#include "unitadata.h"

#include <QString>
#include <QStringList>
#include <QVector>
#include <QPair>
#include <QMap>
#include <QDate>
#include <QVariantList>

class ReperibilitaSemplificata;
class Dipendente;
class CompetenzePagate;
class Indennita;

// Classe centralizzata che fornisce metodi sincroni per le chiamate API
class ApiService
{
public:
    // Singleton
    static ApiService& instance();

    // Metodi sincroni che internamente chiamano ApiManager in modo asincrono
    QStringList getTimecardsList(QString* errorMsg = nullptr);

    QMap<int, QString> getUnits(QString* errorMsg = nullptr);

    QMap<int, QString> getDoctorsFromUnit(int unitId, QString* errorMsg = nullptr);

    QVector<UnitaDataTimecard> getUnitaDataFromTimecard(const QString& timecard, QString* errorMsg = nullptr);

    QVector<DoctorData> getDoctorDataFromUnitaInTimecard(const QString& timecard, int idUnita, QString* errorMsg = nullptr);

    QVariantList getDoctorTimecard(const QString& tableName, const QString& modTableName, int doctorId, QString* errorMsg = nullptr);

    OrePagateMap getOrePagateFromUnit(int unitaId, QString* errorMsg = nullptr);

    QPair<int, int> getRecuperiMeseSuccessivo(int anno, int mese, int doctorId, QString* errorMsg = nullptr);

    bool isAbilitatoStraordinario(int matricola, int anno, int mese, QString* errorMsg = nullptr);

    ReperibilitaSemplificata* getReperibilitaSemplificata(int idUnita, int anno, int mese, QString* errorMsg = nullptr);

    bool abilitaDisabilitaStraordinario(int matricola, bool enable, int anno, int mese, QString* errorMsg = nullptr);

    int idSituazioneSaldo(int anno, int mese, int matricola, QString* errorMsg = nullptr);

    bool addSituazioneSaldo(int anno, int mese, int matricola, int saldo, int rmp, int dmp, QString* errorMsg = nullptr);

    bool updateSituazioneSaldo(int id, int saldo, int rmp, int dmp, QString* errorMsg = nullptr);

    bool saveMod(const QString& tableName, const QString& columnName, int id, const QVariant& value, QString* errorMsg = nullptr);

    bool tableExists(const QString& tableName, QString* errorMsg = nullptr);

    bool createTimeCardsTable(const QString& tableName, QString* errorMsg = nullptr);

    bool createPagatoTable(int anno, int mese, QString* errorMsg = nullptr);

    int unitId(int matricola, QString* errorMsg = nullptr);

    bool addTimeCard(const QString& tableName, const Dipendente* dipendente, QString* errorMsg = nullptr);

    int doctorId(int matricola, QString* errorMsg = nullptr);

    bool editUnit(const QString& id, const QString& raggruppamento, const QString& nome, const QString& breve, QString* errorMsg = nullptr);

    bool editDoctor(const QString& id, const QString& matricola, const QString& nome, QString* errorMsg = nullptr);

    bool insertPayload(int id_unita, const QString& data, int ore_tot, int ore_pagate, int paga_diurno, QString* errorMsg = nullptr);

    bool resetTimeCard(const QString& tableName, int doctorId, QString* errorMsg = nullptr);

    bool resetAllDoctorMods(const QString& tableName, int id, QString* errorMsg = nullptr);

    bool resetStringValue(const QString& tableName, const QString& columnName, int id, QString* errorMsg = nullptr);

    int numDoctorsFromUnitInTimecard(const QString& timecard, int unitId, QString* errorMsg = nullptr);

    int numDoctorsInTimecard(const QString& timecard, QString* errorMsg = nullptr);

    QVector<int> getDoctorsIdsFromUnitInTimecard(const QString& timecard, int unitId, QString* errorMsg = nullptr);

    QVariantList getOrePagateFromId(int id, QString* errorMsg = nullptr);

    QVector<DoctorData> getTuttiMatricoleNomi(QString* errorMsg = nullptr);

    QVector<UnitaDataTimecard> getUnitaDataTimecardAll(QString* errorMsg = nullptr);

    UnitaData getUnitaDataById(int idUnita, QString* errorMsg = nullptr);

    CompetenzePagate* getCompetenzePagate(int ci, int anno, int mese, QString* errorMsg = nullptr);

    bool saveCompetenzePagate(CompetenzePagate* pagato, int anno, int mese, QString* errorMsg = nullptr);

    QString getUnitaNomeCompleto(int id, QString* errorMsg = nullptr);

    QString getDoctorName(int matricola, QString* errorMsg = nullptr);

    int getDoctorMatricola(int id, QString* errorMsg = nullptr);

    Indennita* getIndennita(int anno, int mese, QString* errorMsg = nullptr);

    bool timeCardExists(const QString& tableName, int doctorId, QString* errorMsg = nullptr);

    QVector<UnitaData> getAllUnitaData(QString* errorMsg = nullptr);

    bool setUnitaMedico(int docId, int unitId, QString* errorMsg = nullptr);

private:
    ApiService() = default;
    ~ApiService() = default;

    // Disabilita costruttore di copia e operatore di assegnazione
    ApiService(const ApiService&) = delete;
    ApiService& operator=(const ApiService&) = delete;
};

#endif // APISERVICE_H
