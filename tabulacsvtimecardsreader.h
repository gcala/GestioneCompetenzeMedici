/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef TABULACSVTIMECARDSREADER_H
#define TABULACSVTIMECARDSREADER_H

#include <QThread>
#include <QVector>

class Dipendente;

class TabulaCsvTimeCardsReader : public QThread
{
    Q_OBJECT
public:
    TabulaCsvTimeCardsReader(QObject *parent = nullptr);
    ~TabulaCsvTimeCardsReader();

    void setFile(const QString &);

protected:
    void run() override;

signals:
      void timeCardsRead();
      void totalRows(int);
      void currentRow(int);
      void selectUnit(QString, int&);

private:
      QStringList causaliFerie;
      QStringList causaliMalattia;
      QStringList causaliCongedi;
      QStringList causaliRMP;
      QStringList causaliRMC;
      QStringList causaliDaValutare;
      QString fileName;
      Dipendente *m_dipendente;
      bool m_timeCardBegin;
      QVector<int> m_giorniRiposo;

      int mese2Int(const QString &mese);
      int inMinuti(const QString &time);
};

#endif // TABULACSVTIMECARDSREADER_H
