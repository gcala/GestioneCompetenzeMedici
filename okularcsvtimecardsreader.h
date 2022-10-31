/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef OKULARCSVTIMECARDSREADER_H
#define OKULARCSVTIMECARDSREADER_H

#include <QThread>

class Dipendente;
class NomiUnitaDialog;

class OkularCsvTimeCardsReader : public QThread
{
    Q_OBJECT
public:
    OkularCsvTimeCardsReader(QObject *parent = nullptr);
    ~OkularCsvTimeCardsReader();

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

#endif // OKULARCSVTIMECARDSREADER_H
