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

#include "humanplayer.h"

#include "cardlist.h"
#include "game.h"
#include "gamecanvas.h"
#include "gameinfo.h"

#include <klocale.h>

HumanPlayer::HumanPlayer(unsigned int id, Game* game)
 : Player(id,game)
{
    m_allowed = NULL;
    m_card = NULL;
}

HumanPlayer::~HumanPlayer()
{
	delete m_allowed;
}

void HumanPlayer::klopfen()
{
    int* ret = (int*)m_game->postEvent( QuestionYesNo, id(), 0, i18n("Do you want to double?"), true );
    m_geklopft = ( *ret == YES );
    delete ret;
    
    Player::klopfen();
}

Card *HumanPlayer::play()
{
    int* ret;
    int cpy;
    int* cpylist;
    unsigned int i;
    
    m_allowed = allowedCards();
    m_card=NULL;
    
    ret=(int*)m_game->postEvent( HumanPlayerGetCard, id(), NULL, QString::null, true );
    cpy = *ret;
    delete ret;
    
    while( cpy != -1 && !m_allowed->contains( cpy ) )
    {
        cpylist = new int[2];
        cpylist[0] = cpy;
        cpylist[1] = 0;
        m_game->postEvent( ForbiddenCard, id(), cpylist, QString::null, true );

        ret=(int*)m_game->postEvent( HumanPlayerGetCard, id(), NULL, QString::null, true );
        cpy = *ret;
        delete ret;
    }
    
    for( i=0;i<m_allowed->count();i++ )
        if( m_allowed->at(i)->id() == cpy )
        {
            m_card = m_allowed->at(i);
            break;
        }
           
    delete m_allowed;
    m_allowed = NULL;
    return m_card;
}

GameInfo* HumanPlayer::gameInfo( bool force )
{
    if( force )
    {
        return (GameInfo*)m_game->postEvent( ForcedSelectGame, id(), m_cards->toIntList(), QString::null, true );
    }
    else
    {
        int* ret = (int*)m_game->postEvent( QuestionYesNo, id(), NULL, i18n("Do you want to play?"), true );
        if( *ret == YES )
        {
            delete ret;
            return (GameInfo*)m_game->postEvent( SelectGame, id(), m_cards->toIntList(), QString::null, true );
        }
        else
            delete ret;
    }
    
    return NULL;
}


