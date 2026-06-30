// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "openbook.h"

#include "cards.h"
#include "game.h"
#include "gameinfo.h"
#include "computerplayer.h"

#include <iostream>


namespace OpeningRules
{

enum SpielerStatus : bool
{
  NichtSpieler = false,
  Spieler = true
};

struct Rule
{
  const std::string description;
  const SpielerStatus player{NichtSpieler};
  std::function<bool(const GameInfo&, const CardList&)> condition;
  std::function<bool(const GameInfo&, CardId)> eval;

  CardList execute(const CardList& allowed, const Game& game) const
  {
    std::cout << "Executing Rule: " << description << std::endl;
    const auto info = game.gameInfo();
    if (!condition(info, allowed))
    {
      return {};
    }
    CardList list;
    for (int i = 0; i < allowed.size(); i++)
    {
      Card c = allowed.at(i);
      if (eval(info, c.id)) { list.push_back(allowed.at(i)); }
    }
    return list;
  }
};

const auto player1 = Rule{"davon-laufen - haegt von allowedCards ab und laeuft nicht mit dem zehner davon", Spieler,
  [] (auto info, auto allowed) { return info.mode() == GameVariant::RUFSPIEL && !Cards::find(allowed, {CardId::Sau, info.color()}); },
  [] (auto info, auto c) { return !info.istTrumpf(c) && c.color == info.color() && c.type != CardId::Sau && c.type != CardId::Zehn; }
};

const auto player2 = Rule{"player plays trump", Spieler,
  [] (auto, auto) { return true; },
  [] (auto info, auto c) { return info.istTrumpf(c); }
};

const auto rule1 = Rule{"play an ace on a solo", NichtSpieler,
  [] (auto info, auto) { return info.mode() == GameVariant::STICHT || info.mode() == GameVariant::WENZ || info.mode() == GameVariant::GEIER; },
  [] (auto info, auto c) { return !info.istTrumpf(c) && c.type == CardId::Sau; }
};

const auto rule2 = Rule{"such die sau auf die gespielt wird", NichtSpieler,
  [] (auto info, auto) { return info.mode() == GameVariant::RUFSPIEL; },
  [] (auto info, auto c) { return !info.istTrumpf(c) && c.color == info.color(); }
};

const auto rule3 = Rule{"nicht spieler spielt farbe", NichtSpieler,
  [] (auto, auto) { return true; },
  [] (auto info, auto c) {return !info.istTrumpf(c); }
};

const auto rules = std::vector<Rule>{player1, player2, rule1, rule2, rule3};

} // namespace OpeningRules

using namespace OpeningRules;

RuleBook::RuleBook(ComputerPlayer* player, const Game& game)
: m_game(game)
, m_self(player)
{}

//////////////////////////////////
// OpenBook
//////////////////////////////////
OpenBook::OpenBook(ComputerPlayer* player, const Game& game)
: RuleBook(player, game)
{}

CardList OpenBook::possibleCards()
{
    CardList allowed = m_self->allowedCards();

    for (const auto& rule : rules)
    {
        if (rule.player == m_self->isPlayer())
        {
            CardList l = rule.execute(allowed, m_game);
            if (!l.empty())
            {
               return l;
            }
        }
    }

    return {};
}

//////////////////////////////////
// StrategyBook 
//////////////////////////////////
StrategyBook::StrategyBook( ComputerPlayer* player, const Game& game )
    : RuleBook( player, game )
{}

CardList StrategyBook::possibleCards()
{
    // the strategy book works only when it is called
    // for the last player
    const auto& stiche = m_game.currStich();
    if (stiche.size() != 3 )
      return {};
        
    /* Moeglichkeiten:
     * 1. Der stich gehoert dem mitspieler 
     *    - schmieren
     *    - abspatzen
     * 2. Der stich gehoert den gegner:
     *    - stechen
     *    - abspatzen
     */
    bool unser_stich{false};
    const Card highest = m_game.highestCardInStich();
    if (highest.owner->isPlayer())
    {
        // der stich gehört der spieler partei
        unser_stich = m_self->isPlayer();
    }
    else
    {
        // der stich gehört nicht der spieler partei
        unser_stich = !m_self->isPlayer();
    }

    const CardList allowed = m_self->allowedCards();
    CardList list;
    bool abspatzen{false};
    if( unser_stich )
    {
        // unser stich: suche die karte mit den meisten punkten
        Card c = m_self->findSchmiere(allowed);
        // nur karten die hoeher als ein koenig sind schmieren
        if (c.id != CardId{} && Cards::getPoints(c) >= 4 )
            list.push_back(c);
        else
            abspatzen = true;
    }
    else
    {
        // stich des gegners
        // 1. feststellen ob wir den stich ueberhaupt machen koennen
        // 2. feststellen ob sich der stich lohnen wuerde
        Card c = m_game.gameInfo().highestCard(allowed);
        if (c.id != CardId{} && m_game.gameInfo().isHigher(c.id, highest.id) && Cards::calcPoints(stiche) > 6)
            list.push_back(m_self->findLowestPossibleCard(highest, allowed));
        else
            abspatzen = true;
    }

    if( abspatzen )
    {
        // nichts zum schmieren gefunden
        // abspatzen:
        Card c = m_self->findCheapestCard(allowed);
        if (c.id != CardId{} && Cards::getPoints(c) == 0 )
            list.push_back(c);
    }
    
    return list;
}
