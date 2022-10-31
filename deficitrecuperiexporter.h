/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef DEFICITRECUPERIEXPORTER_H
#define DEFICITRECUPERIEXPORTER_H

#include <QThread>

class QPainter;
class Competenza;

class DeficitRecuperiExporter : public QThread
{
    Q_OBJECT
public:
    DeficitRecuperiExporter(QObject *parent = nullptr);
    ~DeficitRecuperiExporter();

    void setPath(const QString &path);
    void setUnita(int id);
    void setMese(const QString &timecard);
    void setType(const QString &type);

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
    QString m_type;
    Competenza *m_competenza{};

    struct Doctor{
        int badge;
        QString name;
        QString deficitProgressivo;
        QString deficitPuntuale;
        QString oreRecuperabili;
        QString oreNonRecuperabili;
        QList<QDate> ferie;
        QList<QDate> congedi;
        QList<QDate> malattie;
        QList<QDate> recuperi;
    };

    const int m_pageWidth;
    const int m_pageHeight;
    const int m_badgeWidth;
    const int m_nameWidth;
    const int m_deficitWidth;
    const int m_cellSpacing;
    const int m_titleHeight;
    const int m_unitHeight;
    const int m_rowHeight;
    const int m_rowFontPixels;
    const int m_unitFontPixels;
    const int m_titleFontPixels;
    const int m_pageFontPixels;
    int m_offset{};

    void printTitle(QPainter &painter, const QString &text);
    void printUnita(QPainter &painter, const int &id, const QString &text);
    void printBadge(QPainter &painter, const int &text, int row);
    void printName(QPainter &painter, const QString &text, int row);
    void printDeficit(QPainter &painter, const QString &text, int row);
    void printPageNumber(QPainter &painter, int page);
    QFont titleFont();
    QFont unitFont();
    QFont rowFont();
    QFont pageFont();
    void printPdf(const QString &fileName, const QString &mese, const QVector<int> &unitaIdList);
    void printCsv(const QString &fileName, const QString &mese, const QVector<int> &unitaIdList);
};

#endif // DEFICITRECUPERIEXPORTER_H
