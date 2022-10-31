#ifndef TIMBRATURA_H
#define TIMBRATURA_H

#include <QObject>
#include <QSharedDataPointer>

class TimbraturaData;

class Timbratura
{
public:
    Timbratura();
    Timbratura(QString text);
    Timbratura(const Timbratura &);
    Timbratura &operator=(const Timbratura &);
    bool operator==(const Timbratura &rhs) const;
    ~Timbratura();

    QString text() const;
    QTime ora() const;
    bool isCanceled() const;
    bool isCorrect() const;
    bool isIngressoRep() const;
    bool isUscitaRep() const;
    bool isEmpty() const;
    bool isValid() const;

private:
    QSharedDataPointer<TimbraturaData> data;
};

#endif // TIMBRATURA_H
