#ifndef UNITADATA_H
#define UNITADATA_H

#include <QString>

struct UnitaData {
    int id;
    QString nome;
    QString raggruppamento;
    QString breve;
    QString pseudo;

    // Costruttore per inizializzazione id,raggruppamento,nome,breve,pseudo
    UnitaData(const QString& nome = QString(), const QString& raggruppamento = QString(), const QString& breve = QString(), const QString& pseudo = QString(), int id = 0)
        : raggruppamento(raggruppamento), nome(nome), breve(breve),  pseudo(pseudo), id(id) {}
};

#endif // UNITADATA_H
