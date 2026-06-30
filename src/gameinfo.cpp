// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "gameinfo.h"

#include "cards.h"
#include "player.h"
#include "settings.h"
#include "schafkopfdef.h"

#include <KLocalizedString>

#include <QString>

GameInfo::GameInfo(GameVariant game)
: m_game(game)
{}

const QString GameInfo::toString() const
{
    QString s, c, g;
    switch (m_game.color)
    {
        case CardId::NoColor:
            c = i18n("colorless"); break;
        case CardId::Eichel:
            c = i18n("Eichel"); break;
        case CardId::Gras:
            c = i18n("Gras"); break;
        case CardId::Herz:
            c = i18n("Heart"); break;
        case CardId::Schellen:
            c = i18n("Schellen"); break;
        default:
            c = QString();
            break;
    }

    switch (m_game.mode)
    {
        case GameVariant::STICHT:
            g = i18n("Sticht"); break;
        case GameVariant::GEIER:
            g = i18n("Geier"); break;
        case GameVariant::WENZ:
            g = i18n("Wenz"); break;
        case GameVariant::RAMSCH:
            g = i18n("Ramsch"); break;
        case GameVariant::DACHS:
            g = i18n("Badger"); break;
        case GameVariant::RUFSPIEL:
        default:            
            g = QString();
    }

    if (m_game.mode == GameVariant::RUFSPIEL)
        s = i18n("%1 plays on the %2 Ace.", m_spieler ? m_spieler->name() : QString(), c);
    else if (m_game.mode == GameVariant::RAMSCH)
        s = i18n("Ramsch is played.");
    else
        s = i18n("%1 plays %2 %3.", m_spieler ? m_spieler->name() : QString(), c, g);

    return s; 
}

int GameInfo::weight(CardId card, GameVariant game)
{
    if (istTrumpf(card, game))
    {
        if (card.type == CardId::Ober && game.mode != GameVariant::WENZ)
            return 3;
        else if (card.type == CardId::Unter && game.mode != GameVariant::WENZ)
            return 2;
        else if (card.type == CardId::Unter && game.mode == GameVariant::WENZ)
            return 3;
        else
            return 1;
    }
    else 
    {
        if (card.type == CardId::Sau)
            return 1;
        else
            // TODO: ausser man hat auch die SAU
            return -1;
    }
}

bool GameInfo::istTrumpf(CardId card, GameVariant game)
{
  switch (game.mode)
  {
  case GameVariant::RUFSPIEL:
  case GameVariant::RAMSCH:
    if (card.type == CardId::Ober || card.type == CardId::Unter || card.color == CardId::Herz)
      return true;
    break;

  case GameVariant::STICHT:
    if (card.type == CardId::Ober || card.type == CardId::Unter || card.color == game.color)
      return true;
    break;

  case GameVariant::GEIER:
    if (card.type == CardId::Ober || card.color == game.color)
      return true;
    break;

  case GameVariant::WENZ:
    if (card.type == CardId::Unter || card.color == game.color)
      return true;
    break;

  case GameVariant::DACHS:
    if (card.type == CardId::Sau || card.type == CardId::Zehn)
      return true;
    break;

  default:
    break;
  };

  return false;
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
    return false;

  if (game.mode == GameVariant::RUFSPIEL)
  {
    if (std::count(std::begin(cards), std::end(cards), CardId{CardId::Sau, game.color}) || game.color == CardId::Herz)
      return false;

    // ok now we have to test if we have a card in this color with is no trumpf_index
    for (const auto& c : cards)
    {
      if (c.color == game.color && c.type != CardId::Ober && c.type != CardId::Unter)
      {
        return true;
      }
    }
    return false;
  } else if (game.mode == GameVariant::STICHT && game.color == CardId::NoColor)
    return false;

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
    if( this->mode() == info.mode() )
    {
        // according to the above paragraph,
        // we have only to care about NOCOLOR
        // e.g. Farblos Wenz > Herz Wenz 
        // but Eichel Wenz == Gras Wenz
        return (this->color() == CardId::NoColor);
    } else
        return this->mode() < info.mode();
}

int GameInfo::evalCard(CardId card) const
{
    /* Not very nice. Try to find a better way */

    int *l_trumpf;
    int *l_cards;
    int l_trumpf_std[]={ CardId::Ober, CardId::Unter };
    int l_trumpf_geier=CardId::Ober;
    int l_trumpf_wenz=CardId::Unter;
    int l_trumpf_dachs[]={ CardId::Sau, CardId::Zehn };
    int l_cards_std[]={ CardId::Sau, CardId::Zehn, CardId::Koenig, CardId::Neun, CardId::Acht, CardId::Sieben, CardId::NoStich };
    int l_cards_geier[]={ CardId::Sau, CardId::Zehn, CardId::Koenig, CardId::Unter, CardId::Neun, CardId::Acht, CardId::Sieben };
    int l_cards_wenz[]={ CardId::Sau, CardId::Zehn, CardId::Koenig, CardId::Ober, CardId::Neun, CardId::Acht, CardId::Sieben };
    int l_cards_dachs[]={ CardId::Koenig, CardId::Ober, CardId::Unter, CardId::Neun, CardId::Acht, CardId::Sieben, CardId::NoStich };
    int l_colors[4];
    int trumpf_index=-1;
    int cards_index=-1;
    int colors_index=-1;
    int trumpf_cnt;
    int col = CardId::NoColor;
    int i, a;

    switch (mode())
    {
        default:
        case GameVariant::RAMSCH:
        case GameVariant::RUFSPIEL:
            trumpf_cnt=2;
            l_trumpf=l_trumpf_std;
            l_cards=l_cards_std;
            col=CardId::Herz;
            break;
        case GameVariant::STICHT:
            trumpf_cnt=2;
            l_trumpf=l_trumpf_std;
            l_cards=l_cards_std;
            col = color();
            break;
        case GameVariant::GEIER:
            trumpf_cnt=1;
            l_trumpf=&l_trumpf_geier;
            l_cards=l_cards_geier;
            col = color();
            break;
        case GameVariant::WENZ:
            //qDebug("Wenz, color: "+ QString::number(gameinfo->color()));
            trumpf_cnt=1;
            l_trumpf=&l_trumpf_wenz;
            l_cards=l_cards_wenz;
            col = color();
            break;
        case GameVariant::DACHS:
            trumpf_cnt=2;
            l_trumpf=l_trumpf_dachs;
            l_cards=l_cards_dachs;
            col=CardId::NoColor;
            break;
    }
    if (col == CardId::NoColor)
        col = CardId::Eichel;
    l_colors[0]=col;
    for(i=0, a=1;i<4;i++)
    {
        if(col==i)
            continue;
        else
            l_colors[a++]=i;
    }
    for(i=0;i<trumpf_cnt;i++)
    {
        if (card.type == l_trumpf[i])
        {
            trumpf_index=i;
            break;
        }
    }
    for(i=0;i<7;i++)
    {
        if (card.type == l_cards[i])
        {
            cards_index=i;
            break;
        }
    }
    for(i=0;i<4;i++)
    {
        if (card.color == l_colors[i])
        {
            colors_index=i;
            break;
        }
    }
    if(trumpf_index!=-1)
        return CARD_CNT - (trumpf_index * NUMTRUMPF + card.color);
    return CARD_CNT-(trumpf_cnt*NUMTRUMPF+colors_index*(NUMCARDS-trumpf_cnt)+cards_index);
}

int GameInfo::laufende()
{
  if (m_game.mode == GameVariant::RAMSCH)
    return 0;

  /** TODO: jeder spieler sollte eine laufende property haben und
    * selst zählen wie viele laufende er hat.
    * danach können diese in results einfach addiert werden
    */

  auto laufend = spieler()->cards();
  if (mitspieler())
  {
    laufend.append(mitspieler()->cards());
  }

  auto all = Cards::initIds();
  std::sort(std::begin(all), std::end(all), [this] (CardId lhs, CardId rhs) { return evalCard(lhs) < evalCard(rhs); });

  int l{0};
  for (auto it = std::rbegin(all); it != std::rend(all); ++it)
  {
    const auto card = Cards::find(laufend, *it);
    if (card && (l > 0 || it == std::rbegin(all)))
      l++;
    else if ((!card && l < 0) || it == std::rbegin(all))
      l--;
    else
    {
      break;
    }
  }

  return l;
}
