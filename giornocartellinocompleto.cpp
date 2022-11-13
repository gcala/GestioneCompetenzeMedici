#include "giornocartellinocompleto.h"
#include "timbratura.h"
#include "utilities.h"

#include <QTime>

class GiornoCartellinoCompletoData : public QSharedData
{
public:
    int m_giorno;
    int m_turno;
    QString m_tipo;
    QString m_indennita;
    Timbratura m_entrata1;
    Timbratura m_uscita1;
    Timbratura m_entrata2;
    Timbratura m_uscita2;
    Timbratura m_entrata3;
    Timbratura m_uscita3;
    Timbratura m_entrata4;
    Timbratura m_uscita4;
    QTime m_ordinario;
    QTime m_assenza;
    QTime m_eccedenza;
    QTime m_daRecuperare;
    QTime m_recuperate;
    QString m_causale1;
    QTime m_ore1;
    QString m_causale2;
    QTime m_ore2;
    QString m_causale3;
    QTime m_ore3;
    QTime m_repDiurna;
    QTime m_repNotturna;
};

GiornoCartellinoCompleto::GiornoCartellinoCompleto(QObject *parent) : QObject(parent), data(new GiornoCartellinoCompletoData)
{

}

GiornoCartellinoCompleto::GiornoCartellinoCompleto(const GiornoCartellinoCompleto &rhs) : data(rhs.data)
{

}

GiornoCartellinoCompleto &GiornoCartellinoCompleto::operator=(const GiornoCartellinoCompleto &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

GiornoCartellinoCompleto::~GiornoCartellinoCompleto()
{

}

void GiornoCartellinoCompleto::setGiorno(const int &giorno)
{
    data->m_giorno = giorno;
}

void GiornoCartellinoCompleto::setTurno(const int &turno)
{
    data->m_turno = turno;
}

void GiornoCartellinoCompleto::setTipo(const QString &tipo)
{
    data->m_tipo = tipo;
}

void GiornoCartellinoCompleto::setIndennita(const QString &indennita)
{
    data->m_indennita = indennita;
}

void GiornoCartellinoCompleto::setEntrata1(const Timbratura &entrata1)
{
    data->m_entrata1 = entrata1;
}

void GiornoCartellinoCompleto::setUscita1(const Timbratura &uscita1)
{
    data->m_uscita1 = uscita1;
}

void GiornoCartellinoCompleto::setEntrata2(const Timbratura &entrata2)
{
    data->m_entrata2 = entrata2;
}

void GiornoCartellinoCompleto::setUscita2(const Timbratura &uscita2)
{
    data->m_uscita2 = uscita2;
}

void GiornoCartellinoCompleto::setEntrata3(const Timbratura &entrata3)
{
    data->m_entrata3 = entrata3;
}

void GiornoCartellinoCompleto::setUscita3(const Timbratura &uscita3)
{
    data->m_uscita3 = uscita3;
}

void GiornoCartellinoCompleto::setEntrata4(const Timbratura &entrata4)
{
    data->m_entrata4 = entrata4;
}

void GiornoCartellinoCompleto::setUscita4(const Timbratura &uscita4)
{
    data->m_uscita4 = uscita4;
}

void GiornoCartellinoCompleto::setOrdinario(const QTime &ordinario)
{
    data->m_ordinario = ordinario;
}

void GiornoCartellinoCompleto::setAssenza(const QTime &assenza)
{
    data->m_assenza = assenza;
}

void GiornoCartellinoCompleto::setEccedenza(const QTime &eccedenza)
{
    data->m_eccedenza = eccedenza;
}

void GiornoCartellinoCompleto::setDaRecuperare(const QTime &daRecuperare)
{
    data->m_daRecuperare = daRecuperare;
}

void GiornoCartellinoCompleto::setRecuperate(const QTime &recuperate)
{
    data->m_recuperate = recuperate;
}

void GiornoCartellinoCompleto::setCausale1(const QString &causale1)
{
    data->m_causale1 = causale1;
}

void GiornoCartellinoCompleto::setOre1(const QTime &ore1)
{
    data->m_ore1 = ore1;
}

void GiornoCartellinoCompleto::setCausale2(const QString &causale2)
{
    data->m_causale2 = causale2;
}

void GiornoCartellinoCompleto::setOre2(const QTime &ore2)
{
    data->m_ore2 = ore2;
}

void GiornoCartellinoCompleto::setCausale3(const QString &causale3)
{
    data->m_causale3 = causale3;
}

void GiornoCartellinoCompleto::setOre3(const QTime &ore3)
{
    data->m_ore3 = ore3;
}

void GiornoCartellinoCompleto::setRepDiurna(const QTime &repDiurna)
{
    data->m_repDiurna = repDiurna;
}

void GiornoCartellinoCompleto::setRepNotturna(const QTime &repNotturna)
{
    data->m_repNotturna = repNotturna;
}

int GiornoCartellinoCompleto::giorno() const
{
    return data->m_giorno;
}

int GiornoCartellinoCompleto::turno() const
{
    return data->m_turno;
}

QString GiornoCartellinoCompleto::tipo() const
{
    return data->m_tipo;
}

QString GiornoCartellinoCompleto::indennita() const
{
    return data->m_indennita;
}

Timbratura GiornoCartellinoCompleto::entrata1() const
{
    return data->m_entrata1;
}

Timbratura GiornoCartellinoCompleto::uscita1() const
{
    return data->m_uscita1;
}

Timbratura GiornoCartellinoCompleto::entrata2() const
{
    return data->m_entrata2;
}

Timbratura GiornoCartellinoCompleto::uscita2() const
{
    return data->m_uscita2;
}

Timbratura GiornoCartellinoCompleto::entrata3() const
{
    return data->m_entrata3;
}

Timbratura GiornoCartellinoCompleto::uscita3() const
{
    return data->m_uscita3;
}

Timbratura GiornoCartellinoCompleto::entrata4() const
{
    return data->m_entrata4;
}

Timbratura GiornoCartellinoCompleto::uscita4() const
{
    return data->m_uscita4;
}

QTime GiornoCartellinoCompleto::ordinario() const
{
    return data->m_ordinario;
}

QTime GiornoCartellinoCompleto::assenza() const
{
    return data->m_assenza;
}

QTime GiornoCartellinoCompleto::eccedenza() const
{
    return data->m_eccedenza;
}

QTime GiornoCartellinoCompleto::daRecuperare() const
{
    return data->m_daRecuperare;
}

QTime GiornoCartellinoCompleto::recuperate() const
{
    return data->m_recuperate;
}

QString GiornoCartellinoCompleto::causale1() const
{
    return data->m_causale1;
}

QTime GiornoCartellinoCompleto::ore1() const
{
    return data->m_ore1;
}

QString GiornoCartellinoCompleto::causale2() const
{
    return data->m_causale2;
}

QTime GiornoCartellinoCompleto::ore2() const
{
    return data->m_ore2;
}

QString GiornoCartellinoCompleto::causale3() const
{
    return data->m_causale3;
}

QTime GiornoCartellinoCompleto::ore3() const
{
    return data->m_ore3;
}

QTime GiornoCartellinoCompleto::repDiurna() const
{
    return data->m_repDiurna;
}

QTime GiornoCartellinoCompleto::repNotturna() const
{
    return data->m_repNotturna;
}

int GiornoCartellinoCompleto::minutiCausale(const QString &causale) const
{
    if(data->m_causale1 == causale)
        return Utilities::inMinuti(data->m_ore1);
    if(data->m_causale2 == causale)
        return Utilities::inMinuti(data->m_ore2);
    if(data->m_causale3 == causale)
        return Utilities::inMinuti(data->m_ore3);
    return 0;
}
