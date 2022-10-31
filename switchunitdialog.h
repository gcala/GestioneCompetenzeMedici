#ifndef SWITCHUNITDIALOG_H
#define SWITCHUNITDIALOG_H

#include <QDialog>

class QAbstractItemModel;

namespace Ui {
class SwitchUnitDialog;
}

class SwitchUnitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SwitchUnitDialog(QWidget *parent = nullptr);
    ~SwitchUnitDialog();

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void on_meseCompetenzeCB_currentIndexChanged(int index);
    void on_unitaCompetenze1CB_currentIndexChanged(int index);
    void on_unitaCompetenze2CB_currentIndexChanged(int index);
    void listsChanged();
    void on_restoreButton_clicked();
    void on_saveButton_clicked();

private:
    Ui::SwitchUnitDialog *ui;
    QAbstractItemModel *m_dirigenti1Model;
    QAbstractItemModel *m_dirigenti2Model;

    QVector<int> m_dirigenti1Initial;
    QVector<int> m_dirigenti1Current;
    QVector<int> m_dirigenti2Initial;
    QVector<int> m_dirigenti2Current;

    void populateMeseCompetenzeCB();
    void populateUnitaCompetenzeCB();
    void enableButtons();
    void enableComboBoxes();
    void enableLists();
};

#endif // SWITCHUNITDIALOG_H
