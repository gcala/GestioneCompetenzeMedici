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
        QString badge;
        QString name;
        QString deficit;
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
    void printBadge(QPainter &painter, const QString &text, int row);
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
