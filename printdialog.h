/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <QDialog>

namespace Ui {
class PrintDialog;
}

class PrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrintDialog(QWidget *parent = nullptr);
    ~PrintDialog();

    enum ToolOps { PrintUnits, PrintDoctors, CalcDpm, PrintDeficit };
    Q_ENUM(ToolOps)

    void addUnita(const QString &unita, const QVariant &id);
    void addMese(const QString &mese, const QVariant &id);
    void addDirigente(const QString &dirigente, const QVariant &id);
    void clearUnita();
    void clearMese();
    void clearDirigente();
    QString currentMeseData() const;
    int currentUnitaData() const;
    int currentDirigenteData() const;
    void setCurrentMese(int index);
    void setCurrentUnita(int index);
    void setCurrentDirigente(int index);
    bool proceed;
    QString type;
    QString path() const;
    void setPath(const QString &path);
    void setCurrentOp(const ToolOps &op);
    ToolOps currentOp() const;
    bool casiIsChecked() const;
    bool dataIsChecked() const;

private slots:
    void on_cancelButton_clicked();
    void on_saveButton_clicked();
    void on_unitaCB_currentIndexChanged(int index);
    void on_meseCB_currentIndexChanged(int index);
    void on_browseButton_clicked();
    void on_actionGeneraCSV_triggered();

private:
    Ui::PrintDialog *ui;
    ToolOps m_currOp;
};

#endif // PRINTDIALOG_H
