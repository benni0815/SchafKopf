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
    terminated = false;
    CardList *playercards[PLAYERS];

    terminated=false;
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

void Game::gameLoop()
{
    start();

    int i, a, index;
    Player *tmp[PLAYERS];
	Card *c;
    Timer timer;
    
    // cards should be given to the player now
    // it's time to ask wether he wants to double
    // or not
    for(i=0;i<PLAYERS && !terminated;i++)
    {
        m_players[i]->klopfen();
        if( m_players[i]->geklopft() )   
            m_canvas->information( i18n("%1 has doubled.").arg( m_players[i]->name() ) );
    }
    
    m_canvas->redrawPlayers();
    
    // find a player you can playercards
    // and setup m_gameinfo    
    if( !setupGameInfo() )
        gameLoop();
    
    for(i=0;i<PLAYERS && !terminated;i++)
		m_players[i]->sortCards();
    m_canvas->redrawPlayers();
    
    for(i=0;i<TURNS && !terminated ;i++)
    {
        m_currstich.clear();
           
        for(a=0;a<PLAYERS;a++) 
        {
            if(m_players[a])
				c = m_players[a]->play();
			if(terminated || c==NULL)
				return;
			
			for(unsigned int z=0;z<m_players[a]->cards()->count();z++) 
			{
                if(m_players[a]->cards()->at(z) == c) 
                {
                    m_players[a]->cards()->take(z);
                    break;
                }
			}
            
            m_currstich.append(c);
            emit playerPlayedCard(m_players[a]->id(),c);
            timer.block( 1 );
        }
        
        index = highestCard();
        if( terminated )
			return;
		m_players[index]->addStich( m_currstich );
        emit playerMadeStich(m_players[index]->id());
        // Sortiere so, das der stecher n�hste karte spielt 
        for(a=0;a<PLAYERS;a++)
            tmp[a]=m_players[a];
        for(a=0;a<PLAYERS;a++)
            m_players[a]=tmp[(a+index)%PLAYERS];
    }
    
    if( !terminated )
    { 
        gameResults();
        gameLoop();
    }
}

const CardList *Game::currStich() const
{
    return &m_currstich;
}

void Game::endGame(void)
{
	terminated=true;
    emit gameEnded();
    
	EXIT_LOOP();
}

const GameInfo *Game::gameInfo() const
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

int Game::highestCard()
{
    Card* high = m_currstich.first();
    Card* card = m_currstich.first();
    while( (card = m_currstich.next() ) )
    {
        if( isHigher( card, high ) )
        {
            high = card;
            continue;
        }
    }
    
    int i = 0;
    for( ; i < (signed int)m_currstich.count(); i++ )
        if( m_currstich.at(i) == high )
            break;
            
    return i;
}

bool Game::isHigher( Card* card, Card* high )
{
    if( m_gameinfo.istTrumpf( card ) && !m_gameinfo.istTrumpf( high ) )
        return true;
    else if( m_gameinfo.istTrumpf( card ) && m_gameinfo.istTrumpf( high ) )
    {
        if( card->card() == high->card()  )
            return (card->color() < high->color());
        else if( card->card() != high->card() )
        {
            switch( m_gameinfo.mode() )
            {
                case GameInfo::RAMSCH:
                case GameInfo::RUFSPIEL:
                case GameInfo::STICHT:
                    if( card->card() == Card::OBER )
                        return true;
                    else if( high->card() == Card::OBER )
                        return false;
                    else if( card->card() == Card::UNTER )
                        return !(high->card() == Card::OBER);
                    else if( high->card() == Card::UNTER )        
                        return !(card->card() == Card::OBER );
                    break;
                case GameInfo::GEIER:
                    if( card->card() == Card::OBER )
                        return true;
                    else if( high->card() == Card::OBER )
                        return false;
                case GameInfo::WENZ:
                    if( card->card() == Card::UNTER )
                        return true;
                    else if( high->card() == Card::UNTER )
                        return false;
                case GameInfo::DACHS:
                    if( card->card() == Card::SAU )
                        return true;
                    else if( high->card() == Card::SAU )
                        return false;
                default:
                    break;
            }
            
            return (*card < high);
        }
    }
    else if( !m_gameinfo.istTrumpf( card ) && !m_gameinfo.istTrumpf( high ) ) // beide kein trumpf
    {
        // die farbe ist anders als die farbe der ersten/hoechsten 
        // karte aber diese karte is kein trumfh, kann
        // also ned stechen
        if( card->color() == high->color() && *card < high )
            return true;
    }

    return false;

//	return evalCard(card, &m_gameinfo) > evalCard(high, &m_gameinfo);
}

void Game::gameResults()
{
    Results* r = Settings::instance()->results();
    r->setLaufende( m_laufende );
    r->setGameInfo( &m_gameinfo );
    for( unsigned int i=0;i<PLAYERS;i++)
        emit playerResult( m_players[i]->name(), r->formatedPoints(m_players[i])  );
    
    m_canvas->information( r->result() );
    // TODO:
    // BIG TODO:
    // IT CRASHES WITH THIS LINE UNCOMMENTED
    // BUT, WE HAVE TO DELETE r, OTHERWISE WE HAVE A MEMORY LEAK!
    //delete r;
}

bool Game::setupGameInfo()
{
    // list of games the players want to playercards
    // maximum 4 entries
    QPtrList<GameInfo> games;
    games.setAutoDelete( true );
    
    unsigned int i = 0;
    for( i=0;i<PLAYERS;i++)
    {
        GameInfo* info = m_players[i]->gameInfo();
        if( info )
        {
            info->setSpieler( m_players[i] );
            games.append( info );
            m_canvas->information( i18n("%1 has a game.").arg( m_players[i]->name() ) );
        } else
            m_canvas->information( i18n("%1 has no game.").arg( m_players[i]->name() ) );
    }
    
    if( games.isEmpty() )
    {
        m_canvas->information( i18n("No one wants to play. Cards will thrown together.") );
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
                if( m_players[i]->cards()->at(z)->isEqual( &sau ) )
                {
                    m_gameinfo.setMitspieler(m_players[i]);
                    break;
                }
        }
    }
    
    m_laufende = m_gameinfo.laufende();
    m_canvas->information( m_gameinfo.toString() );
    return true;
}

#include "game.moc"
