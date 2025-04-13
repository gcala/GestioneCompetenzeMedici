/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "competenza.h"
#include "dipendente.h"
#include "almanac.h"
#include "utilities.h"
#include "apiservice.h"

#include <QDebug>
#include <QEventLoop>

class CompetenzaData : public QSharedData
{
public:
    QPair<int,int> m_recuperiMeseSuccessivo;
    Dipendente *m_dipendente;
    QMap<int, Utilities::GuardiaType> m_guardiaNotturnaMap;
    QMap<int, Utilities::GuardiaType> m_guardiaDiurnaMap;
    QMap<int, Utilities::GuardiaType> m_mezzaGuardiaDiurnaMap;
    QMap<QDate, Utilities::ValoreRep> m_turniProntaDisponibilita;
    QMap<QDate, Utilities::ValoreRep> m_defaultTurniProntaDisponibilita;
    QMap<QDate, Utilities::ValoreRep> m_teleconsulto;
    QMap<QDate, Utilities::ValoreRep> m_defaultTeleconsulto;

    int m_g_d_fer_F;
    int m_g_d_fer_S;
    int m_g_d_fer_D;
    int m_g_d_fes_F;
    int m_g_d_fes_S;
    int m_g_d_fes_D;
    int m_g_n_fer_F;
    int m_g_n_fer_S;
    int m_g_n_fer_D;
    int m_g_n_fes_F;
    int m_g_n_fes_S;
    int m_g_n_fes_D;
    int m_totOreGuardie;
    int m_dmp;
    int m_dmp_calcolato;
    int m_orarioGiornaliero;
    int m_defaultDmp;
    int m_oreTotaliNotte;
    int m_orePagateNotte;
    bool m_pagaDiurno;
    bool m_pagaOreLavorate;
    QString m_note;
    QString m_defaultNote;
    bool m_modded;
    bool m_turniProntaDisponibilitaModded;
    bool m_teleconsultoModded;
    bool m_dmpModded;
    bool m_noteModded;
    bool m_pagaStrGuardia;

    //------------------
    int m_numeroGrandiFestivitaPagate;
    int m_oreGuardieNotturnePagate;
    int m_oreStraordinarioNotturnoORDPagate;
    int m_oreStraordinarioNotturnoNOFPagate;
    int m_oreStraordinarioNotturnoNEFPagate;
    int m_oreReperibilitaPagate;
    int m_oreReperibilitaORDPagate;
    int m_oreReperibilitaNOFPagate;
    int m_oreReperibilitaNEFPagate;
    int m_oreGuardieDiurnePagate;
    int m_minutiRecuperabili;
    int m_minutiNonRecuperabili;
    int m_minutiSaldoMese;
    int m_minutiFestiviRecuperabili;
    int m_minutiNottiNonRecuperabili;
    int m_oreSaldoMeseNonPagate;
    int m_oreLavoratePagate;
};

Competenza::Competenza(const QString &tableName, const int id, bool isExporting)
    : data(new CompetenzaData)
    , m_tableName(tableName)
    , m_id(id)
    , m_exporting(isExporting)
{
    if(m_tableName.isEmpty()) {
        qDebug() << Q_FUNC_INFO << "ERROR :: la stringa tableName è vuota";
        return;
    }

    m_modTableName = m_tableName;
    m_modTableName.replace("_","m_");
    m_unitaId = -1;

    data->m_g_d_fer_F = 0;
    data->m_g_d_fer_S = 0;
    data->m_g_d_fer_D = 0;
    data->m_g_d_fes_F = 0;
    data->m_g_d_fes_S = 0;
    data->m_g_d_fes_D = 0;
    data->m_g_n_fer_F = 0;
    data->m_g_n_fer_S = 0;
    data->m_g_n_fer_D = 0;
    data->m_g_n_fes_F = 0;
    data->m_g_n_fes_S = 0;
    data->m_g_n_fes_D = 0;
    data->m_oreGuardieDiurnePagate = 0;
    data->m_totOreGuardie = 0;
    data->m_dmp = 0;
    data->m_pagaStrGuardia = true;
    data->m_orarioGiornaliero = 0;
    data->m_dmp_calcolato = 0;
    data->m_defaultDmp = 0;
    data->m_oreTotaliNotte = 0;
    data->m_pagaDiurno = false;
    data->m_pagaOreLavorate = false;
    data->m_note.clear();
    data->m_defaultNote.clear();
    data->m_modded = false;
    data->m_turniProntaDisponibilitaModded = false;
    data->m_teleconsultoModded = false;
    data->m_dmpModded = false;
    data->m_noteModded = false;
    data->m_orePagateNotte = 0;
    data->m_oreLavoratePagate = 0;

    data->m_dipendente = new Dipendente;
    data->m_dipendente->setAnno(m_tableName.split("_").last().left(4).toInt());
    data->m_dipendente->setMese(m_tableName.split("_").last().right(2).toInt());

    buildDipendente();
}

Competenza::Competenza(const Competenza &rhs)
    : data(rhs.data)
{

}

Competenza &Competenza::operator=(const Competenza &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

bool Competenza::operator ==(const Competenza &rhs) const
{
    if(this == &rhs)
        return true;

    return data->m_orePagateNotte == rhs.orePagateNotte() &&
            data->m_recuperiMeseSuccessivo == rhs.recuperiMeseSuccessivo() &&
            data->m_dipendente == rhs.dipendente() &&
            data->m_guardiaNotturnaMap == rhs.guardiaNotturnaMap() &&
            data->m_guardiaDiurnaMap == rhs.guardiaDiurnaMap() &&
            data->m_mezzaGuardiaDiurnaMap == rhs.mezzaGuardiaDiurnaMap() &&
            data->m_turniProntaDisponibilita == rhs.turniProntaDisponibilita() &&
            data->m_teleconsulto == rhs.teleconsulto() &&
            data->m_oreTotaliNotte == rhs.oreTotaliNotte() &&
            data->m_pagaDiurno == rhs.pagaDiurno() &&
            data->m_defaultTurniProntaDisponibilita == rhs.defaultRep() &&
            data->m_defaultTeleconsulto == rhs.defaultTeleconsulto() &&
            data->m_g_d_fer_F == rhs.g_d_fer_F() &&
            data->m_g_d_fer_S == rhs.g_d_fer_S() &&
            data->m_g_d_fer_D == rhs.g_d_fer_D() &&
            data->m_g_d_fes_F == rhs.g_d_fes_F() &&
            data->m_g_d_fes_S == rhs.g_d_fes_S() &&
            data->m_g_d_fes_D == rhs.g_d_fes_D() &&
            data->m_g_n_fer_F == rhs.g_n_fer_F() &&
            data->m_g_n_fer_S == rhs.g_n_fer_S() &&
            data->m_g_n_fer_D == rhs.g_n_fer_D() &&
            data->m_g_n_fes_F == rhs.g_n_fes_F() &&
            data->m_g_n_fes_S == rhs.g_n_fes_S() &&
            data->m_g_n_fes_D == rhs.g_n_fes_D() &&
            data->m_totOreGuardie == rhs.totOreGuardie() &&
            data->m_dmp == rhs.dmp() &&
            data->m_pagaStrGuardia == rhs.pagaStrGuardia() &&
            data->m_dmp_calcolato == rhs.dmpCalcolato() &&
            data->m_orarioGiornaliero == rhs.orarioGiornaliero() &&
            data->m_defaultDmp == rhs.defaultDmp() &&
            data->m_note == rhs.note() &&
            data->m_defaultNote == rhs.defaultNote() &&
            data->m_modded == rhs.modded() &&
            data->m_turniProntaDisponibilitaModded == rhs.turniProntaDisponibilitaModded() &&
            data->m_teleconsultoModded == rhs.teleconsultoModded() &&
            data->m_dmpModded == rhs.dmpModded() &&
            data->m_noteModded == rhs.noteModded() &&
            data->m_oreLavoratePagate == rhs.oreLavoratePagate();
}

Competenza::~Competenza()
{

}

void Competenza::buildDipendente()
{
    m_unitaId = -1;
    data->m_oreTotaliNotte = 0;
    data->m_orePagateNotte = 0;
    data->m_pagaDiurno = false;
    data->m_pagaOreLavorate = false;
    data->m_dipendente->resetProperties();
    data->m_defaultDmp = -1;
    data->m_defaultNote.clear();
    data->m_recuperiMeseSuccessivo.first = 0;  // minuti giornalieri
    data->m_recuperiMeseSuccessivo.second = 0; // num rmp
    data->m_defaultTurniProntaDisponibilita.clear();
    data->m_guardiaNotturnaMap.clear();
    data->m_guardiaDiurnaMap.clear();
    data->m_mezzaGuardiaDiurnaMap.clear();
    data->m_turniProntaDisponibilita.clear();
    data->m_teleconsulto.clear();

    if(m_id == 0) {
        qDebug() << Q_FUNC_INFO << ":: ERRORE :: id doctor uguale a zero";
        return;
    }

    QString errorMsg;
    QVariantList doctorTimecard = ApiService::instance().getDoctorTimecard(m_tableName, m_modTableName, m_id, &errorMsg);

    if (!errorMsg.isEmpty()) {
        qDebug() << Q_FUNC_INFO << ":: ERRORE :: " << errorMsg;
        return;
    }
    if(doctorTimecard.isEmpty()) {
        qDebug() << Q_FUNC_INFO << ":: ERRORE :: Risultato doctorTimecard vuoto";
        return;
    }

    if(doctorTimecard.size() != 34) {
        qDebug() << Q_FUNC_INFO << ":: ERRORE :: dimensione doctorTimecard non corretta";
        return;
    }

    data->m_dipendente->setNome(doctorTimecard.at(0).toString());           // nome
    data->m_dipendente->setMatricola(doctorTimecard.at(1).toInt());         // matricola
    data->m_dipendente->setUnita(doctorTimecard.at(2).toInt());             // unità
    data->m_dipendente->setRiposi(Utilities::stringlistToVectorInt(doctorTimecard.at(3).toString().split(",")));            // riposi
    data->m_dipendente->setMinutiGiornalieri(doctorTimecard.at(4).toInt()); // orario giornaliero
    if(doctorTimecard.at(4).toInt() > Utilities::m_maxMinutiGiornalieri) {
        data->m_dipendente->setNumGiorniCartellino(doctorTimecard.at(5).toInt());
    } else {
        if(!doctorTimecard.at(5).toString().trimmed().isEmpty()) {
            if(data->m_dipendente->minutiGiornalieriVeri() <= Utilities::m_maxMinutiGiornalieri) {
                data->m_dipendente->addAltraCausale("FERIE",doctorTimecard.at(5).toString().replace(",","~"), data->m_dipendente->minutiGiornalieriVeri()*doctorTimecard.at(5).toString().split(",").count());
            } else {
                for(const auto &f : Utilities::stringlistToVectorInt(doctorTimecard.at(5).toString().split(","))) { // ferie
                    data->m_dipendente->addFerie(f);
                }
            }
        }
    }
    if(!doctorTimecard.at(6).toString().trimmed().isEmpty()) {
        if(data->m_dipendente->minutiGiornalieriVeri() <= Utilities::m_maxMinutiGiornalieri) {
            data->m_dipendente->addAltraCausale("CONGEDI",doctorTimecard.at(6).toString().replace(",","~"),data->m_dipendente->minutiGiornalieriVeri()*doctorTimecard.at(6).toString().split(",").count());
        } else {
            for(const auto &f : Utilities::stringlistToVectorInt(doctorTimecard.at(6).toString().split(","))) { // congedo
                data->m_dipendente->addCongedo(f);
            }
        }
    }
    if(!doctorTimecard.at(7).toString().trimmed().isEmpty()) {
        if(data->m_dipendente->minutiGiornalieriVeri() <= Utilities::m_maxMinutiGiornalieri) {
            data->m_dipendente->addAltraCausale("MALATTIA",doctorTimecard.at(7).toString().replace(",","~"),data->m_dipendente->minutiGiornalieriVeri()*doctorTimecard.at(7).toString().split(",").count());
        } else {
            for(const auto &f : Utilities::stringlistToVectorInt(doctorTimecard.at(7).toString().split(","))) { // malattia
                data->m_dipendente->addMalattia(f);
            }
        }
    }
    if(!doctorTimecard.at(8).toString().trimmed().isEmpty()) {
        if(data->m_dipendente->minutiGiornalieriVeri() <= Utilities::m_maxMinutiGiornalieri) {
            data->m_dipendente->addAltraCausale("RMP",doctorTimecard.at(8).toString().replace(",","~"),data->m_dipendente->minutiGiornalieriVeri()*doctorTimecard.at(8).toString().split(",").count());
        } else {
            for(const auto &f : Utilities::stringlistToVectorInt(doctorTimecard.at(8).toString().split(","))) { // rmp
                data->m_dipendente->addRmp(f);
            }
        }
    }
    if(!doctorTimecard.at(9).toString().trimmed().isEmpty()) {
        if(data->m_dipendente->minutiGiornalieriVeri() <= Utilities::m_maxMinutiGiornalieri) {
            data->m_dipendente->addAltraCausale("RMC",doctorTimecard.at(9).toString().replace(",","~"),data->m_dipendente->minutiGiornalieriVeri()*doctorTimecard.at(9).toString().split(",").count());
        } else {
            for(const auto &f : Utilities::stringlistToVectorInt(doctorTimecard.at(9).toString().split(","))) { // rmc
                data->m_dipendente->addRmc(f);
            }
        }
    }
    if(!doctorTimecard.at(10).toString().trimmed().isEmpty()) {
        for(const auto &f : doctorTimecard.at(10).toString().split(";")) { // altre causali
            if(!f.isEmpty()) {
                const auto assenze = f.split(",");
                data->m_dipendente->addAltraCausale(assenze.at(0),assenze.at(1),assenze.at(2).toInt());
            }
        }
    }

    if(!doctorTimecard.at(11).toString().trimmed().isEmpty()) {
        for(const auto &f : doctorTimecard.at(11).toString().split(",")) { // guardie diurne
            if(!f.contains("*")) {
                data->m_dipendente->addGuardiaDiurna(f.toInt());
                addGuardiaDiurnaDay(f.toInt());
            } else if(f.contains("*")) {
                const int v = QString(f).replace("*","").toInt();
                data->m_dipendente->addMezzaGuardiaDiurna(v);
                addMezzaGuardiaDiurnaDay(v);
            }
        }
    }

    if(!doctorTimecard.at(12).toString().trimmed().isEmpty()) {
        for(const auto &f : Utilities::stringlistToVectorInt(doctorTimecard.at(12).toString().split(","))) { // guardie notturne
            data->m_dipendente->addGuardiaNotturna(f);
            addGuardiaNotturnaDay(f);
        }
    }

    if(!doctorTimecard.at(13).toString().trimmed().isEmpty()) {
        for(const auto &f : doctorTimecard.at(13).toString().split(";")) { // grep
            QStringList fields = f.split(",");
            if(fields.count() != 3)
                continue;
            data->m_dipendente->addGrep(fields.at(0).toInt(), fields.at(1).toInt(), fields.at(2).toInt());
        }
    }

    data->m_dipendente->addMinutiCongedo(doctorTimecard.at(14).toInt());     // minuti di congedi
    data->m_dipendente->addMinutiEccr(doctorTimecard.at(15).toInt());        // minuti di eccr
    data->m_dipendente->addMinutiGrep(doctorTimecard.at(16).toInt());        // minuti di grep
    data->m_dipendente->addMinutiGuar(doctorTimecard.at(17).toInt());        // minuti di guar
    data->m_dipendente->addMinutiRmc(doctorTimecard.at(18).toInt());         // minuti di rmc
    data->m_dipendente->addMinutiFatti(doctorTimecard.at(19).toInt());       // minuti fatti

    if(doctorTimecard.at(4).toInt() <= Utilities::m_maxMinutiGiornalieri) { // per retro-compatibilità
        if(!doctorTimecard.at(5).toString().isEmpty()) { // ferie per retro-compatibilità
            data->m_dipendente->addMinutiFatti(data->m_dipendente->minutiGiornalieriVeri()*doctorTimecard.at(5).toString().split(",").count());
        }

        if(!doctorTimecard.at(6).toString().trimmed().isEmpty()) { // congedi per retro-compatibilità
            data->m_dipendente->addMinutiFatti(data->m_dipendente->minutiGiornalieriVeri()*doctorTimecard.at(6).toString().split(",").count());
        }

        if(!doctorTimecard.at(7).toString().trimmed().isEmpty()) { // malattia per retro-compatibilità
            data->m_dipendente->addMinutiFatti(data->m_dipendente->minutiGiornalieriVeri()*doctorTimecard.at(7).toString().split(",").count());
        }

        if(!doctorTimecard.at(8).toString().trimmed().isEmpty()) { // RMP per retro-compatibilità
            data->m_dipendente->addMinutiFatti(data->m_dipendente->minutiGiornalieriVeri()*doctorTimecard.at(8).toString().split(",").count());
        }
        const auto annomese = m_tableName.split('_').last();
        const QDate mesecorrente(annomese.left(4).toInt(), annomese.right(2).toInt(),1);
        if(!doctorTimecard.at(10).toString().trimmed().isEmpty()) { // altre causali per retrocompatibilità
            for(const auto &f : doctorTimecard.at(10).toString().split(";")) {
                if(!f.isEmpty()) {
                    const auto assenze = f.split(",");
                    if(assenze.at(0) == "CV04" || assenze.at(0) == "CV05" || assenze.at(0) == "CV03" || assenze.at(0) == "CV01") {
                        data->m_dipendente->addMinutiFatti(assenze.at(2).toInt());
                    }
                }
            }
        }
    }

    if(!doctorTimecard.at(22).toString().trimmed().isEmpty()) {        // turni reperibilita
        for(const auto &f : doctorTimecard.at(22).toString().split(";")) {
            if(f == "0,0")
                continue;
            data->m_turniProntaDisponibilita[QDate(data->m_dipendente->anno(), data->m_dipendente->mese(), f.split(",").first().toInt())] = static_cast<Utilities::ValoreRep>(f.split(",").last().toInt());
            data->m_modded = true;
            data->m_turniProntaDisponibilitaModded = true;
        }
    }
    data->m_defaultTurniProntaDisponibilita = data->m_turniProntaDisponibilita;

    if(!doctorTimecard.at(33).toString().trimmed().isEmpty()) {        // turni teleconsulto
        for(const auto &f : doctorTimecard.at(33).toString().split(";")) {
            if(f == "0,0")
                continue;
            data->m_teleconsulto[QDate(data->m_dipendente->anno(), data->m_dipendente->mese(), f.split(",").first().toInt())] = static_cast<Utilities::ValoreRep>(f.split(",").last().toInt());
            data->m_modded = true;
            data->m_teleconsultoModded = true;
        }
    }
    data->m_defaultTeleconsulto = data->m_teleconsulto;

    data->m_dmp = doctorTimecard.at(23).toInt();       // dmp
    if(data->m_dmp >= 0) {
        data->m_modded = true;
        data->m_dmpModded = true;
    }
    data->m_defaultDmp = data->m_dmp;

    data->m_dmp_calcolato = doctorTimecard.at(24).toInt();      // dmp_calcolato

    m_unitaId = doctorTimecard.at(26).toInt();       // id unità
    if(m_unitaId < 0) {
        qDebug() << Q_FUNC_INFO << "ERROR :: unità non trovata";
    }

    data->m_note = doctorTimecard.at(27).toString();       // nota
    if(!data->m_note.isEmpty()) {
        data->m_modded = true;
        data->m_noteModded = true;
    }
    data->m_defaultNote = data->m_note;

    if(!doctorTimecard.at(28).toString().trimmed().isEmpty()) {
        for(const auto &f : Utilities::stringlistToVectorInt(doctorTimecard.at(28).toString().split(","))) { // scoperti
            data->m_dipendente->addScoperto(f);
        }
    }

    data->m_orarioGiornaliero = data->m_dipendente->minutiGiornalieri();
    if(doctorTimecard.at(29).toInt() >= 0) {
        data->m_modded = true;
        data->m_orarioGiornaliero = doctorTimecard.at(29).toInt();  // orario giornaliero
    }

    if(!doctorTimecard.at(31).toString().trimmed().isEmpty()) {
        if(doctorTimecard.at(31).toString().contains(",")) {
            for(const auto &f : Utilities::stringlistToVectorInt(doctorTimecard.at(31).toString().split(","))) { // indennità festiva
                data->m_dipendente->addIndennitaFestiva(1);
            }
        } else {
            data->m_dipendente->addIndennitaFestiva(doctorTimecard.at(31).toDouble());
        }
    }

    if(!doctorTimecard.at(32).toString().trimmed().isEmpty()) {
        for(const auto &f : Utilities::stringlistToVectorInt(doctorTimecard.at(32).toString().split(","))) { // indennità notturna
            data->m_dipendente->addIndennitaNotturna(f);
        }
    }


    getStraordinarioPagato();
    getOrePagate();
    if(!m_exporting)
        getRecuperiMeseSuccessivo();
    calcOreGuardia();
    distribuzioneOrePagate();
}

int Competenza::badgeNumber() const
{
    return data->m_dipendente->matricola();
}

QString Competenza::name() const
{
    return data->m_dipendente->nome();
}

QDate Competenza::dataIniziale() const
{
    return QDate(data->m_dipendente->anno(), data->m_dipendente->mese(), 1);
}

QDate Competenza::dataFinale() const
{
    return QDate(data->m_dipendente->anno(), data->m_dipendente->mese(), QDate(data->m_dipendente->anno(), data->m_dipendente->mese(), 1).daysInMonth());
}

QString Competenza::tableName() const
{
    return m_tableName;
}

QString Competenza::modTableName() const
{
    return m_modTableName;
}

int Competenza::doctorId()
{
    return m_id;
}

int Competenza::giorniLavorativi() const
{
    if(data->m_dipendente->minutiGiornalieri() > Utilities::m_maxMinutiGiornalieri)
        return data->m_dipendente->numGiorniCartellino() - data->m_dipendente->riposiCount();

    return QDate(data->m_dipendente->anno(), data->m_dipendente->mese(), 1).daysInMonth() - data->m_dipendente->riposiCount();
}

QString Competenza::orarioGiornaliero()
{
    if(data->m_dipendente->minutiGiornalieri() > Utilities::m_maxMinutiGiornalieri)
        return Utilities::inOrario(data->m_dipendente->minutiGiornalieri() / (data->m_dipendente->numGiorniCartellino() - data->m_dipendente->riposiCount()));

    return Utilities::inOrario((data->m_orarioGiornaliero >= 0 ? data->m_orarioGiornaliero : data->m_dipendente->minutiGiornalieri()));
}

QString Competenza::oreDovute()
{
    return Utilities::inOrario(minutiDovuti());
}

int Competenza::minutiDovuti() const
{
    if(data->m_dipendente->minutiGiornalieriVeri() > Utilities::m_maxMinutiGiornalieri) {
        return data->m_dipendente->minutiGiornalieriVeri();
    }
    return (data->m_orarioGiornaliero >= 0 ? data->m_orarioGiornaliero : data->m_dipendente->minutiGiornalieriVeri()) * giorniLavorativi();
}

QString Competenza::oreEffettuate()
{
    return Utilities::inOrario(data->m_dipendente->minutiFatti() + data->m_dipendente->minutiEccr() + data->m_dipendente->minutiGrep() + data->m_dipendente->minutiGuar());
}

int Competenza::minutiLavoratiSenzaGrep() const
{
    return data->m_dipendente->minutiFatti() + data->m_dipendente->minutiEccr() + data->m_dipendente->minutiGuar();
}

QString Competenza::differenzaOre()
{
    return Utilities::inOrario(minutiSaldoMese());
}

QString Competenza::differenzaOreSenzaDmp()
{
    return Utilities::inOrario(data->m_dipendente->minutiFatti()
                    + data->m_dipendente->minutiEccr()
                    + data->m_dipendente->minutiGrep()
                    + data->m_dipendente->minutiGuar()
                    - minutiDovuti());
}

int Competenza::minutiSaldoMese() const
{
    return data->m_dipendente->minutiFatti()
           + data->m_dipendente->minutiEccr()
           + data->m_dipendente->minutiGrep()
           + data->m_dipendente->minutiGuar()
           - dmp()
           - minutiDovuti();
}

int Competenza::deficitOrario()
{
    return data->m_dipendente->minutiFatti()
            + data->m_dipendente->minutiEccr()
            + data->m_dipendente->minutiGrep()
            + data->m_dipendente->minutiGuar()
            - dmp()
            - minutiDovuti();
}

QString Competenza::deficitPuntuale()
{
    int val = data->m_dipendente->minutiFatti()
            + data->m_dipendente->minutiEccr()
            + data->m_dipendente->minutiGrep()
            + data->m_dipendente->minutiGuar()
            - minutiDovuti();

    if( val < 0)
        return Utilities::inOrario(abs(val));

    return "//";
}

QList<QDate> Competenza::ferieDates() const
{
    QList<QDate> dates;
    for(const auto &s : data->m_dipendente->ferie()) {
        QDate date(data->m_dipendente->anno(), data->m_dipendente->mese(), s);
        dates << date;
    }

    return dates;
}

QList<QDate> Competenza::congediDates() const
{
    QList<QDate> dates;
    for(const auto &s : data->m_dipendente->congedi()) {
        QDate date(data->m_dipendente->anno(), data->m_dipendente->mese(), s);
        dates << date;
    }

    return dates;
}

QList<QDate> Competenza::malattiaDates() const
{
    QList<QDate> dates;
    for(const auto &s : data->m_dipendente->malattia()) {
        QDate date(data->m_dipendente->anno(), data->m_dipendente->mese(), s);
        dates << date;
    }

    return dates;
}

QList<QDate> Competenza::rmpDates() const
{
    QList<QDate> dates;
    for(const auto &s : data->m_dipendente->rmp()) {
        QDate date(data->m_dipendente->anno(), data->m_dipendente->mese(), s);
        dates << date;
    }

    return dates;
}

QList<QDate> Competenza::rmcDates() const
{
    QList<QDate> dates;
    for(const auto &s : data->m_dipendente->rmc()) {
        QDate date(data->m_dipendente->anno(), data->m_dipendente->mese(), s);
        dates << date;
    }

    return dates;
}

QMap<int, Utilities::GuardiaType> Competenza::guardiaDiurnaMap() const
{
    return data->m_guardiaDiurnaMap;
}

QMap<int, Utilities::GuardiaType> Competenza::mezzaGuardiaDiurnaMap() const
{
    return data->m_mezzaGuardiaDiurnaMap;
}

QMap<int, Utilities::GuardiaType> Competenza::guardiaNotturnaMap() const
{
    return data->m_guardiaNotturnaMap;
}

void Competenza::setTurniProntaDisponibilita(const QMap<QDate, Utilities::ValoreRep> &map)
{
    data->m_turniProntaDisponibilita = map;
}

void Competenza::setTeleconsulto(const QMap<QDate, Utilities::ValoreRep> &map)
{
    data->m_teleconsulto = map;
}

QMap<QDate, Utilities::ValoreRep> Competenza::turniProntaDisponibilita() const
{
    return data->m_turniProntaDisponibilita;
}

QMap<QDate, Utilities::ValoreRep> Competenza::teleconsulto() const
{
    return data->m_teleconsulto;
}

void Competenza::setDmp(const int &minutes)
{
    data->m_dmp = minutes;
    distribuzioneOrePagate();
}

void Competenza::setPagaStrGuardia(const bool &ok)
{
    data->m_pagaStrGuardia = ok;
}

void Competenza::setDmpCalcolato(const int &minutes)
{
    data->m_dmp_calcolato = minutes;
}

int Competenza::dmp() const
{
    if(data->m_dmp >= 0)
        return data->m_dmp;
    return data->m_dmp_calcolato;
}

bool Competenza::pagaStrGuardia() const
{
    return data->m_pagaStrGuardia;
}

void Competenza::setNote(const QString &note)
{
    data->m_note = note;
}

QString Competenza::note() const
{
    return data->m_note;
}

bool Competenza::isModded() const
{
    return (data->m_dmp != data->m_defaultDmp ||
//            data->m_guardiaDiurnaMap != data->m_defaultGDDates ||
//            data->m_guardiaNotturnaMap != data->m_defaultGNDates ||
            data->m_turniProntaDisponibilita != data->m_defaultTurniProntaDisponibilita ||
            // data->m_altreAssenze != data->m_defaultAltreAssenze ||
            data->m_note != data->m_defaultNote ||
//            data->m_orarioGiornaliero != data->m_defaultOrarioGiornaliero ||
            data->m_teleconsulto != data->m_defaultTeleconsulto);
}

bool Competenza::isRestorable() const
{
    return (data->m_modded || isModded());
}

void Competenza::saveMods()
{
    // salva dmp calcolato
    if(data->m_dmp_calcolato >= 0) {
        QString errorMsg;
        bool success = ApiService::instance().saveMod(
            m_modTableName,
            "dmp_calcolato",
            m_id,
            data->m_dmp_calcolato,
            &errorMsg
            );

        if (!success && !errorMsg.isEmpty()) {
            qDebug() << Q_FUNC_INFO << "ERROR: " << errorMsg;
        }
    }

    if(data->m_defaultNote != data->m_note) {
        QString errorMsg;
        bool success = ApiService::instance().saveMod(
            m_modTableName,
            "nota",
            m_id,
            data->m_note,
            &errorMsg
            );

        if (!success && !errorMsg.isEmpty()) {
            qDebug() << Q_FUNC_INFO << "ERROR: " << errorMsg;
        }
    }

    if(data->m_defaultDmp != data->m_dmp) {
        if(data->m_dmp == 0)
            data->m_dmp = -1;
        QString errorMsg;
        bool success = ApiService::instance().saveMod(
            m_modTableName,
            "dmp",
            m_id,
            data->m_dmp,
            &errorMsg
            );
        if (!success && !errorMsg.isEmpty()) {
            qDebug() << Q_FUNC_INFO << "ERROR: " << errorMsg;
        }

        int anno = 0;
        int mese = 0;
        Utilities::extractAnnoMeseFromTimecard(m_modTableName, anno, mese);
        const QDate dataCorrente(anno,mese,1);
        if(dataCorrente >= Utilities::regolamentoOrario2025Date) {
            QString errorMsg;
            const int idSaldo = ApiService::instance().idSituazioneSaldo(anno, mese, dipendente()->matricola(), &errorMsg);

            if (!errorMsg.isEmpty()) {
                qDebug() << Q_FUNC_INFO << "Errore nel recupero dell'ID situazione saldo:" << errorMsg;
            } else {
                if(idSaldo > 0) {
                    // Aggiornamento di un record esistente
                    bool updated = ApiService::instance().updateSituazioneSaldo(
                        idSaldo,
                        saldoMinuti(),
                        dipendente()->minutiCausale("RMP"),
                        dmp(),
                        &errorMsg
                        );

                    if (!updated || !errorMsg.isEmpty()) {
                        qDebug() << Q_FUNC_INFO << "Errore nell'aggiornamento della situazione saldo:" << errorMsg;
                    }
                } else {
                    // Creazione di un nuovo record
                    bool added = ApiService::instance().addSituazioneSaldo(
                        anno,
                        mese,
                        dipendente()->matricola(),
                        saldoMinuti(),
                        dipendente()->minutiCausale("RMP"),
                        dmp(),
                        &errorMsg
                        );

                    if (!added || !errorMsg.isEmpty()) {
                        qDebug() << Q_FUNC_INFO << "Errore nell'aggiunta della situazione saldo:" << errorMsg;
                    }
                }
            }
        }
    }

    if(data->m_defaultTurniProntaDisponibilita != data->m_turniProntaDisponibilita) {
        QStringList temp;
        auto i = data->m_turniProntaDisponibilita.constBegin();
        while (i != data->m_turniProntaDisponibilita.constEnd()) {
            QString s = QString::number(i.key().day()) + "," + QString::number(i.value());
            temp << s;
            i++;
        }
        if(temp.count() == 0)
            temp << "0,0";

        QString errorMsg;
        bool success = ApiService::instance().saveMod(
            m_modTableName,
            "turni_reperibilita",
            m_id,
            temp.join(";"),
            &errorMsg
            );

        if (!success && !errorMsg.isEmpty()) {
            qDebug() << Q_FUNC_INFO << "ERROR: " << errorMsg;
        }
    }

    if(data->m_defaultTeleconsulto != data->m_teleconsulto) {
        QStringList temp;
        auto i = data->m_teleconsulto.constBegin();
        while (i != data->m_teleconsulto.constEnd()) {
            QString s = QString::number(i.key().day()) + "," + QString::number(i.value());
            temp << s;
            i++;
        }
        if(temp.count() == 0)
            temp << "0,0";

        QString errorMsg;
        bool success = ApiService::instance().saveMod(
            m_modTableName,
            "turni_teleconsulto",
            m_id,
            temp.join(";"),
            &errorMsg
            );

        if (!success && !errorMsg.isEmpty()) {
            qDebug() << Q_FUNC_INFO << "ERROR: " << errorMsg;
        }
    }

    data->m_modded = true;
}

void Competenza::addGuardiaDiurnaDay(int day)
{
    if(data->m_guardiaDiurnaMap.keys().contains(day))
        data->m_guardiaDiurnaMap.remove(day);
    else
        data->m_guardiaDiurnaMap[day] = tipoGuardia(QString::number(day));

    calcOreGuardia();
}

void Competenza::addMezzaGuardiaDiurnaDay(int day)
{
    if(data->m_mezzaGuardiaDiurnaMap.keys().contains(day))
        data->m_mezzaGuardiaDiurnaMap.remove(day);
    else
        data->m_mezzaGuardiaDiurnaMap[day] = tipoGuardia(QString::number(day));

    calcOreGuardia();
}

void Competenza::addGuardiaNotturnaDay(int day)
{
    if(data->m_guardiaNotturnaMap.keys().contains(day))
        data->m_guardiaNotturnaMap.remove(day);
    else
        data->m_guardiaNotturnaMap[day] = tipoGuardia(QString::number(day));

    calcOreGuardia();
}

int Competenza::orePagateNotte() const
{
    return data->m_orePagateNotte;
}

// ccnl 16/18
int Competenza::notte() const
{
    int tot = 0;
    auto i = data->m_guardiaNotturnaMap.constBegin();
    while (i != data->m_guardiaNotturnaMap.constEnd()) {
        if(i.value() != Utilities::GuardiaType::GrandeFestivita)
            tot += data->m_oreTotaliNotte - data->m_orePagateNotte;
        i++;
    }

    // somma eventuali grandi festività non pagate
    tot += (numeroGrandiFestivitaFatte() - numeroGrandiFestivitaPagate()) * (data->m_oreTotaliNotte - data->m_orePagateNotte);

    return tot;
}

double Competenza::numeroTurniProntaDisponibilita() const
{
    double tot = 0.0;
    auto i = data->m_turniProntaDisponibilita.constBegin();
    while (i != data->m_turniProntaDisponibilita.constEnd()) {
        switch (i.value()) {
        case Utilities::ValoreRep::Mezzo:
            tot += 0.5;
            break;
        case Utilities::ValoreRep::Uno:
            tot += 1.0;
            break;
        case Utilities::ValoreRep::UnoMezzo:
            tot += 1.5;
            break;
        case Utilities::ValoreRep::Due:
            tot += 2.0;
            break;
        case Utilities::ValoreRep::DueMezzo:
            tot += 2.5;
            break;
        default:
            break;
        }
        i++;
    }

    return tot;
}

double Competenza::numeroTurniTeleconsulto() const
{
    double tot = 0.0;
    auto i = data->m_teleconsulto.constBegin();
    while (i != data->m_teleconsulto.constEnd()) {
        switch (i.value()) {
        case Utilities::ValoreRep::Teleconsulto6:
            tot += 6.0;
            break;
        case Utilities::ValoreRep::Teleconsulto12:
            tot += 12.0;
            break;
        case Utilities::ValoreRep::Teleconsulto18:
            tot += 18.0;
            break;
        case Utilities::ValoreRep::Teleconsulto24:
            tot += 24.0;
            break;
        default:
            break;
        }
        i++;
    }

    return tot;
}

int Competenza::minutiGrep()
{
    return data->m_dipendente->minutiGrep();
}



int Competenza::g_d_fer_F() const
{
    return data->m_g_d_fer_F;
}

int Competenza::g_d_fer_S() const
{
    return data->m_g_d_fer_S;
}

int Competenza::g_d_fer_D() const
{
    return data->m_g_d_fer_D;
}

int Competenza::g_d_fes_F() const
{
    return data->m_g_d_fes_F;
}

int Competenza::g_d_fes_S() const
{
    return data->m_g_d_fes_S;
}

int Competenza::g_d_fes_D() const
{
    return data->m_g_d_fes_D;
}

int Competenza::g_n_fer_F() const
{
    return data->m_g_n_fer_F;
}

int Competenza::g_n_fer_S() const
{
    return data->m_g_n_fer_S;
}

int Competenza::g_n_fer_D() const
{
    return data->m_g_n_fer_D;
}

int Competenza::g_n_fes_F() const
{
    return data->m_g_n_fes_F;
}

int Competenza::g_n_fes_S() const
{
    return data->m_g_n_fes_S;
}

int Competenza::g_n_fes_D() const
{
    return data->m_g_n_fes_D;
}

int Competenza::totOreGuardie() const
{
    return data->m_totOreGuardie;
}

int Competenza::r_d_fer()
{
    int minuti = 0;
    auto i = data->m_dipendente->grep().constBegin();
    while (i != data->m_dipendente->grep().constEnd()) {
        if(tipoReperibilita(i.key(), i.value().second) == Utilities::RepType::FerDiu)
            minuti += i.value().first;
        i++;
    }

    return minuti;
}

int Competenza::r_d_fes()
{
    int minuti = 0;
    auto i = data->m_dipendente->grep().constBegin();
    while (i != data->m_dipendente->grep().constEnd()) {
        if(tipoReperibilita(i.key(), i.value().second) == Utilities::RepType::FesDiu)
            minuti += i.value().first;
        i++;
    }

    return minuti;
}

int Competenza::r_n_fer()
{
    int minuti = 0;
    auto i = data->m_dipendente->grep().constBegin();
    while (i != data->m_dipendente->grep().constEnd()) {
        if(tipoReperibilita(i.key(), i.value().second) == Utilities::RepType::FerNot)
            minuti += i.value().first;
        i++;
    }

    return minuti;
}

int Competenza::r_n_fes()
{
    int minuti = 0;
    auto i = data->m_dipendente->grep().constBegin();
    while (i != data->m_dipendente->grep().constEnd()) {
        if(tipoReperibilita(i.key(), i.value().second) == Utilities::RepType::FesNot)
            minuti += i.value().first;
        i++;
    }

    return minuti;
}

bool Competenza::isReperibilitaModded() const
{
    return data->m_turniProntaDisponibilitaModded;
}

bool Competenza::isTeleconsultoModded() const
{
    return data->m_teleconsultoModded;
}

bool Competenza::isDmpModded() const
{
    return data->m_dmpModded;
}

bool Competenza::isNoteModded() const
{
    return data->m_noteModded;
}

Utilities::GuardiaType Competenza::tipoGuardia(const QString &giorno)
{
    QDate dataCorrente(data->m_dipendente->anno(), data->m_dipendente->mese(), giorno.toInt());

    if(The::almanac()->isGrandeFestivita(dataCorrente.addDays(1)) || The::almanac()->isGrandeFestivita(dataCorrente)) {
        return Utilities::GuardiaType::GrandeFestivita;
    } else if(dataCorrente.addDays(1).dayOfWeek() == 1) {
        return Utilities::GuardiaType::Domenica;
    } else if(dataCorrente.addDays(1).dayOfWeek() == 7) {
        return Utilities::GuardiaType::Sabato;
    }

    return Utilities::GuardiaType::Feriale;
}

Utilities::RepType Competenza::tipoReperibilita(const int giorno, const int tipo)
{
    QDate dataCorrente(data->m_dipendente->anno(), data->m_dipendente->mese(), giorno);

    Utilities::RepType value;

    if(tipo == 0) { // notturno
        if(The::almanac()->isGrandeFestivita(dataCorrente.addDays(1)) || The::almanac()->isGrandeFestivita(dataCorrente)) {
            value = Utilities::RepType::FesNot;
        } else if(dataCorrente.addDays(1).dayOfWeek() == 1) { // domenica
            value = Utilities::RepType::FerNot;
        } else if(dataCorrente.addDays(1).dayOfWeek() == 7) { // sabato
            value = Utilities::RepType::FesNot;
        } else {
            value = Utilities::RepType::FerNot;
        }
    } else { // diurno
        if(The::almanac()->isGrandeFestivita(dataCorrente)) {
            value = Utilities::RepType::FesDiu;
        } else if(dataCorrente.dayOfWeek() == 7) {
            value = Utilities::RepType::FesDiu;
        } else {
            value = Utilities::RepType::FerDiu;
        }
    }

    return value;
}

void Competenza::calcOreGuardia()
{
    data->m_g_d_fer_F = 0;
    data->m_g_d_fer_S = 0;
    data->m_g_d_fer_D = 0;
    data->m_g_d_fes_F = 0;
    data->m_g_d_fes_S = 0;
    data->m_g_d_fes_D = 0;
    data->m_g_n_fer_F = 0;
    data->m_g_n_fer_S = 0;
    data->m_g_n_fer_D = 0;
    data->m_g_n_fes_F = 0;
    data->m_g_n_fes_S = 0;
    data->m_g_n_fes_D = 0;
    data->m_totOreGuardie = 0;

    auto i = data->m_guardiaNotturnaMap.constBegin();
    while (i != data->m_guardiaNotturnaMap.constEnd()) {
        switch (i.value()) {
        case Utilities::GuardiaType::Sabato:
            data->m_g_d_fer_S += 2;
            data->m_g_d_fes_S += 2;
            data->m_g_n_fer_S += 2;
            data->m_g_n_fes_S += 6;
            data->m_totOreGuardie += 12;
            break;
        case Utilities::GuardiaType::Domenica:
            data->m_g_d_fer_D += 2;
            data->m_g_d_fes_D += 2;
            data->m_g_n_fer_D += 6;
            data->m_g_n_fes_D += 2;
            data->m_totOreGuardie += 12;
            break;
        case Utilities::GuardiaType::Feriale:
            data->m_g_d_fer_F += 4;
            data->m_g_n_fer_F += 8;
            data->m_totOreGuardie += 12;
            break;
        default:
            break;
        }
        ++i;
    }

    auto i2 = data->m_guardiaDiurnaMap.constBegin();
    while (i2 != data->m_guardiaDiurnaMap.constEnd()) {
        data->m_g_d_fes_F += 12;
        data->m_totOreGuardie += 12;
        ++i2;
    }
}

void Competenza::getOrePagate()
{
    QString errorMsg;
    const OrePagateMap orePagateMap = ApiService::instance().getOrePagateFromUnit(m_unitaId, &errorMsg);

    if (!errorMsg.isEmpty()) {
        qDebug() << "Errore nel recupero delle ore pagate:" << errorMsg;
        return;
    }

    auto i = orePagateMap.constEnd();
    while (i != orePagateMap.constBegin()) {
        --i;
        QDate currDate(data->m_dipendente->anno(), data->m_dipendente->mese(), 1);
        if (i.key() <= currDate) {
            data->m_oreTotaliNotte = i.value().oreTotali;
            data->m_orePagateNotte = i.value().orePagate;
            data->m_pagaDiurno = (i.value().pagaDiurno > 0);
            data->m_pagaOreLavorate = (i.value().pagaOreLavorate > 0);
            break;
        }
    }
}

void Competenza::getStraordinarioPagato()
{
    QString errorMsg;
    data->m_pagaStrGuardia = ApiService::instance().isAbilitatoStraordinario(
        data->m_dipendente->matricola(),
        data->m_dipendente->anno(),
        data->m_dipendente->mese(),
        &errorMsg
        );

    if (!errorMsg.isEmpty()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << errorMsg;
    }
}

void Competenza::getRecuperiMeseSuccessivo()
{
    QString errorMsg;
    data->m_recuperiMeseSuccessivo = ApiService::instance().getRecuperiMeseSuccessivo(
        data->m_dipendente->anno(),
        data->m_dipendente->mese(),
        doctorId(),
        &errorMsg
        );

    if (!errorMsg.isEmpty()) {
        qDebug() << Q_FUNC_INFO << "ERROR: " << errorMsg;
    }
}

QPair<int, int> Competenza::recuperiMeseSuccessivo() const
{
    return data->m_recuperiMeseSuccessivo;
}

Dipendente * Competenza::dipendente() const
{
    return data->m_dipendente;
}

QMap<QDate, Utilities::ValoreRep> Competenza::defaultRep() const
{
    return data->m_defaultTurniProntaDisponibilita;
}

QMap<QDate, Utilities::ValoreRep> Competenza::defaultTeleconsulto() const
{
    return data->m_defaultTeleconsulto;
}

int Competenza::oreTotaliNotte() const
{
    return data->m_oreTotaliNotte;
}

bool Competenza::pagaDiurno() const
{
    return data->m_pagaDiurno;
}

bool Competenza::pagaOreLavorate() const
{
    return data->m_pagaOreLavorate;
}

int Competenza::dmpCalcolato() const
{
    return data->m_dmp_calcolato;
}

int Competenza::orarioGiornaliero() const
{
    return data->m_orarioGiornaliero;
}

int Competenza::defaultDmp() const
{
    return data->m_defaultDmp;
}

QString Competenza::defaultNote() const
{
    return data->m_defaultNote;
}

bool Competenza::modded() const
{
    return data->m_modded;
}

bool Competenza::turniProntaDisponibilitaModded() const
{
    return data->m_turniProntaDisponibilitaModded;
}

bool Competenza::teleconsultoModded() const
{
    return data->m_teleconsultoModded;
}

bool Competenza::dmpModded() const
{
    return data->m_dmpModded;
}

bool Competenza::noteModded() const
{
    return data->m_noteModded;
}

void Competenza::distribuzioneOrePagate()
{
    // TODO: azzerare tutte le indennità
    data->m_numeroGrandiFestivitaPagate = 0;
    data->m_oreGuardieNotturnePagate = 0;
    data->m_oreStraordinarioNotturnoNEFPagate = 0;
    data->m_oreStraordinarioNotturnoNOFPagate = 0;
    data->m_oreStraordinarioNotturnoORDPagate = 0;
    data->m_oreReperibilitaPagate = 0;
    data->m_oreReperibilitaNEFPagate = 0;
    data->m_oreReperibilitaNOFPagate = 0;
    data->m_oreReperibilitaORDPagate = 0;
    data->m_minutiRecuperabili = 0;
    data->m_minutiSaldoMese = 0;
    data->m_minutiFestiviRecuperabili = 0;
    data->m_minutiNottiNonRecuperabili = 0;
    data->m_oreSaldoMeseNonPagate = 0;
    data->m_minutiNonRecuperabili = 0;
    data->m_oreLavoratePagate = 0;
    data->m_minutiSaldoMese = minutiSaldoMese();
    const QDate currDate(data->m_dipendente->anno(), data->m_dipendente->mese(), 1);

    if(data->m_minutiSaldoMese > 0) {
        data->m_numeroGrandiFestivitaPagate = numeroGrandiFestivitaFatte();
        for(int i = data->m_numeroGrandiFestivitaPagate; i >= 0; i--) {
            if((i * 12 * 60) <= minutiSaldoMese()) {
                data->m_numeroGrandiFestivitaPagate = i;
                break;
            }
        }

        data->m_minutiSaldoMese -= data->m_numeroGrandiFestivitaPagate * 12 * 60 * (data->m_pagaStrGuardia ? 1 : 0);

        int saldoMeseInOre = Utilities::arrotondaMinutiInOre(data->m_minutiSaldoMese > 0 ? data->m_minutiSaldoMese : 0);

        if(currDate >= Utilities::regolamentoOrario2025Date) {
            data->m_oreGuardieNotturnePagate = 0;
        } else {
            data->m_oreGuardieNotturnePagate = qMin(
                                                   (numeroGuardieNotturneFatte() + (numeroGrandiFestivitaFatte() - data->m_numeroGrandiFestivitaPagate)) * data->m_orePagateNotte,
                                                   saldoMeseInOre > 0 ? saldoMeseInOre : 0) * (data->m_pagaStrGuardia ? 1 : 0);
        }

        data->m_minutiSaldoMese -= data->m_oreGuardieNotturnePagate * 60;

        if(currDate < Utilities::regolamentoOrario2025Date) {
            if(data->m_pagaStrGuardia) {
                // Straordinario NEF
                if((g_n_fes_F() + g_n_fes_S() + g_n_fes_D()) >= data->m_oreGuardieNotturnePagate) {
                    data->m_oreStraordinarioNotturnoNEFPagate = data->m_oreGuardieNotturnePagate;
                } else {
                    data->m_oreStraordinarioNotturnoNEFPagate = g_n_fes_F() + g_n_fes_S() + g_n_fes_D();
                }
                // Straordinario NOF
                int restoOre = data->m_oreGuardieNotturnePagate - data->m_oreStraordinarioNotturnoNEFPagate;
                if(restoOre > 0) {
                    if((g_n_fer_F() + g_n_fer_S() + g_n_fer_D() + g_d_fes_F() + g_d_fes_S() + g_d_fes_D()) >= restoOre) {
                        data->m_oreStraordinarioNotturnoNOFPagate = restoOre;
                    } else {
                        data->m_oreStraordinarioNotturnoNOFPagate = g_n_fer_F() + g_n_fer_S() + g_n_fer_D() + g_d_fes_F() + g_d_fes_S() + g_d_fes_D();
                    }
                }
                // Straordinario ORD
                restoOre = data->m_oreGuardieNotturnePagate - data->m_oreStraordinarioNotturnoNOFPagate - data->m_oreStraordinarioNotturnoNEFPagate;
                if(restoOre > 0) {
                    if((g_d_fer_F() + g_d_fer_S() + g_d_fer_D()) >= restoOre) {
                        data->m_oreStraordinarioNotturnoORDPagate = restoOre;
                    } else {
                        data->m_oreStraordinarioNotturnoORDPagate = g_d_fer_F() + g_d_fer_S() + g_d_fer_D();
                    }
                }
            }
        }

        const int oreGrepArrotondate = Utilities::arrotondaMinutiInOre(data->m_dipendente->minutiGrep());

        data->m_oreSaldoMeseNonPagate = Utilities::arrotondaMinutiInOre(data->m_minutiSaldoMese > 0 ? data->m_minutiSaldoMese : 0);
        data->m_oreReperibilitaPagate = qMin(oreGrepArrotondate, data->m_oreSaldoMeseNonPagate);

        const int oreRepNEF = r_n_fes() % 60 <= Utilities::m_minutiArrotondamento ? r_n_fes() / 60 : r_n_fes() / 60 + 1;
        if(oreRepNEF >=  data->m_oreReperibilitaPagate) {
            data->m_oreReperibilitaNEFPagate = data->m_oreReperibilitaPagate;
        } else {
            data->m_oreReperibilitaNEFPagate = oreRepNEF;
        }

        int restoOreReperibilita = oreGrepArrotondate - data->m_oreReperibilitaNEFPagate;
        if(restoOreReperibilita > 0) {
            const int oreRepNOF = (r_n_fer() + r_d_fes()) % 60 <= Utilities::m_minutiArrotondamento ? (r_n_fer() + r_d_fes()) / 60 :(r_n_fer() + r_d_fes()) / 60 + 1;
            if(oreRepNOF >= restoOreReperibilita) {
                data->m_oreReperibilitaNOFPagate = restoOreReperibilita;
            } else {
                data->m_oreReperibilitaNOFPagate = Utilities::arrotondaMinutiInOre(r_n_fer() + r_d_fes());
            }
        }

        restoOreReperibilita = data->m_oreReperibilitaPagate - data->m_oreReperibilitaNEFPagate - data->m_oreReperibilitaNOFPagate;
        if(restoOreReperibilita > 0) {
            const int oreRepORD = r_d_fer() % 60 <= Utilities::m_minutiArrotondamento ? r_d_fer() / 60 :r_d_fer() / 60 + 1;
            if(oreRepORD >= restoOreReperibilita) {
                data->m_oreReperibilitaORDPagate = restoOreReperibilita;
            } else {
                data->m_oreReperibilitaORDPagate = Utilities::arrotondaMinutiInOre(r_d_fer());
            }
        }

        restoOreReperibilita = data->m_oreReperibilitaPagate - (data->m_oreReperibilitaNEFPagate + data->m_oreReperibilitaNOFPagate + data->m_oreReperibilitaORDPagate);
        if(restoOreReperibilita == 2) {
            data->m_oreReperibilitaNEFPagate++;
            data->m_oreReperibilitaNOFPagate++;
        } else if(restoOreReperibilita == 1) {
            const int restoMinutiRepNEF = Utilities::restoMinutiDopoArrotondamento(r_n_fes());
            const int restoMinutiRepNOF = Utilities::restoMinutiDopoArrotondamento(r_n_fer() + r_d_fes());
            const int restoMinutiRepORD = Utilities::restoMinutiDopoArrotondamento(r_d_fer());
            if(restoMinutiRepNEF >= restoMinutiRepNOF) {
                if(restoMinutiRepNEF >= restoMinutiRepORD) {
                    data->m_oreReperibilitaNEFPagate++;
                } else {
                    data->m_oreReperibilitaORDPagate++;
                }
            } else if(restoMinutiRepNOF >= restoMinutiRepORD) {
                data->m_oreReperibilitaNOFPagate++;
            } else {
                data->m_oreReperibilitaORDPagate++;
            }
        }
        //

        data->m_oreSaldoMeseNonPagate -= data->m_oreReperibilitaPagate;
        data->m_minutiSaldoMese -= data->m_oreReperibilitaPagate*60;

        if(currDate >= Utilities::regolamentoOrario2025Date) {
            data->m_oreGuardieDiurnePagate = 0;
        } else {
            data->m_oreGuardieDiurnePagate = data->m_pagaDiurno ? qMin(numeroGuardieDiurneFatte() * data->m_orePagateNotte, data->m_oreSaldoMeseNonPagate > 0 ? data->m_oreSaldoMeseNonPagate : 0) : 0;
        }

        data->m_oreSaldoMeseNonPagate -= data->m_oreGuardieDiurnePagate;
        data->m_minutiSaldoMese -= data->m_oreGuardieDiurnePagate*60;

        // Festivi Recuperabili
        if(numeroGuardieDiurneFatte() > 0) {
            const int maxMins = numeroGuardieDiurneFatte() * 2 * data->m_dipendente->minutiGiornalieri();

            if(data->m_minutiSaldoMese <= maxMins) {
                data->m_minutiFestiviRecuperabili = data->m_minutiSaldoMese;
            } else {
                if(data->m_dipendente->minutiGiornalieri() != 0) {
                    const int val = data->m_minutiSaldoMese / data->m_dipendente->minutiGiornalieri();

                    if(val <= numeroGuardieDiurneFatte() * 2) {
                        data->m_minutiFestiviRecuperabili = val * data->m_dipendente->minutiGiornalieri();
                    } else {
                        data->m_minutiFestiviRecuperabili = numeroGuardieDiurneFatte() * 2 * data->m_dipendente->minutiGiornalieri();
                    }
                }

                data->m_minutiFestiviRecuperabili = numeroGuardieDiurneFatte() * 2 * data->m_dipendente->minutiGiornalieri();
            }
        }

        // Notti Recuperabili
        if(data->m_orePagateNotte <= 0) {
            if(numeroGuardieNotturneFatte() > 0) {
                const int maxMins = numeroGuardieNotturneFatte() * data->m_dipendente->minutiGiornalieri();
                const int residuo = data->m_minutiSaldoMese - data->m_minutiFestiviRecuperabili;
                if(residuo <= maxMins) {
                    data->m_minutiNottiNonRecuperabili = residuo;
                } else {
                    const int val = residuo / data->m_dipendente->minutiGiornalieri();

                    if(val <= numeroGuardieNotturneFatte()) {
                        data->m_minutiNottiNonRecuperabili = val * data->m_dipendente->minutiGiornalieri();
                    } else {
                        data->m_minutiNottiNonRecuperabili = numeroGuardieNotturneFatte() * data->m_dipendente->minutiGiornalieri();
                    }
                }

                data->m_minutiNottiNonRecuperabili = numeroGuardieNotturneFatte() * data->m_dipendente->minutiGiornalieri();
            }
        }

        // minuti recuperabili
        if(currDate >= Utilities::regolamentoOrario2025Date) {
            if(data->m_oreGuardieNotturnePagate != 0 || data->m_numeroGrandiFestivitaPagate != 0)
                data->m_minutiRecuperabili = data->m_minutiSaldoMese - data->m_recuperiMeseSuccessivo.first * data->m_recuperiMeseSuccessivo.second;
            else
                data->m_minutiRecuperabili = data->m_minutiSaldoMese > 0 ? data->m_minutiSaldoMese : 0;
        } else {
            if(data->m_pagaStrGuardia)
                data->m_minutiRecuperabili = qMin(data->m_minutiSaldoMese <= 0 ? 0 : data->m_minutiSaldoMese , (data->m_minutiFestiviRecuperabili + data->m_minutiNottiNonRecuperabili) - data->m_recuperiMeseSuccessivo.first * data->m_recuperiMeseSuccessivo.second);
            else if(data->m_oreGuardieNotturnePagate != 0 || data->m_numeroGrandiFestivitaPagate != 0)
                data->m_minutiRecuperabili = data->m_minutiSaldoMese - data->m_recuperiMeseSuccessivo.first * data->m_recuperiMeseSuccessivo.second;
            else
                data->m_minutiRecuperabili = data->m_minutiSaldoMese > 0 ? data->m_minutiSaldoMese : 0;
        }


        // minuti NON recuperabili
        if(currDate >= Utilities::regolamentoOrario2025Date) {
            data->m_minutiNonRecuperabili = data->m_minutiSaldoMese - data->m_minutiRecuperabili - data->m_oreReperibilitaPagate * 60;
        } else {
            if(data->m_pagaStrGuardia){
                data->m_minutiNonRecuperabili = data->m_minutiSaldoMese - data->m_minutiFestiviRecuperabili - data->m_minutiNottiNonRecuperabili;
            } else {
                data->m_minutiNonRecuperabili = data->m_minutiSaldoMese - data->m_minutiRecuperabili - data->m_oreReperibilitaPagate * 60;
            }
        }
    }

    if(currDate >= Utilities::ccnl1921Date) {
        if(data->m_pagaOreLavorate) {
            data->m_oreLavoratePagate = Utilities::arrotondaMinutiInOre(minutiLavoratiSenzaGrep());
        }
    }
}

int Competenza::numeroGrandiFestivitaFatte() const
{
    int numGrFest = 0;

    auto i = guardiaNotturnaMap().constBegin();
    while(i != guardiaNotturnaMap().constEnd()) {
        if(i.value() == Utilities::GuardiaType::GrandeFestivita)
            numGrFest++;
        i++;
    }

    return numGrFest;
}

int Competenza::numeroGuardieNotturneFatte() const
{
    int num = 0;
    auto i = guardiaNotturnaMap().constBegin();
    while(i != guardiaNotturnaMap().constEnd()) {
        if(i.value() != Utilities::GuardiaType::GrandeFestivita)
            num++;
        i++;
    }

    return num;
}

int Competenza::numeroGuardieDiurneFatte() const
{
    return data->m_guardiaDiurnaMap.count();
}

double Competenza::numeroMezzeGuardieDiurneFatte() const
{
    return data->m_mezzaGuardiaDiurnaMap.count() / 2.0;
}

int Competenza::numeroGrandiFestivitaPagate() const
{
    return data->m_numeroGrandiFestivitaPagate;
}

int Competenza::numeroOreGuardieNotturnePagate() const
{
    return data->m_oreGuardieNotturnePagate;
}

int Competenza::minutiRecuperabili() const
{
    return data->m_minutiRecuperabili;
}

int Competenza::minutiNonRecuperabili() const
{
    return data->m_minutiNonRecuperabili;
}

int Competenza::saldoMinuti() const
{
    return data->m_minutiSaldoMese;
}

int Competenza::saldoOreNonPagate() const
{
    return data->m_oreSaldoMeseNonPagate;
}

int Competenza::oreLavoratePagate() const
{
    return data->m_oreLavoratePagate;
}

int Competenza::oreStraordinarioNotturnoNEFPagate() const
{
    return data->m_oreStraordinarioNotturnoNEFPagate;
}

int Competenza::oreStraordinarioNotturnoORDPagate() const
{
    return data->m_oreStraordinarioNotturnoORDPagate;
}

int Competenza::oreStraordinarioNotturnoNOFPagate() const
{
    return data->m_oreStraordinarioNotturnoNOFPagate;
}

int Competenza::oreReperibilitaPagate() const
{
    return data->m_oreReperibilitaPagate;
}

int Competenza::oreReperibilitaORDPagate() const
{
    return data->m_oreReperibilitaORDPagate;
}

int Competenza::oreReperibilitaNOFPagate() const
{
    return data->m_oreReperibilitaNOFPagate;
}

int Competenza::oreReperibilitaNEFPagate() const
{
    return data->m_oreReperibilitaNEFPagate;
}

int Competenza::oreGuardieDiurnePagate() const
{
    return data->m_oreGuardieDiurnePagate;
}

