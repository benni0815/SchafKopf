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

#include <kapplication.h>
#if QT_VERSION >= 0x030100
    #include <qeventloop.h>
#else
    #include <qapplication.h>
#endif

Game::Game(QObject *parent, const char *name)
 : QObject(parent, name)
{
    CardList *playercards[PLAYERS];
    
    terminated=false;
    m_allcards.randomize();
    for( unsigned int i=0; i<PLAYERS; i++)
        playercards[i]=new CardList();
    for( unsigned int i=0; i<CARD_CNT; i++)
        playercards[i%PLAYERS]->append(m_allcards.at(i));
        
    m_players[0] = new HumanPlayer(playercards[0] ,this );
    for( unsigned int i=1;i<PLAYERS;i++)
        m_players[i] = new ComputerPlayer(playercards[i] ,this );
    m_gameinfo.color=Card::HERZ;
    m_gameinfo.mode=Game::STICHT;
    m_gameinfo.spieler=m_players[0];
}


Game::~Game()
{
    int i;
    
    for(i=0;i<PLAYERS;i++)
        delete m_players[i];
}

void Game::gameLoop()
{
    int i, a, index;
    Player *tmp[PLAYERS];
        
    for(i=0;i<TURNS || terminated;i++)
    {
        m_currstich.clear();
        for(a=0;a<PLAYERS;a++) {
            m_currstich.append(m_players[a]->play());
            emit gameStateChanged();
        }
        
        index = highestCard();
        m_players[index]->addStich( m_currstich );
        // Sortiere so, das der stecher nächste karte spielt 
        for(a=0;a<PLAYERS;a++)
            tmp[a]=m_players[a];
        for(a=0;a<PLAYERS;a++)
            m_players[a]=tmp[(a+index)%PLAYERS];
    }
}

const CardList *Game::currStich() const
{
    return &m_currstich;
}

void Game::endGame(void)
{
    terminated=true;
#if QT_VERSION >= 0x030100
    while( kapp->eventLoop()->loopLevel() )
        kapp->eventLoop()->exitLoop();
#else
    while( kapp->loopLevel() )
        kapp->exit_loop();
#endif
}

const Game::game_info *Game::gameInfo() const
{
    return &m_gameinfo;
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
        case RUFSPIEL:
        case RAMSCH:
                if(card->card()==Card::OBER || card->card()==Card::UNTER || card->color()==Card::HERZ)
                    return true;
        case STICHT:
                if(card->card()==Card::OBER || card->card()==Card::UNTER || card->color()==m_gameinfo.color)
                    return true;
        case GEIER:
                if(card->card()==Card::OBER || card->color()==m_gameinfo.color)
                    return true;
        case WENZ:
                if(card->card()==Card::UNTER || card->color()==m_gameinfo.color)
                    return true;
        default:
                break;
    };
    return false;
}

int Game::highestCard()
{
    return 0;
}

#include "game.moc"
