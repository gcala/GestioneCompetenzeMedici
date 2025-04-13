#ifndef REPERIBILITASEMPLIFICATA_H
#define REPERIBILITASEMPLIFICATA_H

#include <QObject>
#include <QSharedDataPointer>
#include <QDate>

class ReperibilitaSemplificataData;

class ReperibilitaSemplificata : public QObject
{
    Q_OBJECT
public:
    explicit ReperibilitaSemplificata(int idUnita = 0,
                                      QDate decorrenza = QDate(2017,9,1),
                                      double feriale = 0.0,
                                      double sabato = 0.0,
                                      double festivo = 0.0,
                                      bool prefestivo = false);
    ReperibilitaSemplificata(const ReperibilitaSemplificata &);
    ReperibilitaSemplificata &operator=(const ReperibilitaSemplificata &);
    bool operator==(const ReperibilitaSemplificata &rhs) const;
    ~ReperibilitaSemplificata();

    int idUnita() const;
    QDate decorrenza() const;
    double feriale() const;
    double sabato() const;
    double festivo() const;
    bool prefestivo() const;

signals:


private:
    QSharedDataPointer<ReperibilitaSemplificataData> data;
};

#endif // REPERIBILITASEMPLIFICATA_H
