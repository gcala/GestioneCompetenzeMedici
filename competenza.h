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

#ifndef COMPETENZA_H
#define COMPETENZA_H

#include "defines.h"

#include <QObject>
#include <QSharedDataPointer>
#include <QMap>
#include <QDate>

class CompetenzaData;

class Competenza : public QObject
{
    Q_OBJECT
    

public:
    explicit Competenza(const QString &tableName, const int id, QObject *parent = nullptr);
    Competenza(const Competenza &);
    Competenza &operator=(const Competenza &);
    ~Competenza();

    QString badgeNumber() const;
    QString name() const;
    QDate dataIniziale() const;
    QDate dataFinale() const;
    QString modTableName() const;
    int doctorId();
    QString giorniLavorati() const;
    QString giorniLavorativi() const;
    QString assenzeTotali() const;
    QString orarioGiornaliero();
    QString oreDovute();
    QString oreEffettuate();
    int oreProntaDisp();
    QString differenzaOre();
    int differenzaMin() const;
    QString differenzaOreSenzaDmp();
    QString deficitOrario();
    int minutiAltreCausali() const;
    QString oreAltreCausali();
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
    QMap<int, GuardiaType> guardiaDiurnaMap() const;
    QMap<int, GuardiaType> guardiaNotturnaMap() const;
    void addGuardiaDiurnaDay(int day);
    void addGuardiaNotturnaDay(int day);
    void setRep(const QMap<QDate, ValoreRep> &map);
    QMap<QDate, ValoreRep> rep() const;
    void setDmp(const int &minutes);
    int dmp() const;
    QList<QDate> altreAssenzeDates() const;
    void setAltreAssenze(const QList<QDate> &assenze);
    bool isModded() const;
    bool isRestorable() const;
    void saveMods();
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
    bool isGuardieDiurneModded() const;
    bool isGuardieNotturneModded() const;
    bool isReperibilitaModded() const;
    bool isDmpModded() const;
    bool isAltreModded() const;

signals:

public slots:

private:
    QSharedDataPointer<CompetenzaData> data;
};

#endif // COMPETENZA_H
