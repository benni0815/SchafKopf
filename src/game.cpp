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
#include "settings.h"
#include "timer.h"

#include <setjmp.h>

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
    
    m_gameinfo.color=Card::EICHEL;
    m_gameinfo.mode=Game::RUFSPIEL;
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
    Timer timer;
        
    for(i=0;i<TURNS || terminated;i++)
    {
        m_currstich.clear();
        emit clearStich();
        
        for(a=0;a<PLAYERS;a++) 
        {
            Card *c = m_players[a]->play();
            for(unsigned int z=0;z<m_players[a]->cards()->count();z++) 
                if(m_players[a]->cards()->at(z) == c) 
                {
                    m_players[a]->cards()->take(z);
                    break;
                }
            
            m_currstich.append(c);
            emit playerPlayedCard(m_players[a]->id(),c);
            timer.block( 1 );
        }
        
        index = highestCard();
        m_players[index]->addStich( m_currstich );
        emit playerMadeStich(m_players[index]->id());
        // Sortiere so, das der stecher nächste karte spielt 
        for(a=0;a<PLAYERS;a++)
            tmp[a]=m_players[a];
        for(a=0;a<PLAYERS;a++)
            m_players[a]=tmp[(a+index)%PLAYERS];
    }
    
    m_currstich.clear();
    emit clearStich();
    
}

CardList *Game::currStich() const
{
    return &m_currstich;
}

void Game::endGame(void)
{
#if QT_VERSION >= 0x030100
//    while( kapp->eventLoop()->loopLevel() > 1 )
    if( kapp->eventLoop()->loopLevel() > 1 )
        kapp->eventLoop()->exitLoop();
#else
//    while( kapp->loopLevel() - 1)
    if( kapp->loopLevel() > 1 )
        kapp->exit_loop();
#endif
    terminated=true;
}

const Game::game_info *Game::gameInfo() const
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
        qDebug("BEIDE TRUMPF");
        if( card->card() == high->card() && card->color() < high->color() )
            return true;
        else 
        {
            bool higher = false;
            switch( m_gameinfo.mode )
            {
                case RAMSCH:
                case RUFSPIEL:
                case STICHT:
                    if( (card->card() == Card::OBER && high->card() != Card::OBER) ||
                        ( card->card() == Card::UNTER && high->card() != Card::UNTER ) )
                        higher = true;
                    break;
                case GEIER:
                    if( card->card() == Card::OBER && high->card() != Card::OBER )
                        higher = true; 
                    break;
                case WENZ:
                    if( card->card() == Card::UNTER && high->card() != Card::UNTER )
                        higher = true;
                    break;
                default:
                    break;
            }
            
            if( higher || (!higher && *card < high)) 
                return true;
        }
    }
    else if( !istTrumpf( card ) && !istTrumpf( high ) ) // beide kein trumpf
    {
        qDebug("KEIN TRUMPF");
        // die farbe ist anders als die farbe der ersten/hoechsten 
        // karte aber diese karte is kein trumfh, kann
        // also ned stechen
        if( card->color() != high->color() )
            return false;
                
        if( *card < high )
        {
            qDebug("---2. FARBE IST GLEICH" );
            return true;
        }
    }

    return false;
}

#include "game.moc"
