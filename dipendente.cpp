/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "dipendente.h"
#include "utilities.h"

#include <QDebug>

class DipendenteData : public QSharedData
{
public:
    int m_anno;
    int m_mese;
    QString m_nome;
    int m_matricola;
    int m_unita;
    QVector<int> m_riposi;
    QVector<int> m_guardieDiurne;
    QVector<int> m_guardieNotturne;
    QMultiMap<int, QPair<int, int> > m_grep;
    QVector<int> m_rmp;
    QVector<int> m_rmc;
    QVector<int> m_ferie;
    int m_numGiorniCartellino;
    QVector<int> m_scoperti;
    QVector<int> m_congedi;
    QVector<int> m_malattia;
    QVector<int> m_indennitaFestive;
    QVector<int> m_indennitaNotturne;
    QMap<QString, QPair<QVector<int>, int> > m_altreCausali;
    int m_minutiFatti;
    int m_minutiCongedi;
    int m_minutiGiornalieriVeri;
    int m_minutiGrep;
    int m_minutiEccr;
    int m_minutiGuar;
    int m_minutiRmc;
};

Dipendente::Dipendente()
    : data(new DipendenteData)
{
    resetProperties();
}

Dipendente::Dipendente(const Dipendente &rhs)
    : data(rhs.data)
{

}

Dipendente &Dipendente::operator=(const Dipendente &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

bool Dipendente::operator==(const Dipendente &rhs) const
{
    if(this == &rhs)
        return true;

    return data->m_anno == rhs.anno() &&
            data->m_mese == rhs.mese() &&
            data->m_nome == rhs.nome() &&
            data->m_matricola == rhs.matricola() &&
            data->m_unita == rhs.unita() &&
            data->m_riposi == rhs.riposi() &&
            data->m_guardieDiurne == rhs.guardieDiurne() &&
            data->m_guardieNotturne == rhs.guardieNotturne() &&
            data->m_grep == rhs.grep() &&
            data->m_rmp == rhs.rmp() &&
            data->m_rmc == rhs.rmc() &&
            data->m_ferie == rhs.ferie() &&
            data->m_numGiorniCartellino == rhs.numGiorniCartellino() &&
            data->m_scoperti == rhs.scoperti() &&
            data->m_indennitaFestive == rhs.indennitaFestiva() &&
            data->m_indennitaNotturne == rhs.indennitaNotturna() &&
            data->m_congedi == rhs.congedi() &&
            data->m_malattia == rhs.malattia() &&
            data->m_altreCausali == rhs.altreCausali() &&
            data->m_minutiFatti == rhs.minutiFatti() &&
            data->m_minutiCongedi == rhs.minutiCongedi() &&
            data->m_minutiGiornalieriVeri == rhs.minutiGiornalieriVeri() &&
            data->m_minutiGrep == rhs.minutiGrep() &&
            data->m_minutiEccr == rhs.minutiEccr() &&
            data->m_minutiGuar == rhs.minutiGuar() &&
            data->m_minutiRmc == rhs.minutiRmc();
}

Dipendente::~Dipendente()
{

}

void Dipendente::resetProperties()
{
    data->m_riposi.clear();
    data->m_minutiFatti = 0;
    data->m_minutiGiornalieriVeri = 0;
    data->m_minutiCongedi = 0;
    data->m_minutiEccr = 0;
    data->m_minutiGrep = 0;
    data->m_minutiGuar = 0;
    data->m_minutiRmc = 0;
    data->m_numGiorniCartellino = 0;
    data->m_ferie.clear();
    data->m_congedi.clear();
    data->m_malattia.clear();
    data->m_guardieDiurne.clear();
    data->m_guardieNotturne.clear();
    data->m_grep.clear();
    data->m_rmc.clear();
    data->m_rmp.clear();
    data->m_altreCausali.clear();
    data->m_indennitaFestive.clear();
    data->m_indennitaNotturne.clear();
}

QVector<int> Dipendente::indennitaFestiva() const
{
    return data->m_indennitaFestive;
}

void Dipendente::addIndennitaFestiva(int giorno)
{
    data->m_indennitaFestive <<  giorno;
}

QVector<int> Dipendente::indennitaNotturna() const
{
    return data->m_indennitaNotturne;
}

void Dipendente::addIndennitaNotturna(int giorno)
{
    data->m_indennitaNotturne <<  giorno;
}

int Dipendente::anno() const
{
    return data->m_anno;
}

void Dipendente::setAnno(int anno)
{
    data->m_anno = anno;
}

int Dipendente::mese() const
{
    return data->m_mese;
}

void Dipendente::setMese(int mese)
{
    data->m_mese = mese;
}

QString Dipendente::nome() const
{
    return data->m_nome;
}

void Dipendente::setNome(QString nome)
{
    data->m_nome = nome;
}

int Dipendente::matricola() const
{
    return data->m_matricola;
}

void Dipendente::setMatricola(int matricola)
{
    data->m_matricola = matricola;
}

int Dipendente::unita() const
{
    return data->m_unita;
}

void Dipendente::setUnita(int unita)
{
    data->m_unita = unita;
}

int Dipendente::riposiCount() const
{
    if(data->m_minutiGiornalieriVeri > Utilities::m_maxMinutiGiornalieri) {
        return data->m_riposi.count();
    }

    if(data->m_riposi.count() > 0)
        return data->m_riposi.at(0);

    return 0;
}

QVector<int> Dipendente::riposi() const
{
    return data->m_riposi;
}

void Dipendente::addRiposo(int giorno)
{
    data->m_riposi << giorno;
}

QVector<int> Dipendente::guardieDiurne() const
{
    return data->m_guardieDiurne;
}

void Dipendente::addGuardiaDiurna(int giorno)
{
    data->m_guardieDiurne.append(giorno);
}

QVector<int> Dipendente::guardieNotturne() const
{
    return data->m_guardieNotturne;
}

void Dipendente::addGuardiaNotturna(int giorno)
{
    data->m_guardieNotturne.append(giorno);
}

QMultiMap<int, QPair<int, int> > Dipendente::grep() const
{
    return data->m_grep;
}

void Dipendente::addGrep(int giorno, int minuti, int tipo)
{
    QPair<int, int> value;
    value.first = minuti;
    value.second = tipo;
    data->m_grep.insert(giorno,value);
}

QVector<int> Dipendente::rmp() const
{
    return data->m_rmp;
}

void Dipendente::addRmp(int giorno)
{
    data->m_rmp.append(giorno);
}

QVector<int> Dipendente::rmc() const
{
    return data->m_rmc;
}

void Dipendente::addRmc(int giorno)
{
    data->m_rmc.append(giorno);
}

QVector<int> Dipendente::ferie() const
{
    return data->m_ferie;
}

void Dipendente::addFerie(int giorno)
{
    data->m_ferie.append(giorno);
}

void Dipendente::addNumGiorniCartellino(int num)
{
    data->m_numGiorniCartellino = num;
}

QVector<int> Dipendente::scoperti() const
{
    return data->m_scoperti;
}

void Dipendente::addScoperto(int giorno)
{
    data->m_scoperti.append(giorno);
}

QVector<int> Dipendente::congedi() const
{
    return data->m_congedi;
}

void Dipendente::addCongedo(int giorno)
{
    data->m_congedi.append(giorno);
}

QVector<int> Dipendente::malattia() const
{
    return data->m_malattia;
}

void Dipendente::addMalattia(int giorno)
{
    data->m_malattia.append(giorno);
}

QMap<QString, QPair<QVector<int>, int> > Dipendente::altreCausali() const
{
    return data->m_altreCausali;
}

int Dipendente::altreCausaliCount() const
{
    int count = 0;

    auto i = data->m_altreCausali.constBegin();
    while(i != data->m_altreCausali.constEnd()) {
        count += i.value().first.count();
        i++;
    }

    return count;
}

void Dipendente::addAltraCausale(QString causale, QString date, int minuti)
{
    if(data->m_altreCausali.keys().contains(causale)) {
        auto value = data->m_altreCausali[causale];
        value.first << Utilities::stringlistToVectorInt(date.split("~"));
        value.second += minuti;
        data->m_altreCausali[causale] = value;
    } else {
        QPair<QVector<int>, int> value;
        value.first << Utilities::stringlistToVectorInt(date.split("~"));
        value.second = minuti;
        data->m_altreCausali[causale] = value;
    }
}

int Dipendente::minutiCausale(const QString &causale)
{
    if(data->m_altreCausali.keys().contains(causale))
        return data->m_altreCausali.value(causale).second;
    return 0;
}

int Dipendente::minutiFatti() const
{
    return data->m_minutiFatti;
}

void Dipendente::addMinutiFatti(int minuti)
{
    data->m_minutiFatti += minuti;
}

int Dipendente::minutiCongedi() const
{
    return data->m_minutiCongedi;
}

void Dipendente::addMinutiCongedo(int minuti)
{
    data->m_minutiCongedi += minuti;
}

int Dipendente::minutiGiornalieri() const
{
    if(data->m_minutiGiornalieriVeri > Utilities::m_maxMinutiGiornalieri)
        return data->m_minutiGiornalieriVeri / (data->m_numGiorniCartellino - riposiCount());
    return data->m_minutiGiornalieriVeri;
}

int Dipendente::minutiGiornalieriVeri() const
{
    return data->m_minutiGiornalieriVeri;
}

int Dipendente::numGiorniCartellino() const
{
    return data->m_numGiorniCartellino;
}

void Dipendente::setMinutiGiornalieri(int minuti)
{
    data->m_minutiGiornalieriVeri = minuti;
}

void Dipendente::setNumGiorniCartellino(int num)
{
    data->m_numGiorniCartellino = num;
}

int Dipendente::minutiGrep() const
{
    return data->m_minutiGrep;
}

void Dipendente::addMinutiGrep(int minuti)
{
    data->m_minutiGrep += minuti;
}

int Dipendente::minutiEccr() const
{
    return data->m_minutiEccr;
}

void Dipendente::addMinutiEccr(int minuti)
{
    data->m_minutiEccr += minuti;
}

int Dipendente::minutiGuar() const
{
    return data->m_minutiGuar;
}

void Dipendente::addMinutiGuar(int minuti)
{
    data->m_minutiGuar += minuti;
}

int Dipendente::minutiRmc() const
{
    return data->m_minutiRmc;
}

void Dipendente::addMinutiRmc(int minuti)
{
    data->m_minutiRmc += minuti;
}

void Dipendente::setRiposi(QVector<int> giorni)
{
    data->m_riposi = giorni;
}
