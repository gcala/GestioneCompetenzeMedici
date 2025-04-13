#ifndef COMPETENZEPAGATE_H
#define COMPETENZEPAGATE_H

#include <QObject>
#include <QSharedDataPointer>

class CompetenzePagateData;

class CompetenzePagate : public QObject
{
    Q_OBJECT
public:
    explicit CompetenzePagate(QObject *parent = nullptr);
    CompetenzePagate(const CompetenzePagate &);
    CompetenzePagate &operator=(const CompetenzePagate &);
    bool operator==(const CompetenzePagate &rhs) const;
    ~CompetenzePagate();

    CompetenzePagate diff(const CompetenzePagate &old);

    int ci() const;
    QDateTime dataElaborazione() const;
    int deficit() const;
    int indNotturna() const;
    double indFestiva() const;
    int str_reparto_ord() const;
    int str_reparto_nof() const;
    int str_reparto_nef() const;
    int str_repe_ord() const;
    int str_repe_nof() const;
    int str_repe_nef() const;
    int str_guard_ord() const;
    int str_guard_nof() const;
    int str_guard_nef() const;
    int turni_repe() const;
    int ore_repe() const;
    double guard_diu() const;
    int guard_not() const;
    int grande_fes() const;
    double repOltre10() const;
    double teleconsulto() const;
    double oreLavorate() const;

    void setCi(const int ci);
    void setDateTime(const QDateTime &dataElaborazione);
    void setDeficit(const int deficit);
    void setIndNotturna(const int indNotturna);
    void setIndFestiva(const double indFestiva);
    void setStr_reparto_ord(const int str_reparto_ord);
    void setStr_reparto_nof(const int str_reparto_nof);
    void setStr_reparto_nef(const int str_reparto_nef);
    void setStr_repe_ord(const int str_repe_ord);
    void setStr_repe_nof(const int str_repe_nof);
    void setStr_repe_nef(const int str_repe_nef);
    void setStr_guard_ord(const int str_guard_ord);
    void setStr_guard_nof(const int str_guard_nof);
    void setStr_guard_nef(const int str_guard_nef);
    void setTurni_repe(const int turni_repe);
    void setOre_repe(const int ore_repe);
    void setGuard_diu(const double guard_diu);
    void setGuard_not(const int guard_not);
    void setGrande_fes(const int grande_fes);
    void setRepOltre10(const double repOltre10);
    void setTeleconsulto(const double teleconsulto);
    void setOreLavorate(const double oreLavorate);

signals:


private:
    QSharedDataPointer<CompetenzePagateData> data;
};

#endif // COMPETENZEPAGATE_H
