/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef RENDERAREA_H
#define RENDERAREA_H

#include "defines.h"

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMap>

class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void setGuardiaMap(const QMap<int, GuardiaType> &guardiaMap);
    void setDiurna(const bool ok);
    void setMeseAnno(const int &mese, const int &anno);

public slots:

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMap<int, GuardiaType> m_guardiaMap;
    bool m_diurna;
    int m_mese;
    int m_anno;
};

#endif // RENDERAREA_H
