/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
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

    int badgeNumber() const;
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
    int minutiDovuti() const;
    QString oreEffettuate();
    int oreRepPagate() const;
    QString differenzaOre();
    int differenzaMin() const;
    QString differenzaOreSenzaDmp();
    QString deficitOrario();
    QString deficitPuntuale();
    int minutiAltreCausali() const;
    QString oreAltreCausali();
    QString ferieCount() const;
    QList<QDate> ferieDates() const;
    QList<QDate> scopertiDates() const;
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
    QList<QDate> altreCausaliDates() const;
    QMap<int, GuardiaType> guardiaDiurnaMap() const;
    QMap<int, GuardiaType> guardiaNotturnaMap() const;
    void addGuardiaDiurnaDay(int day);
    void addGuardiaNotturnaDay(int day);
    void setRep(const QMap<QDate, ValoreRep> &map);
    QMap<QDate, ValoreRep> rep() const;
    void setDmp(const int &minutes);
    void setPagaStrGuardia(const bool &ok);
    void setOrarioGiornalieroMod(const int &minutes);
    void setDmpCalcolato(const int &minutes);
    int dmp() const;
    bool pagaStrGuardia() const;
    void setNote(const QString &note);
    QString note() const;
    QList<QDate> altreAssenzeDates() const;
    void setAltreAssenze(const QList<QDate> &assenze);
    bool isModded() const;
    bool isRestorable() const;
    void saveMods();
    int orePagate() const;
    int notte() const;
    int numGuarDiurne() const;
    int numGuarNottPag() const;
    QString repCount() const;
    QString oreGrep();
    int numGrFestPagabili() const;
    int numOreGuarPagabili() const;
    int numGuar() const;
    int numGuarGFNonPag() const;
    int numOreGuarFesENot() const;
    int numOreGuarFesONot() const;
    int numOreGuarOrd() const;
    int numOreRep(Reperibilita rep);
    int residuoOreNonPagate();
    int numFestiviRecuperabili();
    int numNottiRecuperabili();
    int numOreRecuperabili();
    QString residuoOreNonRecuperabili();
    QPair<int, int> recuperiMesiSuccessivo() const;

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
    bool isNoteModded() const;
    bool isOrarioGiornalieroModded() const;

signals:

public slots:

private:
    QSharedDataPointer<CompetenzaData> data;
};

#endif // COMPETENZA_H
