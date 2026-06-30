// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "computerplayer.h"

#include "card.h"
#include "cardlist.h"
#include "game.h"
#include "gameinfo.h"
#include "openbook.h"

#include <QDebug>
#include <QString>

#include <iostream>


ComputerPlayer::ComputerPlayer(unsigned int id,Game* game)
: Player(id,game)
, m_openBook(this, game)
{}

void ComputerPlayer::klopfen()
{
    int i;
    const int anz = NUMCARDS/2;
    int start = ( isLast() ? anz : 0 );
    int ende = start + anz;
    int count = 0;
    m_geklopft = false;
    
    /** check 2 possibilities:
      * - all cards have the same color
      * - all cards trump in herz sticht oder sau
      */

    // are all cards in the same color?
    for( i=start;i<ende;i++ )
        if (m_cards.at(i)->color() == m_cards.at(start)->color())
            count++;

    if( count == anz )
    {
        m_geklopft = true;
        Player::klopfen();
        return;
    }

    // are all cards trump?
    GameInfo info;
    info.setMode(GameInfo::STICHT);
    info.setColor(Card::HERZ);
    count = 0;
    for (int i = start; i < ende; ++i)
        if (m_cards.at(i)->card() == Card::SAU || info.istTrumpf(m_cards.at(i)))
            count++;

    if( count == anz )
    {
        m_geklopft = true;
        Player::klopfen();
        return;
    }

    Player::klopfen();
}

void ComputerPlayer::init()
{
    if(m_game->gameInfo()->mitspieler()==this)
    {
        for(mitspieler=0;mitspieler<PLAYERS;mitspieler++)
            if(m_game->findIndex(mitspieler)==m_game->gameInfo()->spieler())
                break;
    }
    else if(m_game->gameInfo()->mode()!=GameInfo::RUFSPIEL &&
            m_game->gameInfo()->mode()!=GameInfo::RAMSCH &&
            m_game->gameInfo()->spieler()!=this)
            mitspieler=m_game->gameInfo()->spieler()->id();
        else
            mitspieler=-1;
}

Card *ComputerPlayer::play()
{
    CardList allowed;
    int i;

    if (m_game->currStich().isEmpty())
    {
        allowed = m_openBook.possibleCards();
        if (allowed.empty())
            allowed = allowedCards();
        i=0;
        for(;;)
        {
            if (!(i < allowed.count() && allowed.count() > 1))
                break;
            if (allowed.at(i)->points() == 10)
                allowed.removeCard(i);
            else
                i++;
        }
        i=0;
        for(;;)
        {
            if (!(i < allowed.count() && allowed.count() > 1))
                break;
            if (allowed.at(i)->points() == 4)
                allowed.removeCard(i);
            else
                i++;
        }
        log(QString("allowed cards: %1").arg(allowed.count()));
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
        allowed = allowedCards();
    
    return findCardToPlay(allowed);
}

std::optional<GameInfo> ComputerPlayer::gameInfo(bool force)
{
    QList<game_data> lst;
    
    for(int i=GameInfo::STICHT;i<=GameInfo::RUFSPIEL;i++)
    {
        for( int z=Card::NOCOLOR;z<=Card::SCHELLEN;z++)
        {
            if (!GameInfo::isAllowed(m_cards, i, z ))
              continue;
         
            game_data d;
            d.trumpf = d.fehlfarbe = d.weight = 0;
            d.info.setMode( i );
            d.info.setColor( z );        
            
            for (int c = 0; c < m_cards.count(); c++)
            {
                d.weight += d.info.weight(m_cards.at(c));
                if (d.info.istTrumpf(m_cards.at(c)))
                    d.trumpf++;
                else if (!d.info.istTrumpf(m_cards.at(c)) && m_cards.at(c)->card() != Card::SAU)
                {
                    CardList list = m_cards.FindCards(m_cards.at(c)->color(), Card::SAU);
                    if (list.isEmpty())
                        d.fehlfarbe++;
                }
            }

            if( i==GameInfo::RUFSPIEL && d.weight>=8 && ((d.fehlfarbe<=2 && d.trumpf>=5) || d.fehlfarbe<=1) )
                lst.append( d );
            else if( i!=GameInfo::RUFSPIEL && d.weight>=9 && d.fehlfarbe<=1 && d.trumpf>=5 )
                lst.append( d );
        }
    }
    
    // now we have all possible games in list,
    // let's find the best one;
    if( !lst.isEmpty() )
    {
        int best = 0;
        for( int i=0; i<lst.count(); i++ )
            if( lst[i].weight > lst[best].weight || (lst[i].trumpf > lst[best].trumpf && lst[i].fehlfarbe < lst[i].fehlfarbe) )
            {
                best = i;
                continue;
            }
        
        return lst[best].info;
    }
    
    if( force && lst.isEmpty() )
    {
        // Play eichel sticht for now. Optimize later
        GameInfo gi;
        gi.setColor(Card::EICHEL);
        gi.setMode(GameInfo::STICHT);
        return gi;
    }
    
    return {};
}

Card* ComputerPlayer::findCardToPlay(const CardList& cards)
{
        Card* highestTrump = highestTrumpfInGame();

    if(ownStich())
    {
                // nur schmieren wenn :
            // -man letzter ist,
            // -der partner mit dem hoechsten trumpf im spiel 
            //  abzueglich der eigenen truempfe gestochen hat 
            // -oder wenn der darauf folgende spieler der selben
            //  partei angehoert wie man selbst (wichtig fuer solos)
            if (m_game->currStich().count() == PLAYERS - 1 ||
                ( highestTrump && 
                  findHighestCard(m_game->currStich() )->id() == highestTrump->id() ) ||
                (m_game->currStich().count() == PLAYERS - 2 &&
                  m_game->nextPlayer()->isPlayer() == this->isPlayer() ) )
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
        const auto& stich = m_game->currStich();
        bool gestochen = false;

        // sicher stellen das noch niemand gestochen hat
        for (int i = 0; i < stich.count(); ++i)
        {
          if (m_game->gameInfo()->istTrumpf(stich.at(i)))
          {
            gestochen = true;
            break;
          }
        }

        if (!stich.isEmpty() && !gestochen && cardsStillInGame(m_game->currStich().at(0)->color()) >= 3)
        {
            // falls der computer spieler farbfrei ist und noch genügend karten von dieser farbe im spieler
            // sind versucht er so niedrig wie möglich zu stechen
            log("Versuche so niedrig wie möglich zu stechen.");
            return findLowestPossibleCard(findHighestCard(stich), cards);
        }
        else
        {
          log("Versuche zu stechen.");
          // Wenn ich letzter Spieler bin reicht der kleinste Trumpf
          if (m_game->currStich().count() == PLAYERS - 1)
          {
            return findLowestPossibleCard(findHighestCard(stich), cards);
          }
          else
          {
            return findHighestCard(cards);
          }
        }
    }
    log("Spiele billigste karte.");
    return findCheapestCard(cards);
}

Card* ComputerPlayer::findHighestCard(const CardList& cards) const
{
  Card* high{};
  if (!cards.empty()) high = cards.first();

  Card* card{};
  for (int i = 0; i < cards.count(); i++)
  {
    card = cards.at(i);
    if (m_game->isHigher(card, high))
    {
      high = card;
      continue;
    }
  }
  return high;
}

Card* ComputerPlayer::findLowestPossibleCard(Card* highest, const CardList& cards) const
{
    Card* high = findHighestCard(cards);
    Card* card;

    for (int i = 0; i < cards.count(); ++i)
    {
        card = cards.at(i);
        if( m_game->isHigher( card, highest ) && !m_game->isHigher( card, high ) )
            high = card;
    }
    
    return high;
}

Card* ComputerPlayer::findSchmiere(const CardList& cards) const
{
    Card* schmiere{};
    if (!cards.empty()) { schmiere = cards.first(); }
    Card* card{};

    for (int i = 0; i < cards.count(); i++)
    {
        card = cards.at(i);
        if( card->points() > schmiere->points() )
        {
            schmiere = card;
            continue;
        }
    }
    
    return schmiere;
}

Card* ComputerPlayer::findCheapestCard(const CardList& cards) const
{
    Card* low{};
    if (!cards.empty()) { low = cards.first(); }
    Card* card{};

    for (int i = 0; i < cards.count(); ++i)
    {
        card = cards.at(i);
        if(card->points()<low->points() )
        {
            low = card;
            continue;
        }
    }
    return low;
}

bool ComputerPlayer::canMakeStich(const CardList& cards)
{
    Card* highestCard{};
    Card* card{};

    if (!m_game->currStich().empty()) highestCard = m_game->currStich().at(m_game->highestCard());
    if (!cards.empty()) { card = cards.first(); }
    if(!highestCard)
        return true;
    for (int i = 0; i < cards.count(); ++i)
    {
        card = cards.at(i);
        if(m_game->isHigher(card, highestCard))
            return true;
    }
    return false;
}

bool ComputerPlayer::ownStich()
{
    Card* highestCard{};
    if (!m_game->currStich().empty()) highestCard = m_game->currStich().at(m_game->highestCard());

    /* The stich cannot belong to us if we are to play
     * the first card! 
     */
    if (!m_game->currStich().count())
        return false;

    /* This looks butt ugly but it works!
     * We can only check if the card belongs to the player party, so we check afterwars
     * if we are part of the player part!! Think about it... it does really work :)
     */
    std::cout << QString("%1 ist %2Spieler.").arg(name()).arg(this->isPlayer() ? "" : "Nicht-").toStdString() << std::endl;
    if( highestCard->owner()->isPlayer() )
        return ( this->isPlayer() ? true : false );
    else
        return ( this->isPlayer() ? false : true );
}

Card *ComputerPlayer::highestTrumpfInGame()
{
    CardList trumpfs;

    auto istTrumpf = [this] (auto card) { return m_game->gameInfo()->istTrumpf(card); };

    const auto& allCards = m_game->allCards();
    std::copy_if(std::begin(allCards), std::end(allCards), std::back_inserter(trumpfs), istTrumpf);

    auto removeCardsFromTrumpfs = [&] (Card* card) { trumpfs.removeCard(card); };

    const auto& played = m_game->playedCards();
    std::for_each(std::begin(played), std::end(played), removeCardsFromTrumpfs);

    std::for_each(std::begin(m_cards), std::end(m_cards), removeCardsFromTrumpfs);

    trumpfs.sort([this] (Card* card) { return m_game->gameInfo()->evalCard(card); });
    if (trumpfs.empty())
    {
      return {};
    }

    return trumpfs.back();
}

int ComputerPlayer::myTrumpfs()
{
    int trumpfs=0;
    Card *c;

    for (int i = 0; i < m_cards.count(); ++i)
    {
        c = m_cards.at( i );
        if( m_game->gameInfo()->istTrumpf( c ) )
            trumpfs++;
    }
    return trumpfs;
}

void ComputerPlayer::cardPlayed(unsigned int player, Card *c)
{
    qDebug() << "********* cardPlayed (" << c->toString() << ")";

    // TODO: was called by signal before the thread change
    // has to be implemented in another way
    if(mitspieler!=-1)
        return;
    if(m_game->gameInfo()->mode()==GameInfo::RUFSPIEL && c->card()==Card::SAU && c->color()==m_game->gameInfo()->color())
    {
        if(m_game->gameInfo()->spieler()==this)
            mitspieler=player;
        else
            mitspieler=0+1+2+3-(id()+m_game->gameInfo()->spieler()->id()+player);
        log(QString("Mitspieler gefunden: %1").arg(m_game->findIndex(mitspieler)->name()));
    }
}

int ComputerPlayer::cardsStillInGame(int c)
{
    const CardList all = m_game->allCards();

    CardList played;
    played.appendList(m_game->playedCards());
    played.appendList(m_game->currStich());

    auto istTrumpf = [this] (auto card) { return m_game->gameInfo()->istTrumpf(card); };
    auto matches = [&] (auto card) { return (istTrumpf(card) && Card::NOCOLOR == c) || card->color() == c; };

    return std::count_if(std::begin(all), std::end(all), matches) -
           std::count_if(std::begin(played), std::end(played), matches);
}

void ComputerPlayer::log(const QString& msg) const
{
  std::cout << QString("%1: %2").arg(name()).arg(msg).toStdString() << std::endl;
}
