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
    void setPrintPdf(bool ok);

protected:
    void run() override;

signals:
      void exportFinished(const QString &file);
      void totalRows(int);
      void currentRow(int);

private:
    QString m_path;
    int m_idUnita;
    QString m_timecard;
    Competenza *m_competenze{};
    CompetenzePagate *m_competenzePagate{};
    QDate m_currentMonthYear;

    const int m_firstHeaderHeight;
    const int m_secondHeaderHeight;
    const int m_thirdHeaderHeight;
    const int m_gridWidth;
    const int m_gridHeight;
    const int m_totalRows;
    const int m_tableWidth;
    const int m_totalHeaderHeight;

    int m_casiIndennitaNotturna{};
    int m_casiIndennitaFestiva{};
    int m_casiStrRepaOrd{};
    int m_casiStrRepaFesONott{};
    int m_casiStrRepaFesENott{};
    int m_casiStrRepeOrd{};
    int m_casiStrRepeFesONott{};
    int m_casiStrRepeFesENott{};
    int m_casiStrGuarOrd{};
    int m_casiStrGuarFesONott{};
    int m_casiStrGuarFesENott{};
    int m_casiRepeTurni{};
    int m_casiRepeOre{};
    int m_casiGuarDiur{};
    int m_casiGuarNott{};
    int m_casiGranFest{};

    bool m_printPdf{};
    bool m_storicizza{};

    const int m_tableHeight;

    void disegnaTabella(QPainter &painter);
    QFont numberFont();
    QFont unitaFont();
    QFont headerFont();
    QFont headerLightFont();
    QFont badgeFont();

    void printMonth(QPainter &painter, const QString &text);
    void printUnitaName(QPainter &painter, const QString &text);
    void printUnitaNumber(QPainter &painter, const int &id);
    void printBadge(QPainter &painter, const int &text, const int row);
    void printName(QPainter &painter, const QString &text, const int row);
    void printDeficit(QPainter&painter, const QString &text, const int row);
    void printNotturno(QPainter &painter, const int value, const int row);
    void printFestivo(QPainter &painter, const int value, const int row);
    void printStrRepartoOrdin(QPainter &painter, const int value, const int row);
    void printStrRepartoFesONott(QPainter &painter, const int value, const int row);
    void printStrRepartoFesENott(QPainter &painter, const int value, const int row);
    void printRepNumTurni(QPainter &painter, const int value, const int row);
    void printRepNumOre(QPainter &painter, const int value, const int row);
    void printNumGuarDiur(QPainter &painter, const int value, const int row);
    void printNumGuarNott(QPainter &painter, const int value, const int row);
    void printNumGfFesNott(QPainter &painter, const int value, const int row);
    void printNumOreGuarFesENot(QPainter &painter, const int value, const int row);
    void printNumOreGuarFesONot(QPainter &painter, const int value, const int row);
    void printNumOreGuarOrd(QPainter &painter, const int value, const int row);
    void printNumOreRepFesENot(QPainter &painter, const int value, const int row);
    void printNumOreRepFesONot(QPainter &painter, const int value, const int row);
    void printNumOreRepOrd(QPainter &painter, const int value, const int row);
    void printCasi(QPainter &painter);
    void printNote(QPainter &painter, const QStringList &note);
    void printData(QPainter &painter, const QString &text);
    QRect getRect(int row, int column) const;
};

#endif // DIFFERENZEEXPORTER_H
