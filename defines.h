/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef GUARDIA_ENUM_H
#define GUARDIA_ENUM_H

enum GuardiaType
    {
        Feriale,
        Sabato,
        Domenica,
        GrandeFestivita
    };

enum ValoreRep
    {
        Mezzo,
        Uno,
        UnoMezzo,
        Due,
        DueMezzo
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
    OreReperibilita
};


#endif // GUARDIA_ENUM_H
