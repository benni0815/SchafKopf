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
#include "settings.h"
#include "timer.h"

#include <kmessagebox.h>

Game::Game(QObject *parent, const char *name)
 : QObject(parent, name)
{
    m_canvas = NULL;
    
    CardList *playercards[PLAYERS];
    
    terminated=false;
    m_allcards.randomize();
    for( unsigned int i=0; i<PLAYERS; i++)
        playercards[i]=new CardList();
    for( unsigned int i=0; i<CARD_CNT; i++)
        playercards[i%PLAYERS]->append(m_allcards.at(i));

    QStringList list = Settings::instance()->playerNames();
    m_players[0] = new HumanPlayer(playercards[0] ,this );
    m_players[0]->setName( list[0] );
    for( unsigned int i=1;i<PLAYERS;i++)
    {
        m_players[i] = new ComputerPlayer(playercards[i] ,this );
        m_players[i]->setName( list[i] );
    }
}


Game::~Game()
{
    int i;
    
	m_currstich.clear();
	for(i=0;i<PLAYERS;i++)
	{
        delete m_players[i];
		m_players[i]=NULL;
	}
}

void Game::gameLoop()
{
    int i, a, index;
    Player *tmp[PLAYERS];
	Card *c;
    Timer timer;
    
    // find a player you can playercards
    // and setup m_gameinfo    
    setupGameInfo();
    
    for(i=0;i<TURNS;i++)
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
			//sleep(1);
        }
        
        index = highestCard();
        if( terminated )
			return;
		m_players[index]->addStich( m_currstich );
        emit playerMadeStich(m_players[index]->id());
        // Sortiere so, das der stecher nächste karte spielt 
        for(a=0;a<PLAYERS;a++)
            tmp[a]=m_players[a];
        for(a=0;a<PLAYERS;a++)
            m_players[a]=tmp[(a+index)%PLAYERS];
    }
    
    if( !terminated )
        gameResults();
}

const CardList *Game::currStich() const
{
    return &m_currstich;
}

void Game::endGame(void)
{
	terminated=true;
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

bool Game::istTrumpf(Card *card)
{
    switch(m_gameinfo.mode)
    {
        case GameInfo::RUFSPIEL:
        case GameInfo::RAMSCH:
                if(card->card()==Card::OBER || card->card()==Card::UNTER || card->color()==Card::HERZ)
                    return true;
                break;
        case GameInfo::STICHT:
                if(card->card()==Card::OBER || card->card()==Card::UNTER || card->color()==m_gameinfo.color)
                    return true;
                break;
        case GameInfo::GEIER:
                if(card->card()==Card::OBER || card->color()==m_gameinfo.color)
                    return true;
                break;
        case GameInfo::WENZ:
                if(card->card()==Card::UNTER || card->color()==m_gameinfo.color)
                    return true;
        default:
                break;
    };
    return false;
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
    for( ; i < m_currstich.count(); i++ )
        if( m_currstich.at(i) == high )
            break;
            
    return i;
}

bool Game::isHigher( Card* card, Card* high )
{
    if( istTrumpf( card ) && !istTrumpf( high ) )
        return true;
    else if( istTrumpf( card ) && istTrumpf( high ) )
    {
        if( card->card() == high->card()  )
            return (card->color() < high->color());
        else if( card->card() != high->card() )
        {
            switch( m_gameinfo.mode )
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
                default:
                    break;
            }
            
            return (*card < high);
        }
    }
    else if( !istTrumpf( card ) && !istTrumpf( high ) ) // beide kein trumpf
    {
        // die farbe ist anders als die farbe der ersten/hoechsten 
        // karte aber diese karte is kein trumfh, kann
        // also ned stechen
        if( card->color() == high->color() && *card < high )
            return true;
    }

    return false;
}

void Game::gameResults()
{
    // TODO: handle schneider + schwarz ....
    // sauberen code...
    int points = m_gameinfo.spieler->stiche()->points();
    if( m_gameinfo.mitspieler )
        points += m_gameinfo.mitspieler->stiche()->points();
        
        
    if( points > 60 )
        KMessageBox::information( 0,m_gameinfo.spieler->name() + QString(" gewinnt mit %1 Punkten.").arg( points ) );
    else
        KMessageBox::information( 0,m_gameinfo.spieler->name() + QString(" verliert mit %1 Punkten.").arg( points ) );
}

void Game::setupGameInfo()
{
    // list of games the players want to playercards
    // maximum 4 entries
    QPtrList<GameInfo> games;
    games.setAutoDelete( true );
    
    unsigned int i = 0;
    for( i=0;i<PLAYERS;i++)
    {
        GameInfo* info = m_players[i]->game();
        if( info )
            games.append( info );
    }
    
    if( games.isEmpty() )
    {
        // nobody wants to play
        // TODO: future: Ramsch, zamschmeissen, etc.
        m_gameinfo.color=Card::EICHEL;
        m_gameinfo.mode=GameInfo::RUFSPIEL;
        m_gameinfo.spieler=m_players[0];
        m_gameinfo.mitspieler=0;
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
    if( m_gameinfo.mode==GameInfo::RUFSPIEL ) 
    {
        Card sau( Card::SAU, static_cast<enum Card::color>(m_gameinfo.color) );
        for( i=0;i<PLAYERS || !m_gameinfo.mitspieler;i++ )
        {
            for( unsigned int z=0;z<CARD_CNT/PLAYERS;z++ )
                if( m_players[i]->cards()->at(z)->isEqual( &sau ) )
                {
                    m_gameinfo.mitspieler=m_players[i];
                    break;
                }
        }
    }
}

#include "game.moc"
