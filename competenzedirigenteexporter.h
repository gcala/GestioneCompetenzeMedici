/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
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
    CompetenzeDirigenteExporter(QObject *parent = nullptr);
    ~CompetenzeDirigenteExporter();

    void setPath(const QString &path);
    void setUnita(int id);
    void setTable(const QString &tableName);
    void setDirigente(int id);

protected:
    void run() override;

signals:
      void exportFinished(QString file);
      void totalRows(int);
      void currentRow(int);

private:
    const int m_maxPageWidth;
    const int m_rowHeight;
    const int m_assenzeVOffset;
    const int m_boxVOffset;
    const int m_boxHeight;
    const int m_boxSpacing;
    const int m_riepilogoVOffset;
    QString m_path;
    int m_idUnita;
    QString m_tableName;
    QString m_unitaName;
    QString m_mese;
    int m_idDirigente;
    Competenza *m_competenza{};

    void disegnaScheletro(QPainter &painter);
    void printDirigente(QPainter &painter);
    QFont numberFont();
    QFont unitaFont();
    QFont nameFont();
    QFont meseFont();
    QFont bodyFont();
    QFont bodyFontBold();
    QFont totalsFont();
    QFont sectionFont();

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
