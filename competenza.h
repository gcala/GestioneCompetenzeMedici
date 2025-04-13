/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef COMPETENZA_H
#define COMPETENZA_H

#include "utilities.h"

#include <QObject>
#include <QSharedDataPointer>
#include <QMap>
#include <QDate>

class CompetenzaData;

class Dipendente;

class Competenza : public QObject
{
    Q_OBJECT
    

public:
    explicit Competenza(const QString &tableName, const int id, bool isExporting = false);
    Competenza(const Competenza &);
    Competenza &operator=(const Competenza &);
    bool operator==(const Competenza &rhs) const;
    ~Competenza();

    int badgeNumber() const;
    QString name() const;
    QDate dataIniziale() const;
    QDate dataFinale() const;
    QString tableName() const;
    QString modTableName() const;
    int doctorId();
//    int giorniLavorati() const;
    int giorniLavorativi() const;
//    QString assenzeTotali() const;
    QString orarioGiornaliero();
    QString oreDovute();
    int minutiDovuti() const;
    QString oreEffettuate();
    int minutiLavoratiSenzaGrep() const;
//    int oreRepPagate() const;
    QString differenzaOre();
    int minutiSaldoMese() const;
    QString differenzaOreSenzaDmp();
    int deficitOrario();
    QString deficitPuntuale();
    // int minutiAltreCausali() const;
    // QString oreAltreCausali();
    // QString ferieCount() const;
    QList<QDate> ferieDates() const;
    // QList<QDate> scopertiDates() const;
    // QString congediCount() const;
    QList<QDate> congediDates() const;
    // QString malattiaCount() const;
    QList<QDate> malattiaDates() const;
    // QString rmpCount() const;
    QList<QDate> rmpDates() const;
    QString rmcCount() const;
    QList<QDate> rmcDates() const;
    // QList<QDate> gdDates() const;
    // QList<QDate> mgdDates() const;
    // QList<QDate> gnDates() const;
    // QList<QDate> altreCausaliDates() const;
    QMap<int, Utilities::GuardiaType> guardiaDiurnaMap() const;
    QMap<int, Utilities::GuardiaType> mezzaGuardiaDiurnaMap() const;
    QMap<int, Utilities::GuardiaType> guardiaNotturnaMap() const;
    void addGuardiaDiurnaDay(int day);
    void addMezzaGuardiaDiurnaDay(int day);
    void addGuardiaNotturnaDay(int day);
    void setTurniProntaDisponibilita(const QMap<QDate, Utilities::ValoreRep> &map);
    void setTeleconsulto(const QMap<QDate, Utilities::ValoreRep> &map);
    QMap<QDate, Utilities::ValoreRep> turniProntaDisponibilita() const;
    QMap<QDate, Utilities::ValoreRep> teleconsulto() const;
    void setDmp(const int &minutes);
    void setPagaStrGuardia(const bool &ok);
//    void setOrarioGiornalieroMod(const int &minutes);
    void setDmpCalcolato(const int &minutes);
    int dmp() const;
    bool pagaStrGuardia() const;
    void setNote(const QString &note);
    QString note() const;
    // QList<QDate> altreAssenzeDates() const;
    // void setAltreAssenze(const QList<QDate> &assenze);
    bool isModded() const;
    bool isRestorable() const;
    void saveMods();
    int orePagateNotte() const;

    // ccnl 16/18
    int notte() const;

//    int numGuardieDiurne() const;
//    int numGuardieNotturneEscluseGrandiFestivitaPagabili() const;
    double numeroTurniTeleconsulto() const;
    int minutiGrep();
//    int numGrandiFestivitaPagabili() const;
//    int numOreGuardieNotturnePagabili() const;
//    int numGuardieNotturneEscluseGrandiFestivita() const;
//    int numGrandiFestivitaNonPagate() const;
//    int numOreGuardieNotturneNEF() const;
//    int numOreGuardieNotturneNOF() const;
//    int numOreGuardieNotturneORD() const;
//    int numOreRep(Utilities::Reperibilita rep);
//    int residuoOreNonPagate();
//    int numFestiviRecuperabili();
//    int numNottiRecuperabili();
//    int numOreRecuperabili();
//    int minutiNonRecuperabili();
    QPair<int, int> recuperiMeseSuccessivo() const;
    Dipendente *dipendente() const;
//    QMap<int, Utilities::GuardiaType> defaultGNDates() const;
//    QMap<int, Utilities::GuardiaType> defaultGDDates() const;
    QMap<QDate, Utilities::ValoreRep> defaultRep() const;
    QMap<QDate, Utilities::ValoreRep> defaultTeleconsulto() const;
    int oreTotaliNotte() const;
    int dmpCalcolato() const;
    int orarioGiornaliero() const;
    int defaultDmp() const;
//    int defaultOrarioGiornaliero() const;
    QString defaultNote() const;
    // QVector<int> altreAssenze() const;
    // QVector<int> defaultAltreAssenze() const;
    bool modded() const;
//    bool gdiurneModded() const;
//    bool gnotturneModded() const;
    bool turniProntaDisponibilitaModded() const;
    bool teleconsultoModded() const;
    bool dmpModded() const;
//    bool orarioGiornalieroModded() const;
    // bool altreModded() const;
    bool noteModded() const;
    // int minutiRecuperati() const;
    bool pagaDiurno() const;
    bool pagaOreLavorate() const;
    int numeroGuardieNotturneFatte() const;
//    int oreGuardieNotturneFatte() const;
//    int oreGuardieDiurneFatte() const;
//    int oreGrandiFestivitaFatte() const;
//    int oreReperibilitaFatte() const;
//    int minutiFestiviRecuperabili() const;

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

//    QString oreStraordinarioGuardie() const;
//    bool isGuardieDiurneModded() const;
//    bool isGuardieNotturneModded() const;
    bool isReperibilitaModded() const;
    bool isTeleconsultoModded() const;
    bool isDmpModded() const;
    // bool isAltreModded() const;
    bool isNoteModded() const;
//    bool isOrarioGiornalieroModded() const;

    void distribuzioneOrePagate();
//    int minutiFestiviRecuperabili(const int minutiResidui);
//    int minutiNottiRecuperabili(const int minutiResidui, const int numeroGrandiFestivitaPagabili);

    int oreStraordinarioNotturnoORDPagate() const;
    int oreStraordinarioNotturnoNOFPagate() const;
    int oreStraordinarioNotturnoNEFPagate() const;
    int oreReperibilitaPagate() const;
    int oreReperibilitaORDPagate() const;
    int oreReperibilitaNOFPagate() const;
    int oreReperibilitaNEFPagate() const;
    int oreGuardieDiurnePagate() const;

    double numeroTurniProntaDisponibilita() const;
    double numeroOreProntaDisponibilita() const;
    int numeroGuardieDiurneFatte() const;
    double numeroMezzeGuardieDiurneFatte() const;
    int numeroGrandiFestivitaPagate() const;
    int numeroGrandiFestivitaFatte() const;
    int numeroOreGuardieNotturnePagate() const;
    int numeroTurniReperibilitaOltre10() const;
    int numeroOreTeleconsulto() const;
    int minutiRecuperabili() const;
    int minutiNonRecuperabili() const;
    int saldoMinuti() const;
    int saldoOreNonPagate() const;
    int oreLavoratePagate() const;

signals:

public slots:

private:
    QSharedDataPointer<CompetenzaData> data;

    QString m_tableName;
    QString m_modTableName;
    int m_id;
    int m_unitaId;
    bool m_exporting;

    void buildDipendente();
    Utilities::GuardiaType tipoGuardia(const QString &day);
    Utilities::RepType tipoReperibilita(const int giorno, const int tipo);
    void calcOreGuardia();
    void getOrePagate();
    void getStraordinarioPagato();
    void getRecuperiMeseSuccessivo();
    // int grandiFestivitaFatte() const;
    // void rimuoviAltreAssenzeDoppie();
};

#endif // COMPETENZA_H
