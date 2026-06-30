// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "humanplayer.h"

#include "cardlist.h"
#include "game.h"
#include "gameinfo.h"

#include <KLocalizedString>


HumanPlayer::HumanPlayer(unsigned int id, Game* game)
 : Player(id,game)
{}

void HumanPlayer::klopfen()
{
    int* ret = (int*)m_game->postEvent(QuestionYesNo, id(), {}, i18n("Do you want to double?"), true);
    m_geklopft = ( *ret == YES );
    delete ret;
    
    Player::klopfen();
}

Card *HumanPlayer::play()
{
    int* ret;
    int cpy;
    
    auto allowed = allowedCards();
    m_card = nullptr;
    
    ret = (int*)m_game->postEvent(HumanPlayerGetCard, id(), {}, QString(), true);
    cpy = *ret;
    delete ret;
    
    while (cpy != -1 && !allowed.contains(cpy))
    {
        m_game->postEvent( ForbiddenCard, id(), {cpy}, QString(), true );

        ret = (int*)m_game->postEvent(HumanPlayerGetCard, id(), {}, QString(), true);
        cpy = *ret;
        delete ret;
    }
    
    for (int i = 0; i < allowed.count(); ++i)
    {
        if (allowed.at(i)->id() == cpy)
        {
            m_card = allowed.at(i);
            break;
        }
    }
    return m_card;
}

GameInfo* HumanPlayer::gameInfo( bool force )
{
    if( force )
    {
        return (GameInfo*)m_game->postEvent(ForcedSelectGame, id(), m_cards.toInts(), QString(), true);
    }
    else
    {
        int* ret = (int*)m_game->postEvent(QuestionYesNo, id(), {}, i18n("Do you want to play?"), true);
        if( *ret == YES )
        {
            delete ret;
            return (GameInfo*)m_game->postEvent(SelectGame, id(), m_cards.toInts(), QString(), true);
        }
        else
            delete ret;
    }
    
    return nullptr;
}
