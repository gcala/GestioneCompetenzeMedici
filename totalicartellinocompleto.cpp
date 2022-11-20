/*
 *  SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>
 * 
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "totalicartellinocompleto.h"

class TotaliCartellinoCompletoData : public QSharedData
{
public:
    int m_disponibile;
    int m_ordinario;
    int m_recuperato;
    int m_giustificato;
    int m_daRecuperare;
    int m_eccedenza;
    int m_strPagato;
    int m_strReperibilita;
    int m_bancaOre;
    int m_scarRes;
    int m_saldoMese;
    int m_totale;
};

TotaliCartellinoCompleto::TotaliCartellinoCompleto(QObject *parent)
    : QObject(parent), data(new TotaliCartellinoCompletoData)
{
    data->m_disponibile = 0;
    data->m_ordinario = 0;
    data->m_recuperato = 0;
    data->m_giustificato = 0;
    data->m_daRecuperare = 0;
    data->m_eccedenza = 0;
    data->m_strPagato = 0;
    data->m_strReperibilita = 0;
    data->m_bancaOre = 0;
    data->m_scarRes = 0;
    data->m_saldoMese = 0;
    data->m_totale = 0;
}

TotaliCartellinoCompleto::TotaliCartellinoCompleto(const TotaliCartellinoCompleto &rhs)
    : data(rhs.data)
{

}

TotaliCartellinoCompleto &TotaliCartellinoCompleto::operator=(const TotaliCartellinoCompleto &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

bool TotaliCartellinoCompleto::operator==(const TotaliCartellinoCompleto &rhs) const
{
    if(this == &rhs)
        return true;
    return disponibile() == rhs.disponibile() &&
            ordinario() == rhs.ordinario() &&
            recuperato() == rhs.recuperato() &&
            giustificato() == rhs.giustificato() &&
            daRecuperare() == rhs.daRecuperare() &&
            eccedenza() == rhs.eccedenza() &&
            strPagato() == rhs.strPagato() &&
            strReperibilita() == rhs.strReperibilita() &&
            bancaOre() == rhs.bancaOre() &&
            scarRes() == rhs.scarRes() &&
            saldoMese() == rhs.saldoMese() &&
            totale() == rhs.totale();
}

TotaliCartellinoCompleto::~TotaliCartellinoCompleto()
{

}

int TotaliCartellinoCompleto::disponibile() const
{
    return data->m_disponibile;
}

int TotaliCartellinoCompleto::ordinario() const
{
    return data->m_ordinario;
}

int TotaliCartellinoCompleto::recuperato() const
{
    return data->m_recuperato;
}

int TotaliCartellinoCompleto::giustificato() const
{
    return data->m_giustificato;
}

int TotaliCartellinoCompleto::daRecuperare() const
{
    return data->m_daRecuperare;
}

int TotaliCartellinoCompleto::eccedenza() const
{
    return data->m_eccedenza;
}

int TotaliCartellinoCompleto::strPagato() const
{
    return data->m_strPagato;
}

int TotaliCartellinoCompleto::strReperibilita() const
{
    return data->m_strReperibilita;
}

int TotaliCartellinoCompleto::bancaOre() const
{
    return data->m_bancaOre;
}

int TotaliCartellinoCompleto::scarRes() const
{
    return data->m_scarRes;
}

int TotaliCartellinoCompleto::saldoMese() const
{
    return data->m_saldoMese;
}

int TotaliCartellinoCompleto::totale() const
{
    return data->m_totale;
}

void TotaliCartellinoCompleto::setDisponibile(int value)
{
    data->m_disponibile = value;
}

void TotaliCartellinoCompleto::setOrdinario(int value)
{
    data->m_ordinario = value;
}

void TotaliCartellinoCompleto::setRecuperato(int value)
{
    data->m_recuperato = value;
}

void TotaliCartellinoCompleto::setGiustificato(int value)
{
    data->m_giustificato = value;
}

void TotaliCartellinoCompleto::setDaRecuperare(int value)
{
    data->m_daRecuperare = value;
}

void TotaliCartellinoCompleto::setEccedenza(int value)
{
    data->m_eccedenza = value;
}

void TotaliCartellinoCompleto::setStrPagato(int value)
{
    data->m_strPagato = value;
}

void TotaliCartellinoCompleto::setStrReperibilita(int value)
{
    data->m_strReperibilita = value;
}

void TotaliCartellinoCompleto::setBancaOre(int value)
{
    data->m_bancaOre = value;
}

void TotaliCartellinoCompleto::setScarRes(int value)
{
    data->m_scarRes = value;
}

void TotaliCartellinoCompleto::setSaldoMese(int value)
{
    data->m_saldoMese = value;
}

void TotaliCartellinoCompleto::setTotale(int value)
{
    data->m_totale = value;
}
