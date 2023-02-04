/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef RENDERAREAREP_H
#define RENDERAREAREP_H

#include "utilities.h"

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMap>
#include <QDate>

class RenderAreaRep : public QWidget
{
    Q_OBJECT

public:
    RenderAreaRep(QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void setRepMap(const QMap<QDate, Utilities::ValoreRep> &repMap);

public slots:

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMap<QDate, Utilities::ValoreRep> m_repMap;
    QList<QDate> m_grandiFestivita;
    int year;

    void calcolaFeste();
    void caricaGrandiFestivita(int anno);
    void caricaPasqua(int anno);
};

#endif // RENDERAREAREP_H
