/*
    SPDX-FileCopyrightText: 2023 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef COMPETENZEEXPORTERDIALOG_H
#define COMPETENZEEXPORTERDIALOG_H

#include <QDialog>

namespace Ui {
class CompetenzeExporterDialog;
}

class CompetenzeExporterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CompetenzeExporterDialog(QWidget *parent = nullptr);
    ~CompetenzeExporterDialog();

    void clearMese();
    void addMese(const QString &mese, const QVariant &id);
    void setCurrentMese(int index);
    void setCurrentUnita(int index);
    void setPath(const QString &path);
    QString path() const;
    int currentUnitaData() const;
    QString meseDa() const;
    QString meseA() const;
    QVector<int> matricole() const;
    bool proceed;

private slots:
    void browseButtonClicked();

private:
    Ui::CompetenzeExporterDialog *ui;
};

#endif // COMPETENZEEXPORTERDIALOG_H
