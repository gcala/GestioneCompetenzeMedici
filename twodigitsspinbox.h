/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef TWODIGITSSPINBOX_H
#define TWODIGITSSPINBOX_H

#include <QSpinBox>

class TwoDigitsSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit TwoDigitsSpinBox(QWidget *parent = nullptr);

protected:
    virtual QString textFromValue ( int value ) const;
};

#endif // TWODIGITSSPINBOX_H
