// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "results.h"

#include "schafkopfdef.h"

#include "cards.h"
#include "game.h"
#include "gameinfo.h"
#include "player.h"
#include "settings.h"

#include <KLocalizedString>

#include <QLocale>

Results::Results(const Game& game)
: m_game(game)
{}

QString Results::result()
{
    parse();
    const bool positive = m_laufende > 0;
   
    QString s;
    const QString schneider = m_schneider ? i18n(" schneider") : QString();
    const QString schwarz = m_schwarz ? i18n(" no-trick") : QString();
    Player* winner = m_game.gameInfo().spieler();
    bool won = m_points > 60;
    
    if( !winner )
    {
        won = m_winners.count() == 1;
        winner = won ? m_winners.at(0) : m_losers.at(0);
    }
    
    if( won )
        s = winner->name() + i18n(" wins with %1 points%2.", m_points, schneider + schwarz);
    else
        s = winner->name() + i18n(" loses with %1 points%2.", m_points, schneider + schwarz);
    
    if( m_laufende )
    {
        s += "\n";
        if( positive )
            s += i18n("%1 got %2 cards in a row.", winner->name(), m_laufende);
        else
            s += i18n("%1 played without %2 cards in a row.", winner->name(), std::abs(m_laufende));
    }
    
    return s;
}

void Results::parse()
{
    m_schneider = false;
    m_schwarz = false;
    m_winners.clear();
    m_losers.clear();
    m_laufende = determineLaufende();

    const auto players = m_game.players();

    auto createList = [&] (std::function<bool(Player*)> func)
    {
      QList<Player*> p;
      std::copy_if(std::begin(players), std::end(players), std::back_inserter(p), func);
      return p;
    };

    const auto& gameInfo = m_game.gameInfo();
    if (gameInfo.mode() == GameVariant::RAMSCH)
    {
        // suche den spieler mit den meisten punkten, denn er hat verloren...
        auto lessPoints = [] (Player* lhs, Player* rhs) { return Cards::calcPoints(lhs->stiche()) < Cards::calcPoints(rhs->stiche()); };
        auto it = std::max_element(std::begin(players), std::end(players), lessPoints);
        if (it == std::end(players))
        {
          return;
        }

        const auto mostPointsPlayer = *it;
        m_points = Cards::calcPoints(mostPointsPlayer->stiche());
        m_losers = {mostPointsPlayer};
        m_winners = createList([&] (Player* p) { return p != mostPointsPlayer; });

        // ... oder wenn er alle stiche hat, dann hat er mit durchmarsch gewonnen
        if (mostPointsPlayer->stiche().size() == TURNS)
        {
          std::swap(m_losers, m_winners);
        }
    }
    else
    {
        auto isSpieler = [] (const Player* p) { return p->isPlayer(); };

        const auto spielerList = createList(isSpieler);
        CardList stiche;
        for (Player* p : spielerList)
        {
          stiche.append(p->stiche());
        }

        m_points = Cards::calcPoints(stiche);
        m_schwarz = (stiche.empty() || stiche.size() == NUMCARDS * PLAYERS);
        m_schneider = (m_points < 31 || m_points >= 91 );
        m_winners = spielerList;
        m_losers = createList(std::not_fn(isSpieler));
        if (m_points <= 60)
        {
          std::swap(m_losers, m_winners);
        }
    }
}

int Results::determineLaufende() const
{
  const auto allCards = m_game.allCards();
  CardList spielerCards;
  std::copy_if(std::begin(allCards), std::end(allCards), std::back_inserter(spielerCards), [] (const Card& c) { return c.owner->isPlayer(); });
  return m_game.gameInfo().laufende(spielerCards);
}

double Results::points(const Player* player)
{
    ResultValues r = (Settings::instance()->resultsType() == Settings::MONEY) ? Settings::instance()->moneyResults()
                                                                              : Settings::instance()->pointResults();
    double m = 0.0;
    
    parse();

    const auto& gameMode = m_game.gameInfo().mode();
    if (gameMode != GameVariant::RUFSPIEL && gameMode != GameVariant::RAMSCH)
        m = r.solo; // SOLO = 20cent
    else if (gameMode == GameVariant::RUFSPIEL)
        m = r.rufspiel;
    else if (gameMode == GameVariant::RAMSCH)
        m = r.ramsch;
        
    m += m_schneider ? r.schneider : 0.0;
    m += m_schwarz ? r.schwarz : 0.0;
    m += m_laufende * r.laufende;
    m *= pow(2, m_game.timesDoubled());
    
    if( !m_winners.contains( player ) )
        m *= -1.0;

    if( m_winners.count() == 1 )
    {
        if( m_winners.contains( player ) )
            m *= (PLAYERS - 1);
    }
    else if( m_winners.count() == PLAYERS - 1 )
    {
        if( !m_winners.contains( player ) )
            m *= m_winners.count();
    }
       
    return m;
}

QString Results::formatedPoints( Player* player )
{
    player->setPoints( points( player ) + player->points() );
    
    if( Settings::instance()->resultsType() == Settings::MONEY )
    {
        return QLocale().toCurrencyString(player->points());
    }
    else
    {
        return QString::number( (int)player->points() );    
    }
}

