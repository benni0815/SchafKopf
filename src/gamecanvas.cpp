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
    m_names[0] = NULL;
    m_stich = NULL;
    
    canvas()->setBackgroundColor( Qt::darkGreen );
    canvas()->setAdvancePeriod( 200 );
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
    if( m_items[0] )
    {       
        for( unsigned int i = 0; i < PLAYERS; i++ ) {
            for( unsigned int z = 0; z < m_items[i]->count(); z++ )
                delete (*m_items[i])[z];
            
            delete m_items[i];
            m_items[i] = NULL;
        }
    }
    
    if( m_names[0] )
    {
        for( unsigned int i = 0; i < PLAYERS; i++ )
        {
            delete m_names[i];
            m_names[i] = NULL;
        }
    }
    
    if( m_stich )
    {
        for( unsigned int i = 0; i < m_stich->count(); i++ )
            delete (*m_stich)[i];
            
        delete m_stich;
        m_stich = NULL;
    }
    
    redrawAll();
}

void GameCanvas::setGame( Game* game )
{
    m_game = game;
    
    if( m_game == NULL )
        clearObjects();
    else {
        m_game->setCanvas( this );
        connect( m_game, SIGNAL(playerPlayedCard(unsigned int,Card*)), this,SLOT(slotPlayerPlayedCard(unsigned int,Card*)));       
        connect( m_game, SIGNAL(playerMadeStich(unsigned int)), this,SLOT(slotPlayerMadeStich(unsigned int)));
        connect( m_game, SIGNAL(clearStich()),this,SLOT(slotClearStich()));
    }
    
    createObjects();
}

void GameCanvas::createObjects()
{
    if( !m_game )
        return;
        
    for( unsigned int i = 0; i < PLAYERS; i++ ) 
    {
        m_items[i] = new QCanvasItemList();
        m_names[i] = new QCanvasText( canvas() );
        m_names[i]->setColor( Qt::white );
        m_names[i]->setFont( QFont( "Helvetica", 24 ) );
        m_names[i]->show();
    }
    
    for( unsigned int i = 0; i < PLAYERS; i++ ) {
        Player* player = m_game->findIndex( i );
        m_names[i]->setText( player->name() );
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
    
    m_stich = new QCanvasItemList();
    
    positionObjects();
}

void GameCanvas::positionObjects()
{
    if( !m_items[0] || !m_game || !m_stich )
        return;

    int w = canvas()->width()-DIST;
    int h = canvas()->height()-DIST;
    int cardw = Card::backgroundPixmap()->width();
    int cardh = Card::backgroundPixmap()->height();
    
    for( unsigned int i = 0; i < PLAYERS; i++ ) {
        int x = 0, y = 0;
        int num=m_items[i]->count();
        QCanvasItemList* list = m_items[i];
    
        if(i==1||i==3)
            qSwap( cardw, cardh );
        
        switch( i ) {
            case 0:
                x=(w-cardw*num)/2;
                y=h-cardh; 
                
                m_names[i]->move( (w-m_names[i]->boundingRect().width())/2, y-m_names[i]->boundingRect().height() );
                break;
            case 1:
                x=w-cardw;
                y=(h-((cardh/2)*(num-1)+cardh))/2; 
                
                m_names[i]->move(x, y-m_names[i]->boundingRect().height());
                break;
            case 2: 
                x=(w-((cardw/2)*(num-1)+cardw))/2;
                y=DIST;
                
                m_names[i]->move( (w-m_names[i]->boundingRect().width())/2, y+cardh );

                break;
            case 3:
            default:
                x=DIST; 
                y=(h-((cardh/2)*(num-1)+cardh))/2; 
                
                m_names[i]->move(x,y-m_names[i]->boundingRect().height());
                break;
        }
        
       for( unsigned int z = 0; z < list->count(); z++ ) {
            CanvasCard* card = static_cast<CanvasCard*>((*list)[z]); 
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
    
    for( unsigned int i = 0; i < m_stich->count(); i++ ) 
    {
        CanvasCard* card = static_cast<CanvasCard*>((*m_stich)[i]);
        QPoint p = getStichPosition(i);
        card->move( (int)p.x(), (int)p.y() );
    }
    
    redrawAll();        
}

QPoint GameCanvas::getStichPosition( int player )
{
    QPoint p;
    int w = canvas()->width()-DIST;
    int h = canvas()->height()-DIST;
    int cardw = Card::backgroundPixmap()->width();
    int cardh = Card::backgroundPixmap()->height();
    int cx = w/2;
    int cy = h/2;
    
    switch( player ) 
    {
        case 0:
            p = QPoint(cx, cy+(cardh/2)); break;
        case 1:
            p = QPoint(cx+cardw, cy); break;
        case 2:
            p = QPoint(cx, cy-(cardh/2)); break;
        case 3:
        default:
            p = QPoint(cx-cardw, cy); break;
    };
        
    return p;
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

void GameCanvas::slotPlayerPlayedCard( unsigned int player, Card *c )
{
    if( !m_items[player] || !m_game || !m_stich )
        return;    
    
    for(unsigned int i=0;i<m_items[player]->count();i++)
    {
        CanvasCard* card = static_cast<CanvasCard*>((*m_items[player])[i]);
        if(card->card() == c)
        {
            m_items[player]->remove( card );
            m_stich->append( card );
            
            card->setRotation( 0 );
            card->setZ( player );
            
            card->moveTo( getStichPosition(player) );
            
            
            break;
        }
    }
}

void GameCanvas::slotPlayerMadeStich(unsigned int player)
{
    int x = 0, y = 0;
    int w = canvas()->width()-DIST;
    int h = canvas()->height()-DIST;
    int cardw = Card::backgroundPixmap()->width();
    int cardh = Card::backgroundPixmap()->height();
    switch( player ) {
        case 0:
            x=(w-cardw)/2;
            y=h-cardh; 
            break;
        case 1:
            x=w-cardw;
            y=(h-cardh)/2; 
            break;
        case 2: 
            x=(w-cardw)/2;
            y=DIST;
            break;
        case 3:
        default:
            x=DIST; 
            y=(h-cardh)/2; 
            break;
    }                
 
    for( unsigned int i=0; i < m_stich->count();i++) {
        QCanvasItem* item = (*m_stich)[i];
        CanvasCard* card = static_cast<CanvasCard*>(item);
        card->moveTo( QPoint(x, y) );
    }
}

void GameCanvas::slotClearStich()
{
    while( !m_stich->isEmpty() ) {
        QCanvasItem* item = m_stich->first();
        m_stich->remove( item );
        delete item;
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
