/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef DIFFERENZEEXPORTER_H
#define DIFFERENZEEXPORTER_H

#include <QThread>
#include <QDate>

class QPainter;
class Competenza;
class CompetenzePagate;

class DifferenzeExporter : public QThread
{
    Q_OBJECT
public:
    DifferenzeExporter(QObject *parent = nullptr);
    ~DifferenzeExporter();

    void setPath(const QString &path);
    void setUnita(int id);
    void setMese(const QString &timecard);
    void setStoricizza(bool ok);
    void setMesePagamento(int mese);
    void setAnnoPagamento(int anno);

protected:
    void run() override;

signals:
      void exportFinished(const QString &file);
      void totalRows(int);
      void currentRow(int);

private:
    QString m_path;
    int m_idUnita;
    int m_mesePagamento;
    int m_annoPagamento;
    QString m_timecard;
    Competenza *m_competenze{};
    CompetenzePagate *m_competenzePagate{};
    QDate m_currentMonthYear;

    bool m_storicizza{};
};

#endif // DIFFERENZEEXPORTER_H
