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
#include "openbook.h"

#include "card.h"
#include "cardlist.h"
#include "game.h"
#include "gameinfo.h"
#include "player.h"

OpenBook::OpenBook( Player* player, Game* game )
{
    m_self = player;
    m_game = game;
    
    connect( m_game, SIGNAL( playerPlayedCard( unsigned int, Card* ) ), this, SLOT( cardPlayed(unsigned int,Card*) ) );
    m_player = (m_self == m_game->gameInfo()->spieler() || m_self == m_game->gameInfo()->mitspieler() );
}


OpenBook::~OpenBook()
{
}

void OpenBook::spieler( CardList* list )
{
    /* Spieler spielt trumpf */

}

void OpenBook::nichtSpieler( CardList* list )
{
    CardList* tmp = 0;
    
    /* Nicht spieler spielt farbe */
    switch( m_game->gameInfo()->mode() )
    {
        case GameInfo::RUFSPIEL:
            /* such die sau */
            tmp = list->FindCards( m_game->gameInfo()->color(), Card::NOSTICH );
            if( !tmp->isEmpty() )
                *list = *tmp;
            delete tmp;
            break;
        
        case GameInfo::STICHT:
        case GameInfo::GEIER:
        case GameInfo::WENZ:
            /* auf solo gehöhrt sau */
            tmp = list->FindCards( Card::NOCOLOR, Card::SAU );
            if( !tmp->isEmpty() )
            {
                /* entferne die trumpfsau */
                CardList* t = tmp->FindCards(  m_game->gameInfo()->color(), Card::SAU );
                tmp->RemoveCards( t );
                delete t;

                *list = *tmp;               
            }
            delete tmp;
        break;
    }

}

CardList* OpenBook::possibleCards()
{
    CardList* list = m_self->allowedCards();
    
    if( m_player )
        spieler( list );
    else
        nichtSpieler( list );
        
    return list;
}

void OpenBook::cardPlayed( unsigned int player, Card* card )
{
    // todo: move this in to an own class later
    Player* p = m_game->findId( player );
}
