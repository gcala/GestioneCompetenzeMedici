#include "reperibilitasemplificata.h"


class ReperibilitaSemplificataData : public QSharedData
{
public:
    int m_idUnita;
    QDate m_decorrenza;
    double m_feriale;
    double m_sabato;
    double m_festivo;
    bool m_prefestivo;
};

ReperibilitaSemplificata::ReperibilitaSemplificata(int idUnita,
                                                   QDate decorrenza,
                                                   double feriale,
                                                   double sabato,
                                                   double festivo,
                                                   bool prefestivo)
    : data(new ReperibilitaSemplificataData)
{
    data->m_idUnita = idUnita;
    data->m_decorrenza = decorrenza;
    data->m_feriale = feriale;
    data->m_sabato = sabato;
    data->m_festivo = festivo;
    data->m_prefestivo = prefestivo;
}

ReperibilitaSemplificata::ReperibilitaSemplificata(const ReperibilitaSemplificata &rhs)
    : data{rhs.data}
{

}

ReperibilitaSemplificata &ReperibilitaSemplificata::operator=(const ReperibilitaSemplificata &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

bool ReperibilitaSemplificata::operator==(const ReperibilitaSemplificata &rhs) const
{
    if(this == &rhs)
        return true;

    return data->m_idUnita == rhs.idUnita() &&
            data->m_decorrenza == rhs.decorrenza() &&
            data->m_feriale == rhs.feriale() &&
            data->m_sabato == rhs.sabato() &&
            data->m_festivo == rhs.festivo() &&
            data->m_prefestivo == rhs.prefestivo();
}

ReperibilitaSemplificata::~ReperibilitaSemplificata()
{

}

int ReperibilitaSemplificata::idUnita() const
{
    return data->m_idUnita;
}

QDate ReperibilitaSemplificata::decorrenza() const
{
    return data->m_decorrenza;
}

double ReperibilitaSemplificata::feriale() const
{
    return data->m_feriale;
}

double ReperibilitaSemplificata::sabato() const
{
    return data->m_sabato;
}

double ReperibilitaSemplificata::festivo() const
{
    return data->m_festivo;
}

bool ReperibilitaSemplificata::prefestivo() const
{
    return data->m_prefestivo;
}
