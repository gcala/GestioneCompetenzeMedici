/*
 *  SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>
 * 
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cartellinocompleto.h"
#include "giornocartellinocompleto.h"
#include "sqlqueries.h"
#include "totalicartellinocompleto.h"

#include <QTime>
#include <QDebug>

class CartellinoCompletoData : public QSharedData
{
public:
    QList<GiornoCartellinoCompleto> m_giorni;
    int m_anno;
    int m_mese;
    int m_matricola;
    TotaliCartellinoCompleto m_totali;

    QMap<QString, int> m_causali;
    QMap<QString, QVector<int>> m_giorniCausali;
    int m_ordinarie;
    int m_recuperate;
    int m_giustificate;
    int m_saldo;
    QVector<int> m_strlList;
};

CartellinoCompleto::CartellinoCompleto(QObject *parent)
    : QObject(parent)
    , data(new CartellinoCompletoData)
{

}

CartellinoCompleto::CartellinoCompleto(int anno, int mese, int matricola)
    : data(new CartellinoCompletoData)
{
    data->m_anno = anno;
    data->m_mese = mese;
    data->m_matricola = matricola;
//    data->m_giorni = SqlQueries::getGiorniCartellinoCompleto(QString::number(anno) + QString::number(mese).rightJustified(2, '0'), matricola);
//    data->m_totali = SqlQueries::getDisponibile(matricola, anno, mese);
    data->m_anno = 0;
    data->m_mese = 0;
    data->m_matricola = 0;
    data->m_ordinarie = 0;
    data->m_recuperate = 0;
    data->m_giustificate = 0;
    data->m_causali.clear();
    data->m_giorniCausali.clear();

    aggiornaValori();
}

CartellinoCompleto::CartellinoCompleto(const CartellinoCompleto &rhs) : data(rhs.data)
{

}

CartellinoCompleto &CartellinoCompleto::operator=(const CartellinoCompleto &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

bool CartellinoCompleto::operator==(const CartellinoCompleto &rhs) const
{
    if(this == &rhs)
        return true;
    return totali() == rhs.totali() &&
            ordinarie() == rhs.ordinarie() &&
            recuperate() == rhs.recuperate() &&
            giustificate() == rhs.giustificate() &&
            strlPagato() == rhs.strlPagato() &&
            saldo() == rhs.saldo() &&
            causali() == rhs.causali() &&
            giorniCausali() == rhs.giorniCausali();
}

CartellinoCompleto::~CartellinoCompleto()
{

}

void CartellinoCompleto::addGiorno(const GiornoCartellinoCompleto &giorno)
{
    data->m_giorni << giorno;
}

void CartellinoCompleto::addTotali(const TotaliCartellinoCompleto &totali)
{
    data->m_totali = totali;
}

void CartellinoCompleto::aggiornaValori()
{
    for(const auto &giorno : qAsConst(data->m_giorni)) {
        // ordinarie
        if(giorno.ordinario().isValid())
            data->m_ordinarie += (giorno.ordinario().hour() * 60 + giorno.ordinario().minute());

        // recuperate
        if(giorno.recuperate().isValid())
            data->m_recuperate += (giorno.recuperate().hour() * 60 + giorno.recuperate().minute());

        if(!giorno.causale1().isEmpty()) {
            data->m_causali[giorno.causale1()] += (giorno.ore1().hour() * 60 + giorno.ore1().minute());
            data->m_giorniCausali[giorno.causale1()] << giorno.giorno();
            if(giorno.causale1() == "ECCR" || giorno.causale1() == "STRL" || giorno.causale1() == "GUAR") {
                if(giorno.causale1() == "STRL")
                    data->m_strlList << giorno.ore1().hour() * 60 + giorno.ore1().minute();
                data->m_recuperate += (giorno.ore1().hour() * 60 + giorno.ore1().minute());
            } else if(giorno.causale1() == "RMC" || giorno.causale1() == "GREP"){
                // non fare niente
            } else {
                data->m_giustificate += (giorno.ore1().hour() * 60 + giorno.ore1().minute());
            }
        }
        if(!giorno.causale2().isEmpty()) {
            data->m_causali[giorno.causale2()] += (giorno.ore2().hour() * 60 + giorno.ore2().minute());
            data->m_giorniCausali[giorno.causale2()] << giorno.giorno();
            if(giorno.causale2() == "ECCR" || giorno.causale2() == "STRL" || giorno.causale2() == "GUAR") {
                if(giorno.causale2() == "STRL")
                    data->m_strlList << giorno.ore2().hour() * 60 + giorno.ore2().minute();
                data->m_recuperate += (giorno.ore2().hour() * 60 + giorno.ore2().minute());
            } else if(giorno.causale2() == "RMC" || giorno.causale1() == "GREP"){
                // non fare niente
            } else {
                data->m_giustificate += (giorno.ore2().hour() * 60 + giorno.ore2().minute());
            }
        }
        if(!giorno.causale3().isEmpty()) {
            data->m_causali[giorno.causale3()] += (giorno.ore3().hour() * 60 + giorno.ore3().minute());
            data->m_giorniCausali[giorno.causale3()] << giorno.giorno();
            if(giorno.causale3() == "ECCR" || giorno.causale3() == "STRL" || giorno.causale3() == "GUAR") {
                if(giorno.causale3() == "STRL")
                    data->m_strlList << giorno.ore3().hour() * 60 + giorno.ore3().minute();
                data->m_recuperate += (giorno.ore3().hour() * 60 + giorno.ore3().minute());
            } else if(giorno.causale3() == "RMC" || giorno.causale1() == "GREP"){
                // non fare niente
            } else {
                data->m_giustificate += (giorno.ore3().hour() * 60 + giorno.ore3().minute());
            }
        }
    }
}

QList<GiornoCartellinoCompleto> CartellinoCompleto::giorni() const
{
    return data->m_giorni;
}

TotaliCartellinoCompleto CartellinoCompleto::totali() const
{
    return data->m_totali;
}

int CartellinoCompleto::ordinarie() const
{
    return data->m_ordinarie;
}

int CartellinoCompleto::recuperate() const
{
    return data->m_recuperate;
}

int CartellinoCompleto::giustificate() const
{
    return data->m_giustificate;
}

int CartellinoCompleto::strlPagato() const
{
    const int saldoSenzaStrl = data->m_ordinarie + data->m_recuperate + data->m_giustificate - data->m_totali.disponibile() - data->m_causali.value("STRL");

    if(data->m_causali.value("STRL") > 0) {
        if(saldoSenzaStrl >= 0) {
            return arrotondamentoStrl(data->m_strlList);
        } else {
            if(qAbs(saldoSenzaStrl) >= arrotondamentoStrl(data->m_strlList))
                return 0;
            return arrotondamento(arrotondamentoStrl(data->m_strlList) + saldoSenzaStrl);
        }
    }

    return 0;
}

int CartellinoCompleto::saldo() const
{
    return data->m_ordinarie + data->m_recuperate + data->m_giustificate - data->m_totali.disponibile() - arrotondamentoStrl(data->m_strlList) + (arrotondamentoStrl(data->m_strlList) - strlPagato());
}

int CartellinoCompleto::minutiCausale(const QString &causale) const
{
    return data->m_causali.value(causale);
}

QMap<QString, int> CartellinoCompleto::causali() const
{
    return data->m_causali;
}

QMap<QString, QVector<int> > CartellinoCompleto::giorniCausali() const
{
    return data->m_giorniCausali;
}

QStringList CartellinoCompleto::timbratureGiorno(const int giorno)
{
    QStringList list;
    for(const auto &g: qAsConst(data->m_giorni)) {
        if(g.giorno() == giorno) {
//            list << g.entrata1().text();
//            list << g.uscita1().text();
//            list << g.entrata2().text();
//            list << g.uscita2().text();
//            list << g.entrata3().text();
//            list << g.uscita3().text();
//            list << g.entrata4().text();
//            list << g.uscita4().text();
            // entrata1
            if(g.entrata1().text().isEmpty()) {
                if(!g.uscita1().text().isEmpty()) {
                    list << g.entrata1().text();
                }
            } else {
                list << g.entrata1().text();
            }
            // uscita1
            if(g.uscita1().text().isEmpty()) {
                if(!g.entrata2().text().isEmpty()) {
                    list << g.uscita1().text();
                }
            } else {
                list << g.uscita1().text();
            }
            // entrata2
            if(g.entrata2().text().isEmpty()) {
                if(!g.uscita2().text().isEmpty()) {
                    list << g.entrata2().text();
                }
            } else {
                list << g.entrata2().text();
            }
            // uscita2
            if(g.uscita2().text().isEmpty()) {
                if(!g.entrata3().text().isEmpty()) {
                    list << g.uscita2().text();
                }
            } else {
                list << g.uscita2().text();
            }
            // entrata3
            if(g.entrata3().text().isEmpty()) {
                if(!g.uscita3().text().isEmpty()) {
                    list << g.entrata3().text();
                }
            } else {
                list << g.entrata3().text();
            }
            // uscita3
            if(g.uscita3().text().isEmpty()) {
                if(!g.entrata4().text().isEmpty()) {
                    list << g.uscita3().text();
                }
            } else {
                list << g.uscita3().text();
            }
            // entrata4
            if(g.entrata4().text().isEmpty()) {
                if(!g.uscita4().text().isEmpty()) {
                    list << g.entrata4().text();
                }
            } else {
                list << g.entrata4().text();
            }
            // uscita4
            if(!g.uscita4().text().isEmpty()) {
                list << g.uscita4().text();
            }
         }
    }
    return list;
}

int CartellinoCompleto::arrotondamento(int minuti) const
{
    int ore = 0;

    if(minuti > 0) {
        ore = minuti / 60;
        minuti -= ore * 60;
        if(minuti < 20) { // era 21
            minuti = 0;
        } else if(minuti < 45 && minuti >= 20) { // era 21 era 46
            minuti = 30;
        } else if(minuti >= 45) { // era 46
            minuti = 0;
            ore++;
        }
    }

    return ore * 60 + minuti;
}

int CartellinoCompleto::arrotondamentoStrl(QVector<int> strlList) const
{
    int minuti = 0;
    for(auto strl : strlList) {
        minuti += arrotondamento(strl);
    }
    return minuti;
}
