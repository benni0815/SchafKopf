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
#include <qimage.h>

GameCanvas::GameCanvas(QCanvas* c, QWidget *parent, const char *name)
 : QCanvasView(c,parent, name)
{
    unsigned int i = 0;
    for(;i<PLAYERS;i++)
    {
        m_stich[i]=new CanvasCard( c );
        m_players[i]=new CanvasPlayer( i, c );
    }
    
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
    
    // Does not work :-(
    /*
    m_message->setZ( 100 );
    m_yes->setZ( 100 );
    m_no->setZ( 100 );
    m_ok->setZ( 100 );
    */
    
    m_result = 0;
    
    canvas()->setBackgroundColor( Qt::darkGreen );
    
    loadOK = ImgBack.load( Settings::instance()->backgroundImage() );
    canvas()->setAdvancePeriod( 20 );
    update();
    
    connect( Settings::instance(), SIGNAL(cardChanged()), this, SLOT(redrawPlayers()));
    connect( Settings::instance(), SIGNAL(cardChanged()), this, SLOT(positionObjects()));
    connect( this, SIGNAL(clicked( QCanvasItem* )), this, SLOT(yesNoClicked(QCanvasItem*)));
}    

GameCanvas::~GameCanvas()
{
    for( unsigned int i = 0; i < PLAYERS; i++ )
    {
        if( m_stich[i]->card() )
            delete m_stich[i]->card();
            
        delete m_stich[i];
        delete m_players[i];
    }
}

void GameCanvas::cardForbidden( int cardid )
{
    for(unsigned int z=0;z<PLAYERS;z++)
	{
        CanvasCard* c = m_players[z]->hasCard( cardid );
        if( c )            
        {
            c->forbidden();
            break;
        }
    }      
}

void GameCanvas::positionObjects(bool redraw)
{
    for( unsigned int i = 0; i < PLAYERS; i++ )
        m_players[i]->position();
    
    for( unsigned int i = 0; i < PLAYERS; i++ ) 
    {
        QPoint p = getStichPosition(i);
        m_stich[i]->move( (int)p.x(), (int)p.y() );
    }

    m_message->move( canvas()->width()/2, canvas()->height()/2 );
    m_message->setTextFlags(Qt::AlignCenter);
    m_ok->move( ( canvas()->width() - m_ok->boundingRect().width() )/2,
                     m_message->y() + m_message->boundingRect().height()*2/3 );
    m_yes->move( m_message->x() - m_message->boundingRect().width()/2, 
                 m_message->y() + m_message->boundingRect().height()*2/3 );
    m_no->move( m_message->x() + m_message->boundingRect().width()/2 - m_no->boundingRect().width(), 
                m_message->y() + m_message->boundingRect().height()*2/3 );

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
    int stichcanvasw = canvas()->width() - (2*DIST) - (2*cardh) - 50;
    int stichcanvash = canvas()->height() - (2*DIST) - (2*cardh) - 50;
    int cx = w/2;
    int cy = h/2;
    
    switch( player ) 
    {
        case 0:
            if(stichcanvash>(2*cardh-20))
	    	p = QPoint(cx-(cardw/2)-5, cy-10 );
	    else
	    	p = QPoint(cx-(cardw/2)-5, cy+(stichcanvash/2)-cardh);
	    break;
        case 1:
	    if(stichcanvasw>(2*cardh-20))
            	p = QPoint(cx-cardh+10, cy-(cardw/2)-5);
	    else
	    	p = QPoint(cx-(stichcanvasw/2), cy-(cardw/2)-5);
	    break;
        case 2:
            if(stichcanvash>(2*cardh-20))
            	p = QPoint(cx-(cardw/2)+5, cy-cardh+10 );
	    else
	    	p = QPoint(cx-(cardw/2)+5, cy-(stichcanvash/2));
	    break;
        case 3:
        default:
	    if(stichcanvasw>(2*cardh-20))
            	p = QPoint(cx-10, cy-(cardw/2)+5);
	    else
	    	p = QPoint(cx+(stichcanvasw/2)-cardh, cy-(cardw/2)+5);
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

int GameCanvas::getCard()
{
    connect( this, SIGNAL(clicked( QCanvasItem* )), this, SLOT(cardClicked(QCanvasItem*)));
    m_result = -1;
    ENTER_LOOP();
    
    return m_result;
}

void GameCanvas::cardClicked( QCanvasItem* item )
{
    if( item->rtti() == CANVASCARD ) 
    {
        CanvasCard* card = static_cast<CanvasCard*>(item);
        
        for( unsigned int i = 0; i < PLAYERS; i++ ) 
        {
            if( m_players[i]->isHuman() && m_players[i]->hasCard( card->card()->id() ) )
            {
                m_result = card->card()->id();
                disconnect( this, SIGNAL(clicked( QCanvasItem* )), this, SLOT(cardClicked(QCanvasItem*)));
                EXIT_LOOP();
            }
        }
    }
}

void GameCanvas::slotPlayerPlayedCard( unsigned int player, int cardid )
{
    QPoint point;
    unsigned int i=0;
    CanvasCard* card = 0;
           
    for(i=0;i<PLAYERS;i++)
        if( m_players[i]->id() == player )
        {
            card = m_players[i]->hasCard( cardid );
            break;
        }
    
    if( card )
    {
        m_players[player]->cardPlayed( card->card() );
        if(Settings::instance()->rearrangeCards())
            m_players[player]->position();
  
        CanvasCard* stich = m_stich[player];
        if( stich->card() )
            delete stich->card();
        stich->setCard( new Card( card->card()->id() ) );
        
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
    resizeBackground();
}

void GameCanvas::resizeBackground()
{
    QImage ImgBack2;
    if(loadOK)
    {
         ImgBack2=ImgBack.smoothScale( canvas()->width(), canvas()->height() );
         canvas()->setBackgroundPixmap( ImgBack2 );
    }

}

void GameCanvas::updateBackground()
{
    loadOK = ImgBack.load( Settings::instance()->backgroundImage() );
    resizeBackground();
    if(!loadOK)
         canvas()->setBackgroundPixmap( NULL );
}

void GameCanvas::redrawPlayers()
{
	unsigned int i = 0;
	
    for(i=0;i<PLAYERS;i++)
    {
		m_players[i]->init();
        m_stich[i]->hide();
    }
}

void GameCanvas::contentsMousePressEvent(QMouseEvent* e)
{
    m_item = NULL;
    double z;
    bool first=true;
    if( e->button() == Qt::LeftButton ) {
        QCanvasItemList list = canvas()->allItems();
	for( unsigned int i = 0; i < list.count(); i++ )
            if( list[i]->boundingRect().contains( e->pos() ) && list[i]->isVisible() )
            {
	        if(first)
		{
			first=false;
			z=list[i]->z();
			m_item = list[i];
		}
                //qDebug("z-wert:"+QString::number(list[i]->z()));
                if(list[i]->z()>z)
		{
			z=list[i]->z();
			m_item = list[i];
		}
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

void GameCanvas::playerHasDoubled( unsigned int id, bool value )
{
    int i;
    for(i=0;i<PLAYERS;i++)
        if( m_players[i]->id() == id )
        {
            m_players[i]->setHasDoubled( value );
        }
}

void GameCanvas::playerIsLast( unsigned int id )
{
    int i;
    for(i=0;i<PLAYERS;i++)
        if( m_players[i]->id() == id )
        {
            m_players[i]->setLast( true );
            // TODO: init is not a good name for this function...
            m_players[i]->init();
        }
}

void GameCanvas::setPlayerName( unsigned int id, const QString & name )
{
    int i;
    for(i=0;i<PLAYERS;i++)
        if( m_players[i]->id() == id )
            m_players[i]->setName( name );

}

void GameCanvas::resetPlayers()
{
    int i;
    
    for(i=0;i<PLAYERS;i++)
    {
        m_players[i]->setHasDoubled( false );
        m_players[i]->setLast( false );
    }
}

void GameCanvas::resetPlayerCards()
{
    int i;
    CardList list;
        
    for(i=0;i<PLAYERS;i++)    
        m_players[i]->setCards( &list );
}

void GameCanvas::setPlayerCards( unsigned int id, int* cards )
{
    int i;
    CardList list( cards );
    list.setAutoDelete( false );
    
    for(i=0;i<PLAYERS;i++)
        if( m_players[i]->id() == id )
            m_players[i]->setCards( &list );
}

#include "gamecanvas.moc"
