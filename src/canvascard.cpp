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
#include "canvascard.h"
#include "card.h"
#include "settings.h"
#include "schafkopfdef.h"

#include <qpainter.h>
#include <qtimer.h>
#include <qmatrix.h>
#include <qpixmap.h>

#include <kpixmap.h>
 

CanvasCard::CanvasCard(Q3Canvas*c)
 : Q3CanvasRectangle(c), m_rotation(0)
{
    m_card = NULL;
    m_forbidden = false;
    show();
    timer = new QTimer( this );
    loadAlpha();
        
    connect( Settings::instance(), SIGNAL( cardChanged() ), this, SLOT( cardDeckChanged() ) );
}

CanvasCard::~CanvasCard()
{
}

void CanvasCard::setCard( Card* card )
{
    m_card = card;
}

void CanvasCard::draw( QPainter & p )
{
#ifdef SIMULATION_MODE
    return;
#endif // SIMULATION_MODE    

    if( m_card )
    {
        QPixmap* pixmap = m_visible ? m_card->pixmap() : Card::backgroundPixmap();
    
        // this code handles already matrix transformations
        QMatrix wm = p.worldMatrix();    
        QPoint point( (int)x(), (int)y() );
        point = wm * point;
    
        wm.rotate( (double)m_rotation );
    
        KPixmap pix = pixmap->transformed( wm );
        if( m_forbidden )
            pix = KPixmapEffect::fade( pix, 0.5, Qt::gray );
        else if( isActive() )
            pix = KPixmapEffect::fade( pix, 0.25, Qt::yellow );
            
        setSize( pix.width()+3, pix.height()+3 );
        bitBlt( p.device(), point.x(), point.y(), &pix );
        if(loadOK1 && loadOK2)
        {
            if( m_rotation==0 || m_rotation==180)
                bitBlt( p.device(), point.x(), point.y(), &Shadow );
            else
                bitBlt( p.device(), point.x(), point.y(), &Shadow2 );
        }
    }
}

void CanvasCard::setActive( bool b )
{
    Q3CanvasItem::update();
    Q3CanvasItem::setActive( b );
}

void CanvasCard::setFrontVisible( bool b )
{
    m_visible = b;
}

void CanvasCard::setRotation( int d )
{
    m_rotation = d;
}

void CanvasCard::forbidden()
{
    m_forbidden = true;
    Q3CanvasRectangle::update();
    QTimer::singleShot( 1000, this, SLOT(disableForbidden()));
}

void CanvasCard::disableForbidden()
{
	m_forbidden =false;
	Q3CanvasRectangle::update();
}

void CanvasCard::setDestination( int x, int y )
{
	tox = x;
	toy = y;
}

void CanvasCard::animatedMove()
{
        connect( timer, SIGNAL(timeout()), this, SLOT(moveLoop()) );
        timer->start( 2 );
}

void CanvasCard::moveLoop()
{
	int speedx=1, speedy=1;
	if( tox-x()>60 || x()-tox>60 ) speedx=20;
	if( toy-y()>60 || y()-toy>60 ) speedy=20;
	if(tox>x())
		moveBy( speedx, 0 );
	else if(tox!=x())
		moveBy( -speedx, 0 );
	if(toy>y())
		moveBy( 0, speedy );
	else if(toy!=y())
		moveBy( 0, -speedy );
	if(tox==x() && toy==y())
	{
		timer->stop();
		disconnect( timer, SIGNAL(timeout()), this, SLOT(moveLoop()) );
	}
}

void CanvasCard::loadAlpha()
{
    loadOK1 = Shadow.load( Settings::instance()->cardDeck() + "alpha1.png" );
    loadOK2 = Shadow2.load( Settings::instance()->cardDeck() + "alpha2.png" );
}

void CanvasCard::cardDeckChanged()
{
    if( m_card )
    {
        m_card->cardDeckChanged();
        loadAlpha();
        Q3CanvasItem::update();
    }
}
