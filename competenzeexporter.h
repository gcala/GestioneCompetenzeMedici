/*
    SPDX-FileCopyrightText: 2023 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef COMPETENZEEXPORTER_H
#define COMPETENZEEXPORTER_H

#include <QThread>
#include <QObject>
#include <QDate>

class Competenza;
class CompetenzePagate;

class CompetenzeExporter : public QThread
{
    Q_OBJECT
public:
    explicit CompetenzeExporter(QObject *parent = nullptr);

    void setPath(const QString &path);
    void setUnita(int id);
    void setMatricole(const QVector<int> &matricole);
    void setMeseDa(const QString &timecard);
    void setMeseA(const QString &timecard);

protected:
    void run() override;

signals:
      void exportFinished(const QString &file);
      void totalRows(int);
      void currentRow(int);

private:
    QString m_path;
    int m_idUnita;
    QString m_timecardDa;
    QString m_timecardA;
    QVector<int> m_matricole;
    Competenza *m_competenze{};
    CompetenzePagate *m_competenzePagate{};
    QDate m_daMonthYear;
    QDate m_aMonthYear;
};

#endif // COMPETENZEEXPORTER_H
