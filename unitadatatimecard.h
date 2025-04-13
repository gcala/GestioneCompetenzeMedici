#ifndef UNITADATATIMECARD_H
#define UNITADATATIMECARD_H

#include <QString>

struct UnitaDataTimecard {
    int idUnita;
    QString nome;
    int id;

    // Costruttore per inizializzazione id,raggruppamento,nome,breve,pseudo
    UnitaDataTimecard(int idUnita = 0, const QString& nome = QString(), int id = 0)
        : idUnita(idUnita), nome(nome), id(id) {}
};

#endif // UNITADATATIMECARD_H
