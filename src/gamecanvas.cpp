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
#include "canvasplayer.h"
#include "game.h"
#include "player.h"
#include "settings.h"

#include <kapplication.h>
#include <qtimer.h>

#define NUMCARDS 8

GameCanvas::GameCanvas(QCanvas* c, QWidget *parent, const char *name)
 : QCanvasView(c,parent, name)
{
    unsigned int i = 0;
    for(;i<4;i++)
        m_stich[i]=new CanvasCard( c );
        
    m_game = NULL;
    m_item = NULL;
    m_players[0] = NULL;
    canvas()->setBackgroundColor( Qt::darkGreen );
    canvas()->setAdvancePeriod( 200 );
    update();
    
	connect( Settings::instance(), SIGNAL(cardChanged()), this, SLOT(redrawPlayers()));
    connect( Settings::instance(), SIGNAL(cardChanged()), this, SLOT(positionObjects()));
    connect( this, SIGNAL(clicked( QCanvasItem* )), this, SLOT(cardClicked(QCanvasItem*)));
}
    

GameCanvas::~GameCanvas()
{
    for( unsigned int i = 0; i < 4; i++ )
        delete m_stich[i];
    
    clearObjects();
}

void GameCanvas::clearObjects()
{
    if( m_players[0] ) 
    {
        for( unsigned int i = 0; i < PLAYERS; i++ )
            delete m_players[i];
        m_players[0] = NULL;
    }
}

void GameCanvas::setGame( Game* game )
{
    if( game == NULL )
	{
        if(m_game)
		{
			disconnect( m_game, SIGNAL(playerPlayedCard(unsigned int,Card*)), this,SLOT(slotPlayerPlayedCard(unsigned int,Card*)));       
        	disconnect( m_game, SIGNAL(playerMadeStich(unsigned int)), this,SLOT(slotPlayerMadeStich(unsigned int)));
			disconnect( game, SIGNAL(gameStateChanged()), this, SLOT(redrawPlayers()));
		}
		clearObjects();
		m_game=NULL;
	}
    else 
	{
        game->setCanvas( this );
        connect( game, SIGNAL(playerPlayedCard(unsigned int,Card*)), this,SLOT(slotPlayerPlayedCard(unsigned int,Card*)));       
        connect( game, SIGNAL(playerMadeStich(unsigned int)), this,SLOT(slotPlayerMadeStich(unsigned int)));
		connect( game, SIGNAL(gameStateChanged()), this, SLOT(redrawPlayers()));
		m_game = game;
		createObjects();
    }
}

void GameCanvas::cardForbidden(Card* card)
{
    for(unsigned int z=0;z<PLAYERS;z++)
	{
		if(!m_players[z])
			continue;

        CanvasCard* c = m_players[z]->hasCard( card );
        if( c )            
        {
            c->forbidden();
            break;
        }
    }      
}

void GameCanvas::createObjects()
{
    if( !m_game )
        return;
    
    unsigned int i = 0;
    unsigned int h = 0;
    unsigned int z = 0;
    
    /** We have to make sure that the human player == m_player[0] !
      */
    for( h=0;h<PLAYERS;h++ )    
        if( m_game->findIndex( h )->rtti() == Player::HUMAN )
        {
            m_players[0] = new CanvasPlayer( 0, m_game->findIndex( h ), canvas() );
            break;
        }

    for( i=h+1;i<PLAYERS;i++ )
        m_players[i-h] = new CanvasPlayer( i-h, m_game->findIndex( i ), canvas() );            

    for( z=0;z<h;z++)    
        m_players[i-h+z] = new CanvasPlayer( i-h+z, m_game->findIndex( z ), canvas() );
    
    positionObjects();
}

void GameCanvas::positionObjects(bool redraw)
{
    if( !m_players[0] || !m_game || !m_stich )
        return;

    for( unsigned int i = 0; i < PLAYERS; i++ )
        m_players[i]->position( i );
    
    for( unsigned int i = 0; i < 4; i++ ) 
    {
        QPoint p = getStichPosition(i);
        m_stich[i]->move( (int)p.x(), (int)p.y() );
    }
    
    if(redraw)
    {
        canvas()->setAllChanged();
        canvas()->update();
    }
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
            p = QPoint(cx-cardw, cy); break;
        case 2:
            p = QPoint(cx, cy-(cardh/2)); break;
        case 3:
        default:
            p = QPoint(cx+cardw, cy); break;
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
            Player* player = m_players[i]->player();
            if( m_players[i]->hasCard( card->card() ) && player->rtti() == Player::HUMAN )
            {
                qDebug("card=%i color=%i", card->card()->card(), card->card()->color() );
                emit playCard( card->card() );
            }
        }
    }
}

void GameCanvas::slotPlayerPlayedCard( unsigned int player, Card *c )
{
    QPoint point;
    unsigned int i=0;
    CanvasCard* card = 0;
    if( !m_players[0] || !m_game || !m_stich )
        return;    
           
    for(i=0;i<PLAYERS;i++)
        if( m_players[i]->player()->id() == player )
        {
            card = m_players[i]->hasCard( c );
            break;
        }
    
    if( card )
    {
        m_players[player]->cardPlayed( card->card() );
  
        CanvasCard* stich = m_stich[player];
        stich->setCard( card->card() );
        stich->setRotation( 0 );
        stich->setZ( player );
        stich->setFrontVisible( true );
        stich->show();
    }
}

void GameCanvas::slotPlayerMadeStich(unsigned int)
{
    unsigned int i=0;
    for(;i<4;i++)
        m_stich[i]->hide();
}

void GameCanvas::resizeEvent( QResizeEvent * r )
{
    canvas()->resize( this->width() -2, this->height()-2 );
    QCanvasView::resizeEvent( r );
    
    positionObjects();
}

void GameCanvas::redrawPlayers()
{
    // Maybe we should call this from a QTimer::singleShot( 100 )
    // so that all events are processed, will surely save us a crash!
	int i;
	
    
	for(i=0;i<PLAYERS;i++)
		m_players[i]->init(i);
	clearObjects();
	createObjects();
    
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
