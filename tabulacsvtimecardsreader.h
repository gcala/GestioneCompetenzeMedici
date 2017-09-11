/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017 Giuseppe Calà <giuseppe.cala@mailbox.org>
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

#ifndef TABULACSVTIMECARDSREADER_H
#define TABULACSVTIMECARDSREADER_H

#include <QThread>

class Dipendente;
class NomiUnitaDialog;

class TabulaCsvTimeCardsReader : public QThread
{
    Q_OBJECT
public:
    TabulaCsvTimeCardsReader(QObject *parent = 0);
    ~TabulaCsvTimeCardsReader();

    void setFile(const QString &);

protected:
    void run() override;

signals:
      void timeCardsRead();
      void totalRows(int);
      void currentRow(int);

private:
      QStringList causaliFerie;
      QStringList causaliMalattia;
      QStringList causaliCongedi;
      QStringList causaliRMP;
      QStringList causaliRMC;
      QStringList causaliDaValutare;
      QString fileName;
      Dipendente *m_dipendente;
      NomiUnitaDialog *m_nomiDialog;
      bool m_timeCardBegin;

      int mese2Int(const QString &mese);
      int inMinuti(const QString &time);
};

#endif // TABULACSVTIMECARDSREADER_H
