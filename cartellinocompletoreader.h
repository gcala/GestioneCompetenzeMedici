/*
 *  SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>
 * 
 *  SPDX-License-Identifier: GPL-3.0-or-later
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
    void anomalieFound(QString);
    void timeCardsError(QString);
    void totalRows(int);
    void currentRow(int);
    void showErrorMessage(QString, QString);
    void cartellinoInvalido(QString);
    void turnoNonTrovato(int matricola, int codice);
    void selectUnit(const QString &nominativo, int& idUnit);

private:
    QString fileName;
    QString m_dbFile;
    int m_matricola;
    bool m_timeCardBegin;
    Dipendente *m_dipendente;
    QStringList causaliRMP;
    QStringList causaliRMC;
    QVector<int> m_giorniEccr;
    QVector<int> m_giorniGrep;
    QVector<int> m_giorniGuar;
    QVector<int> m_giorniGuarNot;
    QVector<int> m_giorniGuarDiu;
    QVector<int> m_giorniMezzaGuarDiu;
    QVector<int> m_giorniIndFes;
    QVector<int> m_giorniAltraCausale;

    int mese2Int(const QString &mese);
    QString int2MeseBreve(const int &mese);
    GiornoCartellinoCompleto giorno(const QString &line, const int anno, const int mese);
    TotaliCartellinoCompleto totali(const QStringList &fields);
    void valutaCausale(const QString &causale, const QDate & dataCorrente, const GiornoCartellinoCompleto &giorno, const QTime &orario, bool &guardia, const bool lastDay);
    void valutaIndennitaFestiva(const int anno, const int mese, const GiornoCartellinoCompleto &giorno);
    void valutaEccr(const QString &causale, const QDate &dataCorrente, const QTime &orario);
    void valutaGrep(const QString &causale, const QDate &dataCorrente, const GiornoCartellinoCompleto &giorno, const QTime &orario);
    void valutaAltraCausale(const QString &causale, const QDate &dataCorrente, const QTime &orario);
    void valutaGuardiaDiurna(const QDate &dataCorrente, const GiornoCartellinoCompleto &giorno, const GiornoCartellinoCompleto &giornoSuccessivo, const bool lastDay);
    void valutaGuardiaNotturna(const QDate &dataCorrente, const GiornoCartellinoCompleto &giorno, const GiornoCartellinoCompleto &giornoPrecedente, const bool lastDay);
};

#endif // CARTELLINOCOMPLETOREADER_H
