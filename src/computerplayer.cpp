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
#include "computerplayer.h"

#include "cardlist.h"
#include "game.h"
#include "gameinfo.h"

#include <kapplication.h>

ComputerPlayer::ComputerPlayer(CardList *cards,Game* game)
 : Player(cards,game)
{
}

void ComputerPlayer::klopfen()
{
    m_geklopft = false;
}

Card *ComputerPlayer::play()
{
	CardList* allowed=allowedCards();
	Card* ToPlay=allowed->at(KApplication::random()%allowed->count());

	delete allowed;
	return ToPlay;
}

GameInfo* ComputerPlayer::game()
{
    QValueList<game_data> lst;
        
    for(int i=GameInfo::STICHT;i<=GameInfo::RUFSPIEL;i++)
    {
        for( int z=Card::NOCOLOR;z<=Card::SCHELLEN;z++)
        {
            if( !GameInfo::isAllowed( m_cards, i, z ) )
                continue;
         
            game_data d;
            d.trumpf = d.fehlfarbe = d.weight = 0;
            d.info.setMode( i );
            d.info.setColor( z );        
            
            for( unsigned int c=0;c<m_cards->count();c++)
            {
                d.weight += d.info.weight( m_cards->at(c) );
                if( d.info.istTrumpf( m_cards->at(c) ) )
                    d.trumpf++;
                else if( !d.info.istTrumpf( m_cards->at(c) ) && m_cards->at(c)->card() != Card::SAU )
                {
                    CardList* list = m_cards->FindCards( m_cards->at(c)->color(), Card::SAU );
                    if( list->isEmpty() )
                        d.fehlfarbe++;
                    delete list;
                }
                    
            }

            if( i==GameInfo::RUFSPIEL && d.weight>=8 && ((d.fehlfarbe<=2 && d.trumpf>=5) || d.fehlfarbe<=1) )
                lst.append( d );
            else if( i!=GameInfo::RUFSPIEL && d.weight>=9 && d.fehlfarbe<=1 && d.trumpf>=5 )
                lst.append( d );
        }
    }
    
    // now we have all possible games in list,
    // let's find the best one;
    if( !lst.isEmpty() )
    {
        int best = 0;
        for( unsigned int i=0;i<lst.count();i++)
            if( lst[i].weight > lst[best].weight || (lst[i].trumpf > lst[best].trumpf && lst[i].fehlfarbe < lst[i].fehlfarbe) )
            {
                best = i;
                continue;
            }
        
        GameInfo* gi = new GameInfo;
        *gi = lst[best].info;
        return gi;
    }
    return 0;
}
