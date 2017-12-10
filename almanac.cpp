/*
 * Gestione Competenze Medici
 *
 * Copyright (C) 2017 Giuseppe Calà <giuseppe.cala@mailbox.org>
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

#include "almanac.h"

namespace The {
    static Almanac* s_Almanac_instance = 0;

    Almanac* almanac()
    {
        if( !s_Almanac_instance )
            s_Almanac_instance = new Almanac();

        return s_Almanac_instance;
    }
}

bool Almanac::isGrandeFestivita(const QDate &date)
{
    if(!m_years.contains(date.year())) {
        caricaGrandiFestivita(date.year());
        caricaPasqua(date.year());
        m_years << date.year();
    }

    return m_grandiFestivita.contains(date);
}

Almanac::Almanac()
{
    if(m_grandiFestivita.isEmpty()) {
        const int anno = QDate::currentDate().year();
        m_years << anno;
        caricaGrandiFestivita(anno);
        caricaPasqua(anno);
    }
}

void Almanac::caricaGrandiFestivita(const int &anno)
{
    m_grandiFestivita.append(QDate(anno,1,1));
    m_grandiFestivita.append(QDate(anno,1,6));
    m_grandiFestivita.append(QDate(anno,4,25));
    m_grandiFestivita.append(QDate(anno,5,1));
    m_grandiFestivita.append(QDate(anno,5,30)); // festa patronale potenza
    m_grandiFestivita.append(QDate(anno,6,2));
    m_grandiFestivita.append(QDate(anno,8,15));
    m_grandiFestivita.append(QDate(anno,11,1));
    m_grandiFestivita.append(QDate(anno,12,8));
    m_grandiFestivita.append(QDate(anno,12,25));
    m_grandiFestivita.append(QDate(anno,12,26));
}

void Almanac::caricaPasqua(const int &anno)
{

    int giorno, mese;
    int a, b, c, d, e, m, n;

    switch(anno/100)
    {
        case 15:	// 1583 - 1599 (FALL THROUGH)
        case 16:	// 1600 - 1699
            m=22; n=2; 	break;

        case 17:	// 1700 - 1799
            m=23; n=3; break;

        case 18:	// 1800 - 1899
            m=23; n=4; break;

        case 19:	// 1900 - 1999 (FALL THROUGH)
        case 20:	// 2000 - 2099
            m=24; n=5;break;

        case 21:	// 2100 - 2199
            m=24; n=6; break;

        case 22:	// 2200 - 2299
            m=25; n=0; break;

        case 23:	// 2300 - 2399
            m=26; n=1; break;

        case 24:	// 2400 - 2499
            m=25; n=1; break;
    }

    a=anno%19;
    b=anno%4;
    c=anno%7;
    d=(19*a+m)%30;
    e=(2*b+4*c+6*d+n)%7;
    giorno=d+e;

    if (d+e<10)
    {
        giorno+=22;
        mese=3;
    }

    else
    {
        giorno-=9;
        mese=4;

        if ((giorno==26)||((giorno==25)&&(d==28)&&(e==6)&&(a>10)))
        {
            giorno-=7;
        }
    }

    m_grandiFestivita.append(QDate(anno,mese,giorno));
    m_grandiFestivita.append(QDate(anno,mese,giorno).addDays(1));
}
