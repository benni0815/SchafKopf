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
#include "gamecanvas.h"

#include "card.h"
#include "canvascard.h"
#include "game.h"
#include "player.h"
#include "settings.h"

/** Uncomment this line to make all cards
  * user visible, this is useful for debuggin
  * and improving the AI.
  * But it might cause problems in network mode for cheaters!!
  */
#define CHEAT

#ifdef CHEAT
    #warning "CHEATING ENABLED!!!"
#endif

#define NUMCARDS 8
// DISTANCE from the border
#define DIST 20

GameCanvas::GameCanvas(QCanvas* c, QWidget *parent, const char *name)
 : QCanvasView(c,parent, name)
{
    m_game = NULL;
    m_items[0] = NULL;
    m_item = NULL;
    
    canvas()->setBackgroundColor( Qt::darkGreen );
    update();
    
    connect( Settings::instance(), SIGNAL(cardChanged()), this, SLOT(redrawAll()));
    connect( Settings::instance(), SIGNAL(cardChanged()), this, SLOT(positionObjects()));
    connect( this, SIGNAL(clicked( QCanvasItem* )), this, SLOT(cardClicked(QCanvasItem*)));
}
    

GameCanvas::~GameCanvas()
{
    clearObjects();
}

void GameCanvas::clearObjects()
{
    if( !m_items[0] )
        return;
        
    for( unsigned int i = 0; i < PLAYERS; i++ ) {
        for( unsigned int z = 0; z < m_items[i]->count(); z++ )
            delete (*m_items[i])[z];
            
        delete m_items[i];
        m_items[i] = NULL;
    }
    
    redrawAll();
}

void GameCanvas::setGame( Game* game )
{
    m_game = game;
    
    if( m_game == NULL )
        clearObjects();
    else
        m_game->setCanvas( this );
    
    createObjects();
}

void GameCanvas::createObjects()
{
    if( !m_game )
        return;
        
    for( unsigned int i = 0; i < PLAYERS; i++ ) 
        m_items[i] = new QCanvasItemList();
    
    for( unsigned int i = 0; i < PLAYERS; i++ ) {
        Player* player = m_game->findIndex( i );
        for( unsigned int z = 0; z < player->cards()->count(); z++ ) {
            CanvasCard *c = new CanvasCard( player->cards()->at(z), canvas() );
            c->setZ( double(-1 - z) );
            
            if(i==1)
                c->setRotation(90);
            else if(i==3)
                c->setRotation(270);
#ifdef CHEAT
            c->setFrontVisible( true );
#else            
            c->setFrontVisible( player->rtti() == Player::HUMAN );
#endif
            m_items[i]->append( c );
            
        }
    }
    
    positionObjects();
}

void GameCanvas::positionObjects()
{
    if( !m_items[0] || !m_game )
        return;

    int w = canvas()->width()-DIST;
    int h = canvas()->height()-DIST;
    int cardw = Card::backgroundPixmap()->width();
    int cardh = Card::backgroundPixmap()->height();
    
    for( unsigned int i = 0; i < PLAYERS; i++ ) {
        int x = 0, y = 0;
        QCanvasItemList* list = m_items[i];
    
        if(i==1||i==3)
            qSwap( cardw, cardh );
        
        switch( i ) {
            case 0:
                x=(w-cardw*NUMCARDS)/2;
                y=h-cardh; 
                break;
            case 1:
                x=w-cardw;
                y=(h-((cardh/2)*(NUMCARDS-1)+cardh))/2; 
                break;
            case 2: 
                x=(w-((cardw/2)*(NUMCARDS-1)+cardw))/2;
                y=DIST;
                break;
            case 3:
            default:
                x=DIST; 
                y=(h-((cardh/2)*(NUMCARDS-1)+cardh))/2; 
                break;
        }

        for( unsigned int z = 0; z < list->count(); z++ ) {
            CanvasCard* card = static_cast<CanvasCard*>((*list)[z]); 
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
    
    redrawAll();
}

void GameCanvas::cardClicked( QCanvasItem* item )
{
    if( item->rtti() == CANVASCARD ) 
    {
        CanvasCard* card = static_cast<CanvasCard*>(item);
        
        for( unsigned int i = 0; i < PLAYERS; i++ ) 
        {
            Player* player = m_game->findIndex( i );
            if( player->rtti() == Player::HUMAN ) 
            {
                for( unsigned int z = 0; z < player->cards()->count(); z++ ) 
                    if( player->cards()->at( z ) == card->card() ) {
                        emit playCard( card->card() );
                        break;
                    }
                break;
            }
        }
    }
}

void GameCanvas::resizeEvent( QResizeEvent * r )
{
// TODO: remove this constants...
    canvas()->resize( this->width() -20, this->height()-20 );
    QCanvasView::resizeEvent( r );
    
    positionObjects();
}

void GameCanvas::redrawAll()
{
    canvas()->setAllChanged();
    canvas()->update();
}

void GameCanvas::contentsMousePressEvent(QMouseEvent* e)
{
    m_item = NULL;
    if( e->button() == Qt::LeftButton ) {
        QCanvasItemList list = canvas()->allItems();
        for( unsigned int i = 0; i < list.count(); i++ )
            if( list[i]->boundingRect().contains( e->pos() ) )
                m_item = list[i];
    }
}

void GameCanvas::contentsMouseReleaseEvent(QMouseEvent* e)
{
    if( e->button() == Qt::LeftButton && m_item ) {
        emit clicked( m_item );
        m_item = NULL;
    }
}
#include "gamecanvas.moc"
