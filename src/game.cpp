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
#include "game.h"

#include "humanplayer.h"
#include "computerplayer.h"
#include "cardlist.h"
#include "gamecanvas.h"
#include "gameinfo.h"
#include "results.h"
#include "moneyresults.h"
#include "pointresults.h"

#include "settings.h"
#include "timer.h"

#include <klocale.h>
#include <kmessagebox.h>
#include <string.h>

Game::Game(QObject *parent, const char *name)
 : QObject(parent, name)
{
    unsigned int i;
    terminated = true;
    
    m_canvas = NULL;
    m_laufende = 0;
    
    // Create our player objects
    // delete these only in the destructor    
    QStringList list = Settings::instance()->playerNames();
    m_players[0] = new HumanPlayer( this );
    m_players[0]->setName( list[0] );
	for( i=1;i<PLAYERS;i++)
    {
        m_players[i] = new ComputerPlayer( this );
        m_players[i]->setName( list[i] );
    }
    
    // make sure that results get cleaned up, when the results type is changed
    connect( Settings::instance(), SIGNAL( resultsTypeChanged() ), this, SLOT( resetGameResults() ) );
}

Game::~Game()
{
    unsigned int i;
    
	m_currstich.clear();
	for(i=0;i<PLAYERS;i++)
	{
        delete m_players[i];
		m_players[i]=NULL;
	}
}

void Game::start()
{
    unsigned int i = 0;
    CardList *playercards[PLAYERS];

	m_allcards.randomize();
    for( i=0; i<PLAYERS; i++)
		playercards[i]=new CardList();
    for( i=0; i<CARD_CNT; i++)
        playercards[i%PLAYERS]->append(m_allcards.at(i));

    for( i=0;i<PLAYERS;i++)
    {
        m_players[i]->setCards( playercards[i] );
        m_players[i]->stiche()->clear();
    }
	emit gameStarted();
}

// Wer hat den ganzen rekursiven code in gameLoop zu veantworten? Bitter sofort erschie�n :)
void Game::gameLoop()
{
    int i, a, index, realindex;
    Player *tmp[PLAYERS];
	Card *c=NULL;
    Timer timer;
	int gamecnt=0;
    
	terminated = false;

    resetGameResults();
    
	while(!terminated)
	{
		start();
		for(i=0;i<PLAYERS;i++)
			tmp[i]=m_players[(i+gamecnt)%PLAYERS];
		for(i=0;i<PLAYERS;i++)
    	{
	        tmp[i]->klopfen();
    	    tmp[i]->sortCards();
        	if( tmp[i]->geklopft() && tmp[i]->rtti() != Player::HUMAN ) 
        	{
            	m_canvas->information( i18n("%1 has doubled.").arg( tmp[i]->name() ) );
            	emit signalDoubled();
        	}
			if(terminated)
				return;
		}
    
    	m_canvas->redrawPlayers();
    
    	// find a player you can playercards
    	// and setup m_gameinfo    
		if( !setupGameInfo(tmp) )
			continue;
        for(i=0;i<PLAYERS;i++)
		{
			tmp[i]->sortCards();
			tmp[i]->init();
		}
    	m_canvas->redrawPlayers();
    
    	for(i=0;i<TURNS ;i++)
	    {
    	    m_currstich.clear();
           
        	for(a=0;a<PLAYERS;a++) 
        	{
            	if(tmp[a])
					c = tmp[a]->play();
	    		if(terminated || c==NULL)
					return;
			
	    		for(unsigned int z=0;z<tmp[a]->cards()->count();z++) 
	    		{
                	if(tmp[a]->cards()->at(z) == c) 
                	{
                    	tmp[a]->cards()->take(z);
                    	break;
                	}
	    		}
            
       	    	m_currstich.append(c);
            	emit playerPlayedCard(tmp[a]->id(),c);
            	timer.block( 1 );
				if( terminated )
					return;
        	}
			index = highestCard();
    	    tmp[index]->addStich( m_currstich );
        	emit playerMadeStich(tmp[index]->id());
        	// Sortiere so, das der stecher n�hste karte spielt 
			for(realindex=0;realindex<PLAYERS;realindex++)
				if(m_players[realindex]==tmp[index])
					break;
        	for(a=0;a<PLAYERS;a++)
				tmp[a]=m_players[(a+realindex)%PLAYERS];
    	}
		
		if(!terminated)
			gameResults();
		gamecnt++;
    }
}

/*const*/ CardList *Game::currStich() //const
{
    return &m_currstich;
}

void Game::endGame(void)
{
	terminated=true;
	emit gameEnded();
	EXIT_LOOP();
}

/*const*/ GameInfo *Game::gameInfo() //const
{
    return &m_gameinfo;
}

void Game::setCanvas( GameCanvas* c )
{
    m_canvas = c;
}

Player* Game::findId( unsigned int id ) const
{     
    for( unsigned int i = 0; i < PLAYERS; i++)
        if( id == m_players[i]->id() )
            return m_players[i];
    return 0;
}

Player* Game::findIndex( unsigned int index ) const
{
    return ( index < PLAYERS ? m_players[index] : 0 );
}

int Game::highestCard( CardList* list )
{
    if( !list )
        list = &m_currstich;
        
	Card* high = list->first();
    Card* card = list->first();
	int i = 0;
	
    while( (card = list->next() ) )
    {
        if( isHigher( card, high ) )
        {
            high = card;
            continue;
        }
    }
    
    for( ; i < (signed int)list->count(); i++ )
        if( list->at(i) == high )
            break;
            
    return i;
}

bool Game::isHigher( Card* card, Card* high )
{
	int cardVal, highVal;
	int colcnt;
	
	cardVal=m_gameinfo.evalCard(card, &m_gameinfo);
	highVal=m_gameinfo.evalCard(high, &m_gameinfo);
	if(m_gameinfo.istTrumpf(card) || m_gameinfo.istTrumpf(high))
		return highVal < cardVal;
	else
	{
		switch( m_gameinfo.mode() )
		{
			case GameInfo::GEIER:
			case GameInfo::WENZ:
				colcnt=7;
				break;
			case GameInfo::RAMSCH:
			case GameInfo::RUFSPIEL:
			case GameInfo::STICHT:
			case GameInfo::DACHS:
			default:
				colcnt=6;
				break;
		}
		if((highVal-1)/colcnt==(cardVal-1)/colcnt)
			return highVal < cardVal;
	}
	return false;
}

void Game::gameResults()
{
    Results* r = Settings::instance()->results();
    r->setLaufende( m_laufende );
    r->setGameInfo( &m_gameinfo );
    for( unsigned int i=0;i<PLAYERS;i++)
        emit playerResult( m_players[i]->name(), r->formatedPoints(m_players[i])  );
    m_canvas->information( r->result() );
    delete r;
}

bool Game::setupGameInfo(Player *players[])
{
    // list of games the players want to playercards
    // maximum 4 entries
    QPtrList<GameInfo> games;
    games.setAutoDelete( true );
    unsigned int i = 0;
	
    for( i=0;i<PLAYERS;i++)
    {
        GameInfo* info = players[i]->gameInfo();
		if(terminated)
			return false;
		if( info )
        {
			info->setSpieler( players[i] );
            games.append( info );
            if( players[i]->rtti() != Player::HUMAN )
                m_canvas->information( i18n("%1 has a game.").arg( players[i]->name() ) );
        }
        else
        {
            if( players[i]->rtti() != Player::HUMAN )
                m_canvas->information( i18n("%1 has no game.").arg( players[i]->name() ) );
        }
    }
    
	if(terminated)
		return false;
    if( games.isEmpty() )
    {
        m_canvas->information( i18n("No one wants to play. Cards will thrown together.") );
        m_gameinfo.setValid( false );
        emit signalSetupGameInfo();
        return false;
    } 
    else
    {
        // find the highest game in the list
        GameInfo* best = games.first();
        for( i=0;i<games.count();i++ )        
            if( *games.at( i ) > *best )
                best = games.at( i );
        
        m_gameinfo = *best;
    }
    
    // finde den mitspieler:
    if( m_gameinfo.mode()==GameInfo::RUFSPIEL ) 
    {
        Card sau( Card::SAU, static_cast<enum Card::color>(m_gameinfo.color()) );
        for( i=0;i<PLAYERS || !m_gameinfo.mitspieler();i++ )
        {
            for( unsigned int z=0;z<CARD_CNT/PLAYERS;z++ )
                if( players[i]->cards()->at(z)->isEqual( &sau ) )
                {
                    m_gameinfo.setMitspieler(players[i]);
                    break;
                }
        }
    }
    
    m_laufende = m_gameinfo.laufende();
    m_canvas->information( m_gameinfo.toString() );

    m_gameinfo.setValid( true );
    emit signalSetupGameInfo();
    return true;
}

void Game::resetGameResults()
{
    int i;
    // reset points (i.e. results) from a previous game
    for(i=0;i<PLAYERS;i++)
        m_players[i]->setPoints( 0.0 );    
}

int Game::timesDoubled()
{
    int i, d = 0;
    
    for( i=0;i<PLAYERS;i++)
        if( m_players[i]->geklopft() )
            d++;
            
    return d;
}


#include "game.moc"
