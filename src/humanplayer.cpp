// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "humanplayer.h"

#include "card.h"
#include "schafkopfdef.h"

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

    CardId cpy;
    emit getCard(&cpy);

    while( cpy != CardId{CardId::Type::NoStich} && !allowed.contains( cpy ) )
    {
        emit forbiddenCard(cpy);
        emit getCard(&cpy);
    }

    for (int i = 0; i < allowed.size(); ++i)
    {
        if (allowed.at(i)->id() == cpy)
        {
            m_card = allowed.at(i);
            break;
        }
    }
    return m_card;
}

std::optional<GameVariant> HumanPlayer::gameInfo(bool force)
{
    if( force )
    {
      GameVariant game;
      emit selectGame(&game, m_cards.toIds(), true);
      return game;
    }
    else
    {
      int result{};
      emit questionYesNo(i18n("Do you want to play?"), &result);
      if (result == YES)
      {
        GameVariant game;
        emit selectGame(&game, m_cards.toIds());
        return game;
      }
    }

    return {};
}
