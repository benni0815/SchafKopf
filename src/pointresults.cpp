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
#include "pointresults.h"
#include "player.h"
#include "game.h"
#include "gameinfo.h"
#include "settings.h"

#include <kglobal.h>
#include <klocale.h>

PointResults::PointResults()
 : Results()
{
}

double PointResults::points( Player* player )
{
    int m = 0;
    t_ResultValues* r = Settings::instance()->pointResults();
    
    parse();

    if( m_gameinfo->mode() != GameInfo::RUFSPIEL && m_gameinfo->mode() != GameInfo::RAMSCH )
        m = (int)r->solo; // SOLO = 2
    else if( m_gameinfo->mode() == GameInfo::RUFSPIEL )
        m = (int)r->rufspiel;
        
    m += m_schneider ? (int)r->schneider : 0;
    m += m_schwarz ? (int)r->schwarz : 0;
    m += m_laufende * (int)r->laufende;
    m = klopfen( player->game()->timesDoubled(), m );
    
    if( player == m_gameinfo->spieler() || player == m_gameinfo->mitspieler() )
        m = (m_points > 60 ? m : m * -1);
    else
        m = (m_points >= 60 ? m * -1 : m);
        
    if( player == m_gameinfo->spieler() && !m_gameinfo->mitspieler() )
        m *= 3;
        
    return (double)m;
}

QString PointResults::formatedPoints( Player* player )
{
    player->setPoints( points( player ) + player->points() );
    return QString::number( (int)player->points() );
}

