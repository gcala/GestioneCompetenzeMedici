/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "twodigitsspinbox.h"

#include <QPainter>

TwoDigitsSpinBox::TwoDigitsSpinBox(QWidget *parent) :
    QSpinBox(parent)
{

}

QString TwoDigitsSpinBox::textFromValue ( int value ) const
{
    return QString("%1").arg(value, 2 , 10, QChar('0'));
}
