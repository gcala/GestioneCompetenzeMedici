/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017-2019 Giuseppe Calà <giuseppe.cala@mailbox.org>
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

#ifndef DIPENDENTE_H
#define DIPENDENTE_H

#include <QObject>
#include <QSharedDataPointer>
#include <QMap>
#include <QDate>

class DipendenteData;

class Dipendente : public QObject
{
    Q_OBJECT
    Q_PROPERTY( int anno READ anno WRITE setAnno )
    Q_PROPERTY( int mese READ mese WRITE setMese )
    Q_PROPERTY( QString nome READ nome WRITE setNome )
    Q_PROPERTY( int matricola READ matricola WRITE setMatricola )
    Q_PROPERTY( int unita READ unita WRITE setUnita )
    Q_PROPERTY( int riposi READ riposi )
    Q_PROPERTY( QStringList guardieDiurne READ guardieDiurne )
    Q_PROPERTY( QStringList guardieNotturne READ guardieNotturne )
    Q_PROPERTY( QMultiMap<int, QPair<int, int> > grep READ grep )
    Q_PROPERTY( QStringList rmp READ rmp )
    Q_PROPERTY( QStringList rmc READ rmc )
    Q_PROPERTY( QStringList ferie READ ferie )
    Q_PROPERTY( QStringList congedi READ congedi )
    Q_PROPERTY( QStringList malattia READ malattia )
    Q_PROPERTY( QStringList scoperti READ scoperti )
    Q_PROPERTY( QMap<QString, QPair<QStringList, int> > altreCausali READ altreCausali )
    Q_PROPERTY( int minutiFatti READ minutiFatti WRITE addMinutiFatti )
    Q_PROPERTY( int minutiCongedi READ minutiCongedi WRITE addMinutiCongedo )
    Q_PROPERTY( int minutiGiornalieri READ minutiGiornalieri WRITE setMinutiGiornalieri )
    Q_PROPERTY( int minutiGrep READ minutiGrep WRITE addMinutiGrep )
    Q_PROPERTY( int minutiEccr READ minutiEccr WRITE addMinutiEccr )
    Q_PROPERTY( int minutiGuar READ minutiGuar WRITE addMinutiGuar )
    Q_PROPERTY( int minutiRmc READ minutiRmc WRITE addMinutiRmc )

public:
    explicit Dipendente(QObject *parent = nullptr);
    Dipendente(const Dipendente &);
    Dipendente &operator=(const Dipendente &);
    ~Dipendente();

    int anno() const;
    void setAnno(int anno);
    int mese() const;
    void setMese(int mese);
    QString nome() const;
    void setNome(QString nome);
    int matricola() const;
    void setMatricola(int matricola);
    int unita() const;
    void setUnita(int unita);
    int riposi() const;
    void addRiposi(int num);
    QStringList guardieDiurne() const;
    void addGuardiaDiurna(QString date);
    QStringList guardieNotturne() const;
    void addGuardiaNotturna(QString date);
    QMultiMap<int, QPair<int, int> > grep() const;
    void addGrep(int giorno, int minuti, int tipo);
    QStringList rmp() const;
    void addRmp(QString date);
    QStringList rmc() const;
    void addRmc(QString date);
    QStringList ferie() const;
    void addFerie(QString date);
    QStringList scoperti() const;
    void addScoperto(QString date);
    QStringList congedi() const;
    void addCongedo(QString date);
    QStringList malattia() const;
    void addMalattia(QString date);
    QMap<QString, QPair<QStringList, int> > altreCausali() const;
    void addAltraCausale(QString causale, QString date, int minuti);
    int altreCausaliCount() const;
    int minutiFatti() const;
    void addMinutiFatti(int minuti);
    int minutiCongedi() const;
    void addMinutiCongedo(int minuti);
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
    void setRiposi(int minuti);
    void resetProperties();

signals:

public slots:

private:
    QSharedDataPointer<DipendenteData> data;
};

#endif // DIPENDENTE_H
