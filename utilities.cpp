/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "utilities.h"
#include "timbratura.h"

#include <QRegularExpression>

QString Utilities::m_connectionName;
QString Utilities::m_driver;
QString Utilities::m_host;
QString Utilities::m_dbName;
QString Utilities::m_certFile;
QString Utilities::m_keyFile;
QString Utilities::m_localDbFileName;
QString Utilities::m_lastUsername;
QString Utilities::m_lastPassword;
QString Utilities::m_importPath;
QString Utilities::m_exportPath;
bool Utilities::m_useSSL = false;
QDate Utilities::ccnl1618Date(2020,1,1);
QDate Utilities::ccnl1921Date(2024,2,1);
QDate Utilities::regolamentoOrario2025Date(2025,1,1);
QTime Utilities::orarioInizioNotte(19,30);
int Utilities::m_maxMinutiGiornalieri = 456;
QPair<QTime, QTime> Utilities::inizioMattina = qMakePair(QTime(5,20),QTime(11,00));
QPair<QTime, QTime> Utilities::fineMattina = qMakePair(QTime(12,00),QTime(16,45));
QPair<QTime, QTime> Utilities::inizioPomeriggio = qMakePair(QTime(12,00),QTime(15,30));
QPair<QTime, QTime> Utilities::finePomeriggio = qMakePair(QTime(18,45),QTime(23,30));
QPair<QTime, QTime> Utilities::inizioNotte = qMakePair(QTime(18,30),QTime(21,30));
QPair<QTime, QTime> Utilities::fineNotte = qMakePair(QTime(6,00),QTime(9,55));
int Utilities::m_minutiArrotondamento = 45;

QString Utilities::inOrario(int value )
{
    QString sign;
    if(value < 0) {
        sign = "-";
        value *= -1;
    }
    int ore = value / 60;
    int mins = value - ore * 60;

    return sign + QString::number(ore) + ":" + QString::number(mins).rightJustified(2, '0');
}

int Utilities::inMinuti(QString orario)
{
    bool isNegative = false;
    if(orario.startsWith("-")) {
        isNegative = true;
        orario.remove("-");
    }

    QStringList fields = orario.split(":");
    if(fields.count() != 2)
        return 0;

    return (fields.at(0).toInt() * 60 + fields.at(1).toInt()) * (isNegative ? -1 : 1);
}

int Utilities::inMinuti(const QTime &orario)
{
    if(!orario.isValid() || orario.isNull())
        return 0;

    return orario.hour() * 60 + orario.minute();
}

int Utilities::mese2Int(const QString &mese)
{
    if(mese == "Gennaio")
        return 1;
    if(mese == "Febbraio")
        return 2;
    if(mese == "Marzo")
        return 3;
    if(mese == "Aprile")
        return 4;
    if(mese == "Maggio")
        return 5;
    if(mese == "Giugno")
        return 6;
    if(mese == "Luglio")
        return 7;
    if(mese == "Agosto")
        return 8;
    if(mese == "Settembre")
        return 9;
    if(mese == "Ottobre")
        return 10;
    if(mese == "Novembre")
        return 11;
    if(mese == "Dicembre")
        return 12;

    return 0;
}

QString Utilities::int2MeseBreve(const int &mese)
{
    QString text;
    switch (mese) {
    case 1:
        text = "GEN";
        break;
    case 2:
        text = "FEB";
        break;
    case 3:
        text = "MAR";
        break;
    case 4:
        text = "APR";
        break;
    case 5:
        text = "MAG";
        break;
    case 6:
        text = "GIU";
        break;
    case 7:
        text = "LUG";
        break;
    case 8:
        text = "AGO";
        break;
    case 9:
        text = "SET";
        break;
    case 10:
        text = "OTT";
        break;
    case 11:
        text = "NOV";
        break;
    case 12:
        text = "DIC";
        break;
    default:
        text = "";
    }

    return text;
}

bool Utilities::timbraturaValida(QString text)
{
    QRegularExpression rx("\\d\\d:\\d\\d");
    text.remove(rx);  // orario
    text.remove("r"); // ingresso reperibilità
    text.remove("R"); // uscita reperibilità
    text.remove("^"); // aggiunta da infopoint
    text.remove("*"); // aggiunta da rilpres
    text.remove("ø"); // timbratura annullata

    if(text.trimmed().isEmpty())
        return true;

    return false;
}

QVector<int> Utilities::stringlistToVectorInt(const QStringList &list)
{
    QVector<int> result;
    std::transform(list.begin(), list.end(), std::back_inserter(result), [](const QString &s) { return s.toInt(); });
    return result;
}

QStringList Utilities::vectorIntToStringlist(const QVector<int> &values)
{
    QStringList result;
    std::transform(values.begin(), values.end(), std::back_inserter(result), [](int i) { return QString::number(i); });
    return result;
}

int Utilities::monthsTo(const QDate &start, const QDate &end)
{
    int value = 0;
    if(start.year() == end.year()) {
        return end.month() - start.month() + 1;
    } else {
        value += 12 - start.month() + 1;
        for(int i = start.year() +1; i < end.year(); i++) {
            value += 12;
        }
        value += end.month();
    }

    return value;
}

QString Utilities::indennitaName(const Utilities::VoceIndennita id)
{
    QString name;
    switch (id) {
    case Utilities::VoceIndennita::GranFestivita:
        name = "GrandeFestivita";
        break;
    case Utilities::VoceIndennita::GuardiaDiurna:
        name = "GuardiaDiurna";
        break;
    case Utilities::VoceIndennita::GuardiaNotturna:
        name = "GuardiaNotturna";
        break;
    case Utilities::VoceIndennita::IndennitaFestiva:
        name = "IndennitaFestiva";
        break;
    case Utilities::VoceIndennita::IndennitaNotturna:
        name = "IndennitaNotturna";
        break;
    case Utilities::VoceIndennita::StraordinarioGuardiaNef:
        name = "StraordinarioGuardiaNef";
        break;
    case Utilities::VoceIndennita::StraordinarioGuardiaNof:
        name = "StraordinarioGuardiaNof";
        break;
    case Utilities::VoceIndennita::StraordinarioGuardiaOrd:
        name = "StraordinarioGuardiaOrd";
        break;
    case Utilities::VoceIndennita::StraordinarioReperibilitaNef:
        name = "StraordinarioReperibilitaNef";
        break;
    case Utilities::VoceIndennita::StraordinarioReperibilitaNof:
        name = "StraordinarioReperibilitaNof";
        break;
    case Utilities::VoceIndennita::StraordinarioReperibilitaOrd:
        name = "StraordinarioReperibilitaOrd";
        break;
    case Utilities::VoceIndennita::TurniReperibilita:
        name = "TurniReperibilita";
        break;
    case Utilities::VoceIndennita::OreReperibilita:
        name = "OreReperibilita";
        break;
    case Utilities::VoceIndennita::ReperibilitaOltre10:
        name = "ReperibilitaOltre10";
        break;
    case Utilities::VoceIndennita::Teleconsulto:
        name = "Teleconsulto";
        break;
    default:
        name = "Indefinita";
    }

    return name;
}

Utilities::VoceIndennita Utilities::indennitaEnum(const QString &name)
{
    if(name == "GrandeFestivita")
        return Utilities::VoceIndennita::GranFestivita;

    if(name == "GuardiaDiurna")
        return Utilities::VoceIndennita::GuardiaDiurna;

    if(name == "GuardiaNotturna")
        return Utilities::VoceIndennita::GuardiaNotturna;

    if(name == "IndennitaFestiva")
        return Utilities::VoceIndennita::IndennitaFestiva;

    if(name == "IndennitaNotturna")
        return Utilities::VoceIndennita::IndennitaNotturna;

    if(name == "StraordinarioGuardiaNef")
        return Utilities::VoceIndennita::StraordinarioGuardiaNef;

    if(name == "StraordinarioGuardiaNof")
        return Utilities::VoceIndennita::StraordinarioGuardiaNof;

    if(name == "StraordinarioGuardiaOrd")
        return Utilities::VoceIndennita::StraordinarioGuardiaOrd;

    if(name == "StraordinarioReperibilitaNef")
        return Utilities::VoceIndennita::StraordinarioReperibilitaNef;

    if(name == "StraordinarioReperibilitaNof")
        return Utilities::VoceIndennita::StraordinarioReperibilitaNof;

    if(name == "StraordinarioReperibilitaOrd")
        return Utilities::VoceIndennita::StraordinarioReperibilitaOrd;

    if(name == "TurniReperibilita")
        return Utilities::VoceIndennita::TurniReperibilita;

    if(name == "OreReperibilita")
        return Utilities::VoceIndennita::OreReperibilita;

    if(name == "ReperibilitaOltre10")
        return Utilities::VoceIndennita::ReperibilitaOltre10;

    if(name == "Teleconsulto")
        return Utilities::VoceIndennita::Teleconsulto;

    if(name == "IndennitaPSOraria")
        return Utilities::VoceIndennita::IndennitaPSOraria;

    if(name == "ProntaDisponibilitaOraria")
        return Utilities::VoceIndennita::ProntaDisponibilitaOraria;

    if(name == "IndennitaFestivaOraria")
        return Utilities::VoceIndennita::IndennitaFestivaOraria;

    if(name == "ProntaDisponibilitaOraria30")
        return Utilities::VoceIndennita::ProntaDisponibilitaOraria30;


    return Utilities::VoceIndennita::Indefinita;
}

int Utilities::arrotondaMinutiInOre(const int minuti)
{
    return (minuti % 60 >= m_minutiArrotondamento) ? minuti / 60 + 1 : minuti / 60;
}

int Utilities::restoMinutiDopoArrotondamento(const int minuti)
{
    // r_n_fes() % 60 <= m_arrotondamento ? r_n_fes() / 60 : 0
    return (minuti % 60 <= m_minutiArrotondamento) ? minuti % 60 : 0;
}

void Utilities::extractAnnoMeseFromTimecard(const QString &timecard, int &anno, int &mese)
{
    if(timecard.contains("tcm_") || timecard.contains("tcp_")) {
        anno = timecard.mid(4,4).toInt();
        mese = timecard.right(2).toInt();
    } else if(timecard.contains("tc_")) {
        anno = timecard.mid(3,4).toInt();
        mese = timecard.right(2).toInt();
    } else {
        anno = 0;
        mese = 0;
    }
}
