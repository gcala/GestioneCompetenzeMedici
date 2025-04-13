/*
    SPDX-FileCopyrightText: 2023 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "competenzeexporter.h"
#include "qsqlerror.h"
#include "utilities.h"
//#include "sqlqueries.h"
#include "sqldatabasemanager.h"
#include "competenza.h"
#include "dipendente.h"
#include "apiservice.h"

#include <cmath>
#include <QDate>
#include <QFile>
#include <QDir>
#include <QDebug>


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
    const QString s2 = m_timecardA.split("_").last();
    m_aMonthYear = QDate::fromString(s2+"01", "yyyyMMdd");
//    QDate date = m_currentMonthYear.addDays(m_currentMonthYear.daysInMonth()-1);
//    QString mese = QLocale().monthName(date.month()) + " " + s.left(4);
//    QString pdfFileName = "Esporta_Competenze_";
    QString csvFileName = "Esporta_Competenze_";

    if(m_idUnita != -1) {
        unitaIdList << m_idUnita;
        csvFileName += "_" + QString::number(m_idUnita) + "_" + ApiService::instance().getUnitaDataById(m_idUnita).breve;
    } else {
        const auto units = ApiService::instance().getAllUnitaData();
        for(const auto u : units) {
            unitaIdList << u.id;
        }
    }

    emit totalRows(unitaIdList.count());

//    pdfFileName += ".pdf";
    csvFileName += "_" + now.toString("yyyyMMdd_hhmm") + ".csv";

    QFile outFile(m_path + QDir::separator() + csvFileName);
    if(!outFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Impossibile aprire il file di destinazione:\n\n" + m_path + QDir::separator() + csvFileName;
        return;
    }

    QTextStream out(&outFile);

    out << "UNITA;" << "ANNO;" << "MESE;" << "MATRICOLA;" << "NOMINATIVO;"
        << "SALDO;" << "RESIDUO;" << "NON RECUP.;" << "RECUPERATE;"
        << "STR.REPE.ORD;" << "STR.REPE.NOF;" << "STR.REPE.NEF;"
        << "STR.GUAR.ORD;" << "STR.GUAR.NOF;" << "STR.GUAR.NEF;"
        << "TURN.REPE;" << "ORE REPE;" << "GUA.DIU.;" << "GUA.NOT.;" << "GR.FES.;" << "NOTE"
        << "\n";

    int currRow = 0;
    const auto v = Utilities::monthsTo(m_daMonthYear, m_aMonthYear);
    emit totalRows(Utilities::monthsTo(m_daMonthYear, m_aMonthYear));

    QDate currentMonthYear = m_daMonthYear;

    while(currentMonthYear <= m_aMonthYear) {
        currRow++;
        emit currentRow(currRow);
        for(int unitaId : qAsConst(unitaIdList)) {
            QVector<int> dirigentiIdList = ApiService::instance().getDoctorsIdsFromUnitInTimecard("tc_" + currentMonthYear.toString("yyyyMM"), unitaId);
            if(dirigentiIdList.count() > 0) {
                for(int dirigenteId : qAsConst(dirigentiIdList)) {
                    if(m_matricole.count() > 0) {
                        if(!m_matricole.contains(ApiService::instance().getDoctorMatricola(dirigenteId)))
                            continue;
                    }
                    m_competenze = new Competenza("tc_" + currentMonthYear.toString("yyyyMM"), dirigenteId, true /* esportazione */);
                    double whole, fractional;
                    fractional = std::modf(m_competenze->numeroTurniProntaDisponibilita(), &whole);
                    out << ApiService::instance().getUnitaNomeCompleto(unitaId) << ";";
                    out << currentMonthYear.toString("yyyy") << ";";
                    out << currentMonthYear.toString("MM") << ";";
                    out << m_competenze->dipendente()->matricola() << ";";
                    out << ApiService::instance().getDoctorName(m_competenze->dipendente()->matricola()) << ";";
                    out << Utilities::inOrario(m_competenze->minutiSaldoMese()) << ";";
                    out << Utilities::inOrario(m_competenze->minutiRecuperabili()) << ";";
                    out << Utilities::inOrario(m_competenze->minutiNonRecuperabili()) << ";";
                    out << Utilities::inOrario(m_competenze->dipendente()->minutiCausale("RMP")) << ";";
                    out << m_competenze->oreReperibilitaORDPagate() << ";"; // 70
                    out << m_competenze->oreReperibilitaNOFPagate() << ";"; // 72
                    out << m_competenze->oreReperibilitaNEFPagate() << ";"; // 71
                    out << m_competenze->oreStraordinarioNotturnoORDPagate() << ";"; // 73
                    out << m_competenze->oreStraordinarioNotturnoNOFPagate() << ";"; // 75
                    out << m_competenze->oreStraordinarioNotturnoNEFPagate() << ";"; // 74
                    if(currentMonthYear >= Utilities::ccnl1921Date) {

                    } else {
                        out << whole << ";"; // 25
                        out << (fractional > 0.0 ? 6 : 0) << ";"; // 40
                    }
                    out << m_competenze->numeroGuardieDiurneFatte() << ";"; // 1512
                    out << m_competenze->numeroGuardieNotturneFatte() + m_competenze->numeroGrandiFestivitaFatte() - m_competenze->numeroGrandiFestivitaPagate() << ";"; // 1571
                    out << m_competenze->numeroGrandiFestivitaPagate() << ";"; // 921
                    out << m_competenze->note();
                    out << "\n";
                }

            }
        }
        currentMonthYear = currentMonthYear.addMonths(1);
    }

    outFile.close();

    emit exportFinished(m_path + QDir::separator() + csvFileName);
}
