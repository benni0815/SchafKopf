// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "gameinfo.h"

#include "cards.h"
#include "player.h"
#include "schafkopfdef.h"
#include "settings.h"

#include <KLocalizedString>

#include <QString>


namespace
{

QList<CardId::Type> getTrumpfTypes(GameVariant::Mode gameMode)
{
  const auto map = QMap<GameVariant::Mode, QList<CardId::Type>>{
    {GameVariant::WENZ, {CardId::Unter}},
    {GameVariant::GEIER, {CardId::Ober}},
    {GameVariant::DACHS, {CardId::Sau, CardId::Zehn}},
  };
  const auto defaultTrumpf = QList<CardId::Type>{CardId::Ober, CardId::Unter};
  return map.value(gameMode, defaultTrumpf);
}

CardId::Color getTrumpfColor(GameVariant game)
{
  using Color = CardId::Color;
  const auto map = QMap<GameVariant::Mode, Color>{
    {GameVariant::RUFSPIEL, Color::Herz},
    {GameVariant::RAMSCH, Color::Herz},
    {GameVariant::DACHS, Color::NoColor},
  };
  return map.value(game.mode, game.color);
}

QString toString(GameVariant::Mode gameMode)
{
  const auto gameMap = QMap<GameVariant::Mode, QString>{
    {GameVariant::STICHT, i18n("Sticht")},
    {GameVariant::WENZ, i18n("Wenz")},
    {GameVariant::GEIER, i18n("Geier")},
    {GameVariant::DACHS, i18n("Badger")},
  };
  return gameMap.value(gameMode, "");
}

} // namespace

GameInfo::GameInfo(GameVariant game)
: m_game(game)
{}

const QString GameInfo::toString() const
{
  if (m_game.mode == GameVariant::RAMSCH)
  {
    return i18n("Ramsch is played.");
  }

  auto color = Cards::toString(m_game.color);
  if (color.isEmpty())
  {
    color = i18n("colorless");
  }

  if (m_game.mode == GameVariant::RUFSPIEL)
  {
    return i18n("%1 plays on the %2 Ace.", m_spieler ? m_spieler->name() : QString(), color);
  }

  return i18n("%1 plays %2 %3.", m_spieler ? m_spieler->name() : QString(), color, ::toString(m_game.mode));
}

int GameInfo::weight(CardId card, GameVariant game)
{
  if (istTrumpf(card, game))
  {
    if (card.type == CardId::Ober && game.mode != GameVariant::WENZ)
    {
      return 3;
    }
    else if (card.type == CardId::Unter && game.mode != GameVariant::WENZ)
    {
      return 2;
    }
    else if (card.type == CardId::Unter && game.mode == GameVariant::WENZ)
    {
      return 3;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    if (card.type == CardId::Sau)
    {
      return 1;
    }
    else
    {
      // TODO: ausser man hat auch die SAU
      return -1;
    }
  }
}

bool GameInfo::istTrumpf(CardId card, GameVariant game)
{
  const auto trumpfTypes = getTrumpfTypes(game.mode);
  return getTrumpfColor(game) == card.color || trumpfTypes.contains(card.type);
}

bool GameInfo::istTrumpf(CardId card) const
{
  return GameInfo::istTrumpf(card, {mode(), color()});
}

/** checks wether a game is allowed to play:
  * 1. is it allowed to play this game with this cards (needed for callgames)
  * 2. is it allowed to play this game according to the users preferences
  */
bool GameInfo::isAllowed(const std::vector<CardId>& cards, GameVariant game)
{
  // it is never allowed to decide to play Ramsch
  if (game.mode == GameVariant::RAMSCH)
  {
    return false;
  }

  if (game.mode == GameVariant::RUFSPIEL)
  {
    if (std::count(std::begin(cards), std::end(cards), CardId{CardId::Sau, game.color}) || game.color == CardId::Herz)
    {
      return false;
    }

    // ok now we have to test if we have a card in this color with is no trumpf_index
    for (const auto& c : cards)
    {
      if (c.color == game.color && c.type != CardId::Ober && c.type != CardId::Unter)
      {
        return true;
      }
    }
    return false;
  }
  else if (game.mode == GameVariant::STICHT && game.color == CardId::NoColor)
  {
    return false;
  }

  // check for the users preferences now
  const AllowedGames allowed = Settings::instance()->allowedGames();
  if (game.mode == GameVariant::DACHS && !allowed.dachs)
  {
    return false;
  }

  if ((game.mode == GameVariant::WENZ && !allowed.wenz)
      || (game.mode == GameVariant::WENZ && game.color != CardId::NoColor && !allowed.farb_wenz))
  {
    return false;
  }

  if ((game.mode == GameVariant::GEIER && !allowed.geier)
      || (game.mode == GameVariant::GEIER && game.color != CardId::NoColor && !allowed.farb_geier))
  {
    return false;
  }

  return true;
}

bool GameInfo::operator>(const GameInfo& info)
{
  /*
  2.2.3. Hat ein Teilnehmer einen Wenz oder ein Solo und vorher schon ein Spiel angesagt worden, so muss der Einzelspieler erklären
 "Ich spiele auch". Wenn der Erstmeldende "Gut" sagt, kann der Einzelspieler sein Spiel bekanntgeben. Will der Voransitzende aber selber
 alleine spielen, so nennt er sein Einzelspiel. Der nachfolgende Teilnehmer kann nun nur noch mit einem höherrangigen Einzelspiel ans Spiel
 kommen. Denn: Bei gleichrangigen Spielen entscheidet die Sitzreihenfolge, wer zum Zuge kommt. Farbsoli sind untereinander alle
 gleichberechtigt. Herzsoli geht nicht vor.
*/
  if (this->mode() == info.mode())
  {
    // according to the above paragraph,
    // we have only to care about NOCOLOR
    // e.g. Farblos Wenz > Herz Wenz
    // but Eichel Wenz == Gras Wenz
    return (this->color() == CardId::NoColor);
  }
  else
  {
    return this->mode() < info.mode();
  }
}

int GameInfo::evalCard(CardId card) const
{
  const auto map = QMap<GameVariant::Mode, QList<CardId::Type>>{
    {GameVariant::GEIER, {CardId::Sau, CardId::Zehn, CardId::Koenig, CardId::Unter, CardId::Neun, CardId::Acht, CardId::Sieben}},
    {GameVariant::WENZ,  {CardId::Sau, CardId::Zehn, CardId::Koenig, CardId::Ober, CardId::Neun, CardId::Acht, CardId::Sieben}},
    {GameVariant::DACHS, {CardId::Koenig, CardId::Ober, CardId::Unter, CardId::Neun, CardId::Acht, CardId::Sieben}},
  };

  const auto defaultCards = QList<CardId::Type>{CardId::Sau, CardId::Zehn, CardId::Koenig, CardId::Neun, CardId::Acht, CardId::Sieben};
  const auto cards = map.value(mode(), defaultCards);

  CardId::Color trumpfColor = getTrumpfColor(m_game);
  if (trumpfColor == CardId::NoColor)
  {
    trumpfColor = CardId::Eichel;
  }

  QList<CardId::Color> colors{trumpfColor};
  for (const auto color : {CardId::Eichel, CardId::Gras, CardId::Herz, CardId::Schellen})
  {
    if (trumpfColor != color)
    {
      colors.push_back(color);
    }
  }
  const auto trumpfTypes = getTrumpfTypes(mode());
  const int trumpfIndex = trumpfTypes.indexOf(card.type);
  const int cards_index = cards.indexOf(card.type);
  const int colors_index = colors.indexOf(card.color);

  if (trumpfIndex != -1)
  {
    return CARD_CNT - (trumpfIndex * NUMTRUMPF + card.color);
  }
  const auto trumpf_cnt = trumpfTypes.size();
  return CARD_CNT - (trumpf_cnt * NUMTRUMPF + colors_index * (NUMCARDS - trumpf_cnt) + cards_index);
}

bool GameInfo::isHigher(CardId card, CardId high) const
{
  int colcnt{0};

  const auto cardVal = evalCard(card);
  const auto highVal = evalCard(high);
  if (istTrumpf(card) || istTrumpf(high))
  {
    return highVal < cardVal;
  }
  else
  {
    switch (mode())
    {
    case GameVariant::GEIER:
    case GameVariant::WENZ:
      colcnt = NUMCARDS - 1;
      break;
    case GameVariant::RAMSCH:
    case GameVariant::RUFSPIEL:
    case GameVariant::STICHT:
    case GameVariant::DACHS:
    default:
      colcnt = NUMCARDS - 2;
      break;
    }
    if ((highVal - 1) / colcnt == (cardVal - 1) / colcnt)
    {
      return highVal < cardVal;
    }
  }
  return false;
}

Card GameInfo::highestCard(const CardList& cards) const
{
  auto less = [this](const Card& lhs, const Card& rhs) { return isHigher(rhs.id, lhs.id); };
  return Cards::maxElement(cards, less);
}

int GameInfo::laufende(const CardList& cards) const
{
  if (mode() == GameVariant::RAMSCH)
  {
    return 0;
  }

  // TODO: jeder spieler sollte eine laufende property haben und selbst zählen wie viele laufende er hat.
  // Danach können diese in results einfach addiert werden

  auto all = Cards::initIds();
  std::sort(std::begin(all), std::end(all), [this](CardId lhs, CardId rhs) { return evalCard(lhs) < evalCard(rhs); });

  int l{0};
  for (auto it = std::rbegin(all); it != std::rend(all); ++it)
  {
    if (!istTrumpf(*it))
    {
      break;
    }

    const auto card = Cards::find(cards, *it);
    if (card && (l > 0 || it == std::rbegin(all)))
    {
      l++;
    }
    else if ((!card && l < 0) || it == std::rbegin(all))
    {
      l--;
    }
    else
    {
      break;
    }
  }

  // Laufende are only counted if >= 3. Except for colorless Geier and Wenz (>= 2).
  int laufendThreshold{3};
  if (color() == CardId::NoColor
      && (mode() == GameVariant::GEIER || mode() == GameVariant::WENZ))
  {
    laufendThreshold = 2;
  }

  return std::abs(l) >= laufendThreshold ? l : 0;
}
