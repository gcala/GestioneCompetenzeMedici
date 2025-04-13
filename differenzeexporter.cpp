/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "differenzeexporter.h"
//#include "sqlqueries.h"
#include "competenza.h"
#include "utilities.h"
#include "sqldatabasemanager.h"
#include "competenzepagate.h"
#include "dipendente.h"
#include "indennita.h"
#include "apiservice.h"

#include <QDate>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSqlError>
#include <cmath>

DifferenzeExporter::DifferenzeExporter(QObject *parent)
    : QThread(parent)
{
    m_idUnita = -1;
    m_timecard.clear();
}

DifferenzeExporter::~DifferenzeExporter()
= default;

void DifferenzeExporter::setPath(const QString &path)
{
    m_path = path;
}

void DifferenzeExporter::setUnita(int id)
{
    m_idUnita = id;
}

void DifferenzeExporter::setMese(const QString &timecard)
{
    m_timecard = timecard;
}

void DifferenzeExporter::setStoricizza(bool ok)
{
    m_storicizza = ok;
}

void DifferenzeExporter::setMesePagamento(int mese)
{
    m_mesePagamento = mese;
}

void DifferenzeExporter::setAnnoPagamento(int anno)
{
    m_annoPagamento = anno;
}

void DifferenzeExporter::run()
{
    Utilities::m_connectionName = "DifferenzeExporter";

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
    const QString s = m_timecard.split("_").last();
    m_currentMonthYear = QDate::fromString(s+"01", "yyyyMMdd");
    QDate date = m_currentMonthYear.addDays(m_currentMonthYear.daysInMonth()-1);
    QString mese = QLocale().monthName(date.month()) + " " + s.left(4);
    QString csvFileName = "Differenze_Competenze_" + QString(mese).replace(" ","_");

    if(m_idUnita != -1) {
        unitaIdList << m_idUnita;
        csvFileName += "_" + QString::number(m_idUnita) + "_" + ApiService::instance().getUnitaDataById(m_idUnita).breve;
    } else {
        const auto units = ApiService::instance().getUnitaDataFromTimecard(m_timecard);
        for(const auto u : units) {
            unitaIdList << u.idUnita;
        }
    }

    emit totalRows(unitaIdList.count());

    csvFileName += "_" + now.toString("yyyyMMdd_hhmm") + ".csv";

    QFile outFile(m_path + QDir::separator() + csvFileName);
    if(!outFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Impossibile aprire il file di destinazione:\n\n" + m_path + QDir::separator() + csvFileName;
        return;
    }

    QTextStream out(&outFile);

    out << "INPUT;" << "RIFERIMENTO;"
        << "CI;" << "ANNO;" << "MESE;"
        << "MENSILITA;" << "VOCE;" << "SUB;"
        << "ARR;" << "QTA;"
        << "IMP;" << "SEDE_DEL;" << "ANNO_DEL;"
        << "NUMERO_DEL;" << "DELIBERA" << "\n";

    const auto indennita = ApiService::instance().getIndennita(m_currentMonthYear.year(), m_currentMonthYear.month());

    const QString rowText =
            "LVARIDE;" +
            date.toString("dd/MM/yyyy") +
            ";%1;" +
            QString::number(m_annoPagamento) + ";" +
            QString::number(m_mesePagamento) + ";" +
            QLocale().monthName(m_mesePagamento, QLocale::ShortFormat).toUpper() + ";%2;%3;;%4;%5;;;;";

    int currRow = 0;

    QList<CompetenzePagate *> differenzeDaPagare;
    QList<CompetenzePagate *> competenzeDaStoricizzare;

    if(!ApiService::instance().tableExists("tcp_" + m_currentMonthYear.toString("yyyyMM"))) {
        ApiService::instance().createPagatoTable(m_currentMonthYear.year(), m_currentMonthYear.month());
    }

    foreach (int unitaId, unitaIdList) {
        differenzeDaPagare.clear();
        competenzeDaStoricizzare.clear();
        qDebug() << "4 ------------->" << m_timecard;
        QVector<int> dirigentiIdList = ApiService::instance().getDoctorsIdsFromUnitInTimecard(m_timecard, unitaId);

        foreach (int dirigenteId, dirigentiIdList) {
            m_competenzePagate = ApiService::instance().getCompetenzePagate(ApiService::instance().getDoctorMatricola(dirigenteId), m_currentMonthYear.year(), m_currentMonthYear.month());
            m_competenze = new Competenza(m_timecard,dirigenteId, true /* esportazione */);
            double whole, fractional;
            fractional = std::modf(m_competenze->numeroTurniProntaDisponibilita(), &whole);

            CompetenzePagate *competenzeDaPagare = new CompetenzePagate;

            competenzeDaPagare->setCi(m_competenze->dipendente()->matricola());
            competenzeDaPagare->setDeficit(m_competenze->deficitOrario() < 0 ? m_competenze->deficitOrario() : 0);
            competenzeDaPagare->setIndNotturna(0); // 26 non si paga più?
            competenzeDaPagare->setStr_reparto_ord(0); // 66 solitamente nullo
            competenzeDaPagare->setStr_reparto_nof(0); // 68 solitamente nullo
            competenzeDaPagare->setStr_reparto_nef(0); // 67 solitamente nullo
            competenzeDaPagare->setStr_repe_ord(m_competenze->oreReperibilitaORDPagate()); // 70
            competenzeDaPagare->setStr_repe_nof(m_competenze->oreReperibilitaNOFPagate()); // 72
            competenzeDaPagare->setStr_repe_nef(m_competenze->oreReperibilitaNEFPagate()); // 71
            competenzeDaPagare->setStr_guard_ord(m_competenze->oreStraordinarioNotturnoORDPagate()); // 73
            competenzeDaPagare->setStr_guard_nof(m_competenze->oreStraordinarioNotturnoNOFPagate() + m_competenze->oreGuardieDiurnePagate()); // 75
            competenzeDaPagare->setStr_guard_nef(m_competenze->oreStraordinarioNotturnoNEFPagate()); // 74
            if(m_currentMonthYear >= Utilities::ccnl1921Date) {
                competenzeDaPagare->setTurni_repe(whole > 10 ? 120.0 : (whole+fractional)*12.0); // 25
                competenzeDaPagare->setRepOltre10(whole > 10 ? (whole - 10 + fractional)*12.0 : 0.0);
                competenzeDaPagare->setOreLavorate(m_competenze->oreLavoratePagate()); // Solo Pronto Soccorso
                competenzeDaPagare->setGuard_diu(m_competenze->numeroGuardieDiurneFatte() + m_competenze->numeroMezzeGuardieDiurneFatte()); // 1512
            } else {
                competenzeDaPagare->setIndFestiva(m_competenze->dipendente()->indennitaFestiva()); // 62
                competenzeDaPagare->setTurni_repe(whole > 10 ? 10 : whole); // 25
                competenzeDaPagare->setOre_repe(fractional > 0.0 ? 6 : 0); // 40
                competenzeDaPagare->setRepOltre10(whole > 10 ? whole - 10 : 0.0);
                competenzeDaPagare->setGuard_diu(m_competenze->numeroGuardieDiurneFatte()); // 1512
            }

            competenzeDaPagare->setGuard_not(m_competenze->numeroGuardieNotturneFatte() + m_competenze->numeroGrandiFestivitaFatte() - m_competenze->numeroGrandiFestivitaPagate()); // 1571
            competenzeDaPagare->setGrande_fes(m_competenze->numeroGrandiFestivitaPagate()); // 921
            competenzeDaPagare->setDateTime(now);
            competenzeDaPagare->setTeleconsulto(m_competenze->numeroTurniTeleconsulto());

            if(m_competenzePagate == competenzeDaPagare) {
                continue;
            }
            CompetenzePagate *differenzaCompetenze = new CompetenzePagate;
            differenzaCompetenze->setCi(m_competenze->dipendente()->matricola());
            differenzaCompetenze->setDeficit(m_competenze->deficitOrario() < 0 ? m_competenze->deficitOrario() : 0);
            differenzaCompetenze->setIndNotturna(0); // 26 non si paga più
            differenzaCompetenze->setStr_reparto_ord(0); // 66 solitamente nullo
            differenzaCompetenze->setStr_reparto_nof(0); // 68 solitamente nullo
            differenzaCompetenze->setStr_reparto_nef(0); // 67 solitamente nullo
            differenzaCompetenze->setStr_repe_ord(competenzeDaPagare->str_repe_ord() - m_competenzePagate->str_repe_ord()); // 70
            differenzaCompetenze->setStr_repe_nof(competenzeDaPagare->str_repe_nof() - m_competenzePagate->str_repe_nof()); // 72
            differenzaCompetenze->setStr_repe_nef(competenzeDaPagare->str_repe_nef() - m_competenzePagate->str_repe_nef()); // 71
            differenzaCompetenze->setStr_guard_ord(competenzeDaPagare->str_guard_ord() - m_competenzePagate->str_guard_ord()); // 73
            differenzaCompetenze->setStr_guard_nof(competenzeDaPagare->str_guard_nof() - m_competenzePagate->str_guard_nof()); // 75
            differenzaCompetenze->setStr_guard_nef(competenzeDaPagare->str_guard_nef() - m_competenzePagate->str_guard_nef()); // 74
            if(m_currentMonthYear >= Utilities::ccnl1921Date) {
                differenzaCompetenze->setTurni_repe(competenzeDaPagare->turni_repe() - m_competenzePagate->turni_repe()); // 25
                differenzaCompetenze->setRepOltre10(competenzeDaPagare->repOltre10() - m_competenzePagate->repOltre10());
                differenzaCompetenze->setOreLavorate(competenzeDaPagare->oreLavorate() - m_competenzePagate->oreLavorate()); // Solo Pronto Soccorso
            } else {
                differenzaCompetenze->setIndFestiva(competenzeDaPagare->indFestiva() - m_competenzePagate->indFestiva()); // 62
                differenzaCompetenze->setTurni_repe(competenzeDaPagare->turni_repe() - m_competenzePagate->turni_repe()); // 25
                differenzaCompetenze->setOre_repe(competenzeDaPagare->ore_repe() - m_competenzePagate->ore_repe()); // 40
                differenzaCompetenze->setRepOltre10(competenzeDaPagare->repOltre10() - m_competenzePagate->repOltre10());
            }

            differenzaCompetenze->setGuard_diu(competenzeDaPagare->guard_diu() - m_competenzePagate->guard_diu()); // 1512
            differenzaCompetenze->setGuard_not(competenzeDaPagare->guard_not() - m_competenzePagate->guard_not()); // 1571
            differenzaCompetenze->setGrande_fes(competenzeDaPagare->grande_fes() - m_competenzePagate->grande_fes()); // 921
            differenzaCompetenze->setDateTime(now);
            differenzaCompetenze->setTeleconsulto(competenzeDaPagare->teleconsulto() - m_competenzePagate->teleconsulto());
            differenzeDaPagare << differenzaCompetenze;
            competenzeDaStoricizzare << competenzeDaPagare;
        }

        QVector<int> matricole;
        if(differenzeDaPagare.count() > 0) {
            QStringList notes;
            currRow++;
            emit currentRow(currRow);
            QString unitaName = ApiService::instance().getUnitaNomeCompleto(unitaId);
            int counter = 0;
            for(auto pag : qAsConst(differenzeDaPagare)) {
                const auto nominativo = ApiService::instance().getDoctorName(pag->ci());
                if(qAbs(pag->guard_diu()) > 0) { // 1512
                    if(!matricole.contains(pag->ci()))
                        matricole << pag->ci();
                    out << rowText.arg(pag->ci())
                           .arg(indennita->voce(Utilities::GuardiaDiurna, unitaId))
                           .arg(indennita->sub(Utilities::GuardiaDiurna, unitaId))
                           .arg(QString::number(pag->guard_diu()))
                           .arg(nominativo) << "\n";
                }

                if(pag->grande_fes() != 0) { // 921
                    if(!matricole.contains(pag->ci()))
                        matricole << pag->ci();
                    out << rowText.arg(pag->ci())
                           .arg(indennita->voce(Utilities::GranFestivita, unitaId))
                           .arg(indennita->sub(Utilities::GranFestivita, unitaId))
                           .arg(QString::number(pag->grande_fes()))
                           .arg(nominativo) << "\n";
//                    if(pag->grande_fes() > 0) {
//                        out << rowText.arg(pag->ci())
//                               .arg(indennita.voce(Utilities::GuardiaNotturna, unitaId))
//                               .arg(indennita.sub(Utilities::GuardiaNotturna, unitaId))
//                               .arg("-" + QString::number(pag->grande_fes())) << "\n";
//                    }
                }

                if(qAbs(pag->str_repe_ord()) > 0) { // 70
                    if(!matricole.contains(pag->ci()))
                        matricole << pag->ci();
                    out << rowText.arg(pag->ci())
                           .arg(indennita->voce(Utilities::StraordinarioReperibilitaOrd, unitaId))
                           .arg(indennita->sub(Utilities::StraordinarioReperibilitaOrd, unitaId))
                           .arg(QString::number(pag->str_repe_ord()))
                           .arg(nominativo) << "\n";
                }

                if(qAbs(pag->str_repe_nof()) > 0) { // 72
                    if(!matricole.contains(pag->ci()))
                        matricole << pag->ci();
                    out << rowText.arg(pag->ci())
                           .arg(indennita->voce(Utilities::StraordinarioReperibilitaNof, unitaId))
                           .arg(indennita->sub(Utilities::StraordinarioReperibilitaNof, unitaId))
                           .arg(QString::number(pag->str_repe_nof()))
                           .arg(nominativo) << "\n";
                }

                if(qAbs(pag->str_repe_nef()) > 0) { // 71
                    if(!matricole.contains(pag->ci()))
                        matricole << pag->ci();
                    out << rowText.arg(pag->ci())
                           .arg(indennita->voce(Utilities::StraordinarioReperibilitaNef, unitaId))
                           .arg(indennita->sub(Utilities::StraordinarioReperibilitaNef, unitaId))
                           .arg(QString::number(pag->str_repe_nef()))
                           .arg(nominativo) << "\n";
                }

                if(qAbs(pag->str_guard_ord()) > 0) { // 73
                    if(!matricole.contains(pag->ci()))
                        matricole << pag->ci();
                    out << rowText.arg(pag->ci())
                           .arg(indennita->voce(Utilities::StraordinarioGuardiaOrd, unitaId))
                           .arg(indennita->sub(Utilities::StraordinarioGuardiaOrd, unitaId))
                           .arg(QString::number(pag->str_guard_ord()))
                           .arg(nominativo) << "\n";
                }

                if(qAbs(pag->str_guard_nof()) > 0) { // 75
                    if(!matricole.contains(pag->ci()))
                        matricole << pag->ci();
                    out << rowText.arg(pag->ci())
                           .arg(indennita->voce(Utilities::StraordinarioGuardiaNof, unitaId))
                           .arg(indennita->sub(Utilities::StraordinarioGuardiaNof, unitaId))
                           .arg(QString::number(pag->str_guard_nof()))
                           .arg(nominativo) << "\n";
                }

                if(qAbs(pag->str_guard_nef()) > 0) {// 74
                    if(!matricole.contains(pag->ci()))
                        matricole << pag->ci();
                    out << rowText.arg(pag->ci())
                           .arg(indennita->voce(Utilities::StraordinarioGuardiaNef, unitaId))
                           .arg(indennita->sub(Utilities::StraordinarioGuardiaNef, unitaId))
                           .arg(QString::number(pag->str_guard_nef()))
                           .arg(nominativo) << "\n";
                }

                if(m_currentMonthYear >= Utilities::ccnl1921Date) {
                    if(qAbs(pag->oreLavorate()) > 0) { // Solo Pronto Soccorso
                        if(!matricole.contains(pag->ci()))
                            matricole << pag->ci();
                        out << rowText.arg(pag->ci())
                               .arg(indennita->voce(Utilities::IndennitaPSOraria, unitaId))
                               .arg(indennita->sub(Utilities::IndennitaPSOraria, unitaId))
                               .arg(QString::number(pag->oreLavorate()))
                               .arg(nominativo) << "\n";
                    }

                    if(qAbs(pag->turni_repe()) > 0) { // 25
                        if(!matricole.contains(pag->ci()))
                            matricole << pag->ci();
                        out << rowText.arg(pag->ci())
                               .arg(indennita->voce(Utilities::ProntaDisponibilitaOraria, unitaId))
                               .arg(indennita->sub(Utilities::ProntaDisponibilitaOraria, unitaId))
                               .arg(QString::number(pag->turni_repe()))
                               .arg(nominativo) << "\n";
                    }

                    if(qAbs(pag->repOltre10()) > 0) {
                        if(!matricole.contains(pag->ci()))
                            matricole << pag->ci();
                        out << rowText.arg(pag->ci())
                               .arg(indennita->voce(Utilities::ProntaDisponibilitaOraria30, unitaId))
                               .arg(indennita->sub(Utilities::ProntaDisponibilitaOraria30, unitaId))
                               .arg(QString::number(pag->repOltre10()))
                               .arg(nominativo) << "\n";
                    }
                } else {
                    if(qAbs(pag->indFestiva()) > 0) { // 62
                        if(!matricole.contains(pag->ci()))
                            matricole << pag->ci();
                        out << rowText.arg(pag->ci())
                               .arg(indennita->voce(Utilities::IndennitaFestiva, unitaId))
                               .arg(indennita->sub(Utilities::IndennitaFestiva, unitaId))
                               .arg(QString::number(pag->indFestiva()).replace(".",","))
                               .arg(nominativo) << "\n";
                    }

                    if(qAbs(pag->turni_repe()) > 0) { // 25
                        if(!matricole.contains(pag->ci()))
                            matricole << pag->ci();
                        out << rowText.arg(pag->ci())
                               .arg(indennita->voce(Utilities::TurniReperibilita, unitaId))
                               .arg(indennita->sub(Utilities::TurniReperibilita, unitaId))
                               .arg(QString::number(pag->turni_repe()))
                               .arg(nominativo) << "\n";
                    }

                    if(qAbs(pag->ore_repe()) > 0) { // 40
                        if(!matricole.contains(pag->ci()))
                            matricole << pag->ci();
                        out << rowText.arg(pag->ci())
                               .arg(indennita->voce(Utilities::OreReperibilita, unitaId))
                               .arg(indennita->sub(Utilities::OreReperibilita, unitaId))
                               .arg(QString::number(pag->ore_repe()))
                               .arg(nominativo) << "\n";
                    }

                    if(qAbs(pag->repOltre10()) > 0) {
                        if(!matricole.contains(pag->ci()))
                            matricole << pag->ci();
                        out << rowText.arg(pag->ci())
                               .arg(indennita->voce(Utilities::ReperibilitaOltre10, unitaId))
                               .arg(indennita->sub(Utilities::ReperibilitaOltre10, unitaId))
                               .arg(QString::number(pag->repOltre10()))
                               .arg(nominativo) << "\n";
                    }
                }

                if(qAbs(pag->teleconsulto()) > 0) {
                    if(!matricole.contains(pag->ci()))
                        matricole << pag->ci();
                    out << rowText.arg(pag->ci())
                           .arg(indennita->voce(Utilities::Teleconsulto, unitaId))
                           .arg(indennita->sub(Utilities::Teleconsulto, unitaId))
                           .arg(QString::number(pag->teleconsulto()))
                           .arg(nominativo) << "\n";
                }

                if(qAbs(pag->guard_not()/*+pag->grande_fes()*/) > 0) { // 1571
                    if(!matricole.contains(pag->ci()))
                        matricole << pag->ci();
                    out << rowText.arg(pag->ci())
                           .arg(indennita->voce(Utilities::GuardiaNotturna, unitaId))
                           .arg(indennita->sub(Utilities::GuardiaNotturna, unitaId))
                           .arg(QString::number(pag->guard_not()/*+pag->grande_fes()*/))
                           .arg(nominativo) << "\n";
                }
                counter++;
            }
            if(m_storicizza) {
                for(auto competenza : qAsConst(competenzeDaStoricizzare)) {
                    if(matricole.contains(competenza->ci()))
                        ApiService::instance().saveCompetenzePagate(competenza, m_currentMonthYear.year(), m_currentMonthYear.month());
                }
            }
        }
    }

    outFile.close();

    emit exportFinished(m_path + QDir::separator() + csvFileName);
}
