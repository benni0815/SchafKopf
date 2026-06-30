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
  int result{};
  emit questionYesNo(i18n("Do you want to double?"), &result);
  m_geklopft = ( result == YES );

  Player::klopfen();
}

Card *HumanPlayer::play()
{
    auto allowed = allowedCards();
    m_card = nullptr;

    int cpy{};
    emit getCard(&cpy);

    while( cpy != -1 && !allowed.contains( cpy ) )
    {
        emit forbiddenCard(cpy);
        emit getCard(&cpy);
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

std::optional<GameInfo> HumanPlayer::gameInfo(bool force)
{
    if( force )
    {
      GameInfo gameInfo;
      emit selectGame(&gameInfo, m_cards.toInts(), true);
      return gameInfo;
    }
    else
    {
      int result{};
      emit questionYesNo(i18n("Do you want to play?"), &result);
      if (result == YES)
      {
        GameInfo gameInfo;
        emit selectGame(&gameInfo, m_cards.toInts());
        return gameInfo;
      }
    }

    return {};
}
