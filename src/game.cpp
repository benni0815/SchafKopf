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

#include <kmessagebox.h>
#include <string.h>

Game::Game(QObject *parent, const char *name)
 : QObject(parent, name)
{
    m_canvas = NULL;
    m_players[0] = NULL;
    
    start();
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

void Game::start()
{
    unsigned int i = 0;
    CardList *playercards[PLAYERS];

    terminated=false;
    m_allcards.randomize();
    for( i=0; i<PLAYERS; i++)
        playercards[i]=new CardList();
    for( i=0; i<CARD_CNT; i++)
        playercards[i%PLAYERS]->append(m_allcards.at(i));

    if( m_players[0] == NULL )
    {
        QStringList list = Settings::instance()->playerNames();
        m_players[0] = new HumanPlayer(playercards[0] ,this );
        m_players[0]->setName( list[0] );
        for( i=1;i<PLAYERS;i++)
        {
            m_players[i] = new ComputerPlayer(playercards[i] ,this );
            m_players[i]->setName( list[i] );
        }
    }
    else
    {
        for( i=0;i<PLAYERS;i++)
        {
            m_players[i]->setCards( playercards[i] );
            m_players[i]->stiche()->clear();
        }
    }
    
    // TODO:
    // correct sorting, next player to the player who started the last game
    // has to start this game!
    
    if( m_canvas )
    {
        m_canvas->setGame( NULL );
        m_canvas->setGame( this );    
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
    
	for(i=0;i<PLAYERS;i++)
		m_players[i]->init();
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
            //timer.block( 1 );
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
    { 
        gameResults();
        start();
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

int Game::evalCard(Card *card, GameInfo *gameinfo)
{
	/* Not very nice. Try to find a better way */
	
	int *l_trumpf;
	int *l_cards;
	int l_trumpf_std[]={ Card::OBER, Card::UNTER };
	int l_trumpf_geier=Card::OBER;
	int l_trumpf_wenz=Card::UNTER;
	int l_cards_std[]={ Card::SAU, Card::ZEHN, Card::KOENIG, Card::NEUN, Card::ACHT, Card::SIEBEN, Card::NOSTICH };
	int l_cards_geier[]={ Card::SAU, Card::ZEHN, Card::KOENIG, Card::UNTER, Card::NEUN, Card::ACHT, Card::SIEBEN };
	int l_cards_wenz[]={ Card::SAU, Card::ZEHN, Card::KOENIG, Card::OBER, Card::NEUN, Card::ACHT, Card::SIEBEN };
	int l_colors[4];
	int trumpf_index=-1;
	int cards_index=-1;
	int colors_index=-1;
	int trumpf_cnt;
	int col;
	int i, a;
	
	switch( gameinfo->mode() )
    {
		case GameInfo::RAMSCH:
        case GameInfo::RUFSPIEL:
			trumpf_cnt=2;
			l_trumpf=l_trumpf_std;
			l_cards=l_cards_std;
			col=Card::HERZ;
			
			break;
        case GameInfo::STICHT:
			trumpf_cnt=2;
			l_trumpf=l_trumpf_std;
			l_cards=l_cards_std;
			col=gameinfo->color();
			break;
		case GameInfo::GEIER:
			trumpf_cnt=1;
        	l_trumpf=&l_trumpf_geier;
			l_cards=l_cards_geier;
			col=gameinfo->color();
			break;
		case GameInfo::WENZ:
			trumpf_cnt=1;
        	l_trumpf=&l_trumpf_wenz;
			l_cards=l_cards_wenz;
			col=gameinfo->color();
		default:
        	break;
	}
	l_colors[0]=col;
	for(i=0, a=1;i<4;i++)
	{
		if(col==i)
			continue;
		else
			l_colors[a++]=i;
	}
	for(i=0;i<2;i++)
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
		return 32-(trumpf_index*4+card->color());
	return 32-(trumpf_cnt*4+colors_index*(8-trumpf_cnt)+cards_index);
}

bool Game::isHigher( Card* card, Card* high )
{
/*
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
*/
	return evalCard(card, &m_gameinfo) > evalCard(high, &m_gameinfo);
}

void Game::gameResults()
{
    Results* r = Settings::instance()->results();
    r->setGameInfo( &m_gameinfo );
    for( unsigned int i=0;i<PLAYERS;i++)
        emit playerResult( m_players[i]->name(), r->formatedPoints(m_players[i])  );
    
    KMessageBox::information( 0, r->result() );
    // TODO:
    // BIG TODO:
    // IT CRASHES WITH THIS LINE UNCOMMENTED
    // BUT, WE HAVE TO DELETE r, OTHERWISE WE HAVE A MEMORY LEAK!
    //delete r;
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
        {
            info->setSpieler( m_players[i] );
            games.append( info );
        }
    }
    
    if( games.isEmpty() )
    {
        // nobody wants to play
        // TODO: future: Ramsch, zamschmeissen, etc.
        m_gameinfo.setColor( Card::EICHEL );
        m_gameinfo.setMode( GameInfo::RUFSPIEL );
        m_gameinfo.setSpieler( m_players[0] );
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

    KMessageBox::information( 0, m_gameinfo.toString() );
}

#include "game.moc"
