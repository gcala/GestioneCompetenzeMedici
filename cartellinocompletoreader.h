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

#ifndef CARTELLINOCOMPLETOREADER_H
#define CARTELLINOCOMPLETOREADER_H

#include <QThread>

class Dipendente;
class GiornoCartellinoCompleto;
class TotaliCartellinoCompleto;

class CartellinoCompletoReader : public QThread
{
    Q_OBJECT
public:
    CartellinoCompletoReader(QObject *parent = nullptr);
    ~CartellinoCompletoReader();

    void setFile(const QString &);
    void setDbFile(const QString &file);
    void setDriver(const QString &driver);

protected:
    void run() override;

signals:
    void timeCardsRead();
    void timeCardsError(QString);
    void totalRows(int);
    void currentRow(int);
    void showErrorMessage(QString, QString);
    void turnoNonTrovato(int matricola, int codice);
    void selectUnit(int id, int& idUnit);

private:
    QString fileName;
    QString m_dbFile;
    int m_matricola;
    bool m_timeCardBegin;
    Dipendente *m_dipendente;
    QStringList causaliRMP;
    QStringList causaliRMC;

    int mese2Int(const QString &mese);
    QString int2MeseBreve(const int &mese);
    GiornoCartellinoCompleto giorno(const QString &line);
    TotaliCartellinoCompleto totali(const QStringList &fields);
    void valutaCausale(const QString &causale, const QDate & dataCorrente, const GiornoCartellinoCompleto &giorno, const QTime &orario, bool &guardia, const bool lastDay);
};

#endif // CARTELLINOCOMPLETOREADER_H
