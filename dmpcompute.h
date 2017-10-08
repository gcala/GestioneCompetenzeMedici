#ifndef DPMCOMPUTE_H
#define DPMCOMPUTE_H

#include <QThread>

class DmpCompute : public QThread
{
    Q_OBJECT
public:
    DmpCompute(QObject *parent = 0);
    ~DmpCompute();

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
      int m_idUnita;
      QString m_tableName;
      QString m_unitaName;
      QString m_mese;
      int m_idDirigente;
};

#endif // DPMCOMPUTE_H
