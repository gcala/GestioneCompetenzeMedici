/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "dmpcompute.h"
#include "sqlqueries.h"
#include "competenza.h"
#include "utilities.h"
#include "sqldatabasemanager.h"

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
        if(SqlQueries::timeCardExists(timecards.at(i), idDirigente)) {
            auto competenza = new Competenza(timecards.at(i), idDirigente);
            if(i == 0) {
                dmp = (competenza->differenzaMin() < 0 ? abs(competenza->differenzaMin()) : 0);
                m_currItem++;
                emit currentItem(m_currItem);
                continue;
            }

            competenza->setDmpCalcolato(dmp);
            competenza->saveMods();

            dmp = competenza->differenzaMin() < 0 ? abs(competenza->differenzaMin()) : 0;
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

    QStringList timecards = SqlQueries::timecardsList();

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
        if(m_idUnita != -1) {
            // ricalcolo deficit dei medici di una unità
            for(const QString &card : timecards) {
                itemsCount += SqlQueries::numDoctorsFromUnitInTimecard(card, m_idUnita);
            }
            emit totalItems(itemsCount*timecards.count());
        } else {
            // ricalcolo deficit di tutti i medici di tutte le unità
            for(const QString &card : timecards) {
                itemsCount += SqlQueries::numDoctorsInTimecard(card);
            }
            emit totalItems(itemsCount*(timecards.count()-1));
        }
    }

    m_currItem = 0;
//    int dmp = 0;
    if(m_idDirigente != -1) {
        ricalcolaDmp(timecards, m_idDirigente);
    } else {
        if(m_idUnita != -1) {
            // ricalcolo deficit dei medici di una unità
            QVector<int> idsDirigenti = SqlQueries::getDoctorsIdsFromUnitInTimecard(timecards.at(0), m_idUnita);
            for(int id : idsDirigenti) {
                ricalcolaDmp(timecards, id);
            }
        } else {
            // ricalcolo deficit di tutti i medici di tutte le unità
            QVector<int> idsDirigenti = SqlQueries::getDoctorsIdsFromUnitInTimecard(timecards.at(0), -1);
            for(int id : idsDirigenti) {
                ricalcolaDmp(timecards, id);
            }
        }
    }

    emit computeFinished();
}
