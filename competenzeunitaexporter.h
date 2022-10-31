/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef COMPETENZEUNITAEXPORTER_H
#define COMPETENZEUNITAEXPORTER_H

#include <QThread>
#include <QDate>

class QPainter;
class Competenza;

class CompetenzeUnitaExporter : public QThread
{
    Q_OBJECT
public:
    CompetenzeUnitaExporter(QObject *parent = nullptr);
    ~CompetenzeUnitaExporter();

    void setPath(const QString &path);
    void setUnita(int id);
    void setMese(const QString &timecard);
    void setPrintCasi(bool ok);
    void setPrintData(bool ok);

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
    Competenza *m_competenza{};
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

    bool m_printCasi{};
    bool m_printData{};

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
    void printBadge(QPainter &painter, const int &text, int row);
    void printName(QPainter &painter, const QString &text, int row);
    void printDeficit(QPainter&painter, const QString &text, int row);
    void printNotturno(QPainter &painter, const int value, int row);
    void printFestivo(QPainter &painter, const int value, int row);
    void printStrRepartoOrdin(QPainter &painter, const QString &text, int row);
    void printStrRepartoFesONott(QPainter &painter, const QString &text, int row);
    void printStrRepartoFesENott(QPainter &painter, const QString &text, int row);
    void printRepNumTurni(QPainter &painter, const QString &text, int row);
    void printRepNumOre(QPainter &painter, const QString &text, int row);
    void printNumGuarDiur(QPainter &painter, const QString &text, int row);
    void printNumGuarNott(QPainter &painter, const QString &text, int row);
    void printNumGfFesNott(QPainter &painter, const QString &text, int row);
    void printNumOreGuarFesENot(QPainter &painter, const QString &text, int row);
    void printNumOreGuarFesONot(QPainter &painter, const QString &text, int row);
    void printNumOreGuarOrd(QPainter &painter, const QString &text, int row);
    void printNumOreRepFesENot(QPainter &painter, const QString &text, int row);
    void printNumOreRepFesONot(QPainter &painter, const QString &text, int row);
    void printNumOreRepOrd(QPainter &painter, const QString &text, int row);
    void printCasi(QPainter &painter);
    void printNote(QPainter &painter, const QStringList &note);
    void printData(QPainter &painter, const QString &text);
    QRect getRect(int row, int column) const;
};

#endif // COMPETENZEUNITAEXPORTER_H
