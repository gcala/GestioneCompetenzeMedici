#ifndef NOMIUNITADIALOG_H
#define NOMIUNITADIALOG_H

#include <QDialog>

namespace Ui {
class NomiUnitaDialog;
}

class NomiUnitaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NomiUnitaDialog(QWidget *parent = 0);
    ~NomiUnitaDialog();

    void populateUnits();
    void setUnitaLabel(const QString &name);

private slots:
    void on_okButton_clicked();

private:
    Ui::NomiUnitaDialog *ui;
};

#endif // NOMIUNITADIALOG_H
