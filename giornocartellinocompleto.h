#ifndef GIORNOCARTELLINOCOMPLETO_H
#define GIORNOCARTELLINOCOMPLETO_H

#include "timbratura.h"

#include <QObject>
#include <QSharedDataPointer>

class GiornoCartellinoCompletoData;

class GiornoCartellinoCompleto : public QObject
{
    Q_OBJECT
public:
    explicit GiornoCartellinoCompleto(QObject *parent = nullptr);
    GiornoCartellinoCompleto(const GiornoCartellinoCompleto &);
    GiornoCartellinoCompleto &operator=(const GiornoCartellinoCompleto &);
    ~GiornoCartellinoCompleto();

    int giorno() const;
    int turno() const;
    QString tipo() const;
    QString indennita() const;
    Timbratura entrata1() const;
    Timbratura uscita1() const;
    Timbratura entrata2() const;
    Timbratura uscita2() const;
    Timbratura entrata3() const;
    Timbratura uscita3() const;
    Timbratura entrata4() const;
    Timbratura uscita4() const;
    QTime ordinario() const;
    QTime assenza() const;
    QTime eccedenza() const;
    QTime daRecuperare() const;
    QTime recuperate() const;
    QString causale1() const;
    QTime ore1() const;
    QString causale2() const;
    QTime ore2() const;
    QString causale3() const;
    QTime ore3() const;
    QTime repDiurna() const;
    QTime repNotturna() const;

    void setGiorno(const int &giorno);
    void setTurno(const int &turno);
    void setTipo(const QString &tipo);
    void setIndennita(const QString &indennita);
    void setEntrata1(const Timbratura &entrata1);
    void setUscita1(const Timbratura &uscita1);
    void setEntrata2(const Timbratura &entrata2);
    void setUscita2(const Timbratura &uscita2);
    void setEntrata3(const Timbratura &entrata3);
    void setUscita3(const Timbratura &uscita3);
    void setEntrata4(const Timbratura &entrata4);
    void setUscita4(const Timbratura &uscita4);
    void setOrdinario(const QTime &ordinario);
    void setAssenza(const QTime &assenza);
    void setEccedenza(const QTime &eccedenza);
    void setDaRecuperare(const QTime &daRecuperare);
    void setRecuperate(const QTime &recuperate);
    void setCausale1(const QString &causale1);
    void setOre1(const QTime &ore1);
    void setCausale2(const QString &causale2);
    void setOre2(const QTime &ore2);
    void setCausale3(const QString &causale3);
    void setOre3(const QTime &ore3);
    void setRepDiurna(const QTime &repDiurna);
    void setRepNotturna(const QTime &repNotturna);

signals:

public slots:

private:
    QSharedDataPointer<GiornoCartellinoCompletoData> data;
};

#endif // GIORNOCARTELLINOCOMPLETO_H
