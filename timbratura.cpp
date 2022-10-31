#include "timbratura.h"
#include "utilities.h"

#include <QTime>
#include <QRegularExpression>

class TimbraturaData : public QSharedData
{
public:
    QTime m_ora;
    QString m_text;
    bool m_vuota;
    bool m_corretta;
    bool m_annullata;
    bool m_ingressoRep;
    bool m_uscitaRep;
    bool m_valida;
};

Timbratura::Timbratura()
    : data(new TimbraturaData)
{
    data->m_ora.setHMS(0,0,0);
    data->m_vuota = false;
    data->m_corretta = false;
    data->m_annullata = false;
    data->m_ingressoRep = false;
    data->m_uscitaRep = false;
    data->m_valida = false;
}

Timbratura::Timbratura(QString text)
    : data(new TimbraturaData)
{
    data->m_ora.setHMS(0,0,0);
    data->m_vuota = false;
    data->m_corretta = false;
    data->m_annullata = false;
    data->m_ingressoRep = false;
    data->m_uscitaRep = false;

    data->m_text = text;

    if(Utilities::timbraturaValida(text)) {
        data->m_valida = true;
        if(text.trimmed().isEmpty()) {
            data->m_vuota = true;
        } else {
            QRegularExpression rx("(\\d\\d:\\d\\d)");
            QRegularExpressionMatch match = rx.match(text);
            if(match.hasMatch()) {
                data->m_ora.fromString(match.captured(1), "hh:mm");
                text.remove(rx);
                if(text.contains("R")) {
                    data->m_uscitaRep = true;
                    text.remove("R");
                }
                if(text.contains("r")) {
                    data->m_ingressoRep = true;
                    text.remove("r");
                }
                if(text.contains("^") || text.contains("*")) {
                    data->m_corretta = true;
                    text.remove("^").remove("*");
                }
                if(text.contains("ø")) {
                    data->m_annullata = true;
                    text.remove("ø");
                }
            } else {
                data->m_valida = false;
            }
        }
    } else {
        data->m_valida = false;
    }
}

Timbratura::Timbratura(const Timbratura &rhs)
    : data(rhs.data)
{

}

Timbratura &Timbratura::operator=(const Timbratura &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

bool Timbratura::operator==(const Timbratura &rhs) const
{
    if(this == &rhs)
        return true;

    return data->m_ora == rhs.ora() &&
           data->m_corretta == rhs.isCorrect() &&
           data->m_annullata == rhs.isCanceled() &&
           data->m_vuota == rhs.isEmpty() &&
           data->m_ingressoRep == rhs.isIngressoRep() &&
           data->m_uscitaRep == rhs.isUscitaRep() &&
           data->m_valida == rhs.isValid();
}

Timbratura::~Timbratura()
{

}

QString Timbratura::text() const
{
    return data->m_text;
}

QTime Timbratura::ora() const
{
    return data->m_ora;
}

bool Timbratura::isCanceled() const
{
    return data->m_annullata;
}

bool Timbratura::isCorrect() const
{
    return data->m_corretta;
}

bool Timbratura::isIngressoRep() const
{
    return data->m_ingressoRep;
}

bool Timbratura::isUscitaRep() const
{
    return data->m_uscitaRep;
}

bool Timbratura::isEmpty() const
{
    return data->m_vuota;
}

bool Timbratura::isValid() const
{
    return data->m_valida;
}
