/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017 Giuseppe Calà <giuseppe.cala@mailbox.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "competenza.h"
#include "dipendente.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class CompetenzaData : public QSharedData
{
public:
    CompetenzaData(const QString &tableName, const int id) :
        m_tableName(tableName),
        m_id(id),
        m_arrotondamento(50)
    {
        if(m_tableName.isEmpty()) {
            qDebug() << Q_FUNC_INFO << "ERROR :: la stringa tableName è vuota";
            return;
        }

        m_modTableName = m_tableName;
        m_modTableName.replace("_","m_");

        m_g_d_fer_F = 0;
        m_g_d_fer_S = 0;
        m_g_d_fer_D = 0;
        m_g_d_fes_F = 0;
        m_g_d_fes_S = 0;
        m_g_d_fes_D = 0;
        m_g_n_fer_F = 0;
        m_g_n_fer_S = 0;
        m_g_n_fer_D = 0;
        m_g_n_fes_F = 0;
        m_g_n_fes_S = 0;
        m_g_n_fes_D = 0;
        m_totOreGuardie = 0;
        m_dmp = 0;
        m_defaultDmp = 0;
        m_modded = false;
        m_unitaId = -1;
        m_orePagate = 0;

        m_dipendente = new Dipendente;
        m_dipendente->setAnno(m_tableName.split("_").last().left(4).toInt());
        m_dipendente->setMese(m_tableName.split("_").last().right(2).toInt());
        caricaGrandiFestivita(m_dipendente->anno());
        caricaPasqua(m_dipendente->anno());

        buildDipendente();
    }

    QString badgeNumber() const;
    QString name() const;
    QDate dataIniziale() const;
    QDate dataFinale() const;
    QString giorniLavorati() const;
    QString giorniLavorativi() const;
    QString assenzeTotali() const;
    QString orarioGiornaliero();
    QString oreDovute();
    QString oreEffettuate();
    int oreProntaDisp();
    QString differenzaOre();
    QString differenzaOreSenzaDmp();
    int differenzaMin() const;
    QString deficitOrario();
    int minutiAltreAssenze() const;
    QString oreAltreAssenze();
    QString ferieCount() const;
    QList<QDate> ferieDates() const;
    QString congediCount() const;
    QList<QDate> congediDates() const;
    QString malattiaCount() const;
    QList<QDate> malattiaDates() const;
    QString rmpCount() const;
    QList<QDate> rmpDates() const;
    QString rmcCount() const;
    QList<QDate> rmcDates() const;
    QList<QDate> gdDates() const;
    QList<QDate> gnDates() const;
    QMap<QDate, ValoreRep> rep() const;
    void setRep(const QMap<QDate, ValoreRep> &map);
    QMap<int, GuardiaType> guardiaDiurnaMap() const;
    QMap<int, GuardiaType> guardiaNotturnaMap() const;
    void setDmp(const QTime &time);
    QTime dmp() const;
    QList<QDate> altreAssenzeDates() const;
    void setAltreAssenze(const QList<QDate> &assenze);
    bool isModded() const;
    bool isRestorable() const;
    void saveMods();
    void resetMods();
    void addGuardiaDiurnaDay(int day);
    void addGuardiaNotturnaDay(int day);
    int orePagate() const;
    QString notte() const;
    QString festivo() const;
    QString repCount() const;
    QString oreGrep();
    int numGrFestPagabili() const;
    int numOreGuarPagabili() const;
    int numGuar() const;
    int numGuarGFNonPag() const;
    int numOreGuarFesENot() const;
    int numOreGuarFesONot() const;
    int numOreGuarOrd() const;
    int numOreRepFesENot();
    int numOreRepFesONot();
    int numOreRepOrd();

    int g_d_fer_F() const;
    int g_d_fer_S() const;
    int g_d_fer_D() const;
    int g_d_fes_F() const;
    int g_d_fes_S() const;
    int g_d_fes_D() const;
    int g_n_fer_F() const;
    int g_n_fer_S() const;
    int g_n_fer_D() const;
    int g_n_fes_F() const;
    int g_n_fes_S() const;
    int g_n_fes_D() const;
    int totOreGuardie() const;

    int r_d_fer();
    int r_d_fes();
    int r_n_fer();
    int r_n_fes();

    QString oreStraordinarioGuardie() const;

private:
    const int m_arrotondamento;
    QString m_tableName;
    QString m_modTableName;
    int m_id;
    int m_unitaId;
    int m_orePagate;
    Dipendente *m_dipendente;
    QMap<int, GuardiaType> m_guardiaNotturnaMap;
    QMap<int, GuardiaType> m_guardiaDiurnaMap;
    QMap<int, GuardiaType> m_defaultGNDates;
    QMap<int, GuardiaType> m_defaultGDDates;
    QMap<QDate, ValoreRep> m_rep;
    QMap<QDate, ValoreRep> m_defaultRep;
    QList<QDate> m_grandiFestivita;

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
    int m_defaultDmp;
    QStringList m_altreAssenze;
    QStringList m_defaultAltreAssenze;
    bool m_modded;

    void buildDipendente();
    QString inOrario(int min);
    GuardiaType tipoGuardia(const QString &day);
    RepType tipoReperibilita(const int giorno, const int tipo);
    void caricaGrandiFestivita(int anno);
    void caricaPasqua(int anno);
    void calcOreGuardia();
    void getOrePagate();
    int grFestCount() const;
};


void CompetenzaData::buildDipendente()
{
    m_dipendente->resetProperties();
    m_defaultDmp = 0;
    m_unitaId = -1;
    m_orePagate = 0;
    m_defaultGDDates.clear();
    m_defaultGNDates.clear();
    m_defaultRep.clear();
    m_guardiaNotturnaMap.clear();
    m_guardiaDiurnaMap.clear();
    m_rep.clear();
    m_altreAssenze.clear();
    m_defaultAltreAssenze.clear();

    QSqlQuery query;
    query.prepare("SELECT medici.nome,"
                  "medici.matricola,"
                  "unita.nome_full,"
                  + m_tableName + ".riposi,"
                  + m_tableName + ".minuti_giornalieri,"
                  + m_tableName + ".ferie,"
                  + m_tableName + ".congedi,"
                  + m_tableName + ".malattia,"
                  + m_tableName + ".rmp,"
                  + m_tableName + ".rmc,"
                  + m_tableName + ".altre_assenze,"
                  + m_tableName + ".guardie_diurne,"
                  + m_tableName + ".guardie_notturne,"
                  + m_tableName + ".grep,"
                  + m_tableName + ".congedi_minuti,"
                  + m_tableName + ".eccr_minuti,"
                  + m_tableName + ".grep_minuti,"
                  + m_tableName + ".guar_minuti,"
                  + m_tableName + ".rmc_minuti,"
                  + m_tableName + ".minuti_fatti,"
                  + m_modTableName + ".guardie_diurne,"
                  + m_modTableName + ".guardie_notturne,"
                  + m_modTableName + ".turni_reperibilita,"
                  + m_modTableName + ".dmp,"
                  + m_modTableName + ".altre_assenze,"
                  + m_tableName + ".id_unita "
                  + "FROM " + m_tableName + " LEFT JOIN medici ON medici.id=" + m_tableName + ".id_medico "
                  + "LEFT JOIN unita ON unita.id=" + m_tableName + ".id_unita "
                  + "LEFT JOIN " + m_modTableName + " ON " + m_modTableName + ".id_medico=" + m_tableName + ".id_medico "
                  + "WHERE " + m_tableName + ".id_medico=" + QString::number(m_id) + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return;
    }

    while(query.next()) {
        m_dipendente->setNome(query.value(0).toString());           // nome
        m_dipendente->setMatricola(query.value(1).toString());      // matricola
        m_dipendente->setUnita(query.value(2).toString());          // unità
        m_dipendente->addRiposi(query.value(3).toInt());            // riposi
        m_dipendente->setMinutiGiornalieri(query.value(4).toInt()); // orario giornaliero
        if(!query.value(5).toString().trimmed().isEmpty()) {
            foreach (QString f, query.value(5).toString().split(",")) { // ferie
                m_dipendente->addFerie(f);
            }
        }
        if(!query.value(6).toString().trimmed().isEmpty()) {
            foreach (QString f, query.value(6).toString().split(",")) { // congedi
                m_dipendente->addCongedo(f);
            }
        }
        if(!query.value(7).toString().trimmed().isEmpty()) {
            foreach (QString f, query.value(7).toString().split(",")) { // malattia
                m_dipendente->addMalattia(f);
            }
        }
        if(!query.value(8).toString().trimmed().isEmpty()) {
            foreach (QString f, query.value(8).toString().split(",")) { // rmp
                m_dipendente->addRmp(f);
            }
        }
        if(!query.value(9).toString().trimmed().isEmpty()) {
            foreach (QString f, query.value(9).toString().split(",")) {  // rmc
                m_dipendente->addRmc(f);
            }
        }
        if(!query.value(10).toString().trimmed().isEmpty()) {
            foreach (QString f, query.value(10).toString().split(";")) { // altre assenze
                if(!f.isEmpty()) {
                    QStringList assenze = f.split(",");
                    m_dipendente->addAltraAssenza(assenze.at(0),assenze.at(1),assenze.at(2).toInt());
                }
            }
        }

        if(!query.value(20).toString().trimmed().isEmpty()) {
            foreach (QString f, query.value(20).toString().split(",")) { // guardie diurne mod
                if(f == "0")
                    continue;
                m_dipendente->addGuardiaDiurna(f);
                addGuardiaDiurnaDay(f.toInt());
                m_modded = true;
            }
        } else if(!query.value(11).toString().trimmed().isEmpty()) {
            foreach (QString f, query.value(11).toString().split(",")) { // guardie diurne
                m_dipendente->addGuardiaDiurna(f);
                addGuardiaDiurnaDay(f.toInt());
            }
        }
        m_defaultGDDates = m_guardiaDiurnaMap;

        if(!query.value(21).toString().trimmed().isEmpty()) {
            foreach (QString f, query.value(21).toString().split(",")) { // guardie notturne mod
                if(f == "0")
                    continue;
                m_dipendente->addGuardiaNotturna(f);
                addGuardiaNotturnaDay(f.toInt());
                m_modded = true;
            }
        } else if(!query.value(12).toString().trimmed().isEmpty()) {
            foreach (QString f, query.value(12).toString().split(",")) { // guardie notturne
                m_dipendente->addGuardiaNotturna(f);
                addGuardiaNotturnaDay(f.toInt());
            }
        }
        m_defaultGNDates = m_guardiaNotturnaMap;

        if(!query.value(13).toString().trimmed().isEmpty()) {
            foreach (QString f, query.value(13).toString().split(";")) { // grep
                QStringList fields = f.split(",");
                if(fields.count() != 3)
                    continue;
                m_dipendente->addGrep(fields.at(0).toInt(), fields.at(1).toInt(), fields.at(2).toInt());
            }
        }

        m_dipendente->addMinutiCongedo(query.value(14).toInt());     // minuti di congedi
        m_dipendente->addMinutiEccr(query.value(15).toInt());        // minuti di eccr
        m_dipendente->addMinutiGrep(query.value(16).toInt());        // minuti di grep
        m_dipendente->addMinutiGuar(query.value(17).toInt());        // minuti di guar
        m_dipendente->addMinutiRmc(query.value(18).toInt());         // minuti di rmc
        m_dipendente->addMinutiFatti(query.value(19).toInt());       // minuti fatti

        if(!query.value(22).toString().trimmed().isEmpty()) {        // turni reperibilita
            foreach (QString f, query.value(22).toString().split(";")) {
                if(f == "0,0")
                    continue;
                m_rep[QDate(m_dipendente->anno(), m_dipendente->mese(), f.split(",").first().toInt())] = static_cast<ValoreRep>(f.split(",").last().toInt());
                m_modded = true;
            }
        }
        m_defaultRep = m_rep;

        m_dmp = query.value(23).toInt();       // dmp
        if(m_dmp > 0)
            m_modded = true;
        m_defaultDmp = m_dmp;

        if(!query.value(24).toString().trimmed().isEmpty()) {        // altre assenze
            foreach (QString f, query.value(24).toString().split(",")) {
                if(f == "0")
                    continue;
                m_altreAssenze << f;
                m_modded = true;
            }
        }
        m_defaultAltreAssenze = m_altreAssenze;

        m_unitaId = query.value(25).toInt();       // id unità
        if(m_unitaId < 0) {
            qDebug() << Q_FUNC_INFO << "ERROR :: unità non trovata";
        }
    }

    getOrePagate();
    calcOreGuardia();
}

QString CompetenzaData::inOrario(int mins)
{
    QString sign;
    if(mins < 0) {
        sign = "-";
        mins *= -1;
    }
    int ore = mins / 60;
    int min = mins - ore * 60;

    return sign + QString::number(ore) + ":" + QString::number(min).rightJustified(2, '0');
}


QString CompetenzaData::badgeNumber() const
{
    return m_dipendente->matricola();
}

QString CompetenzaData::name() const
{
    return m_dipendente->nome();
}

QDate CompetenzaData::dataIniziale() const
{
    return QDate(m_dipendente->anno(), m_dipendente->mese(), 1);
}

QDate CompetenzaData::dataFinale() const
{
    return QDate(m_dipendente->anno(), m_dipendente->mese(), QDate(m_dipendente->anno(), m_dipendente->mese(), 1).daysInMonth());
}

QString CompetenzaData::giorniLavorati() const
{
    return QString::number(QDate(m_dipendente->anno(), m_dipendente->mese(), 1).daysInMonth()
            - m_dipendente->riposi()
            - m_dipendente->rmp().count()
            - m_dipendente->ferie().count()
            - m_dipendente->congedi().count()
            - m_dipendente->malattia().count()
            - m_dipendente->altreAssenzeCount()
            - m_altreAssenze.count());
}

QString CompetenzaData::giorniLavorativi() const
{
    return QString::number(QDate(m_dipendente->anno(), m_dipendente->mese(), 1).daysInMonth() - m_dipendente->riposi());
}

QString CompetenzaData::assenzeTotali() const
{
    return QString::number(m_dipendente->rmp().count()
            + m_dipendente->ferie().count()
            + m_dipendente->congedi().count()
            + m_dipendente->malattia().count()
            + m_dipendente->altreAssenzeCount()
            + m_altreAssenze.count());
}

QString CompetenzaData::orarioGiornaliero()
{
    return inOrario(m_dipendente->minutiGiornalieri());
}

QString CompetenzaData::oreDovute()
{
    return inOrario(m_dipendente->minutiGiornalieri() * giorniLavorati().toInt());
}

QString CompetenzaData::oreEffettuate()
{
    return inOrario(m_dipendente->minutiFatti() + m_dipendente->minutiEccr() + m_dipendente->minutiGrep() + m_dipendente->minutiGuar());
}

int CompetenzaData::oreProntaDisp()
{
//    return m_dipendente->minutiGrep() % 60 <= m_arrotondamento ? QString::number(m_dipendente->minutiGrep() / 60) : QString::number(m_dipendente->minutiGrep() / 60 + 1);
    if(m_dipendente->minutiGrep() == 0)
        return 0;

    int oreGrep = m_dipendente->minutiGrep() % 60 <= m_arrotondamento ? m_dipendente->minutiGrep() / 60 : m_dipendente->minutiGrep() / 60 + 1;
    int diffOreArrot = differenzaMin() % 60 <= m_arrotondamento ? differenzaMin() / 60 : differenzaMin() / 60 + 1;

    int residuoOre = diffOreArrot - numOreGuarPagabili() - numGrFestPagabili() * 12;

    if(residuoOre <= 0)
        return 0;

    if(oreGrep <= residuoOre)
        return oreGrep;
    return residuoOre;
}

QString CompetenzaData::differenzaOre()
{
    return inOrario(differenzaMin());
}

QString CompetenzaData::differenzaOreSenzaDmp()
{
    return inOrario(m_dipendente->minutiFatti()
                    + m_dipendente->minutiEccr()
                    + m_dipendente->minutiGrep()
                    + m_dipendente->minutiGuar()
                    - (m_dipendente->minutiGiornalieri() * giorniLavorati().toInt()));
}

int CompetenzaData::differenzaMin() const
{
    return m_dipendente->minutiFatti()
           + m_dipendente->minutiEccr()
           + m_dipendente->minutiGrep()
           + m_dipendente->minutiGuar()
           /*+ minutiAltreAssenze()*/
           - m_dmp
           - (m_dipendente->minutiGiornalieri() * giorniLavorati().toInt());
}

QString CompetenzaData::deficitOrario()
{
    int val = m_dipendente->minutiFatti() + m_dipendente->minutiEccr() + m_dipendente->minutiGrep() + m_dipendente->minutiGuar() /*+ minutiAltreAssenze()*/ - m_dmp - (m_dipendente->minutiGiornalieri() * giorniLavorati().toInt());
    if( val < 0)
        return inOrario(abs(val));

    return "//";
}

int CompetenzaData::minutiAltreAssenze() const
{
     int countMinuti = 0;
     QMap<QString, QPair<QStringList, int> > map = m_dipendente->altreAssenze();
     QMap<QString, QPair<QStringList, int>>::const_iterator i = map.constBegin();
     while (i != map.constEnd()) {
         countMinuti += i.value().second;
         ++i;
     }
     return countMinuti;
}

QString CompetenzaData::oreAltreAssenze()
{
    return inOrario(minutiAltreAssenze());
}

QString CompetenzaData::ferieCount() const
{
    return QString::number(m_dipendente->ferie().count());
}

QList<QDate> CompetenzaData::ferieDates() const
{
    QList<QDate> dates;
    foreach (QString s, m_dipendente->ferie()) {
        QDate date(m_dipendente->anno(), m_dipendente->mese(), s.toInt());
        dates << date;
    }

    return dates;
}

QString CompetenzaData::congediCount() const
{
    return QString::number(m_dipendente->congedi().count());
}

QList<QDate> CompetenzaData::congediDates() const
{
    QList<QDate> dates;
    foreach (QString s, m_dipendente->congedi()) {
        QDate date(m_dipendente->anno(), m_dipendente->mese(), s.toInt());
        dates << date;
    }

    return dates;
}

QString CompetenzaData::malattiaCount() const
{
    return QString::number(m_dipendente->malattia().count());
}

QList<QDate> CompetenzaData::malattiaDates() const
{
    QList<QDate> dates;
    foreach (QString s, m_dipendente->malattia()) {
        QDate date(m_dipendente->anno(), m_dipendente->mese(), s.toInt());
        dates << date;
    }

    return dates;
}

QString CompetenzaData::rmpCount() const
{
    return QString::number(m_dipendente->rmp().count());
}

QList<QDate> CompetenzaData::rmpDates() const
{
    QList<QDate> dates;
    foreach (QString s, m_dipendente->rmp()) {
        QDate date(m_dipendente->anno(), m_dipendente->mese(), s.toInt());
        dates << date;
    }

    return dates;
}

QString CompetenzaData::rmcCount() const
{
    return QString::number(m_dipendente->rmc().count());
}

QList<QDate> CompetenzaData::rmcDates() const
{
    QList<QDate> dates;
    foreach (QString s, m_dipendente->rmc()) {
        QDate date(m_dipendente->anno(), m_dipendente->mese(), s.toInt());
        dates << date;
    }

    return dates;
}

QList<QDate> CompetenzaData::gdDates() const
{
    QList<QDate> dates;
    foreach (QString s, m_dipendente->guardieDiurne()) {
        QDate date(m_dipendente->anno(), m_dipendente->mese(), s.toInt());
        dates << date;
    }

    return dates;
}

QList<QDate> CompetenzaData::gnDates() const
{
    QList<QDate> dates;
    foreach (QString s, m_dipendente->guardieNotturne()) {
        QDate date(m_dipendente->anno(), m_dipendente->mese(), s.toInt());
        dates << date;
    }

    return dates;
}

QMap<int, GuardiaType> CompetenzaData::guardiaDiurnaMap() const
{
    return m_guardiaDiurnaMap;
}

QMap<int, GuardiaType> CompetenzaData::guardiaNotturnaMap() const
{
    return m_guardiaNotturnaMap;
}

void CompetenzaData::setRep(const QMap<QDate, ValoreRep> &map)
{
    m_rep = map;
}

QMap<QDate, ValoreRep> CompetenzaData::rep() const
{
    return m_rep;
}

void CompetenzaData::setDmp(const QTime &time)
{
    m_dmp = time.hour()*60+time.minute();
}

QTime CompetenzaData::dmp() const
{
    if(m_dmp == 0)
        return QTime(0,0,0);

    return QTime(m_dmp/60,m_dmp%60,0);
}

QList<QDate> CompetenzaData::altreAssenzeDates() const
{
    QList<QDate> dates;
    foreach (QString s, m_altreAssenze) {
        QDate date(m_dipendente->anno(), m_dipendente->mese(), s.toInt());
        dates << date;
    }

    return dates;
}

void CompetenzaData::setAltreAssenze(const QList<QDate> &assenze)
{
    m_altreAssenze.clear();
    foreach (QDate giorno, assenze) {
        m_altreAssenze << QString::number(giorno.day());
    }
}

bool CompetenzaData::isModded() const
{
    return (m_dmp != m_defaultDmp ||
            m_guardiaDiurnaMap != m_defaultGDDates ||
            m_guardiaNotturnaMap != m_defaultGNDates ||
            m_rep != m_defaultRep ||
            m_altreAssenze != m_defaultAltreAssenze);
}

bool CompetenzaData::isRestorable() const
{
    return (m_modded || isModded());
}

void CompetenzaData::saveMods()
{
    QSqlQuery query;

    if(m_defaultDmp != m_dmp) {
        query.prepare("UPDATE " + m_modTableName + " " +
                      "SET dmp=:dmp "
                      "WHERE id_medico=" + QString::number(m_id) + ";");
        query.bindValue(":dmp", m_dmp);

        if(!query.exec()) {
            qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        }
    }

    if(m_defaultGDDates != m_guardiaDiurnaMap) {
        QStringList list;
        foreach (int n, m_guardiaDiurnaMap.keys()) {
            list << QString::number(n);
        }
        if(list.count() == 0)
            list << "0";
        query.prepare("UPDATE " + m_modTableName + " " +
                      "SET guardie_diurne=:guardie_diurne "
                      "WHERE id_medico=" + QString::number(m_id) + ";");
        query.bindValue(":guardie_diurne", list.join(","));

        if(!query.exec()) {
            qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        }
    }

    if(m_defaultGNDates != m_guardiaNotturnaMap) {
        QStringList list;
        foreach (int n, m_guardiaNotturnaMap.keys()) {
            list << QString::number(n);
        }
        if(list.count() == 0)
            list << "0";
        query.prepare("UPDATE " + m_modTableName + " " +
                      "SET guardie_notturne=:guardie_notturne "
                      "WHERE id_medico=" + QString::number(m_id) + ";");
        query.bindValue(":guardie_notturne", list.join(","));

        if(!query.exec()) {
            qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        }
    }

    if(m_defaultAltreAssenze != m_altreAssenze) {
        if(m_altreAssenze.count() == 0)
            m_altreAssenze << "0";
        query.prepare("UPDATE " + m_modTableName + " " +
                      "SET altre_assenze=:altre_assenze "
                      "WHERE id_medico=" + QString::number(m_id) + ";");
        query.bindValue(":altre_assenze", m_altreAssenze.join(","));

        if(!query.exec()) {
            qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        }
    }

    if(m_defaultRep != m_rep) {
        QStringList temp;
        QMap<QDate, ValoreRep>::const_iterator i = m_rep.constBegin();
        while (i != m_rep.constEnd()) {
            QString s = QString::number(i.key().day()) + "," + QString::number(i.value());
            temp << s;
            i++;
        }
        if(temp.count() == 0)
            temp << "0,0";

        query.prepare("UPDATE " + m_modTableName + " " +
                      "SET turni_reperibilita=:turni_reperibilita "
                      "WHERE id_medico=" + QString::number(m_id) + ";");
        query.bindValue(":turni_reperibilita", temp.join(";"));

        if(!query.exec()) {
            qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        }
    }

    m_modded = true;
}

void CompetenzaData::resetMods()
{
    QSqlQuery query;
    query.prepare("UPDATE " + m_modTableName + " " +
                  "SET guardie_diurne=:guardie_diurne,guardie_notturne=:guardie_notturne,turni_reperibilita=:turni_reperibilita,dmp=:dmp,altre_assenze=:altre_assenze "
                  "WHERE id_medico=" + QString::number(m_id) + ";");
    query.bindValue(":guardie_diurne", QString());
    query.bindValue(":guardie_notturne", QString());
    query.bindValue(":turni_reperibilita", QString());
    query.bindValue(":dmp", 0);
    query.bindValue(":altre_assenze", QString());

    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
    }

    buildDipendente();
}

void CompetenzaData::addGuardiaDiurnaDay(int day)
{
    if(m_guardiaDiurnaMap.keys().contains(day))
        m_guardiaDiurnaMap.remove(day);
    else
        m_guardiaDiurnaMap[day] = tipoGuardia(QString::number(day));

    calcOreGuardia();
}

void CompetenzaData::addGuardiaNotturnaDay(int day)
{
    if(m_guardiaNotturnaMap.keys().contains(day))
        m_guardiaNotturnaMap.remove(day);
    else
        m_guardiaNotturnaMap[day] = tipoGuardia(QString::number(day));

    calcOreGuardia();
}

int CompetenzaData::orePagate() const
{
    return m_orePagate;
}

QString CompetenzaData::notte() const
{
    int tot = 0;
    QMap<int, GuardiaType>::const_iterator i = m_guardiaNotturnaMap.constBegin();
    while (i != m_guardiaNotturnaMap.constEnd()) {
        if(i.value() != GuardiaType::GrandeFestivita)
            tot += 8 - m_orePagate;
        i++;
    }

    // somma eventuali grandi festività non pagate
    tot += (grFestCount() - numGrFestPagabili()) * (8 - m_orePagate);

    return tot == 0 ? "//" : QString::number(tot);
}

QString CompetenzaData::festivo() const
{
    return  m_guardiaDiurnaMap.count() == 0 ? "//" : QString::number(m_guardiaDiurnaMap.count());
}

QString CompetenzaData::repCount() const
{
    float tot = 0.0;
    QMap<QDate, ValoreRep>::const_iterator i = m_rep.constBegin();
    while (i != m_rep.constEnd()) {
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

    return tot == 0.0 ? "//" : QString::number(tot);
}

QString CompetenzaData::oreGrep()
{
    if(m_dipendente->minutiGrep() == 0)
        return QString();

    return inOrario(m_dipendente->minutiGrep());
}

int CompetenzaData::numGrFestPagabili() const
{
    if(differenzaMin() <= 0)
        return 0;

    int numGrFest = 0;

    QMap<int, GuardiaType>::const_iterator i = guardiaNotturnaMap().constBegin();
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
}

int CompetenzaData::numOreGuarPagabili() const
{
    int totMin = differenzaMin();

    if(totMin <= 0)
        return 0;

    totMin -= numGrFestPagabili() * 12 * 60;

    int numOreGuarNott = numGuar();

    int totMinArrot = (totMin % 60) >= m_arrotondamento ? totMin/60+1 : totMin/60;
    int oreGuar = numOreGuarNott * m_orePagate;
    if(oreGuar <= totMinArrot)
        return oreGuar;

    return totMinArrot;
}

int CompetenzaData::numGuar() const
{
    int num = 0;
    QMap<int, GuardiaType>::const_iterator i = guardiaNotturnaMap().constBegin();
    while(i != guardiaNotturnaMap().constEnd()) {
        if(i.value() != GuardiaType::GrandeFestivita)
            num++;
        i++;
    }

    return num;
}

// numero guardie con grandi festività non pagate
int CompetenzaData::numGuarGFNonPag() const
{
    return numGuar() + (grFestCount() - numGrFestPagabili());
}

int CompetenzaData::numOreGuarFesENot() const
{
    if((g_n_fes_F() + g_n_fes_S() + g_n_fes_D()) >= numOreGuarPagabili())
        return numOreGuarPagabili();
    return (g_n_fes_F() + g_n_fes_S() + g_n_fes_D());
}

int CompetenzaData::numOreGuarFesONot() const
{
    int restoOre = numOreGuarPagabili() - numOreGuarFesENot();
    if(restoOre <= 0)
        return 0;

    if((g_n_fer_F() + g_n_fer_S() + g_n_fer_D() + g_d_fes_F() + g_d_fes_S() + g_d_fes_D()) >= restoOre)
        return restoOre;
    return (g_n_fer_F() + g_n_fer_S() + g_n_fer_D() + g_d_fes_F() + g_d_fes_S() + g_d_fes_D());
}

int CompetenzaData::numOreGuarOrd() const
{
    int restoOre = numOreGuarPagabili() - numOreGuarFesONot() - numOreGuarFesENot();
    if(restoOre <= 0)
        return 0;

    if((g_d_fer_F() + g_d_fer_S() + g_d_fer_D()) >= restoOre)
        return restoOre;
    return (g_d_fer_F() + g_d_fer_S() + g_d_fer_D());
}

int CompetenzaData::numOreRepFesENot()
{
    if(r_n_fes() >=  oreProntaDisp())
        return  oreProntaDisp();

    return r_n_fes();
}

int CompetenzaData::numOreRepFesONot()
{
    int restoOre =  oreProntaDisp() - numOreRepFesENot();
    if(restoOre <= 0)
        return 0;

    if((r_n_fer() + r_d_fes()) >= restoOre)
        return restoOre;

    return r_n_fer() + r_d_fes();
}

int CompetenzaData::numOreRepOrd()
{
    int restoOre = oreProntaDisp() - numOreRepFesENot() - numOreRepFesONot();
    if(restoOre <= 0)
        return 0;

    if(r_d_fer() >= restoOre)
        return restoOre;

    return r_d_fer();
}

int CompetenzaData::g_d_fer_F() const
{
    return m_g_d_fer_F;
}

int CompetenzaData::g_d_fer_S() const
{
    return m_g_d_fer_S;
}

int CompetenzaData::g_d_fer_D() const
{
    return m_g_d_fer_D;
}

int CompetenzaData::g_d_fes_F() const
{
    return m_g_d_fes_F;
}

int CompetenzaData::g_d_fes_S() const
{
    return m_g_d_fes_S;
}

int CompetenzaData::g_d_fes_D() const
{
    return m_g_d_fes_D;
}

int CompetenzaData::g_n_fer_F() const
{
    return m_g_n_fer_F;
}

int CompetenzaData::g_n_fer_S() const
{
    return m_g_n_fer_S;
}

int CompetenzaData::g_n_fer_D() const
{
    return m_g_n_fer_D;
}

int CompetenzaData::g_n_fes_F() const
{
    return m_g_n_fes_F;
}

int CompetenzaData::g_n_fes_S() const
{
    return m_g_n_fes_S;
}

int CompetenzaData::g_n_fes_D() const
{
    return m_g_n_fes_D;
}

int CompetenzaData::totOreGuardie() const
{
    return m_totOreGuardie;
}

int CompetenzaData::r_d_fer()
{
    int minuti = 0;
    QMap<int, QPair<int, int> >::const_iterator i = m_dipendente->grep().constBegin();
    while (i != m_dipendente->grep().constEnd()) {
        if(tipoReperibilita(i.key(), i.value().second) == RepType::FerDiu)
            minuti += i.value().first;
        i++;
    }

    return minuti;
}

int CompetenzaData::r_d_fes()
{
    int minuti = 0;
    QMap<int, QPair<int, int> >::const_iterator i = m_dipendente->grep().constBegin();
    while (i != m_dipendente->grep().constEnd()) {
        if(tipoReperibilita(i.key(), i.value().second) == RepType::FesDiu)
            minuti += i.value().first;
        i++;
    }

    return minuti;
}

int CompetenzaData::r_n_fer()
{
    int minuti = 0;
    QMap<int, QPair<int, int> >::const_iterator i = m_dipendente->grep().constBegin();
    while (i != m_dipendente->grep().constEnd()) {
        if(tipoReperibilita(i.key(), i.value().second) == RepType::FerNot)
            minuti += i.value().first;
        i++;
    }

    return minuti;
}

int CompetenzaData::r_n_fes()
{
    int minuti = 0;
    QMap<int, QPair<int, int> >::const_iterator i = m_dipendente->grep().constBegin();
    while (i != m_dipendente->grep().constEnd()) {
        if(tipoReperibilita(i.key(), i.value().second) == RepType::FesNot)
            minuti += i.value().first;
        i++;
    }

    return minuti;
}

QString CompetenzaData::oreStraordinarioGuardie() const
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

GuardiaType CompetenzaData::tipoGuardia(const QString &giorno)
{
    QDate dataCorrente(m_dipendente->anno(), m_dipendente->mese(), giorno.toInt());

    if(m_grandiFestivita.contains(dataCorrente.addDays(1)) || m_grandiFestivita.contains(dataCorrente)) {
        return GuardiaType::GrandeFestivita;
    } else if(dataCorrente.addDays(1).dayOfWeek() == 1) {
        return GuardiaType::Domenica;
    } else if(dataCorrente.addDays(1).dayOfWeek() == 7) {
        return GuardiaType::Sabato;
    }

    return GuardiaType::Feriale;
}

RepType CompetenzaData::tipoReperibilita(const int giorno, const int tipo)
{
    QDate dataCorrente(m_dipendente->anno(), m_dipendente->mese(), giorno);

    RepType value;

    if(tipo == 0) { // notturno
        if(m_grandiFestivita.contains(dataCorrente.addDays(1)) || m_grandiFestivita.contains(dataCorrente)) {
            value = RepType::FesNot;
        } else if(dataCorrente.addDays(1).dayOfWeek() == 1) { // domenica
            value = RepType::FerNot;
        } else if(dataCorrente.addDays(1).dayOfWeek() == 7) { // sabato
            value = RepType::FesNot;
        } else {
            value = RepType::FerNot;
        }
    } else { // diurno
        if(m_grandiFestivita.contains(dataCorrente)) {
            value = RepType::FesDiu;
        } else if(dataCorrente.dayOfWeek() == 7) {
            value = RepType::FesDiu;
        } else {
            value = RepType::FerDiu;
        }
    }

    return value;

//    if(m_grandiFestivita.contains(dataCorrente)) {
//        if(tipo == 0)
//            return RepType::FesNot;
//        return RepType::FesDiu;
//    } else if(dataCorrente.addDays(1).dayOfWeek() == 1) {
//        if(tipo == 0)
//            return RepType::FesNot;
//        return RepType::FesDiu;
//    } else if(dataCorrente.addDays(1).dayOfWeek() == 7) {
//        if(tipo == 0)
//            return RepType::FerNot;
//        return RepType::FerDiu;
//    }

//    if(tipo == 0)
//        return RepType::FerNot;
//    return RepType::FerDiu;
}

void CompetenzaData::caricaGrandiFestivita(int anno)
{
    m_grandiFestivita.clear();

    m_grandiFestivita.append(QDate(anno,1,1));
    m_grandiFestivita.append(QDate(anno,1,6));
    m_grandiFestivita.append(QDate(anno,4,25));
    m_grandiFestivita.append(QDate(anno,5,1));
    m_grandiFestivita.append(QDate(anno,5,30));
    m_grandiFestivita.append(QDate(anno,6,2));
    m_grandiFestivita.append(QDate(anno,8,15));
    m_grandiFestivita.append(QDate(anno,11,1));
    m_grandiFestivita.append(QDate(anno,12,8));
    m_grandiFestivita.append(QDate(anno,12,25));
    m_grandiFestivita.append(QDate(anno,12,26));
}

void CompetenzaData::caricaPasqua(int anno)
{

    int giorno, mese;
    int a, b, c, d, e, m, n;

    switch(anno/100)
    {
        case 15:	// 1583 - 1599 (FALL THROUGH)
        case 16:	// 1600 - 1699
            m=22; n=2; 	break;

        case 17:	// 1700 - 1799
            m=23; n=3; break;

        case 18:	// 1800 - 1899
            m=23; n=4; break;

        case 19:	// 1900 - 1999 (FALL THROUGH)
        case 20:	// 2000 - 2099
            m=24; n=5;break;

        case 21:	// 2100 - 2199
            m=24; n=6; break;

        case 22:	// 2200 - 2299
            m=25; n=0; break;

        case 23:	// 2300 - 2399
            m=26; n=1; break;

        case 24:	// 2400 - 2499
            m=25; n=1; break;
    }

    a=anno%19;
    b=anno%4;
    c=anno%7;
    d=(19*a+m)%30;
    e=(2*b+4*c+6*d+n)%7;
    giorno=d+e;

    if (d+e<10)
    {
        giorno+=22;
        mese=3;
    }

    else
    {
        giorno-=9;
        mese=4;

        if ((giorno==26)||((giorno==25)&&(d==28)&&(e==6)&&(a>10)))
        {
            giorno-=7;
        }
    }

    m_grandiFestivita.append(QDate(anno,mese,giorno));
    m_grandiFestivita.append(QDate(anno,mese,giorno).addDays(1));
}

void CompetenzaData::calcOreGuardia()
{
    m_g_d_fer_F = 0;
    m_g_d_fer_S = 0;
    m_g_d_fer_D = 0;
    m_g_d_fes_F = 0;
    m_g_d_fes_S = 0;
    m_g_d_fes_D = 0;
    m_g_n_fer_F = 0;
    m_g_n_fer_S = 0;
    m_g_n_fer_D = 0;
    m_g_n_fes_F = 0;
    m_g_n_fes_S = 0;
    m_g_n_fes_D = 0;
    m_totOreGuardie = 0;

    QMap<int, GuardiaType>::const_iterator i = m_guardiaNotturnaMap.constBegin();
    while (i != m_guardiaNotturnaMap.constEnd()) {
        switch (i.value()) {
        case GuardiaType::Sabato:
            m_g_d_fer_S += 2;
            m_g_d_fes_S += 2;
            m_g_n_fer_S += 2;
            m_g_n_fes_S += 6;
            m_totOreGuardie += 12;
            break;
        case GuardiaType::Domenica:
            m_g_d_fer_D += 2;
            m_g_d_fes_D += 2;
            m_g_n_fer_D += 6;
            m_g_n_fes_D += 2;
            m_totOreGuardie += 12;
            break;
        case GuardiaType::Feriale:
            m_g_d_fer_F += 4;
            m_g_n_fer_F += 8;
            m_totOreGuardie += 12;
            break;
        default:
            break;
        }
        ++i;
    }

    QMap<int, GuardiaType>::const_iterator i2 = m_guardiaDiurnaMap.constBegin();
    while (i2 != m_guardiaDiurnaMap.constEnd()) {
        m_g_d_fes_F += 12;
        m_totOreGuardie += 12;
        ++i2;
    }
}

void CompetenzaData::getOrePagate()
{
    QSqlQuery query;
    query.prepare("SELECT ore,data FROM unita_ore_pagate WHERE id_unita=" + QString::number(m_unitaId) + ";");
    if(!query.exec()) {
        qDebug() << "ERROR: " << query.lastQuery() << " : " << query.lastError();
        return;
    }
    while(query.next()) {
        m_orePagate = query.value(0).toInt();
    }
}

int CompetenzaData::grFestCount() const
{
    int numGrFest = 0;

    QMap<int, GuardiaType>::const_iterator i = guardiaNotturnaMap().constBegin();
    while(i != guardiaNotturnaMap().constEnd()) {
        if(i.value() == GuardiaType::GrandeFestivita)
            numGrFest++;
        i++;
    }
    return numGrFest;
}

Competenza::Competenza(const QString &tableName, const int id, QObject *parent) : QObject(parent), data(new CompetenzaData(tableName, id))
{

}

Competenza::Competenza(const Competenza &rhs) : data(rhs.data)
{

}

Competenza &Competenza::operator=(const Competenza &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

Competenza::~Competenza()
{

}

QString Competenza::badgeNumber() const
{
    return data->badgeNumber();
}

QString Competenza::name() const
{
    return data->name();
}

QDate Competenza::dataIniziale() const
{
    return data->dataIniziale();
}

QDate Competenza::dataFinale() const
{
    return data->dataFinale();
}

QString Competenza::giorniLavorati() const
{
    return data->giorniLavorati();
}

QString Competenza::giorniLavorativi() const
{
    return data->giorniLavorativi();
}

QString Competenza::assenzeTotali() const
{
    return data->assenzeTotali();
}

QString Competenza::orarioGiornaliero()
{
    return data->orarioGiornaliero();
}

QString Competenza::oreDovute()
{
    return data->oreDovute();
}

QString Competenza::oreEffettuate()
{
    return data->oreEffettuate();
}

int Competenza::oreProntaDisp()
{
    return data->oreProntaDisp();
}

QString Competenza::differenzaOre()
{
    return data->differenzaOre();
}

QString Competenza::differenzaOreSenzaDmp()
{
    return data->differenzaOreSenzaDmp();
}

QString Competenza::deficitOrario()
{
    return data->deficitOrario();
}

int Competenza::minutiAltreAssenze() const
{
    return data->minutiAltreAssenze();
}

QString Competenza::oreAltreAssenze()
{
    return data->oreAltreAssenze();
}

QString Competenza::ferieCount() const
{
    return data->ferieCount();
}

QList<QDate> Competenza::ferieDates() const
{
    return data->ferieDates();
}

QString Competenza::congediCount() const
{
    return data->congediCount();
}

QList<QDate> Competenza::congediDates() const
{
    return data->congediDates();
}

QString Competenza::malattiaCount() const
{
    return data->malattiaCount();
}

QList<QDate> Competenza::malattiaDates() const
{
    return data->malattiaDates();
}

QString Competenza::rmpCount() const
{
    return data->rmpCount();
}

QList<QDate> Competenza::rmpDates() const
{
    return data->rmpDates();
}

QString Competenza::rmcCount() const
{
    return data->rmcCount();
}

QList<QDate> Competenza::rmcDates() const
{
    return data->rmcDates();
}

QList<QDate> Competenza::gdDates() const
{
    return data->gdDates();
}

QList<QDate> Competenza::gnDates() const
{
    return data->gnDates();
}

QMap<int, GuardiaType> Competenza::guardiaDiurnaMap() const
{
    return data->guardiaDiurnaMap();
}

QMap<int, GuardiaType> Competenza::guardiaNotturnaMap() const
{
    return data->guardiaNotturnaMap();
}

void Competenza::addGuardiaDiurnaDay(int day)
{
    data->addGuardiaDiurnaDay(day);
}

void Competenza::addGuardiaNotturnaDay(int day)
{
    data->addGuardiaNotturnaDay(day);
}

void Competenza::setDmp(const QTime &time)
{
    data->setDmp(time);
}

QTime Competenza::dmp() const
{
    return data->dmp();
}

void Competenza::setRep(const QMap<QDate, ValoreRep> &map)
{
    data->setRep(map);
}

QMap<QDate, ValoreRep> Competenza::rep() const
{
    return data->rep();
}

QList<QDate> Competenza::altreAssenzeDates() const
{
    return data->altreAssenzeDates();
}

void Competenza::setAltreAssenze(const QList<QDate> &assenze)
{
    data->setAltreAssenze(assenze);
}

bool Competenza::isModded() const
{
    return data->isModded();
}

bool Competenza::isRestorable() const
{
    return data->isRestorable();
}

void Competenza::saveMods()
{
    data->saveMods();
}

void Competenza::resetMods()
{
    data->resetMods();
}

int Competenza::orePagate() const
{
    return data->orePagate();
}

QString Competenza::notte() const
{
    return data->notte();
}

QString Competenza::festivo() const
{
    return data->festivo();
}

QString Competenza::repCount() const
{
    return data->repCount();
}

QString Competenza::oreGrep()
{
    return data->oreGrep();
}

int Competenza::numGrFestPagabili() const
{
    return data->numGrFestPagabili();
}

int Competenza::numOreGuarPagabili() const
{
    return data->numOreGuarPagabili();
}

int Competenza::numGuar() const
{
    return data->numGuar();
}

int Competenza::numGuarGFNonPag() const
{
    return data->numGuarGFNonPag();
}

int Competenza::numOreGuarFesENot() const
{
    return data->numOreGuarFesENot();
}

int Competenza::numOreGuarFesONot() const
{
    return data->numOreGuarFesONot();
}

int Competenza::numOreGuarOrd() const
{
    return data->numOreGuarOrd();
}

int Competenza::numOreRepFesENot()
{
    return data->numOreRepFesENot();
}

int Competenza::numOreRepFesONot()
{
    return data->numOreRepFesONot();
}

int Competenza::numOreRepOrd()
{
    return data->numOreRepOrd();
}

int Competenza::g_d_fer_F() const
{
    return data->g_d_fer_F();
}

int Competenza::g_d_fer_S() const
{
    return data->g_d_fer_S();
}

int Competenza::g_d_fer_D() const
{
    return data->g_d_fer_D();
}

int Competenza::g_d_fes_F() const
{
    return data->g_d_fes_F();
}

int Competenza::g_d_fes_S() const
{
    return data->g_d_fes_S();
}

int Competenza::g_d_fes_D() const
{
    return data->g_d_fes_D();
}

int Competenza::g_n_fer_F() const
{
    return data->g_n_fer_F();
}

int Competenza::g_n_fer_S() const
{
    return data->g_n_fer_S();
}

int Competenza::g_n_fer_D() const
{
    return data->g_n_fer_D();
}

int Competenza::g_n_fes_F() const
{
    return data->g_n_fes_F();
}

int Competenza::g_n_fes_S() const
{
    return data->g_n_fes_S();
}

int Competenza::g_n_fes_D() const
{
    return data->g_n_fes_D();
}

int Competenza::totOreGuardie() const
{
    return data->totOreGuardie();
}

int Competenza::r_d_fer()
{
    return data->r_d_fer();
}

int Competenza::r_d_fes()
{
    return data->r_d_fes();
}

int Competenza::r_n_fer()
{
    return data->r_n_fer();
}

int Competenza::r_n_fes()
{
    return data->r_n_fes();
}

QString Competenza::oreStraordinarioGuardie() const
{
    return data->oreStraordinarioGuardie();
}
