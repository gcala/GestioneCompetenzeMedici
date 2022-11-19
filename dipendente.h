/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
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

public:
    explicit Dipendente();
    Dipendente(const Dipendente &);
    Dipendente &operator=(const Dipendente &);
    bool operator==(const Dipendente &rhs) const;
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
    void addNumGiorniCartellino(int num);
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
    int minutiGiornalieriVeri() const;
    int numGiorniCartellino() const;
    void setMinutiGiornalieri(int minuti);
    void setNumGiorniCartellino(int num);
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
