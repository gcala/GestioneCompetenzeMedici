/*
    SPDX-FileCopyrightText: 2023 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "indennita.h"

#include <QMap>

class IndennitaData : public QSharedData
{
public:
    Utilities::VoceIndennita m_tipo;
    QMap<Utilities::VoceIndennita, QMap<int, QPair<QString, QString>>> m_map;

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

    return data->m_map == rhs.map();
}

Indennita::~Indennita()
{

}

QString Indennita::voce(Utilities::VoceIndennita tipo, int unita) const
{
    if(data->m_map.keys().contains(tipo)) {
        const auto val = data->m_map.value(tipo);
        if(val.keys().contains(unita))
            return val.value(unita).first;
        return val.value(0).first;
    }
    return QStringLiteral();
}

QString Indennita::sub(Utilities::VoceIndennita tipo, int unita) const
{
    if(data->m_map.keys().contains(tipo)) {
        const auto val = data->m_map.value(tipo);
        if(val.keys().contains(unita))
            return val.value(unita).second;
        return val.value(0).second;
    }
    return QStringLiteral();
}

void Indennita::setMap(QMap<Utilities::VoceIndennita, QMap<int, QPair<QString, QString> > > map)
{
    data->m_map = map;
}

void Indennita::addItem(Utilities::VoceIndennita tipo, int unita, const QString &voce, const QString &sub)
{
    if(data->m_map.keys().contains(tipo)) {
        auto val = data->m_map.value(tipo);
        val.insert(unita, qMakePair(voce, sub));
        data->m_map[tipo] = val;
    } else {
        QMap<int, QPair<QString, QString> > val;
        val.insert(unita, qMakePair(voce, sub));
        data->m_map[tipo] = val;
    }
}

QMap<Utilities::VoceIndennita, QMap<int, QPair<QString, QString> > > Indennita::map() const
{
    return data->m_map;
}
