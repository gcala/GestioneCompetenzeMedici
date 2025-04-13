#ifndef OREPAGATE_H
#define OREPAGATE_H

#include <QDate>

struct OrePagate {
    int oreTotali = 0;          // ore_tot
    int orePagate = 0;          // ore_pagate
    int pagaDiurno = 0;         // paga_diurno
    int pagaOreLavorate = 0;    // paga_ore_lavorate

    // Costruttore vuoto
    OrePagate() = default;

    // Costruttore con parametri
    OrePagate(int _oreTotali, int _orePagate, int _pagaDiurno, int _pagaOreLavorate)
        : oreTotali(_oreTotali)
        , orePagate(_orePagate)
        , pagaDiurno(_pagaDiurno)
        , pagaOreLavorate(_pagaOreLavorate)
    {}

    // Costruttore da QVector (per compatibilità retroattiva)
    explicit OrePagate(const QVector<int>& values) {
        if (values.size() >= 1) oreTotali = values[0];
        if (values.size() >= 2) orePagate = values[1];
        if (values.size() >= 3) pagaDiurno = values[2];
        if (values.size() >= 4) pagaOreLavorate = values[3];
    }

    // Conversione a QVector (per compatibilità retroattiva)
    QVector<int> toVector() const {
        QVector<int> result;
        result << oreTotali << orePagate << pagaDiurno << pagaOreLavorate;
        return result;
    }
};

// Definiamo un alias per il tipo di mappa che sarà usato dalla funzione
using OrePagateMap = QMap<QDate, OrePagate>;

#endif // OREPAGATE_H
