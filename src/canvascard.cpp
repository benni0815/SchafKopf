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

#include <qpixmap.h>
#include <qwmatrix.h>

#include <kapplication.h>
#if QT_VERSION >= 0x030100
    #include <qeventloop.h>
#else
    #include <qapplication.h>
#endif

CanvasCard::CanvasCard(Card* card,QCanvas*c)
 : QCanvasRectangle(c), m_rotation(0)
{
    m_card = card;
    show();
}


CanvasCard::~CanvasCard()
{
}

void CanvasCard::draw( QPainter & p )
{
    QPixmap* pixmap = m_visible ? m_card->pixmap() : Card::backgroundPixmap();
    
    // this code handles already matrix transformations
    QWMatrix wm = p.worldMatrix();    
    QPoint point( (int)x(), (int)y() );
    point = wm * point;
    
    wm.rotate( (double)m_rotation );
    
    QPixmap pix = pixmap->xForm( wm );
    setSize( pix.width(), pix.height() );
    bitBlt( p.device(), point.x(), point.y(), &pix );
}

void CanvasCard::setFrontVisible( bool b )
{
    m_visible = b;
}

void CanvasCard::setRotation( int d )
{
    m_rotation = d;
}


void CanvasCard::moveTo( QPoint p )
{
    m_point = p;
        
    double my = p.y() - y();
    double mx = p.x() - x();
    setXVelocity( mx );
    setYVelocity( my );
  
    // we better use Timer.block in Game  
/*
#if QT_VERSION >= 0x030100
    kapp->eventLoop()->enterLoop();
#else
    kapp->enter_loop();
#endif
*/
}

void CanvasCard::advance( int phase )
{
    if( (int)x() == (int)m_point.x() )
        setXVelocity( 0 );
    
    if( (int)y() == (int)m_point.y() ) 
        setYVelocity( 0 );
    
    QCanvasItem::advance( phase );
/*    
    if( !xVelocity() && !yVelocity() ) {
#if QT_VERSION >= 0x030100
        kapp->eventLoop()->exitLoop();
#else
        kapp->exit_loop();
#endif
    }
*/
}


