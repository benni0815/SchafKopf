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

#include "results.h"

#include "schafkopfdef.h"

#include "cardlist.h"
#include "gameinfo.h"
#include "player.h"

#include <klocale.h>

Results::Results()
{
    m_laufende = 0;
    m_gameinfo = NULL;
    m_parsed = false;
}

QString Results::result()
{
    bool positive = m_laufende > 0;
    parse();
    
    QString s;
    QString schneider = m_schneider ? i18n(" schneider" ) : QString::null;
    QString schwarz = m_schwarz ? i18n(" no-trick") : QString::null;
    
    if( m_points > 60 )
        s = m_gameinfo->spieler()->name() + QString(i18n(" wins with %1 Points%2.")).arg( m_points ).arg( schneider + schwarz );
    else
        s = m_gameinfo->spieler()->name() + QString(i18n(" looses with %1 Points%2.") ).arg( m_points ).arg( schneider + schwarz );
    
    if( m_laufende )
    {
        if( positive )
            s += m_gameinfo->spieler()->name() + QString( i18n(" got %1 cards in a row.").arg( m_laufende ) );
        else
            s += m_gameinfo->spieler()->name() + QString( i18n(" played without %1 cards in a row.").arg( m_laufende ) );
    }
    
    return s;
}

void Results::parse()
{
    if( m_parsed )
        return;
        
    CardList stiche( *(m_gameinfo->spieler()->stiche()) );
    if( m_gameinfo->mitspieler() )
        stiche.appendList( m_gameinfo->mitspieler()->stiche() );
    
    m_points = stiche.points();
    m_schwarz = ( stiche.isEmpty() || stiche.count() == NUMCARDS * PLAYERS );
    m_schneider = (m_points < 31 || m_points > 90 );

    m_parsed = true;
    
    // ab 2 laufende bei wenz und geier
    if( m_gameinfo->mode() == GameInfo::GEIER || m_gameinfo->mode() == GameInfo::WENZ )
        m_laufende = ( m_laufende >= 2 || m_laufende <= -2 ) ? m_laufende : 0;
    // sonst ab 3 laufenden
    else if( m_gameinfo->mode() == GameInfo::STICHT || m_gameinfo->mode() == GameInfo::RAMSCH 
          || m_gameinfo->mode() == GameInfo::DACHS || m_gameinfo->mode() == GameInfo::RUFSPIEL )
        m_laufende = ( m_laufende >= 3 || m_laufende <= -3 ) ? m_laufende : 0;
    
    m_laufende = m_laufende < 0 ? m_laufende * -1 : m_laufende;
}

double Results::klopfen(int anzahl, double betrag)
{
    if(anzahl==0) 
        return betrag; 
    else 
        return klopfen(anzahl-1, betrag)*2.0; 
}

