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
#include "canvasplayer.h"

#include "canvascard.h"
#include "card.h"
#include "cardlist.h"
#include "player.h"
#include "game.h"

#include <qcanvas.h>
#include <qfont.h>

CanvasPlayer::CanvasPlayer( QCanvas* canvas )
    :m_canvas( canvas )
{
    create();
}

CanvasPlayer::CanvasPlayer( int i, Player* player, QCanvas* canvas )
    : m_player( player ), m_canvas( canvas )
{
    create();
    
    setPlayer( i, player );
}

CanvasPlayer::~CanvasPlayer()
{
    unsigned int i = 0;
    for(i=0;i<NUMCARDS;i++)
        delete m_items[i];
    
    delete m_name;
}

void CanvasPlayer::create()
{
    unsigned int z = 0;
	for(z=0;z<NUMCARDS;z++)
	    m_items[z] = new CanvasCard( m_canvas );

    m_player = NULL;
    m_name = new QCanvasText( m_canvas );
    m_name->setColor( Qt::white );
    m_name->setFont( QFont( "Helvetica", 24 ) );
    m_name->hide();
}

void CanvasPlayer::setPlayer( int i, Player* player )
{
    m_player = player;
    
    if( m_player )
        init(i);    
}

void CanvasPlayer::position( int i )
{
    int x = 0, y = 0;
    int num=NUMCARDS;
    int w = m_canvas->width();//-DIST;
    int h = m_canvas->height();//-DIST;
    int offsetl = 0; //Hiermit kann man am linken Rand Platz schaffen fr z.B. ein Bild des Spielers (fr Netzwerkmodus).
    int availw = m_canvas->width() - 2*DIST - offsetl;
    int cardw = Card::backgroundPixmap()->width();
    int cardh = Card::backgroundPixmap()->height();
        
    if(i==1||i==3)
        qSwap( cardw, cardh );
        
    switch( i ) 
    {
        case 0:
            if(availw>num*cardw+(num-1))
	    	x=(w-cardw*num)/2-(num-1)/2-offsetl;
	    else
	    	x=DIST+offsetl;
            y=h-cardh-DIST; 
        
            m_name->move( (w-m_name->boundingRect().width())/2, y-m_name->boundingRect().height() );
            break;
        case 1:
            x=DIST; 
            y=(h-((cardh/4)*(num-1)+cardh))/2; 
        
            m_name->move(x,y-m_name->boundingRect().height());
            break;
        case 2: 
            x=(w-((cardw/4)*(num-1)+cardw))/2;
            y=DIST;
        
            m_name->move( (w-m_name->boundingRect().width())/2, y+cardh );
            break;
        case 3:
        default:
            x=w-cardw-DIST;
            y=(h-((cardh/4)*(num-1)+cardh))/2; 
        
            m_name->move(x, y-m_name->boundingRect().height());
            break;
    }
        
    for( unsigned int z = 0; z < NUMCARDS; z++ ) {
        CanvasCard* card = m_items[z];
        // only move if necessary!
        if( x != card->x() || y != card->y() )
            card->move( x, y );
                
        if(i==0)
	    if(availw>num*cardw+(num-1))
            	x += cardw+1;
	    else
	    	x += (availw-cardw)/(num-1);
        else if(i==2)
            x += (cardw/6);
        else
            y += (cardh/6);            
    }
        
    // swap them back
    if(i==1||i==3)
        qSwap( cardw, cardh );
}

void CanvasPlayer::init(int i)
{
    m_name->setText( m_player->name() );
    if( !m_player->game()->isTerminated() )
    {
        for( unsigned int z = 0; z < m_player->cards()->count(); z++ ) 
        {
            CanvasCard *c = m_items[z];
            c->setCard( m_player->cards()->at( z ) );
            c->setZ( double(-1 - z) );
            c->show();
            
            if(i==1)
                c->setRotation(270);
            else if(i==3)
                c->setRotation(90);
    #ifdef CHEAT
            c->setFrontVisible( true );
    #else            
            if( m_player->rtti() == Player::HUMAN )
            {
                if( m_player->hasDoubled() )
                    c->setFrontVisible( true );
                else
                {
                    if( m_player->isLast() )
                        c->setFrontVisible( z >= 4 );
                    else
                        c->setFrontVisible( z < 4 );
                }
            }
            else
                c->setFrontVisible( false );
    #endif
        }
        m_name->show();
    }
    else
    {
        for( unsigned int z = 0;z<NUMCARDS;z++)
            m_items[z]->hide();
            
        m_name->hide();
    }
}

CanvasCard* CanvasPlayer::hasCard( Card* c ) const
{
    for(unsigned int i=0;i<NUMCARDS;i++)
    {
        CanvasCard* card = m_items[i];
        if(card->card() == c)
            return card;
    }
    return 0;
}
        
void CanvasPlayer::cardPlayed( Card* c )
{
    CanvasCard* card = hasCard( c );
    if( card )
    {
        card->hide();
    }
}

