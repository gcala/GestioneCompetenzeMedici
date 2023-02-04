/*
    SPDX-FileCopyrightText: 2023 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "indennita.h"

#include <QMap>

class IndennitaData : public QSharedData
{
public:
    VoceIndennita m_tipo;
    QMap<int, QPair<QString, QString>> m_map;

};

Indennita::Indennita(QObject *parent)
    : QObject(parent)
    , data(new IndennitaData)
{

}

Indennita::Indennita(const Indennita &rhs)
    : data(rhs.data)
{

}

Indennita &Indennita::operator=(const Indennita &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

bool Indennita::operator==(const Indennita &rhs) const
{
    if(this == &rhs)
        return true;

    return data->m_tipo == rhs.tipo() &&
            data->m_map == rhs.map();
}

Indennita::~Indennita()
{

}

QString Indennita::voce(int unita) const
{
    if(data->m_map.keys().contains(unita))
        return data->m_map.value(unita).first;
    return data->m_map.value(0).first;
}

QString Indennita::sub(int unita) const
{
    if(data->m_map.keys().contains(unita))
        return data->m_map.value(unita).second;
    return data->m_map.value(0).second;
}

void Indennita::setTipo(VoceIndennita tipo)
{
    data->m_tipo = tipo;
}

void Indennita::setMap(QMap<int, QPair<QString, QString> > map)
{
    data->m_map = map;
}

void Indennita::addItem(int unita, const QString &voce, const QString &sub)
{
    data->m_map.insert(unita, qMakePair(voce, sub));
}

VoceIndennita Indennita::tipo() const
{
    return data->m_tipo;
}

QMap<int, QPair<QString, QString> > Indennita::map() const
{
    return data->m_map;
}
