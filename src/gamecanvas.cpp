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
#include "timer.h"

#include <kapplication.h>
#include <klocale.h>
#include <qtimer.h>

GameCanvas::GameCanvas(QCanvas* c, QWidget *parent, const char *name)
 : QCanvasView(c,parent, name)
{
    unsigned int i = 0;
    for(;i<PLAYERS;i++)
    {
        m_stich[i]=new CanvasCard( c );
        m_players[i]=new CanvasPlayer( c );
    }
    
    m_game = NULL;
    m_item = NULL;
        
    QFont f( "Helvetica", 24 );
    
    m_message = new QCanvasText( QString::null, f, canvas() );;
    m_yes = new QCanvasText( i18n("Yes"), f, canvas() );;
    m_no = new QCanvasText( i18n("No"), f, canvas() );;
    m_ok = new QCanvasText( i18n("OK"), f, canvas() );
    
    m_message->setColor( Qt::yellow );
    m_yes->setColor( Qt::yellow );
    m_no->setColor( Qt::yellow );
    m_ok->setColor( Qt::yellow );
    
    m_result = 0;
    
    canvas()->setBackgroundColor( Qt::darkGreen );
    canvas()->setAdvancePeriod( 200 );
    update();
    
	connect( Settings::instance(), SIGNAL(cardChanged()), this, SLOT(redrawPlayers()));
    connect( Settings::instance(), SIGNAL(cardChanged()), this, SLOT(positionObjects()));
    connect( this, SIGNAL(clicked( QCanvasItem* )), this, SLOT(cardClicked(QCanvasItem*)));
    connect( this, SIGNAL(clicked( QCanvasItem* )), this, SLOT(yesNoClicked(QCanvasItem*)));
}    

GameCanvas::~GameCanvas()
{
    for( unsigned int i = 0; i < PLAYERS; i++ )
    {
        delete m_stich[i];
        delete m_players[i];
    }
}

void GameCanvas::setGame( Game* game )
{
    connect( game, SIGNAL(playerPlayedCard(unsigned int,Card*)), this,SLOT(slotPlayerPlayedCard(unsigned int,Card*)));       
        
    connect( game, SIGNAL(playerMadeStich(unsigned int)), this,SLOT(slotPlayerMadeStich(unsigned int)));
    
    connect( game, SIGNAL(gameStarted()), this, SLOT(redrawPlayers()));
    connect( game, SIGNAL(gameEnded()), this, SLOT(redrawPlayers()));
    
    m_game = game;
    createObjects();
}

void GameCanvas::cardForbidden(Card* card)
{
    for(unsigned int z=0;z<PLAYERS;z++)
	{
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
	{
		if( m_game->findIndex( h )->rtti() == Player::HUMAN )
        {
			m_players[0]->setPlayer( 0, m_game->findIndex( h ) );
            break;
        }
	}
		
    for( i=h+1;i<PLAYERS;i++ )
        m_players[i-h]->setPlayer( i-h, m_game->findIndex( i ) );            
	
    for( z=0;z<h;z++)    
        m_players[i-h+z]->setPlayer( i-h+z, m_game->findIndex( z ) );
	
    positionObjects();
}

void GameCanvas::positionObjects(bool redraw)
{
    if( !m_players[0] || !m_game || !m_stich )
        return;

    for( unsigned int i = 0; i < PLAYERS; i++ )
        m_players[i]->position( i );
    
    for( unsigned int i = 0; i < PLAYERS; i++ ) 
    {
        QPoint p = getStichPosition(i);
        m_stich[i]->move( (int)p.x(), (int)p.y() );
    }

    m_message->move( ( canvas()->width() - m_message->boundingRect().width() )/2,
                   (( canvas()->height() - m_message->boundingRect().height() ) / 2) - m_message->boundingRect().height() );
    m_ok->move( ( canvas()->width() - m_ok->boundingRect().width() )/2,
                     m_message->y() + m_message->boundingRect().height() );
    m_yes->move( m_message->x(), 
                 m_message->y() + m_message->boundingRect().height() );
    m_no->move( m_message->x() + m_message->boundingRect().width() - m_no->boundingRect().width(), 
                m_message->y() + m_message->boundingRect().height() );
    
    if(redraw)
    {
        canvas()->setAllChanged();
        canvas()->update();
    }
}

QPoint GameCanvas::getStichPosition( int player )
{
    QPoint p;
    int w = canvas()->width();//-DIST;
    int h = canvas()->height();//-DIST;
    int cardw = Card::backgroundPixmap()->width();
    int cardh = Card::backgroundPixmap()->height();
    int stichcanvasw = canvas()->width() - (4*DIST) - (2*cardh);
    int stichcanvash = canvas()->height() - (4*DIST) - (2*cardh);
    int cx = w/2;
    int cy = h/2;
    
    switch( player ) 
    {
        case 0:
            if(stichcanvash>(2*cardh))
	    	p = QPoint(cx-(cardw/2), cy );
	    else
	    	p = QPoint(cx-(cardw/2), cy+(stichcanvash/2)-cardh);
	    break;
        case 1:
	    if(stichcanvasw>(2*cardh))
            	p = QPoint(cx-cardh, cy-(cardw/2));
	    else
	    	p = QPoint(cx-(stichcanvasw/2), cy-(cardw/2));
	    break;
        case 2:
            if(stichcanvash>(2*cardh))
            	p = QPoint(cx-(cardw/2), cy-cardh );
	    else
	    	p = QPoint(cx-(cardw/2), cy-(stichcanvash/2));
	    break;
        case 3:
        default:
	    if(stichcanvasw>(2*cardh))
            	p = QPoint(cx, cy-(cardw/2));
	    else
	    	p = QPoint(cx+(stichcanvasw/2)-cardh, cy-(cardw/2));
	    break;
    };
        
    return p;
}

int GameCanvas::getStichRotation( int player )
{
    int r;
    switch( player ) 
    {
	case 0:
		r=0; break;
	case 1:
		r=90; break;
	case 2:
		r=180; break;
	case 3:
	default:
		r=270; break;
    };
    return r;
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
	int r = getStichRotation(player);
        stich->setRotation( r );

        // find out the correct z value of this card so that
        // a new card is always placed on top of the earlier ones
        int z = 0;
        for(unsigned int i=0;i<PLAYERS;i++)
        {
            if( m_stich[i]->isVisible() )        
                z++;
        }
        
        stich->setZ( z );
        stich->setFrontVisible( true );
        stich->show();
    }
}

void GameCanvas::slotPlayerMadeStich(unsigned int)
{
    unsigned int i=0;
    for(;i<PLAYERS;i++)
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
	unsigned int i = 0;
	
    for(i=0;i<PLAYERS;i++)
    {
		m_players[i]->init(i);
        m_stich[i]->hide();
    }
}

void GameCanvas::contentsMousePressEvent(QMouseEvent* e)
{
    m_item = NULL;
    if( e->button() == Qt::LeftButton ) {
        QCanvasItemList list = canvas()->allItems();
        for( unsigned int i = 0; i < list.count(); i++ )
            if( list[i]->boundingRect().contains( e->pos() ) && list[i]->isVisible() )
            {
                m_item = list[i];
                break;
            }
    }
}

void GameCanvas::contentsMouseReleaseEvent(QMouseEvent* e)
{
    if( e->button() == Qt::LeftButton && m_item ) {
        emit clicked( m_item );
        m_item = NULL;
    }
}

bool GameCanvas::questionYesNo( const QString & message )
{
    m_result = NO;
    
    m_message->setText( message );
    m_message->show();
    m_yes->show();
    m_no->show();

    positionObjects();
    
    canvas()->update();
    ENTER_LOOP();
    
    m_message->hide();
    m_yes->hide();
    m_no->hide();
    
    canvas()->update();
        
    return ( m_result == YES );
}

void GameCanvas::yesNoClicked( QCanvasItem* item )
{
    if( m_message && ((m_yes && m_no) || m_ok ))
    {
        if( item == m_yes )
        {
            m_result = YES;
            EXIT_LOOP();
        }
        else if( item == m_no )
        {
            m_result = NO;
            EXIT_LOOP();
        }
        else if( item == m_ok )
        {
            m_result = NO;
            EXIT_LOOP();
        }
    }
}

void GameCanvas::information( const QString & message )
{
    m_message->setText( message );
    
    m_message->show();
    m_ok->show();

    positionObjects();
    
    canvas()->update();
    ENTER_LOOP();

    m_message->hide();
    m_ok->hide();
        
    canvas()->update();
        
    return;
}

#include "gamecanvas.moc"
