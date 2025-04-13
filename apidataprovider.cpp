#include "apidataprovider.h"
#include "reperibilitasemplificata.h"
#include "dipendente.h"
#include "utilities.h"
#include "competenzepagate.h"
#include "indennita.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>

ApiDataProvider::ApiDataProvider(const QString& baseUrl, QObject* parent)
    : IDataProvider(parent)
    , m_baseUrl(baseUrl)
{
    m_networkManager = new QNetworkAccessManager(this);
}

void ApiDataProvider::makeRequest(const QString& endpoint,
                             const std::function<void(const QJsonDocument&)>& successCallback,
                             const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    request.setUrl(QUrl(m_baseUrl + endpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        successCallback(jsonDoc);
    });
}

void ApiDataProvider::handleNetworkError(QNetworkReply* reply,
                                       const std::function<void(const QString&)>& errorCallback)
{
    if (errorCallback) {
        QString errorString;

        switch (reply->error()) {
            case QNetworkReply::ConnectionRefusedError:
                errorString = "La connessione al server è stata rifiutata";
                break;
            case QNetworkReply::RemoteHostClosedError:
                errorString = "Il server ha chiuso la connessione";
                break;
            case QNetworkReply::HostNotFoundError:
                errorString = "Server non trovato";
                break;
            case QNetworkReply::TimeoutError:
                errorString = "Timeout della connessione";
                break;
            case QNetworkReply::OperationCanceledError:
                errorString = "Operazione annullata";
                break;
            case QNetworkReply::SslHandshakeFailedError:
                errorString = "Errore SSL nella connessione";
                break;
            case QNetworkReply::TemporaryNetworkFailureError:
                errorString = "Errore temporaneo di rete";
                break;
            case QNetworkReply::NetworkSessionFailedError:
                errorString = "Sessione di rete fallita";
                break;
            case QNetworkReply::BackgroundRequestNotAllowedError:
                errorString = "Richiesta in background non permessa";
                break;
            case QNetworkReply::ContentNotFoundError:
                errorString = "Contenuto non trovato sul server";
                break;
            case QNetworkReply::AuthenticationRequiredError:
                errorString = "Autenticazione richiesta";
                break;
            case QNetworkReply::ContentAccessDenied:
                errorString = "Accesso al contenuto negato";
                break;
            case QNetworkReply::InternalServerError:
                errorString = "Errore interno del server";
                break;
            case QNetworkReply::ServiceUnavailableError:
                errorString = "Servizio non disponibile";
                break;
            default:
                errorString = "Errore di rete: " + reply->errorString();
        }

        QJsonDocument errorDoc = QJsonDocument::fromJson(reply->readAll());
        if (!errorDoc.isNull() && errorDoc.isObject()) {
            QJsonObject errorObj = errorDoc.object();
            if (errorObj.contains("message")) {
                errorString += "\nMessaggio dal server: " + errorObj["message"].toString();
            }
        }

        errorCallback(errorString);
    }
}

void ApiDataProvider::getUnits(const std::function<void(const QMap<int, QString>&)>& successCallback,
                             const std::function<void(const QString&)>& errorCallback)
{
    makeRequest("/units/read.php",
        [successCallback](const QJsonDocument& doc) {
            QMap<int, QString> unita;
            if (doc.isArray()) {
                QJsonArray array = doc.array();
                for (const QJsonValue& value : array) {
                    QJsonObject obj = value.toObject();
                    unita[obj["id"].toInt()] = obj["nome"].toString();
                }
            }
            successCallback(unita);
        },
        errorCallback);
}

void ApiDataProvider::getDoctorsFromUnit(int unitId,
                                       const std::function<void(const QMap<int, QString>&)>& successCallback,
                                       const std::function<void(const QString&)>& errorCallback)
{
    makeRequest(QString("/doctors/read.php?unit_id=%1").arg(unitId),
        [successCallback](const QJsonDocument& doc) {
            QMap<int, QString> doctors;
            if (doc.isArray()) {
                QJsonArray array = doc.array();
                for (const QJsonValue& value : array) {
                    QJsonObject obj = value.toObject();
                    doctors[obj["id"].toInt()] = obj["nome"].toString();
                }
            }
            successCallback(doctors);
        },
        errorCallback);
}

void ApiDataProvider::getTimecardsList(const std::function<void(const QStringList&)>& successCallback,
                                       const std::function<void(const QString&)>& errorCallback)
{
    makeRequest("/timecards/list.php",
                [successCallback](const QJsonDocument& doc) {
                    QStringList tables;
                    if (doc.isArray()) {
                        QJsonArray array = doc.array();
                        for (const QJsonValue& value : array) {
                            tables << value.toString();
                        }
                    }
                    successCallback(tables);
                },
                errorCallback);
}

void ApiDataProvider::getUnitaDataFromTimecard(const QString& timecard,
                                               const std::function<void(const QVector<UnitaDataTimecard>&)>& successCallback,
                                               const std::function<void(const QString&)>& errorCallback)
{
    makeRequest(QString("/units/from_timecard.php?timecard=%1").arg(timecard),
                [successCallback](const QJsonDocument& doc) {
                    QVector<UnitaDataTimecard> result;
                    if (doc.isArray()) {
                        QJsonArray array = doc.array();
                        for (const QJsonValue& value : array) {
                            QJsonObject obj = value.toObject();
                            UnitaDataTimecard unita;
                            unita.idUnita = obj["id_unita"].toInt();
                            unita.nome = obj["nome"].toString();
                            unita.id = obj["id"].toInt();
                            result.append(unita);
                        }
                    }
                    successCallback(result);
                },
                errorCallback);
}

void ApiDataProvider::getDoctorDataFromUnitaInTimecard(const QString& timecard,
                                                       int idUnita,
                                                       const std::function<void(const QVector<DoctorData>&)>& successCallback,
                                                       const std::function<void(const QString&)>& errorCallback)
{
    QString endpoint = QString("/doctors/from_unit_timecard.php?timecard=%1").arg(timecard);

    // Aggiungi il parametro idUnita solo se diverso da -1
    if (idUnita != -1) {
        endpoint += QString("&unit_id=%1").arg(idUnita);
    }

    makeRequest(endpoint,
                [successCallback](const QJsonDocument& doc) {
                    QVector<DoctorData> result;
                    if (doc.isArray()) {
                        QJsonArray array = doc.array();
                        for (const QJsonValue& value : array) {
                            QJsonObject obj = value.toObject();
                            DoctorData doctor;
                            doctor.id = obj["id"].toInt();
                            doctor.matricola = obj["matricola"].toInt();
                            doctor.nome = obj["nome"].toString();
                            result.append(doctor);
                        }
                    }
                    successCallback(result);
                },
                errorCallback);
}

void ApiDataProvider::getDoctorTimecard(const QString& tableName,
                                     const QString& modTableName,
                                     int doctorId,
                                     const std::function<void(const QVariantList&)>& successCallback,
                                     const std::function<void(const QString&)>& errorCallback)
{
    makeRequest(QString("/timecard/doctor_timecard.php?tableName=%1&modTableName=%2&doctorId=%3")
                  .arg(tableName)
                  .arg(modTableName)
                  .arg(doctorId),
                [successCallback](const QJsonDocument& doc) {
                    QVariantList result;
                    if (doc.isArray()) {
                        QJsonArray array = doc.array();
                        for (int i = 0; i < array.size(); i++) {
                            result << array[i].toVariant();
                        }
                    }
                    successCallback(result);
                },
                errorCallback);
}

void ApiDataProvider::getOrePagateFromUnit(int unitaId,
                                        const std::function<void(const OrePagateMap&)>& successCallback,
                                        const std::function<void(const QString&)>& errorCallback)
{
    makeRequest(QString("/unit/ore_pagate.php?unitaId=%1").arg(unitaId),
                [successCallback](const QJsonDocument& doc) {
                    OrePagateMap result;
                    if (doc.isArray()) {
                        QJsonArray array = doc.array();
                        for (const QJsonValue& value : array) {
                            QJsonObject obj = value.toObject();
                            QString data = obj["data"].toString();
                            QStringList meseAnno = data.split("/");
                            QDate date(meseAnno.at(1).toInt(), meseAnno.at(0).toInt(), 1);

                            OrePagate ore(
                                obj["ore_tot"].toInt(),
                                obj["ore_pagate"].toInt(),
                                obj["paga_diurno"].toInt(),
                                obj["paga_ore_lavorate"].toInt()
                            );

                            result[date] = ore;
                        }
                    }
                    successCallback(result);
                },
                errorCallback);
}

void ApiDataProvider::getRecuperiMeseSuccessivo(int anno,
                                             int mese,
                                             int doctorId,
                                             const std::function<void(const QPair<int, int>&)>& successCallback,
                                             const std::function<void(const QString&)>& errorCallback)
{
    makeRequest(QString("/timecard/recuperi_mese_successivo.php?anno=%1&mese=%2&doctorId=%3")
                  .arg(anno)
                  .arg(mese)
                  .arg(doctorId),
                [successCallback](const QJsonDocument& doc) {
                    QPair<int, int> result(0, 0);

                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        result.first = obj["minuti_giornalieri"].toInt();
                        result.second = obj["rmp_count"].toInt();
                    }

                    successCallback(result);
                },
                errorCallback);
}

void ApiDataProvider::isAbilitatoStraordinario(int matricola,
                                            int anno,
                                            int mese,
                                            const std::function<void(bool)>& successCallback,
                                            const std::function<void(const QString&)>& errorCallback)
{
    makeRequest(QString("/doctor/abilitato_straordinario.php?matricola=%1&anno=%2&mese=%3")
                  .arg(matricola)
                  .arg(anno)
                  .arg(mese),
                [successCallback](const QJsonDocument& doc) {
                    bool abilitato = true; // Default: abilitato

                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        abilitato = obj["abilitato"].toBool();
                    }

                    successCallback(abilitato);
                },
                errorCallback);
}

void ApiDataProvider::getReperibilitaSemplificata(int idUnita,
                                                  int anno,
                                                  int mese,
                                                  const std::function<void(ReperibilitaSemplificata*)>& successCallback,
                                                  const std::function<void(const QString&)>& errorCallback)
{
    // Se i parametri non sono validi, ritorna un oggetto vuoto
    if (idUnita == 0 || anno == 0 || mese == 0) {
        successCallback(new ReperibilitaSemplificata());
        return;
    }

    // Crea la data di decorrenza nel formato YYYYMMDD
    QDate decorrenzaDate(anno, mese, 1);
    QString decorrenza = decorrenzaDate.toString("yyyyMMdd");

    makeRequest(QString("/reperibilita/semplificata.php?idUnita=%1&decorrenza=%2")
                    .arg(idUnita)
                    .arg(decorrenza),
                [successCallback, idUnita, anno, mese](const QJsonDocument& doc) {
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();

                        // Crea l'oggetto ReperibilitaSemplificata dai dati JSON
                        ReperibilitaSemplificata* rep = new ReperibilitaSemplificata(
                            idUnita,
                            QDate(anno, mese, 1),
                            obj["feriale"].toDouble(),
                            obj["sabato"].toDouble(),
                            obj["festivo"].toDouble(),
                            obj["prefestivo"].toBool()
                            );

                        successCallback(rep);
                    } else {
                        // Se la risposta non è un oggetto valido, restituisci un oggetto vuoto
                        successCallback(new ReperibilitaSemplificata());
                    }
                },
                [successCallback, errorCallback](const QString& error) {
                    if (errorCallback) {
                        errorCallback(error);
                    } else if (successCallback) {
                        // In caso di errore, restituisci un oggetto vuoto
                        successCallback(new ReperibilitaSemplificata());
                    }
                });
}

void ApiDataProvider::abilitaDisabilitaStraordinario(int matricola,
                                                     bool enable,
                                                     int anno,
                                                     int mese,
                                                     const std::function<void(bool)>& successCallback,
                                                     const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QString endpoint = QString("/doctor/abilita_straordinario.php?matricola=%1&anno=%2&mese=%3&abilitato=%4")
                           .arg(matricola)
                           .arg(anno)
                           .arg(mese)
                           .arg(enable ? 1 : 0);

    request.setUrl(QUrl(m_baseUrl + endpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool();

        successCallback(success);
    });
}

void ApiDataProvider::idSituazioneSaldo(int anno,
                                        int mese,
                                        int matricola,
                                        const std::function<void(int)>& successCallback,
                                        const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QString endpoint = QString("/situazione_saldo/get_id.php?anno=%1&mese=%2&matricola=%3")
                           .arg(anno)
                           .arg(mese)
                           .arg(matricola);

    request.setUrl(QUrl(m_baseUrl + endpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray data = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        int id = obj["id"].toInt(0);

        successCallback(id);
    });
}

void ApiDataProvider::addSituazioneSaldo(int anno,
                                         int mese,
                                         int matricola,
                                         int saldo,
                                         int rmp,
                                         int dmp,
                                         const std::function<void(bool)>& successCallback,
                                         const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/situazione_saldo/add.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject requestData;
    requestData["anno"] = anno;
    requestData["mese"] = mese;
    requestData["matricola"] = matricola;
    requestData["saldo"] = saldo;
    requestData["rmp"] = rmp;
    requestData["dmp"] = dmp;

    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::updateSituazioneSaldo(int id,
                                            int saldo,
                                            int rmp,
                                            int dmp,
                                            const std::function<void(bool)>& successCallback,
                                            const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/situazione_saldo/update.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject requestData;
    requestData["id"] = id;
    requestData["saldo"] = saldo;
    requestData["rmp"] = rmp;
    requestData["dmp"] = dmp;

    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::saveMod(const QString& tableName,
                              const QString& columnName,
                              int id,
                              const QVariant& value,
                              const std::function<void(bool)>& successCallback,
                              const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/timecard/save_mod.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Preparazione dei dati da inviare
    QJsonObject requestData;
    requestData["tableName"] = tableName;
    requestData["columnName"] = columnName;
    requestData["id"] = id;

    // Gestione dei tipi di QVariant
    if (value.type() == QVariant::Int || value.type() == QVariant::LongLong) {
        requestData["value"] = value.toInt();
    } else if (value.type() == QVariant::Bool) {
        requestData["value"] = value.toBool();
    } else if (value.type() == QVariant::String) {
        requestData["value"] = value.toString();
    } else if (value.type() == QVariant::Double) {
        requestData["value"] = value.toDouble();
    } else if (value.isNull()) {
        requestData["value"] = QJsonValue::Null;
    } else {
        // Per altri tipi, convertiamo a stringa
        requestData["value"] = value.toString();
    }

    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::tableExists(const QString& tableName,
                                  const std::function<void(bool)>& successCallback,
                                  const std::function<void(const QString&)>& errorCallback)
{
    makeRequest(QString("/table/exists.php?tableName=%1").arg(tableName),
                [successCallback](const QJsonDocument& doc) {
                    bool exists = false;
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        exists = obj["exists"].toBool();
                    }
                    successCallback(exists);
                },
                errorCallback);
}

void ApiDataProvider::createTimeCardsTable(const QString& tableName,
                                           const std::function<void(bool)>& successCallback,
                                           const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/table/create_timecards.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject requestData;
    requestData["tableName"] = tableName;

    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::createPagatoTable(int anno,
                                        int mese,
                                        const std::function<void(bool)>& successCallback,
                                        const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/table/create_pagato.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject requestData;
    requestData["anno"] = anno;
    requestData["mese"] = mese;

    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::unitId(int matricola,
                             const std::function<void(int)>& successCallback,
                             const std::function<void(const QString&)>& errorCallback)
{
    makeRequest(QString("/doctor/unit_id.php?matricola=%1").arg(matricola),
                [successCallback](const QJsonDocument& doc) {
                    int id = -1;
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        id = obj["unitId"].toInt(-1);
                    }
                    successCallback(id);
                },
                errorCallback);
}

void ApiDataProvider::addTimeCard(const QString& tableName,
                                const Dipendente* dipendente,
                                const std::function<void(bool)>& successCallback,
                                const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/timecard/add_timecard.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject requestData;
    requestData["tableName"] = tableName;
    requestData["anno"] = dipendente->anno();
    requestData["mese"] = dipendente->mese();
    requestData["matricola"] = dipendente->matricola();
    requestData["nome"] = dipendente->nome();
    requestData["unita"] = dipendente->unita();
    requestData["riposi"] = QJsonArray::fromStringList(Utilities::vectorIntToStringlist(dipendente->riposi()));
    requestData["minuti_giornalieri"] = dipendente->minutiGiornalieriVeri();

    if(dipendente->minutiGiornalieriVeri() <= Utilities::m_maxMinutiGiornalieri)
        requestData["ferie"] = QJsonArray::fromStringList(Utilities::vectorIntToStringlist(dipendente->ferie()));
    else
        requestData["ferie"] = dipendente->numGiorniCartellino();

    requestData["congedi"] = QJsonArray::fromStringList(Utilities::vectorIntToStringlist(dipendente->congedi()));
    requestData["malattia"] = QJsonArray::fromStringList(Utilities::vectorIntToStringlist(dipendente->malattia()));
    requestData["rmp"] = QJsonArray::fromStringList(Utilities::vectorIntToStringlist(dipendente->rmp()));
    requestData["rmc"] = QJsonArray::fromStringList(Utilities::vectorIntToStringlist(dipendente->rmc()));

    // Serialize altre_causali
    QJsonObject altreCausali;
    auto i = dipendente->altreCausali().constBegin();
    while (i != dipendente->altreCausali().constEnd()) {
        QJsonObject causaleObj;
        causaleObj["giorni"] = QJsonArray::fromStringList(Utilities::vectorIntToStringlist(i.value().first));
        causaleObj["minuti"] = i.value().second;
        altreCausali[i.key()] = causaleObj;
        ++i;
    }
    requestData["altre_causali"] = altreCausali;

    // Serialize guardie_diurne
    QJsonArray guardieDiurne = QJsonArray::fromStringList(Utilities::vectorIntToStringlist(dipendente->guardieDiurne()));
    requestData["guardie_diurne"] = guardieDiurne;

    // Serialize mezze_guardie_diurne
    QJsonArray mezzeGuardieDiurne = QJsonArray::fromStringList(Utilities::vectorIntToStringlist(dipendente->mezzeGuardieDiurne()));
    requestData["mezze_guardie_diurne"] = mezzeGuardieDiurne;

    // Serialize guardie_notturne
    requestData["guardie_notturne"] = QJsonArray::fromStringList(Utilities::vectorIntToStringlist(dipendente->guardieNotturne()));

    // Serialize grep
    QJsonObject grep;
    auto it = dipendente->grep().constBegin();
    while (it != dipendente->grep().constEnd()) {
        QJsonObject grepItem;
        grepItem["minuti"] = it.value().first;
        grepItem["tipo"] = it.value().second;
        grep[QString::number(it.key())] = grepItem;
        ++it;
    }
    requestData["grep"] = grep;

    requestData["congedi_minuti"] = dipendente->minutiCongedi();
    requestData["eccr_minuti"] = dipendente->minutiEccr();
    requestData["grep_minuti"] = dipendente->minutiGrep();
    requestData["guar_minuti"] = dipendente->minutiGuar();
    requestData["rmc_minuti"] = dipendente->minutiRmc();
    requestData["minuti_fatti"] = dipendente->minutiFatti();
    requestData["scoperti"] = QJsonArray::fromStringList(Utilities::vectorIntToStringlist(dipendente->scoperti()));
    requestData["indennita_festiva"] = dipendente->indennitaFestiva();
    requestData["indennita_notturna"] = QJsonArray::fromStringList(Utilities::vectorIntToStringlist(dipendente->indennitaNotturna()));

    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::doctorId(int matricola,
                               const std::function<void(int)>& successCallback,
                               const std::function<void(const QString&)>& errorCallback)
{
    makeRequest(QString("/doctor/doctor_id.php?matricola=%1").arg(matricola),
                [successCallback](const QJsonDocument& doc) {
                    int id = -1;
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        id = obj["doctorId"].toInt(-1);
                    }
                    successCallback(id);
                },
                errorCallback);
}

void ApiDataProvider::editUnit(const QString& id,
                               const QString& raggruppamento,
                               const QString& nome,
                               const QString& breve,
                               const std::function<void(bool)>& successCallback,
                               const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/units/edit.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject requestData;
    requestData["id"] = id;
    requestData["raggruppamento"] = raggruppamento;
    requestData["nome"] = nome;
    requestData["breve"] = breve;

    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::editDoctor(const QString& id,
                                 const QString& matricola,
                                 const QString& nome,
                                 const std::function<void(bool)>& successCallback,
                                 const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/doctor/edit.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Preparazione dei dati da inviare
    QJsonObject requestData;
    requestData["id"] = id;
    requestData["matricola"] = matricola;
    requestData["nome"] = nome;

    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::insertPayload(int id_unita,
                                    const QString& data,
                                    int ore_tot,
                                    int ore_pagate,
                                    int paga_diurno,
                                    const std::function<void(bool)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/unit/insert_payload.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Preparazione dei dati da inviare
    QJsonObject requestData;
    requestData["id_unita"] = id_unita;
    requestData["data"] = data;
    requestData["ore_tot"] = ore_tot;
    requestData["ore_pagate"] = ore_pagate;
    requestData["paga_diurno"] = paga_diurno;

    QJsonDocument doc(requestData);
    QByteArray jsonData = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, jsonData);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::resetTimeCard(const QString& tableName,
                                    int doctorId,
                                    const std::function<void(bool)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/timecard/reset.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Preparazione dei dati da inviare
    QJsonObject requestData;
    requestData["tableName"] = tableName;
    requestData["doctorId"] = doctorId;

    QJsonDocument doc(requestData);
    QByteArray jsonData = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, jsonData);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::resetAllDoctorMods(const QString& tableName,
                                         int id,
                                         const std::function<void(bool)>& successCallback,
                                         const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/doctor/reset_mods.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Preparazione dei dati da inviare
    QJsonObject requestData;
    requestData["tableName"] = tableName;
    requestData["doctorId"] = id;

    QJsonDocument doc(requestData);
    QByteArray jsonData = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, jsonData);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::resetStringValue(const QString& tableName,
                                       const QString& columnName,
                                       int id,
                                       const std::function<void(bool)>& successCallback,
                                       const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/timecard/reset_string_value.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Preparazione dei dati da inviare
    QJsonObject requestData;
    requestData["tableName"] = tableName;
    requestData["columnName"] = columnName;
    requestData["doctorId"] = id;

    QJsonDocument doc(requestData);
    QByteArray jsonData = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, jsonData);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::numDoctorsFromUnitInTimecard(const QString& timecard,
                                                   int unitId,
                                                   const std::function<void(int)>& successCallback,
                                                   const std::function<void(const QString&)>& errorCallback)
{
    QString endpoint = QString("/timecard/num_doctors_from_unit.php?timecard=%1&unitId=%2")
    .arg(timecard)
        .arg(unitId);

    makeRequest(endpoint,
                [successCallback](const QJsonDocument& doc) {
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        int count = obj["count"].toInt(0);
                        successCallback(count);
                    } else {
                        successCallback(0);
                    }
                },
                errorCallback);
}

void ApiDataProvider::numDoctorsInTimecard(const QString& timecard,
                                           const std::function<void(int)>& successCallback,
                                           const std::function<void(const QString&)>& errorCallback)
{
    QString endpoint = QString("/timecard/num_doctors.php?timecard=%1")
    .arg(timecard);

    makeRequest(endpoint,
                [successCallback](const QJsonDocument& doc) {
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        int count = obj["count"].toInt(0);
                        successCallback(count);
                    } else {
                        successCallback(0);
                    }
                },
                errorCallback);
}

void ApiDataProvider::getDoctorsIdsFromUnitInTimecard(const QString& timecard,
                                                      int unitId,
                                                      const std::function<void(const QVector<int>&)>& successCallback,
                                                      const std::function<void(const QString&)>& errorCallback)
{
    QString endpoint = QString("/timecard/doctors_ids_from_unit.php?timecard=%1")
    .arg(timecard);

    // Aggiungi il parametro unitId solo se diverso da -1
    if (unitId != -1) {
        endpoint += QString("&unitId=%1").arg(unitId);
    }

    makeRequest(endpoint,
                [successCallback](const QJsonDocument& doc) {
                    QVector<int> ids;
                    if (doc.isArray()) {
                        QJsonArray array = doc.array();
                        for (const QJsonValue& value : array) {
                            ids << value.toInt();
                        }
                    }
                    successCallback(ids);
                },
                errorCallback);
}

void ApiDataProvider::getOrePagateFromId(int id,
                                         const std::function<void(const QVariantList&)>& successCallback,
                                         const std::function<void(const QString&)>& errorCallback)
{
    QString endpoint = QString("/unit/ore_pagate_by_id.php?id=%1")
    .arg(id);

    makeRequest(endpoint,
                [successCallback](const QJsonDocument& doc) {
                    QVariantList result;
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        if (obj.contains("data")) {
                            result << obj["data"].toString();
                            result << obj["ore_tot"].toInt();
                            result << obj["ore_pagate"].toInt();
                            result << obj["paga_diurno"].toInt();
                        }
                    }
                    successCallback(result);
                },
                errorCallback);
}

void ApiDataProvider::getTuttiMatricoleNomi(const std::function<void(const QVector<DoctorData>&)>& successCallback,
                                            const std::function<void(const QString&)>& errorCallback)
{
    makeRequest("/doctors/get_all.php",
                [successCallback](const QJsonDocument& doc) {
                    QVector<DoctorData> result;
                    if (doc.isArray()) {
                        QJsonArray array = doc.array();
                        for (const QJsonValue& value : array) {
                            QJsonObject obj = value.toObject();
                            DoctorData doctor;
                            doctor.id = obj["id"].toInt();
                            doctor.matricola = obj["matricola"].toInt();
                            doctor.nome = obj["nome"].toString();
                            result.append(doctor);
                        }
                    }
                    successCallback(result);
                },
                errorCallback);
}

void ApiDataProvider::getUnitaDataTimecardAll(const std::function<void(const QVector<UnitaDataTimecard>&)>& successCallback,
                                               const std::function<void(const QString&)>& errorCallback)
{
    makeRequest("/timecard/get_all_unit_data.php",
                [successCallback](const QJsonDocument& doc) {
                    QVector<UnitaDataTimecard> result;
                    if (doc.isArray()) {
                        QJsonArray array = doc.array();
                        for (const QJsonValue& value : array) {
                            QJsonObject obj = value.toObject();
                            UnitaDataTimecard unita;
                            unita.id = obj["id"].toInt();
                            unita.idUnita = obj["id"].toInt(); // Assumiamo che id e idUnita siano uguali in questo caso
                            unita.nome = obj["nome"].toString();
                            result.append(unita);
                        }
                    }
                    successCallback(result);
                },
                errorCallback);
}

void ApiDataProvider::getUnitaDataById(int idUnita,
                                       const std::function<void(const UnitaData&)>& successCallback,
                                       const std::function<void(const QString&)>& errorCallback)
{
    QString endpoint = QString("/units/get_by_id.php?id=%1")
    .arg(idUnita);

    makeRequest(endpoint,
                [successCallback](const QJsonDocument& doc) {
                    UnitaData unita;
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        unita.id = obj["id"].toInt();
                        unita.raggruppamento = obj["raggruppamento"].toString();
                        unita.nome = obj["nome"].toString();
                        unita.breve = obj["breve"].toString();
                        unita.pseudo = obj["pseudo"].toString();
                    }
                    successCallback(unita);
                },
                errorCallback);
}

void ApiDataProvider::getCompetenzePagate(int ci,
                                          int anno,
                                          int mese,
                                          const std::function<void(CompetenzePagate*)>& successCallback,
                                          const std::function<void(const QString&)>& errorCallback)
{
    // Se i parametri non sono validi, ritorna un oggetto vuoto
    if(ci == 0 || anno == 0 || mese == 0) {
        successCallback(new CompetenzePagate());
        return;
    }

    QString endpoint = QString("/competenze_pagate/read.php?ci=%1&anno=%2&mese=%3")
                           .arg(ci)
                           .arg(anno)
                           .arg(mese);

    makeRequest(endpoint,
                [successCallback](const QJsonDocument& doc) {
                    CompetenzePagate* pagato = new CompetenzePagate();

                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();

                        pagato->setCi(obj["ci"].toInt());
                        pagato->setDeficit(obj["deficit"].toInt());
                        pagato->setIndNotturna(obj["ind_not"].toInt());
                        pagato->setIndFestiva(obj["ind_fes"].toDouble());
                        pagato->setStr_reparto_ord(obj["str_reparto_ord"].toInt());
                        pagato->setStr_reparto_nof(obj["str_reparto_nof"].toInt());
                        pagato->setStr_reparto_nef(obj["str_reparto_nef"].toInt());
                        pagato->setStr_repe_ord(obj["str_repe_ord"].toInt());
                        pagato->setStr_repe_nof(obj["str_repe_nof"].toInt());
                        pagato->setStr_repe_nef(obj["str_repe_nef"].toInt());
                        pagato->setStr_guard_ord(obj["str_guard_ord"].toInt());
                        pagato->setStr_guard_nof(obj["str_guard_nof"].toInt());
                        pagato->setStr_guard_nef(obj["str_guard_nef"].toInt());
                        pagato->setTurni_repe(obj["turni_repe"].toInt());
                        pagato->setOre_repe(obj["ore_repe"].toInt());
                        pagato->setGuard_diu(obj["guard_diu"].toDouble());
                        pagato->setGuard_not(obj["guard_not"].toInt());
                        pagato->setGrande_fes(obj["grande_fes"].toInt());
                        pagato->setDateTime(QDateTime::fromString(obj["data"].toString(), "yyyyMMddhhmm"));
                        pagato->setRepOltre10(obj["rep_oltre_10"].toDouble());
                        pagato->setTeleconsulto(obj["teleconsulto"].toDouble());
                        pagato->setOreLavorate(obj["ore_lavorate"].toDouble());
                    }

                    successCallback(pagato);
                },
                [successCallback, errorCallback](const QString& error) {
                    if (errorCallback) {
                        errorCallback(error);
                    } else {
                        // In caso di errore, restituiamo un oggetto vuoto
                        successCallback(new CompetenzePagate());
                    }
                });
}

void ApiDataProvider::saveCompetenzePagate(CompetenzePagate* pagato,
                                           int anno,
                                           int mese,
                                           const std::function<void(bool)>& successCallback,
                                           const std::function<void(const QString&)>& errorCallback)
{
    if(pagato->ci() == 0 || anno == 0 || mese == 0) {
        if (errorCallback) {
            errorCallback("Invalid parameters: pagato->ci() or anno or mese is zero");
        }
        return;
    }

    QNetworkRequest request;
    QUrl url(m_baseUrl + "/competenze_pagate/save.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Preparazione dei dati da inviare
    QJsonObject requestData;
    requestData["anno"] = anno;
    requestData["mese"] = mese;
    requestData["ci"] = pagato->ci();
    requestData["deficit"] = pagato->deficit();
    requestData["ind_not"] = pagato->indNotturna();
    requestData["ind_fes"] = pagato->indFestiva();
    requestData["str_reparto_ord"] = pagato->str_reparto_ord();
    requestData["str_reparto_nof"] = pagato->str_reparto_nof();
    requestData["str_reparto_nef"] = pagato->str_reparto_nef();
    requestData["str_repe_ord"] = pagato->str_repe_ord();
    requestData["str_repe_nof"] = pagato->str_repe_nof();
    requestData["str_repe_nef"] = pagato->str_repe_nef();
    requestData["str_guard_ord"] = pagato->str_guard_ord();
    requestData["str_guard_nof"] = pagato->str_guard_nof();
    requestData["str_guard_nef"] = pagato->str_guard_nef();
    requestData["turni_repe"] = pagato->turni_repe();
    requestData["ore_repe"] = pagato->ore_repe();
    requestData["guard_diu"] = pagato->guard_diu();
    requestData["guard_not"] = pagato->guard_not();
    requestData["grande_fes"] = pagato->grande_fes();
    requestData["data"] = pagato->dataElaborazione().toString("yyyyMMddhhmm");
    requestData["rep_oltre_10"] = pagato->repOltre10();
    requestData["teleconsulto"] = pagato->teleconsulto();
    requestData["ore_lavorate"] = pagato->oreLavorate();

    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}

void ApiDataProvider::getUnitaNomeCompleto(int id,
                                           const std::function<void(const QString&)>& successCallback,
                                           const std::function<void(const QString&)>& errorCallback)
{
    QString endpoint = QString("/unit/get_nome_completo.php?id=%1").arg(id);

    makeRequest(endpoint,
                [successCallback](const QJsonDocument& doc) {
                    QString nome;
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        nome = obj["nome"].toString();
                    }
                    successCallback(nome);
                },
                errorCallback);
}

void ApiDataProvider::getDoctorName(int matricola,
                                    const std::function<void(const QString&)>& successCallback,
                                    const std::function<void(const QString&)>& errorCallback)
{
    QString endpoint = QString("/doctor/get_name.php?matricola=%1").arg(matricola);

    makeRequest(endpoint,
                [successCallback](const QJsonDocument& doc) {
                    QString name;
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        name = obj["nome"].toString();
                    }
                    successCallback(name);
                },
                errorCallback);
}

void ApiDataProvider::getDoctorMatricola(int id,
                                         const std::function<void(int)>& successCallback,
                                         const std::function<void(const QString&)>& errorCallback)
{
    QString endpoint = QString("/doctor/get_matricola.php?id=%1").arg(id);

    makeRequest(endpoint,
                [successCallback](const QJsonDocument& doc) {
                    int matricola = 0;
                    if (doc.isObject()) {
                        QJsonObject obj = doc.object();
                        matricola = obj["matricola"].toInt();
                    }
                    successCallback(matricola);
                },
                errorCallback);
}

void ApiDataProvider::getIndennita(int anno,
                                   int mese,
                                   const std::function<void(Indennita*)>& successCallback,
                                   const std::function<void(const QString&)>& errorCallback)
{
    QString endpoint = QString("/indennita/get.php?anno=%1&mese=%2")
    .arg(anno)
        .arg(mese);

    makeRequest(endpoint,
                [successCallback](const QJsonDocument& doc) {
                    Indennita* indennita = new Indennita();

                    if (doc.isArray()) {
                        QJsonArray array = doc.array();
                        for (const QJsonValue& value : array) {
                            QJsonObject obj = value.toObject();
                            indennita->addItem(
                                Utilities::indennitaEnum(obj["tipo"].toString()),
                                obj["id_unita"].toInt(),
                                obj["voce"].toString(),
                                obj["sub"].toString()
                                );
                        }
                    }

                    successCallback(indennita);
                },
                [successCallback, errorCallback](const QString& error) {
                    if (errorCallback) {
                        errorCallback(error);
                    } else {
                        // In caso di errore, restituiamo un oggetto vuoto
                        successCallback(new Indennita());
                    }
                });
}

void ApiDataProvider::timeCardExists(const QString& tableName,
                                     int doctorId,
                                     const std::function<void(bool)>& successCallback,
                                     const std::function<void(const QString&)>& errorCallback)
{
    makeRequest(QString("/timecard/exists.php?tableName=%1&doctorId=%2")
                    .arg(tableName)
                    .arg(doctorId),
                    [successCallback](const QJsonDocument& doc) {
                        bool exists = false;
                        if (doc.isObject()) {
                            QJsonObject obj = doc.object();
                            exists = obj["exists"].toBool();
                        }
                        successCallback(exists);
                    },
                    errorCallback);
}

void ApiDataProvider::getAllUnitaData(const std::function<void(const QVector<UnitaData>&)>& successCallback,
                                      const std::function<void(const QString&)>& errorCallback)
{
    makeRequest("/units/get_all.php",
                [successCallback](const QJsonDocument& doc) {
                    QVector<UnitaData> result;
                    if (doc.isArray()) {
                        QJsonArray array = doc.array();
                        for (const QJsonValue& value : array) {
                            QJsonObject obj = value.toObject();
                            UnitaData unita;
                            unita.id = obj["id"].toInt();
                            unita.raggruppamento = obj["raggruppamento"].toString();
                            unita.nome = obj["nome"].toString();
                            unita.breve = obj["breve"].toString();
                            unita.pseudo = obj["pseudo"].toString();
                            result.append(unita);
                        }
                    }
                    successCallback(result);
                },
                errorCallback);
}

void ApiDataProvider::setUnitaMedico(int docId,
                                     int unitId,
                                     const std::function<void(bool)>& successCallback,
                                     const std::function<void(const QString&)>& errorCallback)
{
    QNetworkRequest request;
    QUrl url(m_baseUrl + "/doctor/set_unit.php");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Preparazione dei dati da inviare
    QJsonObject requestData;
    requestData["docId"] = docId;
    requestData["unitId"] = unitId;

    QJsonDocument doc(requestData);
    QByteArray jsonData = doc.toJson();

    QNetworkReply* reply = m_networkManager->post(request, jsonData);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, errorCallback);
            return;
        }

        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (jsonDoc.isNull()) {
            if (errorCallback) {
                errorCallback("Invalid JSON response");
            }
            return;
        }

        QJsonObject obj = jsonDoc.object();
        bool success = obj["success"].toBool(false);

        successCallback(success);
    });
}
