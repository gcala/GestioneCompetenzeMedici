#ifndef DOCTORDATA_H
#define DOCTORDATA_H

#include <QString>

struct DoctorData {
    int id;
    int matricola;
    QString nome;

    // Costruttore per inizializzazione
    DoctorData(int id = 0, int matricola = 0, const QString& nome = QString())
        : id(id), matricola(matricola), nome(nome) {}
};

#endif // DOCTORDATA_H
