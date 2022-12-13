/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "competenza.h"
#include "dipendente.h"
#include "sqlqueries.h"
#include "almanac.h"
#include "utilities.h"

#include <QDebug>

class CompetenzaData : public QSharedData
{
public:
    int m_orePagate;
    QPair<int,int> m_recuperiMeseSuccessivo;
    Dipendente *m_dipendente;
    QMap<int, GuardiaType> m_guardiaNotturnaMap;
    QMap<int, GuardiaType> m_guardiaDiurnaMap;
    QMap<int, GuardiaType> m_defaultGNDates;
    QMap<int, GuardiaType> m_defaultGDDates;
    QMap<QDate, ValoreRep> m_rep;
    QMap<QDate, ValoreRep> m_defaultRep;

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
    int m_defaultOrarioGiornaliero;
    int m_oreTot;
    QString m_note;
    QString m_defaultNote;
    QVector<int> m_altreAssenze;
    QVector<int> m_defaultAltreAssenze;
    bool m_modded;
    bool m_gdiurneModded;
    bool m_gnotturneModded;
    bool m_repModded;
    bool m_dmpModded;
    bool m_orarioGiornalieroModded;
    bool m_altreModded;
    bool m_noteModded;
    bool m_pagaStrGuardia;
};

Competenza::Competenza(const QString &tableName, const int id, bool isExporting)
    : data(new CompetenzaData)
    , m_tableName(tableName)
    , m_id(id)
    , m_arrotondamento(45)
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
    data->m_totOreGuardie = 0;
    data->m_dmp = 0;
    data->m_pagaStrGuardia = true;
    data->m_orarioGiornaliero = 0;
    data->m_dmp_calcolato = 0;
    data->m_defaultDmp = 0;
    data->m_defaultOrarioGiornaliero = 0;
    data->m_oreTot = 0;
    data->m_note.clear();
    data->m_defaultNote.clear();
    data->m_modded = false;
    data->m_gdiurneModded = false;
    data->m_gnotturneModded = false;
    data->m_repModded = false;
    data->m_dmpModded = false;
    data->m_noteModded = false;
    data->m_altreModded = false;
    data->m_orePagate = 0;

    data->m_dipendente = new Dipendente;
    data->m_dipendente->setAnno(m_tableName.split("_").last().left(4).toInt());
    data->m_dipendente->setMese(m_tableName.split("_").last().right(2).toInt());

    buildDipendente();
}

Competenza::Competenza(const Competenza &rhs)
    : data(rhs.data)
    , m_arrotondamento(45)
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

    return data->m_orePagate == rhs.orePagate() &&
            data->m_recuperiMeseSuccessivo == rhs.recuperiMeseSuccessivo() &&
            data->m_dipendente == rhs.dipendente() &&
            data->m_guardiaNotturnaMap == rhs.guardiaNotturnaMap() &&
            data->m_guardiaDiurnaMap == rhs.guardiaDiurnaMap() &&
            data->m_defaultGNDates == rhs.defaultGNDates() &&
            data->m_defaultGDDates == rhs.defaultGDDates() &&
            data->m_rep == rhs.rep() &&
            data->m_oreTot == rhs.oreTot() &&
            data->m_defaultRep == rhs.defaultRep() &&
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
            data->m_defaultOrarioGiornaliero == rhs.defaultOrarioGiornaliero() &&
            data->m_note == rhs.note() &&
            data->m_defaultNote == rhs.defaultNote() &&
            data->m_altreAssenze == rhs.altreAssenze() &&
            data->m_defaultAltreAssenze == rhs.defaultAltreAssenze() &&
            data->m_modded == rhs.modded() &&
            data->m_gdiurneModded == rhs.gdiurneModded() &&
            data->m_gnotturneModded == rhs.gnotturneModded() &&
            data->m_repModded == rhs.repModded() &&
            data->m_dmpModded == rhs.dmpModded() &&
            data->m_orarioGiornalieroModded == rhs.orarioGiornalieroModded() &&
            data->m_altreModded == rhs.altreModded() &&
            data->m_noteModded == rhs.noteModded();
}

Competenza::~Competenza()
{

}


void Competenza::buildDipendente()
{
    m_unitaId = -1;
    data->m_oreTot = 0;
    data->m_dipendente->resetProperties();
    data->m_defaultDmp = -1;
    data->m_defaultOrarioGiornaliero = 0;
    data->m_defaultNote.clear();
    data->m_orePagate = 0;
    data->m_recuperiMeseSuccessivo.first = 0;  // minuti giornalieri
    data->m_recuperiMeseSuccessivo.second = 0; // num rmp
    data->m_defaultGDDates.clear();
    data->m_defaultGNDates.clear();
    data->m_defaultRep.clear();
    data->m_guardiaNotturnaMap.clear();
    data->m_guardiaDiurnaMap.clear();
    data->m_rep.clear();
    data->m_altreAssenze.clear();
    data->m_defaultAltreAssenze.clear();

    if(m_id == 0)
        return;

    const QVariantList query = SqlQueries::getDoctorTimecard(m_tableName, m_modTableName, m_id);

    if(query.isEmpty()) {
//        qDebug() << Q_FUNC_INFO << ":: ERRORE :: Risultato query vuoto";
        return;
    }

    if(query.size() != 33) {
        qDebug() << Q_FUNC_INFO << ":: ERRORE :: dimensione query non corretta";
        return;
    }

    data->m_dipendente->setNome(query.at(0).toString());           // nome
    data->m_dipendente->setMatricola(query.at(1).toInt());         // matricola
    data->m_dipendente->setUnita(query.at(2).toInt());             // unità
    data->m_dipendente->setRiposi(Utilities::stringlistToVectorInt(query.at(3).toString().split(",")));            // riposi
    data->m_dipendente->setMinutiGiornalieri(query.at(4).toInt()); // orario giornaliero
    if(query.at(4).toInt() > Utilities::m_maxMinutiGiornalieri) {
        data->m_dipendente->setNumGiorniCartellino(query.at(5).toInt());
    } else {
        if(!query.at(5).toString().trimmed().isEmpty()) {
            if(data->m_dipendente->minutiGiornalieriVeri() <= Utilities::m_maxMinutiGiornalieri) {
                data->m_dipendente->addAltraCausale("FERIE",query.at(5).toString().replace(",","~"), data->m_dipendente->minutiGiornalieriVeri()*query.at(5).toString().split(",").count());
            } else {
                for(const auto &f : Utilities::stringlistToVectorInt(query.at(5).toString().split(","))) { // ferie
                    data->m_dipendente->addFerie(f);
                }
            }
        }
    }
    if(!query.at(6).toString().trimmed().isEmpty()) {
        if(data->m_dipendente->minutiGiornalieriVeri() <= Utilities::m_maxMinutiGiornalieri) {
            data->m_dipendente->addAltraCausale("CONGEDI",query.at(6).toString().replace(",","~"),data->m_dipendente->minutiGiornalieriVeri()*query.at(6).toString().split(",").count());
        } else {
            for(const auto &f : Utilities::stringlistToVectorInt(query.at(6).toString().split(","))) { // congedo
                data->m_dipendente->addCongedo(f);
            }
        }
    }
    if(!query.at(7).toString().trimmed().isEmpty()) {
        if(data->m_dipendente->minutiGiornalieriVeri() <= Utilities::m_maxMinutiGiornalieri) {
            data->m_dipendente->addAltraCausale("MALATTIA",query.at(7).toString().replace(",","~"),data->m_dipendente->minutiGiornalieriVeri()*query.at(7).toString().split(",").count());
        } else {
            for(const auto &f : Utilities::stringlistToVectorInt(query.at(7).toString().split(","))) { // malattia
                data->m_dipendente->addMalattia(f);
            }
        }
    }
    if(!query.at(8).toString().trimmed().isEmpty()) {
        if(data->m_dipendente->minutiGiornalieriVeri() <= Utilities::m_maxMinutiGiornalieri) {
            data->m_dipendente->addAltraCausale("RMP",query.at(8).toString().replace(",","~"),data->m_dipendente->minutiGiornalieriVeri()*query.at(8).toString().split(",").count());
        } else {
            for(const auto &f : Utilities::stringlistToVectorInt(query.at(8).toString().split(","))) { // rmp
                data->m_dipendente->addRmp(f);
            }
        }
    }
    if(!query.at(9).toString().trimmed().isEmpty()) {
        if(data->m_dipendente->minutiGiornalieriVeri() <= Utilities::m_maxMinutiGiornalieri) {
            data->m_dipendente->addAltraCausale("RMC",query.at(9).toString().replace(",","~"),data->m_dipendente->minutiGiornalieriVeri()*query.at(9).toString().split(",").count());
        } else {
            for(const auto &f : Utilities::stringlistToVectorInt(query.at(9).toString().split(","))) { // rmc
                data->m_dipendente->addRmc(f);
            }
        }
    }
    if(!query.at(10).toString().trimmed().isEmpty()) {
        for(const auto &f : query.at(10).toString().split(";")) { // altre causali
            if(!f.isEmpty()) {
                const auto assenze = f.split(",");
                data->m_dipendente->addAltraCausale(assenze.at(0),assenze.at(1),assenze.at(2).toInt());
            }
        }
    }

    if(!query.at(20).toString().trimmed().isEmpty()) {
        for(const auto &f : Utilities::stringlistToVectorInt(query.at(20).toString().split(","))) { // guardie diurne mod
            if(f == 0)
                continue;
            data->m_dipendente->addGuardiaDiurna(f);
            addGuardiaDiurnaDay(f);
            data->m_modded = true;
            data->m_gdiurneModded = true;
        }
    } else if(!query.at(11).toString().trimmed().isEmpty()) {
        for(const auto &f : Utilities::stringlistToVectorInt(query.at(11).toString().split(","))) { // guardie diurne
            data->m_dipendente->addGuardiaDiurna(f);
            addGuardiaDiurnaDay(f);
        }
    }
    data->m_defaultGDDates = data->m_guardiaDiurnaMap;

    if(!query.at(21).toString().trimmed().isEmpty()) {
        for(const auto &f : Utilities::stringlistToVectorInt(query.at(21).toString().split(","))) { // guardie notturne mod
            if(f == 0)
                continue;
            data->m_dipendente->addGuardiaNotturna(f);
            addGuardiaNotturnaDay(f);
            data->m_modded = true;
            data->m_gnotturneModded = true;
        }
    } else if(!query.at(12).toString().trimmed().isEmpty()) {
        for(const auto &f : Utilities::stringlistToVectorInt(query.at(12).toString().split(","))) { // guardie notturne
            data->m_dipendente->addGuardiaNotturna(f);
            addGuardiaNotturnaDay(f);
        }
    }
    data->m_defaultGNDates = data->m_guardiaNotturnaMap;

    if(!query.at(13).toString().trimmed().isEmpty()) {
        for(const auto &f : query.at(13).toString().split(";")) { // grep
            QStringList fields = f.split(",");
            if(fields.count() != 3)
                continue;
            data->m_dipendente->addGrep(fields.at(0).toInt(), fields.at(1).toInt(), fields.at(2).toInt());
        }
    }

    data->m_dipendente->addMinutiCongedo(query.at(14).toInt());     // minuti di congedi
    data->m_dipendente->addMinutiEccr(query.at(15).toInt());        // minuti di eccr
    data->m_dipendente->addMinutiGrep(query.at(16).toInt());        // minuti di grep
    data->m_dipendente->addMinutiGuar(query.at(17).toInt());        // minuti di guar
    data->m_dipendente->addMinutiRmc(query.at(18).toInt());         // minuti di rmc
    data->m_dipendente->addMinutiFatti(query.at(19).toInt());       // minuti fatti

    if(query.at(4).toInt() <= Utilities::m_maxMinutiGiornalieri) { // per retro-compatibilità
        if(!query.at(5).toString().isEmpty()) { // ferie per retro-compatibilità
            data->m_dipendente->addMinutiFatti(data->m_dipendente->minutiGiornalieriVeri()*query.at(5).toString().split(",").count());
        }

        if(!query.at(7).toString().trimmed().isEmpty()) { // malattia per retro-compatibilità
            data->m_dipendente->addMinutiFatti(data->m_dipendente->minutiGiornalieriVeri()*query.at(7).toString().split(",").count());
        }

        if(!query.at(8).toString().trimmed().isEmpty()) { // RMP per retro-compatibilità
            data->m_dipendente->addMinutiFatti(data->m_dipendente->minutiGiornalieriVeri()*query.at(8).toString().split(",").count());
        }
    }

    if(!query.at(22).toString().trimmed().isEmpty()) {        // turni reperibilita
        for(const auto &f : query.at(22).toString().split(";")) {
            if(f == "0,0")
                continue;
            data->m_rep[QDate(data->m_dipendente->anno(), data->m_dipendente->mese(), f.split(",").first().toInt())] = static_cast<ValoreRep>(f.split(",").last().toInt());
            data->m_modded = true;
            data->m_repModded = true;
        }
    }
    data->m_defaultRep = data->m_rep;

    data->m_dmp = query.at(23).toInt();       // dmp
    if(data->m_dmp >= 0) {
        data->m_modded = true;
        data->m_dmpModded = true;
    }
    data->m_defaultDmp = data->m_dmp;

    data->m_dmp_calcolato = query.at(24).toInt();      // dmp_calcolato

    if(!query.at(25).toString().trimmed().isEmpty()) {        // altre assenze
        for(const auto &f : query.at(25).toString().split(",")) {
            if(f == "0")
                continue;
            data->m_altreAssenze << f.toInt();
            data->m_modded = true;
            data->m_altreModded = true;
        }
    }
    rimuoviAltreAssenzeDoppie();
    data->m_defaultAltreAssenze = data->m_altreAssenze;

    m_unitaId = query.at(26).toInt();       // id unità
    if(m_unitaId < 0) {
        qDebug() << Q_FUNC_INFO << "ERROR :: unità non trovata";
    }

    data->m_note = query.at(27).toString();       // nota
    if(!data->m_note.isEmpty()) {
        data->m_modded = true;
        data->m_noteModded = true;
    }
    data->m_defaultNote = data->m_note;

    if(!query.at(28).toString().trimmed().isEmpty()) {
        for(const auto &f : Utilities::stringlistToVectorInt(query.at(28).toString().split(","))) { // scoperti
            data->m_dipendente->addScoperto(f);
        }
    }

    data->m_orarioGiornaliero = data->m_dipendente->minutiGiornalieri();
    if(query.at(29).toInt() >= 0) {
        data->m_modded = true;
        data->m_orarioGiornalieroModded = true;
        data->m_orarioGiornaliero = query.at(29).toInt();       // orario giornaliero
    }
    data->m_defaultOrarioGiornaliero = data->m_orarioGiornaliero;

    data->m_pagaStrGuardia = query.at(30).toBool();

    if(!query.at(31).toString().trimmed().isEmpty()) {
        for(const auto &f : Utilities::stringlistToVectorInt(query.at(31).toString().split(","))) { // indennità festiva
            data->m_dipendente->addIndennitaFestiva(f);
        }
    }

    if(!query.at(32).toString().trimmed().isEmpty()) {
        for(const auto &f : Utilities::stringlistToVectorInt(query.at(32).toString().split(","))) { // indennità notturna
            data->m_dipendente->addIndennitaNotturna(f);
        }
    }

    getOrePagate();
    if(!m_exporting)
        getRecuperiMeseSuccessivo();
    calcOreGuardia();
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

int Competenza::giorniLavorati() const
{
    if(data->m_dipendente->minutiGiornalieri() > Utilities::m_maxMinutiGiornalieri)
        return 0;

    if(data->m_dipendente->riposi().count() == 1) {
        return QDate(data->m_dipendente->anno(), data->m_dipendente->mese(), 1).daysInMonth()
                - data->m_dipendente->riposi().at(0) // vecchio database il cui numero rappresenta il numero dei riposi
                - data->m_dipendente->rmp().count()
                - data->m_dipendente->ferie().count()
                - data->m_dipendente->malattia().count()
                - data->m_altreAssenze.count()
                - data->m_dipendente->altreCausaliCount();
    }

    return QDate(data->m_dipendente->anno(), data->m_dipendente->mese(), 1).daysInMonth()
            - data->m_dipendente->riposi().count()
            - data->m_dipendente->rmp().count()
            - data->m_dipendente->ferie().count()
            - data->m_dipendente->malattia().count()
            - data->m_altreAssenze.count()
            - data->m_dipendente->altreCausaliCount();
}

int Competenza::giorniLavorativi() const
{
    if(data->m_dipendente->minutiGiornalieri() > Utilities::m_maxMinutiGiornalieri)
        return data->m_dipendente->numGiorniCartellino() - data->m_dipendente->riposiCount();

    return QDate(data->m_dipendente->anno(), data->m_dipendente->mese(), 1).daysInMonth() - data->m_dipendente->riposiCount();
}

QString Competenza::assenzeTotali() const
{
    if(data->m_dipendente->minutiGiornalieriVeri() > Utilities::m_maxMinutiGiornalieri)
        return QLatin1String();

    return QString::number(data->m_dipendente->rmp().count()
            + data->m_dipendente->ferie().count()
            + data->m_dipendente->congedi().count()
            + data->m_dipendente->malattia().count()
            + data->m_dipendente->altreCausaliCount()
            + data->m_altreAssenze.count());
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

int Competenza::oreRepPagate() const
{
    if(data->m_dipendente->minutiGrep() == 0)
        return 0;

    const int oreGrep = data->m_dipendente->minutiGrep() % 60 <= m_arrotondamento ? data->m_dipendente->minutiGrep() / 60 : data->m_dipendente->minutiGrep() / 60 + 1;
    const int diffOreArrot = differenzaMin() % 60 <= m_arrotondamento ? differenzaMin() / 60 : differenzaMin() / 60 + 1;

    int residuoOre = diffOreArrot;

    residuoOre -= (numOreGuarPagabili() + numGrFestPagabili() * 12);

    if(residuoOre <= 0)
        return 0;

    if(oreGrep <= residuoOre)
        return oreGrep;

    return residuoOre;
}

QString Competenza::differenzaOre()
{
    return Utilities::inOrario(differenzaMin());
}

QString Competenza::differenzaOreSenzaDmp()
{
    return Utilities::inOrario(data->m_dipendente->minutiFatti()
                    + data->m_dipendente->minutiEccr()
                    + data->m_dipendente->minutiGrep()
                    + data->m_dipendente->minutiGuar()
                    - minutiDovuti());
}

int Competenza::differenzaMin() const
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

int Competenza::minutiAltreCausali() const
{
     int countMinuti = 0;
     const auto map = data->m_dipendente->altreCausali();
     auto i = map.constBegin();
     while (i != map.constEnd()) {
         countMinuti += i.value().second;
         ++i;
     }
     return countMinuti;
}

QString Competenza::oreAltreCausali()
{
    return Utilities::inOrario(minutiAltreCausali());
}

QString Competenza::ferieCount() const
{
    return QString::number(data->m_dipendente->ferie().count());
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

QList<QDate> Competenza::scopertiDates() const
{
    QList<QDate> dates;
    for(const auto &s : data->m_dipendente->scoperti()) {
        QDate date(data->m_dipendente->anno(), data->m_dipendente->mese(), s);
        dates << date;
    }

    return dates;
}

QString Competenza::congediCount() const
{
    return QString::number(data->m_dipendente->congedi().count());
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

QString Competenza::malattiaCount() const
{
    return QString::number(data->m_dipendente->malattia().count());
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

QString Competenza::rmpCount() const
{
    return QString::number(data->m_dipendente->rmp().count());
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

QString Competenza::rmcCount() const
{
    return QString::number(data->m_dipendente->rmc().count());
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

QList<QDate> Competenza::gdDates() const
{
    QList<QDate> dates;
    for(const auto &s : data->m_dipendente->guardieDiurne()) {
        QDate date(data->m_dipendente->anno(), data->m_dipendente->mese(), s);
        dates << date;
    }

    return dates;
}

QList<QDate> Competenza::gnDates() const
{
    QList<QDate> dates;
    for(const auto &s : data->m_dipendente->guardieNotturne()) {
        QDate date(data->m_dipendente->anno(), data->m_dipendente->mese(), s);
        dates << date;
    }

    return dates;
}

QList<QDate> Competenza::altreCausaliDates() const
{
    QList<QDate> dates;
    const auto map = data->m_dipendente->altreCausali();
    auto i = map.constBegin();
    while (i != map.constEnd()) {
        const auto causaliDates = i.value().first;
        for (const auto &s : causaliDates) {
            QDate date(data->m_dipendente->anno(), data->m_dipendente->mese(), s);
            dates << date;
        }
        ++i;
    }

    return dates;
}

QMap<int, GuardiaType> Competenza::guardiaDiurnaMap() const
{
    return data->m_guardiaDiurnaMap;
}

QMap<int, GuardiaType> Competenza::guardiaNotturnaMap() const
{
    return data->m_guardiaNotturnaMap;
}

void Competenza::setRep(const QMap<QDate, ValoreRep> &map)
{
    data->m_rep = map;
}

QMap<QDate, ValoreRep> Competenza::rep() const
{
    return data->m_rep;
}

void Competenza::setDmp(const int &minutes)
{
    data->m_dmp = minutes;
}

void Competenza::setPagaStrGuardia(const bool &ok)
{
    data->m_pagaStrGuardia = ok;
}

void Competenza::setOrarioGiornalieroMod(const int &minutes)
{
    data->m_orarioGiornaliero = minutes;
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

QList<QDate> Competenza::altreAssenzeDates() const
{
    QList<QDate> dates;
    for(const auto &s : data->m_altreAssenze) {
        QDate date(data->m_dipendente->anno(), data->m_dipendente->mese(), s);
        dates << date;
    }

    return dates;
}

void Competenza::setAltreAssenze(const QList<QDate> &assenze)
{
    data->m_altreAssenze.clear();
    for(const auto &giorno : assenze) {
        data->m_altreAssenze << giorno.day();
    }
}

bool Competenza::isModded() const
{
    return (data->m_dmp != data->m_defaultDmp ||
            data->m_guardiaDiurnaMap != data->m_defaultGDDates ||
            data->m_guardiaNotturnaMap != data->m_defaultGNDates ||
            data->m_rep != data->m_defaultRep ||
            data->m_altreAssenze != data->m_defaultAltreAssenze ||
            data->m_note != data->m_defaultNote ||
            data->m_orarioGiornaliero != data->m_defaultOrarioGiornaliero);
}

bool Competenza::isRestorable() const
{
    return (data->m_modded || isModded());
}

void Competenza::saveMods()
{
    // salva dmp calcolato
    if(data->m_dmp_calcolato >= 0) {
        SqlQueries::saveMod(m_modTableName, "dmp_calcolato", m_id, data->m_dmp_calcolato);
    }

    if(data->m_defaultNote != data->m_note) {
        SqlQueries::saveMod(m_modTableName, "nota", m_id, data->m_note);
    }

    if(data->m_defaultDmp != data->m_dmp) {
        if(data->m_dmp == 0)
            data->m_dmp = -1;
        SqlQueries::saveMod(m_modTableName, "dmp", m_id, data->m_dmp);
    }

    if(data->m_defaultGDDates != data->m_guardiaDiurnaMap) {
        QStringList list;
        for(const auto &n : data->m_guardiaDiurnaMap.keys()) {
            list << QString::number(n);
        }
        if(list.count() == 0)
            list << "0";
        SqlQueries::saveMod(m_modTableName, "guardie_diurne", m_id, list.join(","));
    }

    if(data->m_defaultGNDates != data->m_guardiaNotturnaMap) {
        QStringList list;
        for(const auto &n : data->m_guardiaNotturnaMap.keys()) {
            list << QString::number(n);
        }
        if(list.count() == 0)
            list << "0";
        SqlQueries::saveMod(m_modTableName, "guardie_notturne", m_id, list.join(","));
    }

    if(data->m_defaultAltreAssenze != data->m_altreAssenze) {
        if(data->m_altreAssenze.count() == 0)
            data->m_altreAssenze << 0;
        SqlQueries::saveMod(m_modTableName, "altre_assenze", m_id, Utilities::vectorIntToStringlist(data->m_altreAssenze).join(","));
    }

    if(data->m_defaultRep != data->m_rep) {
        QStringList temp;
        auto i = data->m_rep.constBegin();
        while (i != data->m_rep.constEnd()) {
            QString s = QString::number(i.key().day()) + "," + QString::number(i.value());
            temp << s;
            i++;
        }
        if(temp.count() == 0)
            temp << "0,0";
        SqlQueries::saveMod(m_modTableName, "turni_reperibilita", m_id, temp.join(";"));
    }

    if(data->m_defaultOrarioGiornaliero != data->m_orarioGiornaliero) {
        SqlQueries::saveMod(m_modTableName, "orario_giornaliero", m_id, data->m_orarioGiornaliero);
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

void Competenza::addGuardiaNotturnaDay(int day)
{
    if(data->m_guardiaNotturnaMap.keys().contains(day))
        data->m_guardiaNotturnaMap.remove(day);
    else
        data->m_guardiaNotturnaMap[day] = tipoGuardia(QString::number(day));

    calcOreGuardia();
}

int Competenza::orePagate() const
{
    return data->m_orePagate;
}

int Competenza::notte() const
{
    int tot = 0;
    auto i = data->m_guardiaNotturnaMap.constBegin();
    while (i != data->m_guardiaNotturnaMap.constEnd()) {
        if(i.value() != GuardiaType::GrandeFestivita)
            tot += data->m_oreTot - data->m_orePagate;
        i++;
    }

    // somma eventuali grandi festività non pagate
    tot += (grFestCount() - numGrFestPagabili()) * (data->m_oreTot - data->m_orePagate);

    return tot;
}

int Competenza::numGuarDiurne() const
{
    return data->m_guardiaDiurnaMap.count();
}

double Competenza::repCount() const
{
    double tot = 0.0;
    auto i = data->m_rep.constBegin();
    while (i != data->m_rep.constEnd()) {
        switch (i.value()) {
        case ValoreRep::Mezzo:
            tot += 0.5;
            break;
        case ValoreRep::Uno:
            tot += 1.0;
            break;
        case ValoreRep::UnoMezzo:
            tot += 1.5;
            break;
        case ValoreRep::Due:
            tot += 2.0;
            break;
        case ValoreRep::DueMezzo:
            tot += 2.5;
            break;
        default:
            break;
        }
        i++;
    }

    return tot;
}

QString Competenza::oreGrep()
{
    if(data->m_dipendente->minutiGrep() == 0)
        return QString();

    return Utilities::inOrario(data->m_dipendente->minutiGrep());
}

int Competenza::numGrFestPagabili() const
{
    if(differenzaMin() <= 0)
        return 0;

    int numGrFest = 0;

    auto i = guardiaNotturnaMap().constBegin();
    while(i != guardiaNotturnaMap().constEnd()) {
        if(i.value() == GuardiaType::GrandeFestivita)
            numGrFest++;
        i++;
    }

    for(int i = numGrFest; i >= 0; i--) {
        if((i * 12 * 60) <= differenzaMin()) {
            numGrFest = i;
            break;
        }
    }
    return numGrFest;

//    const int minutiRimasti = differenzaMin() - oreRepPagate()*12;

//    if(minutiRimasti < 12*60)
//        return 0;

//    int numGrFest = 0;

//    QMap<int, GuardiaType>::const_iterator i = guardiaNotturnaMap().constBegin();
//    while(i != guardiaNotturnaMap().constEnd()) {
//        if(i.value() == GuardiaType::GrandeFestivita)
//            numGrFest++;
//        i++;
//    }

//    for(int i = numGrFest; i >= 0; i--) {
//        if((i * 12 * 60) <= minutiRimasti) {
//            numGrFest = i;
//            break;
//        }
//    }
//    return numGrFest;
}

int Competenza::numOreGuarPagabili() const
{
    int totMin = differenzaMin(); // saldo minuti fine mese

    if(totMin <= 0) // se saldo <= 0
        return 0;

    totMin -= numGrFestPagabili() * 12 * 60; // saldo senza le grandi festività

    // arrotondiamo il saldo
    const int totMinArrot = (totMin % 60) >= m_arrotondamento ? totMin/60+1 : totMin/60;

    // ore delle guardie notturne e delle eventuali grandi festività non pagate
    // m_orePagate: sono le ore pagate di ciascuna notte, varia da reparto a reparto
    const int oreGuar = (numGuar()+numGuarGFNonPag()) * data->m_orePagate;

    if(oreGuar <= totMinArrot)
        return oreGuar;

    return totMinArrot;
}

int Competenza::numGuar() const
{
    int num = 0;
    auto i = guardiaNotturnaMap().constBegin();
    while(i != guardiaNotturnaMap().constEnd()) {
        if(i.value() != GuardiaType::GrandeFestivita)
            num++;
        i++;
    }

    return num;
}

// numero guardie con grandi festività non pagate
int Competenza::numGuarGFNonPag() const
{
    return grFestCount() - numGrFestPagabili();
}

int Competenza::numGuarNottPag() const
{
    return guardiaNotturnaMap().keys().count() - numGrFestPagabili();
}

int Competenza::numOreGuarFesENot() const
{
    if(!data->m_pagaStrGuardia)
        return 0;
    if((g_n_fes_F() + g_n_fes_S() + g_n_fes_D()) >= numOreGuarPagabili())
        return numOreGuarPagabili();
    return (g_n_fes_F() + g_n_fes_S() + g_n_fes_D());
}

int Competenza::numOreGuarFesONot() const
{
    if(!data->m_pagaStrGuardia)
        return 0;

    int restoOre = numOreGuarPagabili() - numOreGuarFesENot();
    if(restoOre <= 0)
        return 0;

    if((g_n_fer_F() + g_n_fer_S() + g_n_fer_D() + g_d_fes_F() + g_d_fes_S() + g_d_fes_D()) >= restoOre)
        return restoOre;
    return (g_n_fer_F() + g_n_fer_S() + g_n_fer_D() + g_d_fes_F() + g_d_fes_S() + g_d_fes_D());
}

int Competenza::numOreGuarOrd() const
{
    if(!data->m_pagaStrGuardia)
        return 0;

    int restoOre = numOreGuarPagabili() - numOreGuarFesONot() - numOreGuarFesENot();
    if(restoOre <= 0)
        return 0;

    if((g_d_fer_F() + g_d_fer_S() + g_d_fer_D()) >= restoOre)
        return restoOre;
    return (g_d_fer_F() + g_d_fer_S() + g_d_fer_D());
}

int Competenza::numOreRep(Reperibilita rep)
{
    int oreRepFesENot = r_n_fes() % 60 <= m_arrotondamento ? r_n_fes() / 60 :r_n_fes() / 60 + 1;
    if(oreRepFesENot >=  oreRepPagate())
        oreRepFesENot = oreRepPagate();

    int oreRepFesONot = 0;
    int restoOre = oreRepPagate() - oreRepFesENot;
    if(restoOre <= 0) {
        oreRepFesONot = 0;
    } else {
        oreRepFesONot = (r_n_fer() + r_d_fes()) % 60 <= m_arrotondamento ? (r_n_fer() + r_d_fes()) / 60 :(r_n_fer() + r_d_fes()) / 60 + 1;
        if(oreRepFesONot >= restoOre)
            oreRepFesONot = restoOre;
    }

    int oreRepOrd = 0;
    restoOre = oreRepPagate() - oreRepFesENot - oreRepFesONot;
    if(restoOre <= 0) {
        oreRepOrd = 0;
    } else {
        oreRepOrd = r_d_fer() % 60 <= m_arrotondamento ? r_d_fer() / 60 :r_d_fer() / 60 + 1;

        if(oreRepOrd >= restoOre)
            oreRepOrd = restoOre;
    }

    const int diff = oreRepPagate() - (oreRepFesENot + oreRepFesONot + oreRepOrd);
    if(diff == 2) {
        oreRepFesENot++;
        oreRepFesONot++;
    } else if(diff == 1) {
        const int restoMinutiRepFesENot = r_n_fes() % 60 <= m_arrotondamento ? r_n_fes() / 60 : 0;
        const int restoMinutiRepFesONot = (r_n_fer() + r_d_fes()) % 60 <= m_arrotondamento ? (r_n_fer() + r_d_fes()) / 60 : 0;
        const int restoMinutiRepOrd = r_d_fer() % 60 <= m_arrotondamento ? r_d_fer() / 60 : 0;
        if(restoMinutiRepFesENot >= restoMinutiRepFesONot) {
            if(restoMinutiRepFesENot >= restoMinutiRepOrd) {
                oreRepFesENot++;
            } else {
                oreRepOrd++;
            }
        } else if(restoMinutiRepFesONot >= restoMinutiRepOrd) {
            oreRepFesONot++;
        } else {
            oreRepOrd++;
        }
    }

    switch (rep) {
    case Reperibilita::Ordinaria:
        return oreRepOrd;
        break;
    case Reperibilita::FestivaENotturna:
        return oreRepFesENot;
        break;
    default:
        return oreRepFesONot;
        break;
    }
}

int Competenza::residuoOreNonPagate()
{
    int oreStrGuaPagate = 0;

    if(data->m_pagaStrGuardia) {
        if(numOreGuarPagabili() != 0 || numGrFestPagabili() != 0)
            oreStrGuaPagate = numGrFestPagabili() * 12 + numOreGuarPagabili();
    }

    const int totMinPagati = oreStrGuaPagate*60 + oreRepPagate()*60;

    if(differenzaMin() - totMinPagati > 0)
        return differenzaMin() - totMinPagati;

    return 0;
}

int Competenza::numFestiviRecuperabili()
{
    if(residuoOreNonPagate() == 0)
        return 0;

    const auto dates = gdDates();

    int num = 0;
    for(QDate date : dates) {
        if(The::almanac()->isGrandeFestivita(date) || date.dayOfWeek() == 7) {
            num++;
        }
    }

    if(num == 0)
        return num;

    const int maxMins = num*2*data->m_dipendente->minutiGiornalieri();

    if(residuoOreNonPagate() <= maxMins)
        return residuoOreNonPagate();

    if(data->m_dipendente->minutiGiornalieri() != 0) {
        const int val = residuoOreNonPagate() / data->m_dipendente->minutiGiornalieri();

        if(val <= num*2) {
            return val*data->m_dipendente->minutiGiornalieri();
        }
    }

    return num*2*data->m_dipendente->minutiGiornalieri();
}

int Competenza::numNottiRecuperabili()
{
    if(data->m_orePagate > 0)
        return 0;

    const int residuo = residuoOreNonPagate() - numFestiviRecuperabili();

    const auto dates = gnDates();

    if(dates.count() == 0)
        return 0;

    const int numNottiNonPagate = dates.count() - numGrFestPagabili();

    if(numNottiNonPagate == 0)
        return 0;

    const int maxMins = numNottiNonPagate*data->m_dipendente->minutiGiornalieri();

    if(residuo <= maxMins)
        return residuo;

    if(data->m_dipendente->minutiGiornalieri() == 0)
        return 0;

    const int val = residuo / data->m_dipendente->minutiGiornalieri();

    if(val <= numNottiNonPagate) {
        return val*data->m_dipendente->minutiGiornalieri();
    }

    return numNottiNonPagate*data->m_dipendente->minutiGiornalieri();
}

int Competenza::numOreRecuperabili()
{
    if(data->m_pagaStrGuardia)
        return (numFestiviRecuperabili() + numNottiRecuperabili()) - data->m_recuperiMeseSuccessivo.first*data->m_recuperiMeseSuccessivo.second;

    if(numOreGuarPagabili() != 0 || numGrFestPagabili() != 0)
        return residuoOreNonPagate() - data->m_recuperiMeseSuccessivo.first*data->m_recuperiMeseSuccessivo.second;

    return residuoOreNonPagate();
}

QString Competenza::residuoOreNonRecuperabili()
{
    int mins = 0;

    if(data->m_pagaStrGuardia){
        mins = residuoOreNonPagate() - numFestiviRecuperabili() - numNottiRecuperabili();
    } else {
        mins = differenzaMin() - numOreRecuperabili() - oreRepPagate()*60;
    }

    if(mins == 0) {
        return "//";
    }
    return Utilities::inOrario( mins );
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
        if(tipoReperibilita(i.key(), i.value().second) == RepType::FerDiu)
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
        if(tipoReperibilita(i.key(), i.value().second) == RepType::FesDiu)
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
        if(tipoReperibilita(i.key(), i.value().second) == RepType::FerNot)
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
        if(tipoReperibilita(i.key(), i.value().second) == RepType::FesNot)
            minuti += i.value().first;
        i++;
    }

    return minuti;
}

QString Competenza::oreStraordinarioGuardie() const
{
    if(numOreGuarPagabili() == 0 && numGrFestPagabili() == 0)
        return "//";

    QString text;
    if(numGrFestPagabili() > 0)
        text += QString::number(480*numGrFestPagabili());
    text += (numOreGuarPagabili() > 0 && numGrFestPagabili() > 0 ? " + " : "");
    if(numOreGuarPagabili() > 0)
        text += QString::number(numOreGuarPagabili());
    if(text.trimmed().isEmpty())
        text = "//";

    return text;
}

bool Competenza::isGuardieDiurneModded() const
{
    return data->m_gdiurneModded;
}

bool Competenza::isGuardieNotturneModded() const
{
    return data->m_gnotturneModded;
}

bool Competenza::isReperibilitaModded() const
{
    return data->m_repModded;
}

bool Competenza::isDmpModded() const
{
    return data->m_dmpModded;
}

bool Competenza::isOrarioGiornalieroModded() const
{
    return data->m_orarioGiornalieroModded;
}

bool Competenza::isNoteModded() const
{
    return data->m_noteModded;
}

bool Competenza::isAltreModded() const
{
    return data->m_altreModded;
}

void Competenza::rimuoviAltreAssenzeDoppie()
{
    QVector<int> altreCausali;

    auto i = data->m_dipendente->altreCausali().constBegin();
    while(i != data->m_dipendente->altreCausali().constEnd()) {
        altreCausali << i.value().first;
        i++;
    }

    for(int s : qAsConst(data->m_altreAssenze)) {
        if(data->m_dipendente->ferie().contains(s)) {
            data->m_altreAssenze.removeOne(s);
            continue;
        }

        if(data->m_dipendente->congedi().contains(s)) {
            data->m_altreAssenze.removeOne(s);
            continue;
        }

        if(data->m_dipendente->malattia().contains(s)) {
            data->m_altreAssenze.removeOne(s);
            continue;
        }

        if(data->m_dipendente->rmc().contains(s)) {
            data->m_altreAssenze.removeOne(s);
            continue;
        }

        if(data->m_dipendente->rmp().contains(s)) {
            data->m_altreAssenze.removeOne(s);
            continue;
        }

        if(altreCausali.contains(s)) {
            data->m_altreAssenze.removeOne(s);
            continue;
        }
    }
}

GuardiaType Competenza::tipoGuardia(const QString &giorno)
{
    QDate dataCorrente(data->m_dipendente->anno(), data->m_dipendente->mese(), giorno.toInt());

    if(The::almanac()->isGrandeFestivita(dataCorrente.addDays(1)) || The::almanac()->isGrandeFestivita(dataCorrente)) {
        return GuardiaType::GrandeFestivita;
    } else if(dataCorrente.addDays(1).dayOfWeek() == 1) {
        return GuardiaType::Domenica;
    } else if(dataCorrente.addDays(1).dayOfWeek() == 7) {
        return GuardiaType::Sabato;
    }

    return GuardiaType::Feriale;
}

RepType Competenza::tipoReperibilita(const int giorno, const int tipo)
{
    QDate dataCorrente(data->m_dipendente->anno(), data->m_dipendente->mese(), giorno);

    RepType value;

    if(tipo == 0) { // notturno
        if(The::almanac()->isGrandeFestivita(dataCorrente.addDays(1)) || The::almanac()->isGrandeFestivita(dataCorrente)) {
            value = RepType::FesNot;
        } else if(dataCorrente.addDays(1).dayOfWeek() == 1) { // domenica
            value = RepType::FerNot;
        } else if(dataCorrente.addDays(1).dayOfWeek() == 7) { // sabato
            value = RepType::FesNot;
        } else {
            value = RepType::FerNot;
        }
    } else { // diurno
        if(The::almanac()->isGrandeFestivita(dataCorrente)) {
            value = RepType::FesDiu;
        } else if(dataCorrente.dayOfWeek() == 7) {
            value = RepType::FesDiu;
        } else {
            value = RepType::FerDiu;
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
        case GuardiaType::Sabato:
            data->m_g_d_fer_S += 2;
            data->m_g_d_fes_S += 2;
            data->m_g_n_fer_S += 2;
            data->m_g_n_fes_S += 6;
            data->m_totOreGuardie += 12;
            break;
        case GuardiaType::Domenica:
            data->m_g_d_fer_D += 2;
            data->m_g_d_fes_D += 2;
            data->m_g_n_fer_D += 6;
            data->m_g_n_fes_D += 2;
            data->m_totOreGuardie += 12;
            break;
        case GuardiaType::Feriale:
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
    const auto map = SqlQueries::getOrePagateFromUnit(m_unitaId);

    QMapIterator<QDate, QPair<int,int> > i(map);
    i.toBack();
    while (i.hasPrevious()) {
        i.previous();
        QDate currDate(data->m_dipendente->anno(),data->m_dipendente->mese(),1);
        if(i.key() <= currDate) {
            data->m_oreTot = i.value().first;
            data->m_orePagate = i.value().second;
            break;
        }
    }
}

void Competenza::getRecuperiMeseSuccessivo()
{
    data->m_recuperiMeseSuccessivo = SqlQueries::getRecuperiMeseSuccessivo(data->m_dipendente->anno(), data->m_dipendente->mese(), doctorId());
}

QPair<int, int> Competenza::recuperiMeseSuccessivo() const
{
    return data->m_recuperiMeseSuccessivo;
}

Dipendente * Competenza::dipendente() const
{
    return data->m_dipendente;
}

QMap<int, GuardiaType> Competenza::defaultGNDates() const
{
    return data->m_defaultGNDates;
}

QMap<int, GuardiaType> Competenza::defaultGDDates() const
{
    return data->m_defaultGDDates;
}

QMap<QDate, ValoreRep> Competenza::defaultRep() const
{
    return data->m_defaultRep;
}

int Competenza::oreTot() const
{
    return data->m_oreTot;
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

int Competenza::defaultOrarioGiornaliero() const
{
    return data->m_defaultOrarioGiornaliero;
}

QString Competenza::defaultNote() const
{
    return data->m_defaultNote;
}

QVector<int> Competenza::altreAssenze() const
{
    return data->m_altreAssenze;
}

QVector<int> Competenza::defaultAltreAssenze() const
{
    return data->m_defaultAltreAssenze;
}

bool Competenza::modded() const
{
    return data->m_modded;
}

bool Competenza::gdiurneModded() const
{
    return data->m_gdiurneModded;
}

bool Competenza::gnotturneModded() const
{
    return data->m_gnotturneModded;
}

bool Competenza::repModded() const
{
    return data->m_repModded;
}

bool Competenza::dmpModded() const
{
    return data->m_dmpModded;
}

bool Competenza::orarioGiornalieroModded() const
{
    return data->m_orarioGiornalieroModded;
}

bool Competenza::altreModded() const
{
    return data->m_altreModded;
}

bool Competenza::noteModded() const
{
    return data->m_noteModded;
}

int Competenza::grFestCount() const
{
    int numGrFest = 0;

    auto i = guardiaNotturnaMap().constBegin();
    while(i != guardiaNotturnaMap().constEnd()) {
        if(i.value() == GuardiaType::GrandeFestivita)
            numGrFest++;
        i++;
    }
    return numGrFest;
}
