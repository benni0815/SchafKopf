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
#include "timer.h"
#include "settings.h"

#include <qpainter.h>
#include <qtimer.h>
#include <qwmatrix.h>
#include <qpixmap.h>

#include <kpixmap.h>
#include <kpixmapeffect.h> 

CanvasCard::CanvasCard(QCanvas*c)
 : QCanvasRectangle(c), m_rotation(0)
{
    m_card = 0;
    m_forbidden = false;
    show();
    timer = new QTimer( this );
    Shadow.load( Settings::instance()->cardDeck() + "alpha1.png" );
    Shadow2.load( Settings::instance()->cardDeck() + "alpha2.png" );
}

CanvasCard::CanvasCard(Card* card,QCanvas*c)
 : QCanvasRectangle(c), m_rotation(0)
{
    setCard( card );
    m_forbidden = false;
    show();
    //timer = new QTimer( this );
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
    if( m_card )
    {
        QPixmap* pixmap = m_visible ? m_card->pixmap() : Card::backgroundPixmap();
    
        // this code handles already matrix transformations
        QWMatrix wm = p.worldMatrix();    
        QPoint point( (int)x(), (int)y() );
        point = wm * point;
    
        wm.rotate( (double)m_rotation );
    
        KPixmap pix = pixmap->xForm( wm );
        if( m_forbidden )
            pix = KPixmapEffect::fade( pix, 0.5, Qt::gray );
        
        setSize( pix.width(), pix.height() );
        bitBlt( p.device(), point.x(), point.y(), &pix );
        if( m_rotation==0 || m_rotation==180)
            bitBlt( p.device(), point.x(), point.y(), &Shadow );
        else
            bitBlt( p.device(), point.x(), point.y(), &Shadow2 );
    }
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
    QCanvasRectangle::update();
    QTimer::singleShot( 1000, this, SLOT(disableForbidden()));
}

void CanvasCard::disableForbidden()
{
	m_forbidden =false;
	QCanvasRectangle::update();
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
	if(tox>x())
		moveBy( 1, 0 );
	else if(tox!=x())
		moveBy( -1, 0 );
	if(toy>y())
		moveBy( 0, 1 );
	else if(toy!=y())
		moveBy( 0, -1 );
	if(tox==x() && toy==y())
	{
		timer->stop();
		disconnect( timer, SIGNAL(timeout()), this, SLOT(moveLoop()) );
	}
}
