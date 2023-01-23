/*
    SPDX-FileCopyrightText: 2023 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "competenzeexporter.h"
#include "qsqlerror.h"
#include "utilities.h"
#include "sqlqueries.h"
#include "sqldatabasemanager.h"
#include "competenza.h"
#include "dipendente.h"

#include <QDate>
#include <QFile>
#include <QDir>

CompetenzeExporter::CompetenzeExporter(QObject *parent)
    : QThread{parent}
{
    m_idUnita = -1;
    m_matricole.clear();
}

void CompetenzeExporter::setPath(const QString &path)
{
    m_path = path;
}

void CompetenzeExporter::setUnita(int id)
{
    m_idUnita = id;
}

void CompetenzeExporter::setMatricole(const QVector<int> &matricole)
{
    m_matricole = matricole;
}

void CompetenzeExporter::setMeseDa(const QString &timecard)
{
    m_timecardDa = timecard;
}

void CompetenzeExporter::setMeseA(const QString &timecard)
{
    m_timecardA = timecard;
}

void CompetenzeExporter::run()
{
    Utilities::m_connectionName = "CompetenzeExporter";

    bool ok = true;

    QSqlDatabase db = The::dbManager()->database(ok, Utilities::m_connectionName);

    if(!ok) {
        qDebug() << "Impossibile creare la connessione" << Utilities::m_connectionName;
        emit exportFinished(QString());
        return;
    }

    if(!QSqlDatabase::connectionNames().contains(Utilities::m_connectionName)) {
        if (!db.open()) {
            qDebug() << QLatin1String("Impossibile connettersi al database.") << db.lastError().text();
            emit exportFinished(QString());
            return;
        }
    }

    const auto now = QDateTime::currentDateTime();

    QVector<int> unitaIdList;
    const QString s1 = m_timecardDa.split("_").last();
    m_daMonthYear = QDate::fromString(s1+"01", "yyyyMMdd");
    const QString s2 = m_timecardDa.split("_").last();
    m_aMonthYear = QDate::fromString(s2+"01", "yyyyMMdd");
//    QDate date = m_currentMonthYear.addDays(m_currentMonthYear.daysInMonth()-1);
//    QString mese = QLocale().monthName(date.month()) + " " + s.left(4);
//    QString pdfFileName = "Esporta_Competenze_";
    QString csvFileName = "Esporta_Competenze_";

    if(m_idUnita != -1) {
        unitaIdList << m_idUnita;
//        pdfFileName += "_" + QString::number(m_idUnita) + "_" + SqlQueries::getUnitaNomeBreve(m_idUnita);
        csvFileName += "_" + QString::number(m_idUnita) + "_" + SqlQueries::getUnitaNomeBreve(m_idUnita);
    } else {
        unitaIdList << SqlQueries::getUnitaIdsAll();
    }

    emit totalRows(unitaIdList.count());

//    pdfFileName += ".pdf";
    csvFileName += ".csv";

    QFile outFile(m_path + QDir::separator() + csvFileName);
    if(!outFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Impossibile aprire il file di destinazione:\n\n" + m_path + QDir::separator() + csvFileName;
        return;
    }

    QTextStream out(&outFile);

    out << "UNITA;" << "ANNO;" << "MESE;" << "MATRICOLA;" << "NOMINATIVO;"
        << "SALDO;" << "RESIDUO;" << "NON RECUP.;"
        << "STR.REPE.ORD;" << "STR.REPE.NOF;" << "STR.REPE.NEF;"
        << "STR.GUAR.ORD;" << "STR.GUAR.NOF;" << "STR.GUAR.NEF;"
        << "TURN.REPE;" << "ORE REPE;" << "GUA.DIU.;" << "GUA.NOT.;" << "GR.FES.;" << "NOTE"
        << "\n";

    int currRow = 0;

    QDate currentMonthYear = m_daMonthYear;

    while(currentMonthYear <= m_aMonthYear) {
        for(int unitaId : unitaIdList) {
            QVector<int> dirigentiIdList = SqlQueries::getDoctorsIdsFromUnitInTimecard("tc_" + currentMonthYear.toString("yyyyMM"), unitaId);
            if(dirigentiIdList.count() > 0) {
                for(int dirigenteId : dirigentiIdList) {
                    if(m_matricole.count() > 0) {
                        if(!m_matricole.contains(dirigenteId))
                            continue;
                    }
                    m_competenze = new Competenza("tc_" + currentMonthYear.toString("yyyyMM"), dirigenteId, true /* esportazione */);
                    double whole, fractional;
                    fractional = std::modf(m_competenze->repCount(), &whole);
                    out << SqlQueries::getUnitaNomeCompleto(unitaId) << ";"
                        << currentMonthYear.toString("yyyy") << ";"
                        << currentMonthYear.toString("MM") << ";"
                        << m_competenze->dipendente()->matricola() << ";"
                        << SqlQueries::doctorName(m_competenze->dipendente()->matricola()) << ";"
                        << Utilities::inOrario(m_competenze->differenzaMin()) << ";"
                        << Utilities::inOrario(m_competenze->residuoOreNonPagate()) << ";"
                        << m_competenze->residuoOreNonRecuperabili() << ";"
                        << m_competenze->numOreRep(Reperibilita::Ordinaria) << ";" // 70
                        << m_competenze->numOreRep(Reperibilita::FestivaONotturna) << ";" // 72
                        << m_competenze->numOreRep(Reperibilita::FestivaENotturna) << ";" // 71
                        << m_competenze->numOreGuarOrd() << ";" // 73
                        << m_competenze->numOreGuarFesONot() << ";" // 75
                        << m_competenze->numOreGuarFesENot() << ";" // 74
                        << whole << ";" // 25
                        << (fractional > 0.0 ? 6 : 0) << ";" // 40
                        << m_competenze->numGuarDiurne() << ";" // 1512
                        << m_competenze->numGuar() + m_competenze->numGuarGFNonPag() << ";" // 1571
                        << m_competenze->numGrFestPagabili() << ";" // 921
                        << m_competenze->note()
                        << "\n";
                }

            }
        }
        currentMonthYear = currentMonthYear.addMonths(1);
    }

    outFile.close();

    emit exportFinished(m_path + QDir::separator() + csvFileName);
}
