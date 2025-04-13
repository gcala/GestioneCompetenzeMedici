/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "dmpcompute.h"
//#include "sqlqueries.h"
#include "competenza.h"
#include "utilities.h"
#include "dipendente.h"
#include "sqldatabasemanager.h"
#include "apiservice.h"

#include <QSqlError>
#include <QDebug>

namespace The {
    static DmpCompute* s_DmpCompute_instance = nullptr;

    DmpCompute* dmpCompute()
    {
        if( !s_DmpCompute_instance )
            s_DmpCompute_instance = new DmpCompute();

        return s_DmpCompute_instance;
    }
}

DmpCompute::DmpCompute()
{
    m_idUnita = -1;
    m_tableName.clear();
    m_idDirigente = -1;
}

void DmpCompute::ricalcolaDmp(const QStringList &timecards, const int &idDirigente)
{
    int dmp = 0;
    for(int i = 0; i < (timecards.count()); i++) {
        if(ApiService::instance().timeCardExists(timecards.at(i), idDirigente)) {
            auto competenza = new Competenza(timecards.at(i), idDirigente);
            if(i == 0) {
                dmp = (competenza->minutiSaldoMese() < 0 ? abs(competenza->minutiSaldoMese()) : 0);
                m_currItem++;
                emit currentItem(m_currItem);
                continue;
            }

            competenza->setDmpCalcolato(dmp);
            competenza->saveMods();

            dmp = competenza->minutiSaldoMese() < 0 ? abs(competenza->minutiSaldoMese()) : 0;

            int anno = 0;
            int mese = 0;
            const auto matricola = ApiService::instance().getDoctorMatricola(idDirigente);
            Utilities::extractAnnoMeseFromTimecard(timecards.at(i), anno, mese);
            const QDate dataCorrente(anno,mese,1);
            if(dataCorrente >= Utilities::regolamentoOrario2025Date) {
                const auto saldo = competenza->saldoMinuti();
                const auto dmp = competenza->dmp();
                const auto rmp = competenza->dipendente()->minutiCausale("RMP");
                const auto idSituazioneSaldo = ApiService::instance().idSituazioneSaldo(anno, mese, matricola);
                if(idSituazioneSaldo > 0) {
                    ApiService::instance().updateSituazioneSaldo(idSituazioneSaldo, saldo, rmp, dmp);
                } else {
                    ApiService::instance().addSituazioneSaldo(anno, mese, matricola, saldo, rmp, dmp);
                }
            }
        } else {
            dmp = 0;
        }

        m_currItem++;
        emit currentItem(m_currItem);
    }
}

void DmpCompute::setUnita(const int &id)
{
    m_idUnita = id;
}

void DmpCompute::setTable(const QString &tableName)
{
    m_tableName = tableName;
}

void DmpCompute::setDirigente(const int &id)
{
    m_idDirigente = id;
}

void DmpCompute::run()
{
    Utilities::m_connectionName = "DmpCompute";

    bool ok = true;

    QSqlDatabase db = The::dbManager()->database(ok, Utilities::m_connectionName);

    if(!ok) {
        qDebug() << "Impossibile creare la connessione" << Utilities::m_connectionName;
        emit computeFinished();
        return;
    }

    if(!QSqlDatabase::connectionNames().contains(Utilities::m_connectionName)) {
        if (!db.open()) {
            qDebug() << QLatin1String("Impossibile connettersi al database.") << db.lastError().text();
            emit computeFinished();
            return;
        }
    }

    int itemsCount = 0;

    QStringList timecards = ApiService::instance().getTimecardsList();

    for(const QString &s: timecards) {
        if(s == m_tableName)
            break;
        timecards.removeOne(s);
    }

    if(timecards.count() <= 1) {
        qDebug() << Q_FUNC_INFO << "Numero di mesi da processare inferiore o uguale a 1. Esco...";
        emit computeFinished();
        return;
    }

    // ricavo quante operazioni saranno necessarie
    if(m_idDirigente != -1) {
        // ricalcolo deficit di un singolo medico
        itemsCount = timecards.count();
        emit totalItems(itemsCount);
    } else {
        for(const QString &card : timecards) {
            itemsCount += ApiService::instance().getDoctorsIdsFromUnitInTimecard(card, m_idUnita).count();
        }
        emit totalItems(itemsCount*timecards.count());
    }

    m_currItem = 0;
    if(m_idDirigente != -1) {
        ricalcolaDmp(timecards, m_idDirigente);
    } else {
        QVector<int> idsDirigenti = ApiService::instance().getDoctorsIdsFromUnitInTimecard(timecards.at(0), m_idUnita);
        for(int id : qAsConst(idsDirigenti)) {
            ricalcolaDmp(timecards, id);
        }
    }

    emit computeFinished();
}
