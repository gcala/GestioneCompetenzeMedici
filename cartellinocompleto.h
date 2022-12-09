/*
 *  SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>
 * 
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CARTELLINOCOMPLETO_H
#define CARTELLINOCOMPLETO_H

#include <QObject>
#include <QSharedDataPointer>

class CartellinoCompletoData;
class TotaliCartellinoCompleto;
class GiornoCartellinoCompleto;

class CartellinoCompleto : public QObject
{
    Q_OBJECT
public:
    explicit CartellinoCompleto(QObject *parent = nullptr);
    CartellinoCompleto(int anno, int mese, int matricola);
    CartellinoCompleto(const CartellinoCompleto &rhs);
    CartellinoCompleto &operator=(const CartellinoCompleto &);
    bool operator==(const CartellinoCompleto &) const;
    ~CartellinoCompleto();

    void addGiorno(const GiornoCartellinoCompleto &giorno);
    void addTotali(const TotaliCartellinoCompleto &totali);
    void aggiornaValori();

    QList<GiornoCartellinoCompleto> giorni() const;
    TotaliCartellinoCompleto totali() const;
    int ordinarie() const;
    int recuperate() const;
    int giustificate() const;
    int strlPagato() const;
    int saldo() const;
    int minutiCausale(const QString &causale) const;
    QMap<QString, int> causali() const;
    QMap<QString, QVector<int>> giorniCausali() const;
    QStringList timbratureGiorno(const int giorno);
    GiornoCartellinoCompleto giorno(int giorno) const;
    bool isLastDay(int giorno);

signals:

public slots:

private:
    QSharedDataPointer<CartellinoCompletoData> data;

    int arrotondamento(int minuti) const;
    int arrotondamentoStrl(QVector<int> strlList) const;
};

#endif // CARTELLINOCOMPLETO_H
