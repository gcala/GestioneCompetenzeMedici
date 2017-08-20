/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017 Giuseppe Calà <giuseppe.cala@mailbox.org>
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

#ifndef COMPETENZEUNITAEXPORTER_H
#define COMPETENZEUNITAEXPORTER_H

#include <QThread>

class Dipendente;
class QPainter;
class Competenza;

class CompetenzeUnitaExporter : public QThread
{
    Q_OBJECT
public:
    CompetenzeUnitaExporter(QObject *parent = 0);
    ~CompetenzeUnitaExporter();

    void setPath(const QString &path);
    void setUnita(int id);
    void setMese(QString id);

protected:
    void run() override;

signals:
      void exportFinished();
      void totalRows(int);
      void currentRow(int);

private:
    QString m_path;
    int m_idUnita;
    QString m_idMese;
    Competenza *m_competenza;

    const int m_firstHeaderHeight;
    const int m_secondHeaderHeight;
    const int m_thirdHeaderHeight;
    const int m_gridWidth;
    const int m_gridHeight;
    const int m_totalRows;
    const int m_tableWidth;
    const int m_totalHeaderHeight;

    const int m_tableHeight;

    void disegnaTabella(QPainter &painter);
    QFont numberFont();
    QFont unitaFont();
    QFont headerFont();
    QFont headerLightFont();
    QFont badgeFont();
    QString getUnitaName(const QString &id);
    QStringList getDirigentiIDs(const QString &id);
    QStringList getUnitaIDs(const QString &id);

    void printMonth(QPainter &painter, const QString &text);
    void printUnitaName(QPainter &painter, const QString &text);
    void printUnitaNumber(QPainter &painter, const QString &text);
    void printBadge(QPainter &painter, const QString &text, int row);
    void printName(QPainter &painter, const QString &text, int row);
    void printNotturno(QPainter &painter, const QString &text, int row);
    void printFestivo(QPainter &painter, const QString &text, int row);
    void printStrRepartoOrdin(QPainter &painter, const QString &text, int row);
    void printStrRepartoFesONott(QPainter &painter, const QString &text, int row);
    void printStrRepartoFesENott(QPainter &painter, const QString &text, int row);
    void printRepNumTurni(QPainter &painter, const QString &text, int row);
    void printRepNumOre(QPainter &painter, const QString &text, int row);
    void printNumGuarNott(QPainter &painter, const QString &text, int row);
    void printNumGfFesNott(QPainter &painter, const QString &text, int row);
    void printNumOreGuarFesENot(QPainter &painter, const QString &text, int row);
    void printNumOreGuarFesONot(QPainter &painter, const QString &text, int row);
    void printNumOreGuarOrd(QPainter &painter, const QString &text, int row);
    void printNumOreRepFesENot(QPainter &painter, const QString &text, int row);
    void printNumOreRepFesONot(QPainter &painter, const QString &text, int row);
    void printNumOreRepOrd(QPainter &painter, const QString &text, int row);
    QRect getRect(int row, int column) const;
};

#endif // COMPETENZEUNITAEXPORTER_H
