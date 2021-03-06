/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017-2019 Giuseppe Calà <giuseppe.cala@mailbox.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef DPMCOMPUTE_H
#define DPMCOMPUTE_H

#include <QThread>

class DmpCompute;

namespace The {
    DmpCompute* dmpCompute();
}

class DmpCompute : public QThread
{
    Q_OBJECT
public:
    friend DmpCompute* The::dmpCompute();

    void setUnita(const int &id);
    void setTable(const QString &tableName);
    void setDirigente(const int &id);

protected:
    void run() override;

signals:
      void computeFinished();
      void totalItems(int);
      void currentItem(int);

private:
      DmpCompute();
      int m_idUnita;
      QString m_tableName;
      QString m_unitaName;
      QString m_mese;
      int m_idDirigente;
      int m_currItem{};

      void ricalcolaDmp(const QStringList &timecards, const int &idDirigente);
};

#endif // DPMCOMPUTE_H
