/***************************************************************************
 *   Copyright (C) 2004 by Dominik Seichter                                *
 *   domseichter@web.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "openbook.h"

#include "card.h"
#include "cardlist.h"
#include "game.h"
#include "gameinfo.h"
#include "computerplayer.h"

/* A few macros which make it quite simple to build macros for openbook rules */
#define RULE( function, description, condition, eval ) \
    CardList* function ( CardList* allowed, Game* game ) \
    { \
        /* description */ \
        qDebug("Executing Rule=" description ); \
        const GameInfo* info = game->gameInfo(); \
        if( !(condition) ) \
            return 0; \
        CardList* list = new CardList(); \
        unsigned int i=0; \
        for( i=0;i<allowed->count();i++ ) \
        { \
            Card* c = allowed->at( i ); \
            if( (eval) ) \
                list->append( allowed->at( i ) ); \
        } \
        return list; \
    }


// Do not polute the global namespace
namespace OpeningRules
{
    typedef struct s_rule 
    {
        CardList* (*rule)( CardList*, Game* );
        bool player;
    };
    
    RULE( player1, "davon-laufen - h�gt von allowedCards ab und l�ft nicht mit dem zehner davon", 
        ( info->mode() == GameInfo::RUFSPIEL && 
          !allowed->contains( info->color(), Card::SAU ) ),
        ( !info->istTrumpf( c ) && c->color() == info->color() && c->card() != Card::SAU && c->card() != Card::ZEHN )
    )
    
    RULE( player2, "player plays trump",
        ( true ),
        ( info->istTrumpf( c ) )
    )

    RULE( rule1, "play an ace on a solo", 
        ( info->mode() == GameInfo::STICHT || 
          info->mode() == GameInfo::WENZ || 
          info->mode() == GameInfo::GEIER ),
        ( !info->istTrumpf( c ) && c->card() == Card::SAU )
    )
    
    // TODO: nur suchen wenn noch niemand gesucht hat 
    // bzw. davon gelaufen ist
    RULE( rule2, "such die sau auf die gespielt wird", 
        ( info->mode() == GameInfo::RUFSPIEL  ),
        ( !info->istTrumpf( c ) && c->color() == info->color() )
    )
    
    RULE( rule3, "nicht spieler spielt farbe", 
        ( true ), // always execute this one
        ( !info->istTrumpf( c ) )
    )
    
    /** This list should contain all rules for the open book:
      */
      
    s_rule rules[] =
    {
        { player1, true },
        { player1, true },
        { rule1, false },
        { rule2, false },
        { rule3, false }
    };

}

using namespace OpeningRules;

RuleBook::RuleBook( ComputerPlayer* player, Game* game )
{
    m_self = player;
    m_game = game;
    
    m_player = ((Player*)m_self == m_game->gameInfo()->spieler() || (Player*)m_self == m_game->gameInfo()->mitspieler() );
}

RuleBook::~RuleBook()
{

}

//////////////////////////////////
// OpenBook
//////////////////////////////////
OpenBook::OpenBook( ComputerPlayer* player, Game* game )
    : RuleBook( player, game )
{

}

CardList* OpenBook::possibleCards()
{
    CardList* allowed = m_self->allowedCards();
    CardList* list = new CardList();
    unsigned int i = 0;
    
    for( i=0;i<sizeof(rules)/sizeof(s_rule);i++)
    {
        s_rule r = rules[i];
        if( r.player == m_player )
        {
            CardList* l = r.rule( allowed, m_game );
            if( l )
            {
                list->appendList( l );
                if( !l->isEmpty() )
                {
                    delete l;
                    break;
                } else
                    delete l;
            }
        } 
    }
    
    delete allowed;
    if( !list->isEmpty() )
        return list;
    else
    {
        delete list;
        return NULL;
    }
}

//////////////////////////////////
// StrategyBook 
//////////////////////////////////
StrategyBook::StrategyBook( ComputerPlayer* player, Game* game )
    : RuleBook( player, game )
{

}
        
CardList* StrategyBook::possibleCards()
{
    // the strategy book works only when it is called
    // for the last player    
    CardList* stiche = m_game->currStich();
    if( stiche && stiche->count() != 3 )
        return NULL;
        
    CardList* allowed = m_self->allowedCards();
    CardList* list = new CardList();
    Card* highest = NULL;
    bool unser_stich = false;
    bool abspatzen = false;
    
    /* Moeglichkeiten:
     * 1. Der stich gehoert dem mitspieler 
     *    - schmieren
     *    - abspatzen
     * 2. Der stich gehoert den gegner:
     *    - stechen
     *    - abspatzen
     */
    highest = stiche->at( m_game->highestCard() );
    if( highest->owner() == m_game->gameInfo()->spieler() || 
        highest->owner() == m_game->gameInfo()->mitspieler() )
    {
        // der stich gehört der spieler partei
        unser_stich = m_player;
    }
    else
        // der stich gehört nicht der spieler partei
        unser_stich = !m_player;
    
        
    if( unser_stich )
    {
        // unser stich: suche die karte mit den meisten punkten
        Card* c = m_self->findSchmiere( allowed );
        // nur karten die hoeher als ein koenig sind schmieren
        if( c && c->points() >= 4 )
            list->append( c );
        else
            abspatzen = true;
    }
    else
    {
        // stich des gegners
        // 1. feststellen ob wir den stich ueberhaupt machen koennen
        // 2. feststellen ob sich der stich lohnen wuerde
        Card* c = allowed->at( m_game->highestCard( allowed ) );
        if( c && m_game->isHigher( c, highest ) && stiche->points() > 6 )
            list->append( m_self->findLowestPossibleCard( highest, allowed ) );
        else
            abspatzen = true;
    }

    if( abspatzen )
    {
        // nichts zum schmieren gefunden
        // abspatzen:
        Card* c = m_self->findCheapestCard( allowed );
        if( c && c->points() == 0 )
            list->append( c );
    }
    
    delete allowed;
    if( !list->isEmpty() )
        return list;
    else
    {
        delete list;
        return NULL;
    }
}

