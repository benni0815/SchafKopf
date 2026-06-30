// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "computerplayer.h"

#include "cards.h"
#include "game.h"
#include "gameinfo.h"
#include "openbook.h"

#include <QDebug>
#include <QString>

#include <iostream>

ComputerPlayer::ComputerPlayer(unsigned int id, const Game& game)
: Player(id, game)
, m_openBook(this, game)
{}

void ComputerPlayer::klopfen()
{
  const int anz = NUMCARDS / 2;
  const int start = isLast() ? anz : 0;
  const int ende = start + anz;
  int count = 0;
  m_geklopft = false;

  // check 2 possibilities:
  // - all cards have the same color
  // - all cards trump in herz sticht oder sau

  // are all cards in the same color?
  for (int i = start; i < ende; ++i)
  {
    if (m_cards.at(i).id.color == m_cards.at(start).id.color)
    {
      count++;
    }
  }

  if (count == anz)
  {
    m_geklopft = true;
    emit doubled(id());
    return;
  }

  // are all cards trump?
  count = 0;
  for (int i = start; i < ende; ++i)
  {
    if (m_cards.at(i).id.type == CardId::Sau || GameInfo({GameVariant::STICHT, CardId::Herz}).istTrumpf(m_cards.at(i).id))
    {
      count++;
    }
  }

  if (count == anz)
  {
    m_geklopft = true;
    emit doubled(id());
    return;
  }

  emit doubled(id());
}

void ComputerPlayer::init()
{
  const auto& gameInfo = m_game.gameInfo();
  if (gameInfo.mitspieler() == this)
  {
    for (mitspieler = 0; mitspieler < PLAYERS; mitspieler++)
    {
      if (m_game.players().at(mitspieler) == gameInfo.spieler())
      {
        break;
      }
    }
  }
  else if (gameInfo.mode() != GameVariant::RUFSPIEL &&
           gameInfo.mode() != GameVariant::RAMSCH &&
           gameInfo.spieler() != this)
  {
    mitspieler = gameInfo.spieler()->id();
  }
  else
  {
    mitspieler = -1;
  }
}

Card ComputerPlayer::play()
{
  CardList allowed;

  if (m_game.currStich().empty())
  {
    allowed = m_openBook.possibleCards();
    if (allowed.empty())
    {
      allowed = allowedCards();
    }

    for (int i = 0;;)
    {
      if (!(i < allowed.size() && allowed.size() > 1))
      {
        break;
      }
      if (Cards::getPoints(allowed.at(i)) == 10)
      {
        allowed.takeAt(i);
      }
      else
      {
        i++;
      }
    }
    for (int i = 0;;)
    {
      if (!(i < allowed.size() && allowed.size() > 1))
      {
        break;
      }
      if (Cards::getPoints(allowed.at(i)) == 4)
      {
        allowed.takeAt(i);
      }
      else
      {
        i++;
      }
    }
    log(QString("allowed cards: %1").arg(allowed.size()));
  }
  /*
      // The AI is more intelligent, let the AI decide
    else if( m_game->currStich()->count() == 3 )
    {
        // last one to play a card
        // don't know if this will be helpfull
        // as it does merely the same the KI does already
        // just for a special case.
        // So it takes some work away from the real KI.
        StrategyBook strategy( this, m_game );
        allowed = strategy.possibleCards();
    }
    */
  if (allowed.empty())
  {
    allowed = allowedCards();
  }

  return findCardToPlay(allowed);
}

std::optional<GameVariant> ComputerPlayer::gameInfo(bool force)
{
  QList<game_data> lst;

  for (const auto mode : {GameVariant::STICHT, GameVariant::WENZ, GameVariant::GEIER, GameVariant::RUFSPIEL, GameVariant::RAMSCH, GameVariant::DACHS})
  {
    for (const auto color : {CardId::NoColor, CardId::Eichel, CardId::Gras, CardId::Herz, CardId::Schellen})
    {
      if (!GameInfo::isAllowed(Cards::toIds(m_cards), {mode, color}))
      {
        continue;
      }

      game_data d;
      d.game = GameVariant{mode, color};

      for (const auto& c : Cards::toIds(m_cards))
      {
        d.weight += GameInfo::weight(c, d.game);
        if (GameInfo::istTrumpf(c, d.game))
        {
          d.trumpf++;
        }
        else if (!GameInfo::istTrumpf(c, d.game) && c.type != CardId::Sau)
        {
          if (!Cards::find(m_cards, {CardId::Sau, c.color}))
          {
            d.fehlfarbe++;
          }
        }
      }

      if (mode == GameVariant::RUFSPIEL && d.weight >= 8 && ((d.fehlfarbe <= 2 && d.trumpf >= 5) || d.fehlfarbe <= 1))
      {
        lst.append(d);
      }
      else if (mode != GameVariant::RUFSPIEL && d.weight >= 9 && d.fehlfarbe <= 1 && d.trumpf >= 5)
      {
        lst.append(d);
      }
    }
  }

  // now we have all possible games in list,
  // let's find the best one;
  if (!lst.isEmpty())
  {
    int best = 0;
    for (int i = 0; i < lst.count(); i++)
    {
      if (lst[i].weight > lst[best].weight || (lst[i].trumpf > lst[best].trumpf && lst[i].fehlfarbe < lst[i].fehlfarbe))
      {
        best = i;
        continue;
      }
    }

    return lst[best].game;
  }

  if (force && lst.isEmpty())
  {
    // Play eichel sticht for now. Optimize later
    return GameVariant{GameVariant::STICHT, CardId::Eichel};
  }

  return {};
}

Card ComputerPlayer::findCardToPlay(const CardList& cards)
{
  Card highestTrump = highestTrumpfInGame();

  if (ownStich())
  {
    // nur schmieren wenn :
    // -man letzter ist,
    // -der partner mit dem hoechsten trumpf im spiel
    //  abzueglich der eigenen truempfe gestochen hat
    // -oder wenn der darauf folgende spieler der selben
    //  partei angehoert wie man selbst (wichtig fuer solos)
    if (m_game.currStich().size() == PLAYERS - 1
        || (highestTrump.id != CardId{} && m_game.highestCardInStich().id == highestTrump.id)
        || (m_game.currStich().size() == PLAYERS - 2 && m_game.nextPlayer()->isPlayer() == this->isPlayer()))
    {
      log("schmiere!");
      return findSchmiere(cards);
    }
  }

  // Schade mehr als es hilft
  /*  else
            {
            // Bei Solos wird geschmiert wenn:
            // -man nicht spieler und nicht letzter oder erster ist
            // -wenn jemand anders den stich noch machen kann
            if( m_game->gameInfo()->mitspieler() == NULL &&      // Solo ?
                m_game->currStich()->count() < PLAYERS - 1 &&    // letzter ?
                m_game->currStich()->count() &&                  // erster ?
                !this->isPlayer() &&                             // selber spieler ?
                ( highestTrump && 
                  findHighestCard( m_game->currStich() )->id() != highestTrump->id()) && // Kann der Stich ueberhaupt gestochen werden
                !canMakeStich(cards) )                           // kann ihn jemand ausser mir machen?
            {
                qDebug("%s: schmiere!", name().toLatin1() );
                return findSchmiere(cards);
            }
        }*/

  if (canMakeStich(cards))
  {
    const auto& stich = m_game.currStich();
    bool gestochen = false;

    // sicher stellen das noch niemand gestochen hat
    for (const auto& card : stich)
    {
      if (m_game.gameInfo().istTrumpf(card.id))
      {
        gestochen = true;
        break;
      }
    }

    if (!stich.empty() && !gestochen && cardsStillInGame(m_game.currStich().at(0).id.color) >= 3)
    {
      // falls der computer spieler farbfrei ist und noch genügend karten von dieser farbe im spieler
      // sind versucht er so niedrig wie möglich zu stechen
      log("Versuche so niedrig wie möglich zu stechen.");
      return findLowestPossibleCard(m_game.highestCardInStich(), cards);
    }
    else
    {
      log("Versuche zu stechen.");
      // Wenn ich letzter Spieler bin reicht der kleinste Trumpf
      if (m_game.currStich().size() == PLAYERS - 1)
      {
        return findLowestPossibleCard(m_game.highestCardInStich(), cards);
      }
      else
      {
        return m_game.gameInfo().highestCard(cards);
      }
    }
  }
  log("Spiele billigste karte.");
  return findCheapestCard(cards);
}

Card ComputerPlayer::findLowestPossibleCard(const Card& highest, const CardList& cards) const
{
  Card high = m_game.gameInfo().highestCard(cards);

  for (const auto& card : cards)
  {
    const auto& info = m_game.gameInfo();
    if (info.isHigher(card.id, highest.id) && !info.isHigher(card.id, high.id))
    {
      high = card;
    }
  }

  return high;
}

Card ComputerPlayer::findSchmiere(const CardList& cards) const
{
  auto less = [](const Card& lhs, const Card& rhs) { return Cards::getPoints(lhs) < Cards::getPoints(rhs); };
  return Cards::maxElement(cards, less);
}

Card ComputerPlayer::findCheapestCard(const CardList& cards) const
{
  auto more = [](const Card& lhs, const Card& rhs) { return Cards::getPoints(lhs) > Cards::getPoints(rhs); };
  return Cards::maxElement(cards, more);
}

bool ComputerPlayer::canMakeStich(const CardList& cards)
{
  Card highestCard{};
  Card card{};

  if (!m_game.currStich().empty())
  {
    highestCard = m_game.highestCardInStich();
  }
  if (!cards.empty())
  {
    card = cards.front();
  }
  if (highestCard.id == CardId{})
  {
    return true;
  }
  for (int i = 0; i < cards.size(); ++i)
  {
    card = cards.at(i);
    if (m_game.gameInfo().isHigher(card.id, highestCard.id))
    {
      return true;
    }
  }
  return false;
}

bool ComputerPlayer::ownStich()
{
  Card highestCard{};
  if (!m_game.currStich().empty())
  {
    highestCard = m_game.highestCardInStich();
  }

  // The stich cannot belong to us if we are to play the first card!
  if (!m_game.currStich().size())
  {
    return false;
  }

  // This looks butt ugly but it works!
  // We can only check if the card belongs to the player party, so we check afterwars
  // if we are part of the player part!! Think about it... it does really work :)
  std::cout << QString("%1 ist %2Spieler.").arg(name()).arg(this->isPlayer() ? "" : "Nicht-").toStdString() << std::endl;
  if (highestCard.owner->isPlayer())
  {
    return (this->isPlayer() ? true : false);
  }
  else
  {
    return (this->isPlayer() ? false : true);
  }
}

Card ComputerPlayer::highestTrumpfInGame()
{
  CardList trumpfs;

  auto istTrumpf = [this](Card card) { return m_game.gameInfo().istTrumpf(card.id); };

  const auto& allCards = m_game.allCards();
  std::copy_if(std::begin(allCards), std::end(allCards), std::back_inserter(trumpfs), istTrumpf);

  auto removeCardsFromTrumpfs = [&](Card card) { Cards::remove(trumpfs, card.id); };

  const auto& played = m_game.playedCards();
  std::for_each(std::begin(played), std::end(played), removeCardsFromTrumpfs);

  std::for_each(std::begin(m_cards), std::end(m_cards), removeCardsFromTrumpfs);

  auto eval = [this](const Card& card) { return m_game.gameInfo().evalCard(card.id); };
  Cards::sort(trumpfs, eval);
  if (trumpfs.empty())
  {
    return {};
  }

  return trumpfs.back();
}

int ComputerPlayer::myTrumpfs()
{
  return std::count_if(std::begin(m_cards), std::end(m_cards), [this] (const Card& c) { return m_game.gameInfo().istTrumpf(c.id); });
}

void ComputerPlayer::cardPlayed(unsigned int player, Card* c)
{
  qDebug() << "********* cardPlayed (" << Cards::toString(*c) << ")";

  // TODO: was called by signal before the thread change
  // has to be implemented in another way
  if (mitspieler != -1)
  {
    return;
  }
  if (m_game.gameInfo().mode() == GameVariant::RUFSPIEL && c->id.type == CardId::Sau && c->id.color == m_game.gameInfo().color())
  {
    if (m_game.gameInfo().spieler() == this)
    {
      mitspieler = player;
    }
    else
    {
      mitspieler = 0 + 1 + 2 + 3 - (id() + m_game.gameInfo().spieler()->id() + player);
    }
    log(QString("Mitspieler gefunden: %1").arg(m_game.players().at(mitspieler)->name()));
  }
}

int ComputerPlayer::cardsStillInGame(int c)
{
  const CardList all = m_game.allCards();

  CardList played;
  played.append(m_game.playedCards());
  played.append(m_game.currStich());

  auto istTrumpf = [this](const Card& card) { return m_game.gameInfo().istTrumpf(card.id); };
  auto matches = [&](const Card& card) { return (istTrumpf(card) && CardId::NoColor == c) || card.id.color == c; };

  return std::count_if(std::begin(all), std::end(all), matches) -
         std::count_if(std::begin(played), std::end(played), matches);
}

void ComputerPlayer::log(const QString& msg) const
{
  std::cout << QString("%1: %2").arg(name()).arg(msg).toStdString() << std::endl;
}
