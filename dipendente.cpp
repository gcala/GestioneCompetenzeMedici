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

#include "dipendente.h"

#include <QDebug>

class DipendenteData : public QSharedData
{
public:
    DipendenteData() {
        m_anno = 0;
        m_mese = 0;
        resetProperties();
    }

    int anno() const;
    void setAnno(int anno);
    int mese() const;
    void setMese(int mese);
    QString nome() const;
    void setNome(QString nome);
    QString matricola() const;
    void setMatricola(QString matricola);
    QString unita() const;
    void setUnita(QString unita);
    int riposi() const;
    void addRiposi(int num);
    QStringList guardieDiurne() const;
    void addGuardiaDiurna(QString date);
    QStringList guardieNotturne() const;
    void addGuardiaNotturna(QString date);
    QMap<int, QPair<int, int> > grep() const;
    void addGrep(int giorno, int minuti, int tipo);
    QStringList rmp() const;
    void addRmp(QString date);
    QStringList rmc() const;
    void addRmc(QString date);
    QStringList ferie() const;
    void addFerie(QString date);
    QStringList congedi() const;
    void addCongedo(QString date);
    QStringList malattia() const;
    void addMalattia(QString date);
    QMap<QString, QPair<QStringList, int> > altreAssenze() const;
    int altreAssenzeCount() const;
    void addAltraAssenza(QString causale, QString date, int minuti);
    int minutiFatti() const;
    void addMinutiFatti(int minuti);
    int minutiCongedi() const;
    void addMinutiCongedo(int minuti);
    int minutiAssenza() const;
    void addMinutiAssenza(int minuti);
    int minutiGiornalieri() const;
    void setMinutiGiornalieri(int minuti);
    int minutiGrep() const;
    void addMinutiGrep(int minuti);
    int minutiEccr() const;
    void addMinutiEccr(int minuti);
    int minutiGuar() const;
    void addMinutiGuar(int minuti);
    int minutiRmc() const;
    void addMinutiRmc(int minuti);
    void resetProperties();

private:
    int m_anno;
    int m_mese;
    QString m_nome;
    QString m_matricola;
    QString m_unita;
    int m_riposi;
    QStringList m_guardieDiurne;
    QStringList m_guardieNotturne;
    QMap<int, QPair<int, int> > m_grep;
    QStringList m_rmp;
    QStringList m_rmc;
    QStringList m_ferie;
    QStringList m_congedi;
    QStringList m_malattia;
    QMap<QString, QPair<QStringList, int> > m_altreAssenze;
    int m_minutiFatti;
    int m_minutiCongedi;
    int m_minutiGiornalieri;
    int m_minutiGrep;
    int m_minutiEccr;
    int m_minutiGuar;
    int m_minutiRmc;
};

int DipendenteData::anno() const
{
    return m_anno;
}

void DipendenteData::setAnno(int anno)
{
    m_anno = anno;
}

int DipendenteData::mese() const
{
    return m_mese;
}

void DipendenteData::setMese(int mese)
{
    m_mese = mese;
}

QString DipendenteData::nome() const
{
    return m_nome;
}

void DipendenteData::setNome(QString nome)
{
    m_nome = nome;
}

QString DipendenteData::matricola() const
{
    return m_matricola;
}

void DipendenteData::setMatricola(QString matricola)
{
    m_matricola = matricola;
}

QString DipendenteData::unita() const
{
    return m_unita;
}

void DipendenteData::setUnita(QString unita)
{
    m_unita = unita;
}

int DipendenteData::riposi() const
{
    return m_riposi;
}

void DipendenteData::addRiposi(int num)
{
    m_riposi += num;
}

QStringList DipendenteData::guardieDiurne() const
{
    return m_guardieDiurne;
}

void DipendenteData::addGuardiaDiurna(QString date)
{
    m_guardieDiurne.append(date);
}

QStringList DipendenteData::guardieNotturne() const
{
    return m_guardieNotturne;
}

void DipendenteData::addGuardiaNotturna(QString date)
{
    m_guardieNotturne.append(date);
}

QMap<int, QPair<int, int> > DipendenteData::grep() const
{
    return m_grep;
}

void DipendenteData::addGrep(int giorno, int minuti, int tipo)
{
    QPair<int, int> value;
    value.first = minuti;
    value.second = tipo;
    m_grep.insertMulti(giorno,value);
}

QStringList DipendenteData::rmp() const
{
    return m_rmp;
}

void DipendenteData::addRmp(QString date)
{
    m_rmp.append(date);
}

QStringList DipendenteData::rmc() const
{
    return m_rmc;
}

void DipendenteData::addRmc(QString date)
{
    m_rmc.append(date);
}

QStringList DipendenteData::ferie() const
{
    return m_ferie;
}

void DipendenteData::addFerie(QString date)
{
    m_ferie.append(date);
}

QStringList DipendenteData::congedi() const
{
    return m_congedi;
}

void DipendenteData::addCongedo(QString date)
{
    m_congedi.append(date);
}

QStringList DipendenteData::malattia() const
{
    return m_malattia;
}

void DipendenteData::addMalattia(QString date)
{
    m_malattia.append(date);
}

QMap<QString, QPair<QStringList, int> > DipendenteData::altreAssenze() const
{
    return m_altreAssenze;
}

int DipendenteData::altreAssenzeCount() const
{
    int count = 0;

    QMap<QString, QPair<QStringList, int> >::const_iterator i = m_altreAssenze.constBegin();
    while(i != m_altreAssenze.constEnd()) {
        count += i.value().first.count();
        i++;
    }

    return count;
}

void DipendenteData::addAltraAssenza(QString causale, QString date, int minuti)
{
    if(m_altreAssenze.keys().contains(causale)) {
        QPair<QStringList, int> value = m_altreAssenze[causale];
        value.first << date.split("~");
        value.second += minuti;
        m_altreAssenze[causale] = value;
    } else {
        QPair<QStringList, int> value;
        value.first << date.split("~");
        value.second = minuti;
        m_altreAssenze[causale] = value;
    }
}

int DipendenteData::minutiFatti() const
{
    return m_minutiFatti;
}

void DipendenteData::addMinutiFatti(int minuti)
{
    m_minutiFatti += minuti;
}

int DipendenteData::minutiCongedi() const
{
    return m_minutiCongedi;
}

void DipendenteData::addMinutiCongedo(int minuti)
{
    m_minutiCongedi += minuti;
}

int DipendenteData::minutiGiornalieri() const
{
    return m_minutiGiornalieri;
}

void DipendenteData::setMinutiGiornalieri(int minuti)
{
    m_minutiGiornalieri = minuti;
}

int DipendenteData::minutiGrep() const
{
    return m_minutiGrep;
}

void DipendenteData::addMinutiGrep(int minuti)
{
    m_minutiGrep += minuti;
}

int DipendenteData::minutiEccr() const
{
    return m_minutiEccr;
}

void DipendenteData::addMinutiEccr(int minuti)
{
    m_minutiEccr += minuti;
}

int DipendenteData::minutiGuar() const
{
    return m_minutiGuar;
}

void DipendenteData::addMinutiGuar(int minuti)
{
    m_minutiGuar += minuti;
}

int DipendenteData::minutiRmc() const
{
    return m_minutiRmc;
}

void DipendenteData::addMinutiRmc(int minuti)
{
    m_minutiRmc += minuti;
}

void DipendenteData::resetProperties()
{
    m_riposi = 0;
    m_minutiFatti = 0;
    m_minutiGiornalieri = 0;
    m_minutiCongedi = 0;
    m_minutiEccr = 0;
    m_minutiGrep = 0;
    m_minutiGuar = 0;
    m_minutiRmc = 0;
    m_ferie.clear();
    m_congedi.clear();
    m_malattia.clear();
    m_guardieDiurne.clear();
    m_guardieNotturne.clear();
    m_grep.clear();
    m_rmc.clear();
    m_rmp.clear();
    m_altreAssenze.clear();
}

Dipendente::Dipendente(QObject *parent) : QObject(parent), data(new DipendenteData)
{

}

Dipendente::Dipendente(const Dipendente &rhs) : data(rhs.data)
{

}

Dipendente &Dipendente::operator=(const Dipendente &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

Dipendente::~Dipendente()
{

}

int Dipendente::anno() const
{
    return data->anno();
}

void Dipendente::setAnno(int anno)
{
    data->setAnno(anno);
}

int Dipendente::mese() const
{
    return data->mese();
}

void Dipendente::setMese(int mese)
{
    data->setMese(mese);
}

QString Dipendente::nome() const
{
    return data->nome();
}

void Dipendente::setNome(QString nome)
{
    data->setNome(nome);
}

QString Dipendente::matricola() const
{
    return data->matricola();
}

void Dipendente::setMatricola(QString matricola)
{
    data->setMatricola(matricola);
}

QString Dipendente::unita() const
{
    return data->unita();
}

void Dipendente::setUnita(QString unita)
{
    data->setUnita(unita);
}

int Dipendente::riposi() const
{
    return data->riposi();
}

void Dipendente::addRiposi(int num)
{
    data->addRiposi(num);
}

QStringList Dipendente::guardieDiurne() const
{
    return data->guardieDiurne();
}

void Dipendente::addGuardiaDiurna(QString date)
{
    data->addGuardiaDiurna(date);
}

QStringList Dipendente::guardieNotturne() const
{
    return data->guardieNotturne();
}

void Dipendente::addGuardiaNotturna(QString date)
{
    data->addGuardiaNotturna(date);
}

QMap<int, QPair<int, int> > Dipendente::grep() const
{
    return data->grep();
}

void Dipendente::addGrep(int giorno, int minuti, int tipo)
{
    data->addGrep(giorno, minuti, tipo);
}

QStringList Dipendente::rmp() const
{
    return data->rmp();
}

void Dipendente::addRmp(QString date)
{
    data->addRmp(date);
}

QStringList Dipendente::rmc() const
{
    return data->rmc();
}

void Dipendente::addRmc(QString date)
{
    data->addRmc(date);
}

QStringList Dipendente::ferie() const
{
    return data->ferie();
}

void Dipendente::addFerie(QString date)
{
    data->addFerie(date);
}

QStringList Dipendente::congedi() const
{
    return data->congedi();
}

void Dipendente::addCongedo(QString date)
{
    data->addCongedo(date);
}

QStringList Dipendente::malattia() const
{
    return data->malattia();
}

void Dipendente::addMalattia(QString date)
{
    data->addMalattia(date);
}

QMap<QString, QPair<QStringList, int> > Dipendente::altreAssenze() const
{
    return data->altreAssenze();
}

int Dipendente::altreAssenzeCount() const
{
    return data->altreAssenzeCount();
}

void Dipendente::addAltraAssenza(QString causale, QString date, int minuti)
{
    data->addAltraAssenza(causale, date, minuti);
}

int Dipendente::minutiFatti() const
{
    return data->minutiFatti();
}

void Dipendente::addMinutiFatti(int minuti)
{
    data->addMinutiFatti(minuti);
}

int Dipendente::minutiCongedi() const
{
    return data->minutiCongedi();
}

void Dipendente::addMinutiCongedo(int minuti)
{
    data->addMinutiCongedo(minuti);
}

int Dipendente::minutiGiornalieri() const
{
    return data->minutiGiornalieri();
}

void Dipendente::setMinutiGiornalieri(int minuti)
{
    data->setMinutiGiornalieri(minuti);
}

int Dipendente::minutiGrep() const
{
    return data->minutiGrep();
}

void Dipendente::addMinutiGrep(int minuti)
{
    data->addMinutiGrep(minuti);
}

int Dipendente::minutiEccr() const
{
    return data->minutiEccr();
}

void Dipendente::addMinutiEccr(int minuti)
{
    data->addMinutiEccr(minuti);
}

int Dipendente::minutiGuar() const
{
    return data->minutiGuar();
}

void Dipendente::addMinutiGuar(int minuti)
{
    data->addMinutiGuar(minuti);
}

int Dipendente::minutiRmc() const
{
    return data->minutiRmc();
}

void Dipendente::addMinutiRmc(int minuti)
{
    data->addMinutiRmc(minuti);
}

void Dipendente::resetProperties()
{
    data->resetProperties();
}
