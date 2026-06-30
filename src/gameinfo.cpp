// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "gameinfo.h"
#include "cardlist.h"
#include "player.h"
#include "settings.h"
#include "schafkopfdef.h"

#include <KLocalizedString>

#include <QString>


const QString GameInfo::toString() const
{
    QString s, c, g;
    switch( m_color )
    {
        case Card::NoColor:
            c = i18n("colorless"); break;
        case Card::Eichel:
            c = i18n("Eichel"); break;
        case Card::Gras:
            c = i18n("Gras"); break;
        case Card::Herz:
            c = i18n("Heart"); break;
        case Card::Schellen:
            c = i18n("Schellen"); break;
        default:
            c = QString();
            break;
    }

    switch( m_mode )
    {
        case GameInfo::STICHT:
            g = i18n("Sticht"); break;
        case GameInfo::GEIER:
            g = i18n("Geier"); break;
        case GameInfo::WENZ:
            g = i18n("Wenz"); break;
        case GameInfo::RAMSCH:
            g = i18n("Ramsch"); break;
        case GameInfo::DACHS:
            g = i18n("Badger"); break;
        case GameInfo::RUFSPIEL:
        default:            
            g = QString();
    }

    if( m_mode == GameInfo::RUFSPIEL )
        s = i18n("%1 plays on the %2 Ace.", m_spieler ? m_spieler->name() : QString(), c);
    else if( m_mode == GameInfo::RAMSCH )
        s = i18n("Ramsch is played.");
    else
        s = i18n("%1 plays %2 %3.", m_spieler ? m_spieler->name() : QString(), c, g);

    return s; 
}

int GameInfo::weight( Card* card ) const
{
    if( istTrumpf( card ) )
    {
        if (card->card() == Card::Ober && mode() != GameInfo::WENZ)
            return 3;
        else if (card->card() == Card::Unter && mode() != GameInfo::WENZ)
            return 2;
        else if (card->card() == Card::Unter && mode() == GameInfo::WENZ)
            return 3;
        else
            return 1;
    }
    else 
    {
        if (card->card() == Card::Sau)
            return 1;
        else
            // TODO: ausser man hat auch die SAU
            return -1;
    }
}

bool GameInfo::istTrumpf(Card *card) const
{
    switch(mode())
    {
        case GameInfo::RUFSPIEL:
        case GameInfo::RAMSCH:
                if (card->card() == Card::Ober || card->card() == Card::Unter || card->color() == Card::Herz)
                    return true;
                break;
        case GameInfo::STICHT:
                if (card->card() == Card::Ober || card->card() == Card::Unter || card->color() == color())
                    return true;
                break;
        case GameInfo::GEIER:
                if (card->card() == Card::Ober || card->color() == color())
                    return true;
                break;
        case GameInfo::WENZ:
                if (card->card() == Card::Unter || card->color() == color())
                    return true;
                break;
        case GameInfo::DACHS:
                if (card->card() == Card::Sau || card->card() == Card::Zehn)
                    return true;
                break;
        default:
                break;
    };
    return false;
}

/** checks wether a game is allowed to play:
  * 1. is it allowed to play this game with this cards (needed for callgames)
  * 2. is it allowed to play this game according to the users preferences
  */
bool GameInfo::isAllowed(const CardList& cards, int mode, Card::Color color)
{
  // it is never allowed to decide to play Ramsch
  if (mode == GameInfo::RAMSCH)
      return false;

  if (mode == GameInfo::RUFSPIEL)
  {
    if (cards.contains({Card::Sau, color}) || color == Card::Herz)
      return false;

    // ok now we have to test if we have a card in this color with is no trumpf_index
    for (int i = 0; i < cards.size(); ++i)
    {
      Card* c = cards.at(i);
      if (c->color() == color && c->card() != Card::Ober && c->card() != Card::Unter)
      {
        return true;
      }
    }
    return false;
  }
  else if (mode == GameInfo::STICHT && color == Card::NoColor)
      return false;

  // check for the users preferences now
  const AllowedGames allowed = Settings::instance()->allowedGames();
  if (mode == GameInfo::DACHS && !allowed.dachs)
  {
    return false;
  }

  if ((mode == GameInfo::WENZ && !allowed.wenz) ||
      (mode == GameInfo::WENZ && color != Card::NoColor && !allowed.farb_wenz))
  {
    return false;
  }

  if ((mode == GameInfo::GEIER && !allowed.geier) ||
      (mode == GameInfo::GEIER && color != Card::NoColor && !allowed.farb_geier))
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
        return (this->color() == Card::NoColor);
    } else
        return this->mode() < info.mode();
}

int GameInfo::evalCard(Card* card)
{
    /* Not very nice. Try to find a better way */

    int *l_trumpf;
    int *l_cards;
    int l_trumpf_std[]={ Card::Ober, Card::Unter };
    int l_trumpf_geier=Card::Ober;
    int l_trumpf_wenz=Card::Unter;
    int l_trumpf_dachs[]={ Card::Sau, Card::Zehn };
    int l_cards_std[]={ Card::Sau, Card::Zehn, Card::Koenig, Card::Neun, Card::Acht, Card::Sieben, Card::NoStich };
    int l_cards_geier[]={ Card::Sau, Card::Zehn, Card::Koenig, Card::Unter, Card::Neun, Card::Acht, Card::Sieben };
    int l_cards_wenz[]={ Card::Sau, Card::Zehn, Card::Koenig, Card::Ober, Card::Neun, Card::Acht, Card::Sieben };
    int l_cards_dachs[]={ Card::Koenig, Card::Ober, Card::Unter, Card::Neun, Card::Acht, Card::Sieben, Card::NoStich };
    int l_colors[4];
    int trumpf_index=-1;
    int cards_index=-1;
    int colors_index=-1;
    int trumpf_cnt;
    int col = Card::NoColor;
    int i, a;

    switch (mode())
    {
        default:
        case GameInfo::RAMSCH:
        case GameInfo::RUFSPIEL:
            trumpf_cnt=2;
            l_trumpf=l_trumpf_std;
            l_cards=l_cards_std;
            col=Card::Herz;
            break;
        case GameInfo::STICHT:
            trumpf_cnt=2;
            l_trumpf=l_trumpf_std;
            l_cards=l_cards_std;
            col = color();
            break;
        case GameInfo::GEIER:
            trumpf_cnt=1;
            l_trumpf=&l_trumpf_geier;
            l_cards=l_cards_geier;
            col = color();
            break;
        case GameInfo::WENZ:
            //qDebug("Wenz, color: "+ QString::number(gameinfo->color()));
            trumpf_cnt=1;
            l_trumpf=&l_trumpf_wenz;
            l_cards=l_cards_wenz;
            col = color();
            break;
        case GameInfo::DACHS:
            trumpf_cnt=2;
            l_trumpf=l_trumpf_dachs;
            l_cards=l_cards_dachs;
            col=Card::NoColor;
            break;
    }
    if (col == Card::NoColor)
        col = Card::Eichel;
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
        if(card->card()==l_trumpf[i])
        {
            trumpf_index=i;
            break;
        }
    }
    for(i=0;i<7;i++)
    {
        if(card->card()==l_cards[i])
        {
            cards_index=i;
            break;
        }
    }
    for(i=0;i<4;i++)
    {
        if(card->color()==l_colors[i])
        {
            colors_index=i;
            break;
        }
    }
    if(trumpf_index!=-1)
        return CARD_CNT-(trumpf_index*NUMTRUMPF+card->color());
    return CARD_CNT-(trumpf_cnt*NUMTRUMPF+colors_index*(NUMCARDS-trumpf_cnt)+cards_index);
}

int GameInfo::laufende()
{
  if (m_mode == RAMSCH)
    return 0;

  /** TODO: jeder spieler sollte eine laufende property haben und
    * selst zählen wie viele laufende er hat.
    * danach können diese in results einfach addiert werden
    */

  int l = 0;
  CardList all;
  CardList laufend = spieler()->cards();
  if (mitspieler())
    laufend.appendList(mitspieler()->cards());

  all.init();
  all.setAutoDelete(false);
  all.sort([this] (Card* card) { return evalCard(card); });
  all.setAutoDelete(true);

  for (int i = all.size() - 1; i >= 0; --i)
  {
    Card* c = all.at(i);
    CardList list = laufend.FindCards(c->color(), c->card());
    if (!list.empty() && (l > 0 || i == all.size() - 1))
      l++;
    else if ((list.empty() && l < 0) || i == all.size() - 1)
      l--;
    else
    {
      break;
    }
  }

  return l;
}
