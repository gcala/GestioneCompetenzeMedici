#ifndef DPMCOMPUTE_H
#define DPMCOMPUTE_H

#include <QThread>

class DmpCompute;

namespace The {
    DmpCompute* dmpCompute();
}

class DmpCompute : public QThread
{
    Q_OBJECT
public:
    friend DmpCompute* The::dmpCompute();

    void setUnita(const int &id);
    void setTable(const QString &tableName);
    void setDirigente(const int &id);

protected:
    void run() override;

signals:
      void computeFinished();
      void totalItems(int);
      void currentItem(int);

private:
      DmpCompute();
      int m_idUnita;
      QString m_tableName;
      QString m_unitaName;
      QString m_mese;
      int m_idDirigente;
      int m_currItem;

      void ricalcolaDmp(const QStringList &timecards, const int &idDirigente);
};

#endif // DPMCOMPUTE_H
