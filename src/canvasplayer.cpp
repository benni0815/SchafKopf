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
#include "settings.h"

#include <qcanvas.h>
#include <qfont.h>
#include <qtimer.h>
#include <qsignal.h>
#include <kpassivepopup.h>
#include <kapplication.h>
#include <qlabel.h>
#include <qwidget.h>

CanvasPlayer::CanvasPlayer( int i, QCanvas* canvas, QCanvasView* view )
    : m_canvas( canvas )
{
    create();
    m_view = view;
    m_position = i;
    // TODO: get a correct id! especially important for networking!
    m_id = i;
    pop = NULL;

#ifndef SIMULATION_MODE
    // TODO: This should be passed as flag depending on Player::isHuman();
    m_is_human = (m_id == 0);
#else
    m_is_human = false;
#endif // SIMULATION_MODE

}

CanvasPlayer::~CanvasPlayer()
{
    unsigned int i = 0;
    // delete m_items before m_cards
    // as m_items has references to cards in
    // m_cards.
    for(i=0;i<NUMCARDS;i++)
        delete m_items[i];
    
    delete m_cards;
    delete m_name;
}

void CanvasPlayer::create()
{
    unsigned int z = 0;
    for(z=0;z<NUMCARDS;z++)
        m_items[z] = new CanvasCard( m_canvas );

    m_cards = new CardList;
    m_cards->setAutoDelete( true );
    
    m_name = new QCanvasText( m_canvas );
    m_name->setColor( Qt::white );
    m_name->setFont( QFont( "Helvetica", 24 ) );
    m_name->hide();
    
    m_is_last = false;
    m_has_doubled = false;
    m_player_name = QString::null;
}

void CanvasPlayer::position()
{
    int x = 0, y = 0;
    int num = 0;
    int w = m_canvas->width();//-DIST;
    int h = m_canvas->height();//-DIST;
    int offsetl = 0; //Hiermit kann man am linken Rand Platz schaffen fr z.B. ein Bild des Spielers (fr Netzwerkmodus).
    int availw = m_canvas->width() - 2*DIST - offsetl;
    int cardw = Card::backgroundPixmap()->width();
    int cardh = Card::backgroundPixmap()->height();
    
    for( unsigned int z = 0; z < NUMCARDS; z++ )
    {
        CanvasCard* card = m_items[z];
        if(card->isVisible())
            num++;
    }
    
    if(!Settings::instance()->rearrangeCards())
    	num=NUMCARDS;
    
    if(m_position==1||m_position==3)
        qSwap( cardw, cardh );
        
    switch( m_position ) 
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
        if(card->isVisible() || !Settings::instance()->rearrangeCards())
	{
	if( x != card->x() || y != card->y() )
	if(num==NUMCARDS)
            card->move( x, y );
	else
	{
	    card->setDestination( x, y );
	    card->animatedMove();
	} 
        if(m_position==0)
	{
	    if(availw>num*cardw+(num-1))
            	x += cardw+1;
	    else
	    	x += (availw-cardw)/(num-1);
	}
        else if(m_position==2)
            x += (cardw/6);
        else
            y += (cardh/6);
	}
    }
        
    // swap them back
    if(m_position==1||m_position==3)
        qSwap( cardw, cardh );
}

void CanvasPlayer::init()
{
    m_name->setText( m_player_name );

    if( m_cards->count() )
    {
        for( unsigned int z = 0; z < m_cards->count(); z++ ) 
        {
            CanvasCard *c = m_items[z];
            c->setCard( m_cards->at( z ) );
            c->setZ( double(-1 - z) );
            c->show();
            
            if(m_position==1)
                c->setRotation(270);
            else if(m_position==3)
                c->setRotation(90);
    #ifdef CHEAT
            c->setFrontVisible( true );
    #else            
            if( m_is_human )
            {
                if( m_has_doubled )
                    c->setFrontVisible( true );
                else
                {
                    if( m_is_last )
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

CanvasCard* CanvasPlayer::canvasCard( int index )
{
    if( index < 0 || index >= NUMCARDS )
        return NULL;
        
    CanvasCard* card = m_items[index];
    if( card->isVisible() )
        return card;
    else 
        return NULL;
}

CanvasCard* CanvasPlayer::hasCard( int cardid ) const
{
    for(unsigned int i=0;i<NUMCARDS;i++)
    {
        CanvasCard* card = m_items[i];
        if(card->card()->id() == cardid )
            return card;
    }
    return NULL;
}
        
void CanvasPlayer::cardPlayed( Card* c )
{
    CanvasCard* card = hasCard( c->id() );
    if( card )
    {
        card->hide();
    }
}

void CanvasPlayer::setCards( CardList* cards )
{
    int i;
    for( i = 0; i < NUMCARDS; i++ ) 
    {
        m_items[i]->setCard( NULL );
    }

    m_cards->clear();
    m_cards->appendList( cards );
}

void CanvasPlayer::say( const QString & message, unsigned int playerid )
{
    QPoint p;
    pop = new KPassivePopup( KApplication::kApplication()->mainWidget() );
    QLabel *Text = new QLabel( message, pop );
    pop->setView( Text );
    pop->setTimeout( 0 );
    pop->show();
    switch(m_position)
    {
    case 1:
		p=m_view->mapToGlobal(QPoint(m_name->x(), m_name->y()-40 ));
    		break;
    case 2:
    		p=m_view->mapToGlobal(QPoint(m_name->x()+m_name->boundingRect().width()/2-pop->width()/2, m_name->y()+40 ));
    		break;
    case 3:
    default:
		p=m_view->mapToGlobal(QPoint(m_name->x(), m_name->y()-40 ));
    		break;
    }
    pop->setGeometry ( QRect ( p, QPoint( 0,0 ) ) );
}

void CanvasPlayer::hideBubble()
{
	if(pop)
		pop->hide();
}

void CanvasPlayer::setHasDoubled( bool h )
{
    m_has_doubled = h;
}

void CanvasPlayer::setLast( bool l )
{
    m_is_last = l;
}

void CanvasPlayer::setName( const QString & name )
{
    m_player_name = name;
}

bool CanvasPlayer::isHuman() const
{
    return m_is_human;
}

unsigned int CanvasPlayer::id() const
{
    return m_id;
}

MyKPassivePopup::MyKPassivePopup( QWidget *parent, const char *name, WFlags f ) : KPassivePopup(parent, name, f)
{
}

void MyKPassivePopup::myShow()
{
	show();
	moveNear( QRect(QPoint(300,300), QPoint(50,50)) );
}

