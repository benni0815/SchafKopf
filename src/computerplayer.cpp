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


ComputerPlayer::~ComputerPlayer()
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
    int trumpf = 0;
    int fehlfarbe = 0;
    unsigned int i = 0, c = 0;
    int z = 0;

    // it's ugly to have three list, but smaller
    // than having one list containing a struct
    QValueList<GameInfo> lstInfo;
    QValueList<int> lstTrumpf;
    QValueList<int> lstFehlFarbe;
        
    for(i=GameInfo::STICHT;i<=GameInfo::RUFSPIEL;i++)
    {
        trumpf = fehlfarbe = 0;
        
        for( z=Card::NOCOLOR;z<=Card::SCHELLEN;z++)
        {
            if( i==GameInfo::RUFSPIEL ) 
            {
                CardList* sau = m_cards->FindCards(m_game->gameInfo()->color(), Card::SAU);
                if( z==Card::NOCOLOR || z==Card::HERZ || !sau->isEmpty() ) {
                    delete sau;
                    continue;
                } else
                    delete sau;
            }
            
            GameInfo info;
            info.setMode( i );
            info.setColor( z );        
            for( c=0;c<m_cards->count();c++)
                if( m_game->istTrumpf( m_cards->at(c), &info ) )
                    trumpf++;
                else if( !m_game->istTrumpf( m_cards->at(c), &info ) && m_cards->at(c)->card() != Card::SAU )
                    fehlfarbe++;
            
            if( trumpf >= 4 && fehlfarbe <= 2 )
            {
                lstInfo.append( info );
                lstTrumpf.append( trumpf );
                lstFehlFarbe.append( fehlfarbe );
            }
        }
    }
    
    // now we have all possible games in list,
    // let's find the best one;
    if( !lstInfo.isEmpty() )
    {
        int best = 0;
        for( i=0;i<lstInfo.count();i++)
            if( lstTrumpf[i] > lstTrumpf[best] && lstFehlFarbe[i] < lstFehlFarbe[i] )
            {
                best = i;
                continue;
            }
        
        GameInfo* gi = new GameInfo;
        *gi = lstInfo[best];
        return gi;
    }
    return 0;
}
