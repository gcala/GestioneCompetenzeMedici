/*
    SPDX-FileCopyrightText: 2023 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef INDENNITA_H
#define INDENNITA_H

#include "utilities.h"

#include <QObject>
#include <QSharedDataPointer>

class IndennitaData;

class Indennita : public QObject
{
    Q_OBJECT
public:
    explicit Indennita(QObject *parent = nullptr);
    Indennita(const Indennita &);
    Indennita &operator=(const Indennita &);
    bool operator==(const Indennita &rhs) const;
    ~Indennita();

    QString voce(Utilities::VoceIndennita tipo, int unita) const ;
    QString sub(Utilities::VoceIndennita tipo, int unita) const ;

    void setMap(QMap<Utilities::VoceIndennita, QMap<int, QPair<QString, QString>>> map);
    void addItem(Utilities::VoceIndennita tipo, int unita, const QString &voce, const QString &sub);
    QMap<Utilities::VoceIndennita, QMap<int, QPair<QString, QString>>> map() const;

signals:


private:
    QSharedDataPointer<IndennitaData> data;
};

#endif // INDENNITA_H
