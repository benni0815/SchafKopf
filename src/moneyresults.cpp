/***************************************************************************
 *   Copyright (C) 2004 by Dominik Seichter                                *
 *   domseichter@web.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "moneyresults.h"

#include "gameinfo.h"

#include <kglobal.h>
#include <klocale.h>

MoneyResults::MoneyResults()
 : Results()
{
}

double MoneyResults::points( Player* player )
{
    parse();
    
    double m = 0.0;
    if( m_gameinfo->mode() != GameInfo::RUFSPIEL && m_gameinfo->mode() != GameInfo::RAMSCH )
        m = 0.20; // SOLO = 20cent
    else if( m_gameinfo->mode() == GameInfo::RUFSPIEL )
        m = 0.10;
        
    m += m_schneider ? 0.10 : 0.0;
    m += m_schwarz ? 0.10 : 0.0;
    m += m_laufende * 0.10;
    
    if( player == m_gameinfo->spieler() || player == m_gameinfo->mitspieler() )
        return (m_points > 60 ? m : m * -1.0);
    else
        return (m_points >= 60 ? m * -1.0 : m);
}

QString MoneyResults::formatedPoints( Player* player )
{
    double p = points( player );
    KLocale* locale = KGlobal::locale();
    return locale->formatMoney( p );
}
