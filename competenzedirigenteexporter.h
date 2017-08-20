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

#ifndef COMPETENZEDIRIGENTEEXPORTER_H
#define COMPETENZEDIRIGENTEEXPORTER_H

#include <QThread>

class Dipendente;
class QPainter;
class Competenza;

class CompetenzeDirigenteExporter : public QThread
{
    Q_OBJECT
public:
    CompetenzeDirigenteExporter(QObject *parent = 0);
    ~CompetenzeDirigenteExporter();

    void setPath(const QString &path);
    void setUnita(int id);
    void setMese(QString id);
    void setDirigente(int id);

protected:
    void run() override;

signals:
      void exportFinished();
      void totalRows(int);
      void currentRow(int);

private:
    const int m_maxPageWidth;
    const int m_rowHeight;
    const int m_assenzeVOffset;
    const int m_lineSpacing;
    QString m_path;
    int m_idUnita;
    QString m_idMese;
    QString m_unitaName;
    QString m_mese;
    int m_idDirigente;
    Competenza *m_competenza;

    void disegnaScheletro(QPainter &painter);
    void printDirigente(QPainter &painter);
    QFont numberFont();
    QFont unitaFont();
    QFont nameFont();
    QFont meseFont();
    QFont bodyFont();
    QFont bodyFontBold();
    QFont sectionFont();
    QString getUnitaName(const QString &id);
    QStringList getDirigentiIDs(const QString &id);
    QStringList getUnitaIDs(const QString &id);

    void printMonth(QPainter &painter, const QString &text);
    void printUnitaName(QPainter &painter, const QString &text);
    void printUnitaNumber(QPainter &painter, const QString &text);
    void printName(QPainter &painter, const QString &text);
    void printGiorniLavorati(QPainter &painter);
    void printFerie(QPainter &painter);
    void printCongedi(QPainter &painter);
    void printMalattia(QPainter &painter);
    void printRmp(QPainter &painter);
    void printRmc(QPainter &painter);
    void printAltreAssenze(QPainter &painter);
    void printGuardieDiurne(QPainter &painter);
    void printGuardieNotturne(QPainter &painter);
    void printDistribuzioneOreGuardia(QPainter &painter);
    void printDistribuzioneOreReperibilita(QPainter &painter);
    void printTurniProntaDisponibilita(QPainter &painter);
    void printOreLavoroDovute(QPainter &painter);
    void printOreLavoroEffettuate(QPainter &painter);
    void printNettoOre(QPainter &painter);
    void printOreStraordinarioGuardie(QPainter &painter);
    void printOreProntaDisponibilita(QPainter &painter);
    void printRep(QPainter &painter);
    void printDeficit(QPainter &painter);
    void printNotte(QPainter &painter);
    void printFestivo(QPainter &painter);
};

#endif // COMPETENZEDIRIGENTEEXPORTER_H
