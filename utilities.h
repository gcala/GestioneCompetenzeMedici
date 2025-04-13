/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>
#include <QDate>
#include <QMetaType>

class Timbratura;

//template<class T> inline T operator~ (T a) { return (T)~(int)a; }
//template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
//template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
//template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
//template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
//template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
//template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }

#include <cstdint>
#include <type_traits>

template<typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
class auto_bool
{
    T val_;
public:
    constexpr auto_bool(T val) : val_(val) {}
    constexpr operator T() const { return val_; }
    constexpr explicit operator bool() const
    {
        return static_cast<std::underlying_type_t<T>>(val_) != 0;
    }
};

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr auto_bool<T> operator&(T lhs, T rhs)
{
    return static_cast<T>(
        static_cast<typename std::underlying_type<T>::type>(lhs) &
        static_cast<typename std::underlying_type<T>::type>(rhs));
}

template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator|(T lhs, T rhs)
{
    return static_cast<T>(
        static_cast<typename std::underlying_type<T>::type>(lhs) |
        static_cast<typename std::underlying_type<T>::type>(rhs));
}

//enum class Turno : uint8_t {
//    Indefinito        = 1,
//    NessunaTimbratura = 2,
//    TimbraturaDispari = 4,
//    InizioNotte       = 8,
//    FineNotte         = 16,
//    Mattina           = 32,
//    Pomeriggio        = 64
//};

class Utilities
{
private:
    Utilities(){}

public:
    enum GuardiaType
        {
            Feriale,
            Sabato,
            Domenica,
            GrandeFestivita
        };

    enum ValoreRep
        {
            Mezzo = 0,
            Uno,
            UnoMezzo,
            Due,
            DueMezzo,
            Teleconsulto6 = 16,
            Teleconsulto12,
            Teleconsulto18,
            Teleconsulto24
        };

    enum RepType
        {
            FerDiu,
            FesDiu,
            FerNot,
            FesNot
        };

    enum Reperibilita {
        Ordinaria,
        FestivaONotturna,
        FestivaENotturna
    };

    enum VoceIndennita {
        Indefinita,
        IndennitaFestiva,
        IndennitaNotturna,
        GuardiaDiurna,
        GuardiaNotturna,
        GranFestivita,
        StraordinarioReperibilitaOrd,
        StraordinarioReperibilitaNof,
        StraordinarioReperibilitaNef,
        StraordinarioGuardiaOrd,
        StraordinarioGuardiaNof,
        StraordinarioGuardiaNef,
        TurniReperibilita,
        OreReperibilita,
        ReperibilitaOltre10,
        Teleconsulto,
        StraordinarioGuardiaDiurna,
        StraordinarioGuardiaNotturna,
        MinutiRecuperabili,
        MinutiResidui,
        IndennitaPSOraria,
        ProntaDisponibilitaOraria,
        ProntaDisponibilitaOraria30,
        IndennitaFestivaOraria
    };

    enum Turno {
        Indefinito        = 1,
        NessunaTimbratura = 2,
        TimbraturaDispari = 4,
        InizioNotte       = 8,
        FineNotte         = 16,
        Mattina           = 32,
        Pomeriggio        = 64,
        MattinaPomeriggio = 128,
        FineInizioNotte   = 256
    };

//    typedef T<enum Turno>  TipoTurno;

    static QString inOrario(int value);
    static QString m_connectionName;
    static QDate ccnl1618Date;
    static QDate ccnl1921Date;
    static QDate regolamentoOrario2025Date;
    static int inMinuti(QString orario);
    static int inMinuti(const QTime &orario);
    static int mese2Int(const QString &mese);
    static QString int2MeseBreve(const int &mese);
    static bool timbraturaValida(QString text);
    static QVector<int> stringlistToVectorInt(const QStringList &list);
    static QStringList vectorIntToStringlist(const QVector<int> &values);
    static int monthsTo(const QDate &start, const QDate &end);
    static QString indennitaName(const Utilities::VoceIndennita id);
    static Utilities::VoceIndennita indennitaEnum(const QString &name);
    static int arrotondaMinutiInOre(const int minuti);
    static int restoMinutiDopoArrotondamento(const int minuti);
    static void extractAnnoMeseFromTimecard(const QString &timecard, int &anno, int &mese);

    static QString m_driver;
    static QString m_host;
    static QString m_dbName;
    static QString m_certFile;
    static QString m_keyFile;
    static QString m_localDbFileName;
    static QString m_lastUsername;
    static QString m_lastPassword;
    static bool m_useSSL;
    static QTime orarioInizioNotte;
    static int m_maxMinutiGiornalieri;
    static QString m_importPath;
    static QString m_exportPath;
    static int m_minutiArrotondamento;

    static QPair<QTime, QTime> inizioMattina;
    static QPair<QTime, QTime> fineMattina;
    static QPair<QTime, QTime> inizioPomeriggio;
    static QPair<QTime, QTime> finePomeriggio;
    static QPair<QTime, QTime> inizioNotte;
    static QPair<QTime, QTime> fineNotte;
};

Q_DECLARE_METATYPE(Utilities::GuardiaType)
Q_DECLARE_METATYPE(Utilities::ValoreRep)
Q_DECLARE_METATYPE(Utilities::RepType)
Q_DECLARE_METATYPE(Utilities::Reperibilita)
Q_DECLARE_METATYPE(Utilities::VoceIndennita)
Q_DECLARE_METATYPE(Utilities::Turno)

#endif // UTILITIES_H
