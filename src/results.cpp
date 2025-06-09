// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "results.h"

#include "schafkopfdef.h"

#include "cardlist.h"
#include "game.h"
#include "gameinfo.h"
#include "player.h"
#include "settings.h"

#include <KLocalizedString>

#include <QLocale>


Results::Results()
{
    m_gameinfo = NULL;
    m_game = NULL;
    m_laufende = 0;
    
    init();
}

void Results::init()
{
    m_schneider = false;
    m_schwarz = false;
    
    m_winners.clear();
    m_losers.clear();
}

QString Results::result()
{
    parse();
    bool positive = m_laufende > 0;
   
    QString s;
    QString schneider = m_schneider ? i18n(" schneider") : QString();
    QString schwarz = m_schwarz ? i18n(" no-trick") : QString();
    Player* winner = m_gameinfo->spieler();
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
        if( positive )
            s += "\n" + winner->name() + i18n(" got %1 cards in a row.", m_laufende);
        else
            s += "\n" + winner->name() + i18n(" played without %1 cards in a row.", m_laufende);
    }
    
    return s;
}

void Results::parse()
{
    int i;

    init();
        
    if( m_gameinfo->mode() == GameInfo::RAMSCH )
    {
        // suche den spieler mit den meisten punkten, denn er hat verloren
        // oder wenn er alle stiche hat, dann hat er mit durchmarsch gewonnen
        int highest = 0;
        for( i=0;i<PLAYERS;i++ )
            if( m_game->findIndex( i )->stiche()->points() > m_game->findIndex( highest )->stiche()->points() )
                highest = i;

        m_points = m_game->findIndex( highest )->stiche()->points();
        if( m_game->findIndex( highest )->stiche()->count() == TURNS )
        {
            m_winners.append( m_game->findIndex( highest ) );
            for( i=0;i<PLAYERS;i++ )
                if( i!=highest )
                    m_losers.append( m_game->findIndex( i ) );
        }
        else
        {
            m_losers.append( m_game->findIndex( highest ) );
            for( i=0;i<PLAYERS;i++ )
                if( i!=highest )
                    m_winners.append( m_game->findIndex( i ) );
        }
    }
    else
    {
        CardList stiche( *(m_gameinfo->spieler()->stiche()) );
        if( m_gameinfo->mitspieler() )
            stiche.appendList( m_gameinfo->mitspieler()->stiche() );
       
        m_points = stiche.points();
        m_schwarz = ( stiche.isEmpty() || stiche.count() == NUMCARDS * PLAYERS );
        m_schneider = (m_points < 31 || m_points >= 91 );
    
        if( m_points > 60 )
        {
            m_winners.append( m_gameinfo->spieler() );
            if( m_gameinfo->mitspieler() )
                m_winners.append( m_gameinfo->mitspieler() );
                
            for( i=0;i<PLAYERS;i++ )
                if( m_game->findIndex( i ) != m_gameinfo->spieler() && 
                    m_game->findIndex( i ) != m_gameinfo->mitspieler() )
                    m_losers.append( m_game->findIndex( i ) );
        }
        else
        {
            m_losers.append( m_gameinfo->spieler() );
            if( m_gameinfo->mitspieler() )
                m_losers.append( m_gameinfo->mitspieler() );
                
            for( i=0;i<PLAYERS;i++ )
                if( m_game->findIndex( i ) != m_gameinfo->spieler() && 
                    m_game->findIndex( i ) != m_gameinfo->mitspieler() )
                    m_winners.append( m_game->findIndex( i ) );
        }
                
        // ab 2 laufende bei wenz und geier
        if( m_gameinfo->mode() == GameInfo::GEIER || m_gameinfo->mode() == GameInfo::WENZ )
            m_laufende = ( m_laufende >= 2 || m_laufende <= -2 ) ? m_laufende : 0;
        // sonst ab 3 laufenden
        else if( m_gameinfo->mode() == GameInfo::STICHT || m_gameinfo->mode() == GameInfo::RAMSCH 
                || m_gameinfo->mode() == GameInfo::DACHS || m_gameinfo->mode() == GameInfo::RUFSPIEL )
            m_laufende = ( m_laufende >= 3 || m_laufende <= -3 ) ? m_laufende : 0;

        m_laufende = m_laufende < 0 ? m_laufende * -1 : m_laufende;
    }
}

double Results::klopfen(int anzahl, double betrag)
{
    if(anzahl==0) 
        return betrag; 
    else 
        return klopfen(anzahl-1, betrag)*2.0; 
}

void Results::setGame( Game* game )
{ 
    m_game = game; 
    m_gameinfo = game->gameInfo();
}

double Results::points( Player* player )
{
    ResultValues r = (Settings::instance()->resultsType() == Settings::MONEY) ? Settings::instance()->moneyResults()
                                                                              : Settings::instance()->pointResults();
    double m = 0.0;
    
    parse();
        
    if( m_gameinfo->mode() != GameInfo::RUFSPIEL && m_gameinfo->mode() != GameInfo::RAMSCH )
        m = r.solo; // SOLO = 20cent
    else if( m_gameinfo->mode() == GameInfo::RUFSPIEL )
        m = r.rufspiel;
    else if( m_gameinfo->mode() == GameInfo::RAMSCH )
        m = r.ramsch;
        
    m += m_schneider ? r.schneider : 0.0;
    m += m_schwarz ? r.schwarz : 0.0;
    m += m_laufende * r.laufende;
    m = klopfen( player->game()->timesDoubled(), m );
    
    //if( player == m_gameinfo->spieler() || player == m_gameinfo->mitspieler() )
    if( !m_winners.contains( player ) )
        m *= -1.0;
        
    //if( player == m_gameinfo->spieler() && !m_gameinfo->mitspieler() )
    //        m *= 3;

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

