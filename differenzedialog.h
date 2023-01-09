#ifndef DIFFERENZEDIALOG_H
#define DIFFERENZEDIALOG_H

#include <QDialog>

namespace Ui {
class DifferenzeDialog;
}

class DifferenzeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DifferenzeDialog(QWidget *parent = nullptr);
    ~DifferenzeDialog();

    void clearMese();
    void addMese(const QString &mese, const QVariant &id);
    QString currentMeseData() const;
    int currentUnitaData() const;
    void setCurrentMese(int index);
    void setCurrentUnita(int index);
    bool proceed;
    QString path() const;
    void setPath(const QString &path);
    bool storicizzaIsChecked() const;
    bool pdfIsChecked() const;

private slots:
    void meseCurrentIndexChanged(int index);
    void browseButtonClicked();

private:
    Ui::DifferenzeDialog *ui;
};

#endif // DIFFERENZEDIALOG_H
