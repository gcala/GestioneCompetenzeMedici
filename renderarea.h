/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017-2018 Giuseppe Calà <giuseppe.cala@mailbox.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
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
