#include "competenzepagate.h"

#include <QDate>

class CompetenzePagateData : public QSharedData
{
public:
    int m_ci;
    QDateTime m_data;
    int m_deficit;
    int m_indNotturna;
    double m_indFestiva;
    int m_str_reparto_ord;
    int m_str_reparto_nof;
    int m_str_reparto_nef;
    int m_str_repe_ord;
    int m_str_repe_nof;
    int m_str_repe_nef;
    int m_str_guard_ord;
    int m_str_guard_nof;
    int m_str_guard_nef;
    int m_turni_repe;
    int m_ore_repe;
    double m_guard_diu;
    int m_guard_not;
    int m_grande_fes;
    double m_repOltre10;
    double m_teleconsulto;
    double m_oreLavorate;
};

CompetenzePagate::CompetenzePagate(QObject *parent)
    : QObject{parent}
    , data(new CompetenzePagateData)
{
    data->m_ci = 0;
    data->m_data = QDateTime(QDate(1900,1,1), QTime(0,0,0));
    data->m_deficit = 0;
    data->m_indNotturna = 0;
    data->m_indFestiva = 0.0;
    data->m_str_reparto_ord = 0;
    data->m_str_reparto_nof = 0;
    data->m_str_reparto_nef = 0;
    data->m_str_repe_ord = 0;
    data->m_str_repe_nof = 0;
    data->m_str_repe_nef = 0;
    data->m_str_guard_ord = 0;
    data->m_str_guard_nof = 0;
    data->m_str_guard_nef = 0;
    data->m_turni_repe = 0;
    data->m_ore_repe = 0;
    data->m_guard_diu = 0.0;
    data->m_guard_not = 0;
    data->m_grande_fes = 0;
    data->m_repOltre10 = 0.0;
    data->m_teleconsulto = 0.0;
    data->m_oreLavorate = 0.0;
}

CompetenzePagate::CompetenzePagate(const CompetenzePagate &rhs)
    : data{rhs.data}
{

}

CompetenzePagate &CompetenzePagate::operator=(const CompetenzePagate &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

bool CompetenzePagate::operator==(const CompetenzePagate &rhs) const
{
    if(this == &rhs)
        return true;

    return data->m_ci == rhs.ci() &&
            data->m_data == rhs.dataElaborazione() &&
            data->m_deficit == rhs.deficit() &&
            data->m_indNotturna == rhs.indNotturna() &&
            data->m_indFestiva == rhs.indFestiva() &&
            data->m_str_reparto_ord == rhs.str_reparto_ord() &&
            data->m_str_reparto_nof == rhs.str_reparto_nof() &&
            data->m_str_reparto_nef == rhs.str_reparto_nef() &&
            data->m_str_repe_ord == rhs.str_repe_ord() &&
            data->m_str_repe_nof == rhs.str_repe_nof() &&
            data->m_str_repe_nef == rhs.str_repe_nef() &&
            data->m_str_guard_ord == rhs.str_guard_ord() &&
            data->m_str_guard_nof == rhs.str_guard_nof() &&
            data->m_str_guard_nef == rhs.str_guard_nef() &&
            data->m_turni_repe == rhs.turni_repe() &&
            data->m_ore_repe == rhs.ore_repe() &&
            data->m_guard_diu == rhs.guard_diu() &&
            data->m_guard_not == rhs.guard_not() &&
            data->m_grande_fes == rhs.grande_fes() &&
            data->m_repOltre10 == rhs.repOltre10() &&
            data->m_teleconsulto == rhs.teleconsulto() &&
            data->m_oreLavorate == rhs.oreLavorate();
}

CompetenzePagate::~CompetenzePagate()
{

}

CompetenzePagate CompetenzePagate::diff(const CompetenzePagate &old)
{
    CompetenzePagate differenza;
    differenza.setCi(data->m_ci);
    differenza.setDateTime(data->m_data);
    differenza.setDeficit(data->m_deficit - old.deficit());
    differenza.setIndNotturna(data->m_indNotturna - old.indNotturna());
    differenza.setIndFestiva(data->m_indFestiva - old.indFestiva());
    differenza.setStr_reparto_ord(data->m_str_reparto_ord - old.str_reparto_ord());
    differenza.setStr_reparto_nof(data->m_str_reparto_nof - old.str_reparto_nof());
    differenza.setStr_reparto_nef(data->m_str_reparto_nef - old.str_reparto_nef());
    differenza.setStr_repe_ord(data->m_str_repe_ord - old.str_repe_ord());
    differenza.setStr_repe_nof(data->m_str_repe_nof - old.str_repe_nof());
    differenza.setStr_repe_nef(data->m_str_repe_nef - old.str_repe_nef());
    differenza.setStr_guard_ord(data->m_str_guard_ord - old.str_guard_ord());
    differenza.setStr_guard_nof(data->m_str_guard_nof - old.str_guard_nof());
    differenza.setStr_guard_nef(data->m_str_guard_nef - old.str_guard_nef());
    differenza.setTurni_repe(data->m_turni_repe - old.turni_repe());
    differenza.setOre_repe(data->m_ore_repe - old.ore_repe());
    differenza.setGuard_diu(data->m_guard_diu - old.guard_diu());
    differenza.setGuard_not(data->m_guard_not - old.guard_not());
    differenza.setGrande_fes(data->m_grande_fes - old.grande_fes());
    differenza.setOreLavorate(data->m_oreLavorate - old.oreLavorate());
    return differenza;
}

int CompetenzePagate::ci() const
{
    return data->m_ci;
}

QDateTime CompetenzePagate::dataElaborazione() const
{
    return data->m_data;
}

int CompetenzePagate::deficit() const
{
    return data->m_deficit;
}

int CompetenzePagate::indNotturna() const
{
    return data->m_indNotturna;
}

double CompetenzePagate::indFestiva() const
{
    return data->m_indFestiva;
}

int CompetenzePagate::str_reparto_ord() const
{
    return data->m_str_reparto_ord;
}

int CompetenzePagate::str_reparto_nof() const
{
    return data->m_str_reparto_nof;
}

int CompetenzePagate::str_reparto_nef() const
{
    return data->m_str_reparto_nef;
}

int CompetenzePagate::str_repe_ord() const
{
    return data->m_str_repe_ord;
}

int CompetenzePagate::str_repe_nof() const
{
    return data->m_str_repe_nof;
}

int CompetenzePagate::str_repe_nef() const
{
    return data->m_str_repe_nef;
}

int CompetenzePagate::str_guard_ord() const
{
    return data->m_str_guard_ord;
}

int CompetenzePagate::str_guard_nof() const
{
    return data->m_str_guard_nof;
}

int CompetenzePagate::str_guard_nef() const
{
    return data->m_str_guard_nef;
}

int CompetenzePagate::turni_repe() const
{
    return data->m_turni_repe;
}

int CompetenzePagate::ore_repe() const
{
    return data->m_ore_repe;
}

double CompetenzePagate::guard_diu() const
{
    return data->m_guard_diu;
}

int CompetenzePagate::guard_not() const
{
    return data->m_guard_not;
}

int CompetenzePagate::grande_fes() const
{
    return data->m_grande_fes;
}

double CompetenzePagate::repOltre10() const
{
    return data->m_repOltre10;
}

double CompetenzePagate::teleconsulto() const
{
    return data->m_teleconsulto;
}

double CompetenzePagate::oreLavorate() const
{
    return data->m_oreLavorate;
}

void CompetenzePagate::setCi(const int ci)
{
    data->m_ci = ci;
}

void CompetenzePagate::setDateTime(const QDateTime &dataElaborazione)
{
    data->m_data = dataElaborazione;
}

void CompetenzePagate::setDeficit(const int deficit)
{
    data->m_deficit = deficit;
}

void CompetenzePagate::setIndNotturna(const int indNotturna)
{
    data->m_indNotturna = indNotturna;
}

void CompetenzePagate::setIndFestiva(const double indFestiva)
{
    data->m_indFestiva = indFestiva;
}

void CompetenzePagate::setStr_reparto_ord(const int str_reparto_ord)
{
    data->m_str_reparto_ord = str_reparto_ord;
}

void CompetenzePagate::setStr_reparto_nof(const int str_reparto_nof)
{
    data->m_str_reparto_nof = str_reparto_nof;
}

void CompetenzePagate::setStr_reparto_nef(const int str_reparto_nef)
{
    data->m_str_reparto_nef = str_reparto_nef;
}

void CompetenzePagate::setStr_repe_ord(const int str_repe_ord)
{
    data->m_str_repe_ord = str_repe_ord;
}

void CompetenzePagate::setStr_repe_nof(const int str_repe_nof)
{
    data->m_str_repe_nof = str_repe_nof;
}

void CompetenzePagate::setStr_repe_nef(const int str_repe_nef)
{
    data->m_str_repe_nef = str_repe_nef;
}

void CompetenzePagate::setStr_guard_ord(const int str_guard_ord)
{
    data->m_str_guard_ord = str_guard_ord;
}

void CompetenzePagate::setStr_guard_nof(const int str_guard_nof)
{
    data->m_str_guard_nof = str_guard_nof;
}

void CompetenzePagate::setStr_guard_nef(const int str_guard_nef)
{
    data->m_str_guard_nef = str_guard_nef;
}

void CompetenzePagate::setTurni_repe(const int turni_repe)
{
    data->m_turni_repe = turni_repe;
}

void CompetenzePagate::setOre_repe(const int ore_repe)
{
    data->m_ore_repe = ore_repe;
}

void CompetenzePagate::setGuard_diu(const double guard_diu)
{
    data->m_guard_diu = guard_diu;
}

void CompetenzePagate::setGuard_not(const int guard_not)
{
    data->m_guard_not = guard_not;
}

void CompetenzePagate::setGrande_fes(const int grande_fes)
{
    data->m_grande_fes = grande_fes;
}

void CompetenzePagate::setRepOltre10(const double repOltre10)
{
    data->m_repOltre10 = repOltre10;
}

void CompetenzePagate::setTeleconsulto(const double teleconsulto)
{
    data->m_teleconsulto = teleconsulto;
}

void CompetenzePagate::setOreLavorate(const double oreLavorate)
{
    data->m_oreLavorate = oreLavorate;
}
