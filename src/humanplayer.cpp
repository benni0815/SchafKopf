// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "humanplayer.h"

#include "cards.h"
#include "schafkopfdef.h"

#include <KLocalizedString>


HumanPlayer::HumanPlayer(unsigned int id, const Game& game)
 : Player(id,game)
{}

void HumanPlayer::klopfen()
{
  int result{};
  emit questionYesNo(i18n("Do you want to double?"), &result);
  m_geklopft = ( result == YES );

  emit doubled(id());
}

Card HumanPlayer::play()
{
    CardId cpy;
    emit getCard(&cpy);

    const auto allowed = allowedCards();
    while (cpy != CardId{} && !Cards::find(allowed, cpy))
    {
        emit forbiddenCard(cpy);
        emit getCard(&cpy);
    }

    const auto it = std::find_if(std::begin(allowed), std::end(allowed), [&] (const Card& c) { return c.id == cpy; });
    if (it == std::end(allowed))
    {
      return {};
    }
    return *it;
}

std::optional<GameVariant> HumanPlayer::gameInfo(bool force)
{
    if( force )
    {
      std::optional<GameVariant> game;
      emit selectGame(&game, Cards::toIds(m_cards), true);
      return game;
    }
    else
    {
      int result{};
      emit questionYesNo(i18n("Do you want to play?"), &result);
      if (result == YES)
      {
        std::optional<GameVariant> game;
        emit selectGame(&game, Cards::toIds(m_cards));
        return game;
      }
    }

    return {};
}
