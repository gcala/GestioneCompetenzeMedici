#include "dmpcompute.h"
#include "sqlqueries.h"
#include "competenza.h"
#include "utilities.h"

#include <QDebug>

DmpCompute::DmpCompute(QObject *parent)
    : QThread(parent)
{
    m_idUnita = -1;
    m_tableName.clear();
    m_idDirigente = -1;
}

DmpCompute::~DmpCompute()
{

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
    QStringList unitaIdList;

    int itemsCount = 0;

    QStringList timecards = SqlQueries::timecardsList();

    for(QString s: timecards) {
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
            for(QString card : timecards) {
                itemsCount += SqlQueries::numDoctorsFromUnitInTimecard(card, m_idUnita);
            }
            emit totalItems(itemsCount*timecards.count());
        } else {
            // ricalcolo deficit di tutti i medici di tutte le unità
            for(QString card : timecards) {
                itemsCount += SqlQueries::numDoctorsInTimecard(card);
            }
        }
    }

    int currItem = 0;
    int dmp = 0;
    if(m_idDirigente != -1) {
        for(int i = 0; i < (timecards.count()); i++) {
            if(SqlQueries::timeCardExists(timecards.at(i), QString::number(m_idDirigente))) {
                Competenza *competenza = new Competenza(timecards.at(i), m_idDirigente);
                if(i == 0) {
                    dmp = (competenza->differenzaMin() < 0 ? abs(competenza->differenzaMin()) : 0);
                    currItem++;
                    emit currentItem(currItem);
                    continue;
                }

                competenza->setDmpCalcolato(dmp);
                competenza->saveMods();

                dmp = competenza->differenzaMin() < 0 ? abs(competenza->differenzaMin()) : 0;
            } else {
                dmp = 0;
            }

            currItem++;
            emit currentItem(currItem);
        }
    }

//    int currRow = 0;

//    if(m_idDirigente != -1) {
//        currRow++;
//        emit currentItem(currRow);
//        QSqlQuery query;
//        query.prepare("SELECT id_unita FROM " + m_idMese + " WHERE id_medico='" + QString::number(m_idDirigente) + "';");
//        if(!query.exec()) {
//            qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
//        }
//        while(query.next()) {
//            m_idUnita = query.value(0).toInt();
//        }

//        m_unitaName = getUnitaName(QString::number(m_idUnita));
//        m_competenza = new Competenza(m_idMese,m_idDirigente);

//        // stampo un singolo report
//        printDirigente(painter);
//    } else {
//        foreach (QString unitaId, unitaIdList) {
//            m_unitaName = getUnitaName(unitaId);
//            m_idUnita = unitaId.toInt();
//            QStringList dirigentiIdList = getDirigentiIDs(unitaId);

//            foreach (QString dirigenteId, dirigentiIdList) {
//                currRow++;
//                emit currentItem(currRow);
//                if(!isFileStart)
//                    writer.newPage();
//                m_competenza = new Competenza(m_idMese,dirigenteId.toInt());
//                printDirigente(painter);
//                isFileStart = false;
//            }
//        }
//    }

    emit computeFinished();
}
