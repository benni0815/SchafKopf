// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "humanplayer.h"

#include "cardlist.h"
#include "game.h"
#include "gameinfo.h"

#include <KLocalizedString>


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
    int* ret = (int*)m_game->postEvent(QuestionYesNo, id(), 0, i18n("Do you want to double?"), true);
    m_geklopft = ( *ret == YES );
    delete ret;
    
    Player::klopfen();
}

Card *HumanPlayer::play()
{
    int* ret;
    int cpy;
    int* cpylist;
    int i;
    
    m_allowed = allowedCards();
    m_card=NULL;
    
    ret=(int*)m_game->postEvent( HumanPlayerGetCard, id(), NULL, QString(), true );
    cpy = *ret;
    delete ret;
    
    while( cpy != -1 && !m_allowed->contains( cpy ) )
    {
        cpylist = new int[2];
        cpylist[0] = cpy;
        cpylist[1] = 0;
        m_game->postEvent( ForbiddenCard, id(), cpylist, QString(), true );

        ret=(int*)m_game->postEvent( HumanPlayerGetCard, id(), NULL, QString(), true );
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
        return (GameInfo*)m_game->postEvent( ForcedSelectGame, id(), m_cards->toIntList(), QString(), true );
    }
    else
    {
        int* ret = (int*)m_game->postEvent(QuestionYesNo, id(), NULL, i18n("Do you want to play?"), true);
        if( *ret == YES )
        {
            delete ret;
            return (GameInfo*)m_game->postEvent( SelectGame, id(), m_cards->toIntList(), QString(), true );
        }
        else
            delete ret;
    }
    
    return NULL;
}


