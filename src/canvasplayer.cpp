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

#include <qcanvas.h>
#include <qfont.h>

/** Uncomment this line to make all cards
  * user visible, this is useful for debuggin
  * and improving the AI.
  * But it might cause problems in network mode for cheaters!!
  */
#define CHEAT

#ifdef CHEAT
    #warning "CHEATING ENABLED!!!"
#endif

CanvasPlayer::CanvasPlayer( int i, Player* player, QCanvas* canvas )
    : m_player( player ), m_canvas( canvas )
{
	m_items=NULL;
	init(i);    
    m_name = new QCanvasText( m_canvas );
    m_name->setColor( Qt::white );
    m_name->setFont( QFont( "Helvetica", 24 ) );
    m_name->setText( player->name() );
    m_name->show();
}


CanvasPlayer::~CanvasPlayer()
{
    unsigned int i = 0;
    
    for(i=0;i<m_items->count();i++)
    {
        QCanvasItem* item = (*m_items)[i];
        delete item;
    }
    
    delete m_items;
    delete m_name;
}

void CanvasPlayer::position( int i )
{
    int x = 0, y = 0;
    int num=m_items->count();
    int w = m_canvas->width()-DIST;
    int h = m_canvas->height()-DIST;
    int cardw = Card::backgroundPixmap()->width();
    int cardh = Card::backgroundPixmap()->height();
        
    if(i==1||i==3)
        qSwap( cardw, cardh );
        
    switch( i ) {
        case 0:
            x=(w-cardw*num)/2;
            y=h-cardh; 
            
            m_name->move( (w-m_name->boundingRect().width())/2, y-m_name->boundingRect().height() );
            break;
        case 1:
            x=DIST; 
            y=(h-((cardh/2)*(num-1)+cardh))/2; 
            
            m_name->move(x,y-m_name->boundingRect().height());
            break;
        case 2: 
            x=(w-((cardw/2)*(num-1)+cardw))/2;
            y=DIST;
            
            m_name->move( (w-m_name->boundingRect().width())/2, y+cardh );

            break;
        case 3:
        default:
            x=w-cardw;
            y=(h-((cardh/2)*(num-1)+cardh))/2; 
            
            m_name->move(x, y-m_name->boundingRect().height());
            break;
        }
        
       for( unsigned int z = 0; z < m_items->count(); z++ ) {
            CanvasCard* card = static_cast<CanvasCard*>((*m_items)[z]); 
            // only move if necessary!
            if( x != card->x() || y != card->y() )
                card->move( x, y );
                
            if(i==0)
                x += cardw;
            else if(i==2)
                x += (cardw/2);
            else
                y += (cardh/2);            
        }
        
        // swap them back
        if(i==1||i==3)
            qSwap( cardw, cardh );
    
}

void CanvasPlayer::init(int i)
{
    unsigned int a = 0;
    
    if(m_items)
	{
		for(a=0;a<m_items->count();a++)
    	{
        	QCanvasItem* item = (*m_items)[a];
        	delete item;
	    }
		delete m_items;
	}
	m_items = new QCanvasItemList();
    for( unsigned int z = 0; z < m_player->cards()->count(); z++ ) 
	{
        CanvasCard *c = new CanvasCard( m_player->cards()->at(z), m_canvas );
        c->setZ( double(-1 - z) );
            
        if(i==1)
            c->setRotation(90);
        else if(i==3)
            c->setRotation(270);
#ifdef CHEAT
        c->setFrontVisible( true );
#else            
        c->setFrontVisible( m_player->rtti() == Player::HUMAN );
#endif
        m_items->append( c );
    }
}

CanvasCard* CanvasPlayer::hasCard( Card* c ) const
{
    for(unsigned int i=0;i<m_items->count();i++)
    {
        QCanvasItem* item = (*m_items)[i];
        CanvasCard* card = static_cast<CanvasCard*>(item);
        if(card->card() == c)
            return card;
    }
    return 0;
}
