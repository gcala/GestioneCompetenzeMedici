#ifndef TOTALICARTELLINOCOMPLETO_H
#define TOTALICARTELLINOCOMPLETO_H

#include <QObject>
#include <QSharedDataPointer>

class TotaliCartellinoCompletoData;

class TotaliCartellinoCompleto : public QObject
{
    Q_OBJECT
public:
    explicit TotaliCartellinoCompleto(QObject *parent = nullptr);
    TotaliCartellinoCompleto(const TotaliCartellinoCompleto &);
    TotaliCartellinoCompleto &operator=(const TotaliCartellinoCompleto &);
    bool operator==(const TotaliCartellinoCompleto &) const;
    ~TotaliCartellinoCompleto();

    int disponibile() const;
    int ordinario() const;
    int recuperato() const;
    int giustificato() const;
    int daRecuperare() const;
    int eccedenza() const;
    int strPagato() const;
    int strReperibilita() const;
    int bancaOre() const;
    int scarRes() const;
    int saldoMese() const;
    int totale() const;

    void setDisponibile(int value);
    void setOrdinario(int value);
    void setRecuperato(int value);
    void setGiustificato(int value);
    void setDaRecuperare(int value);
    void setEccedenza(int value);
    void setStrPagato(int value);
    void setStrReperibilita(int value);
    void setBancaOre(int value);
    void setScarRes(int value);
    void setSaldoMese(int value);
    void setTotale(int value);

signals:


private:
    QSharedDataPointer<TotaliCartellinoCompletoData> data;
};

#endif // TOTALICARTELLINOCOMPLETO_H
