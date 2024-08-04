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

#include <QTimer>
#include <QPainter>


CanvasCard::CanvasCard()
 :  QGraphicsPixmapItem()
{
    m_card = NULL;
    m_forbidden = false;
    setFlag( QGraphicsItem::ItemIsSelectable );
    setTransformationMode( Qt::SmoothTransformation );
    //setGraphicsEffect( new  QGraphicsDropShadowEffect() );
    timer = new QTimer( this );
    connect(Settings::instance(), &Settings::cardChanged, this, &CanvasCard::cardDeckChanged);
}

CanvasCard::~CanvasCard()
{
}

void CanvasCard::setCard( Card* card )
{
    m_card = card;
    if( m_card )
    {
        reloadPixmaps();
    }
}

void CanvasCard::reloadPixmaps()
{
    frontPixmap = *( m_card->pixmap() );

    QImage forbiddenFrontImage = frontPixmap.toImage();

    {
      QPainter p(&forbiddenFrontImage);
      p.setBrush(QBrush(QColor::fromRgbF(1, 1, 1, 0.5)));
      p.drawRect(QRect(QPoint(0, 0), forbiddenFrontImage.size()));
    }

    forbiddenFrontPixmap.convertFromImage( forbiddenFrontImage );

    backPixmap = *( Card::backgroundPixmap() );

    float height = frontPixmap.height();
    float width = frontPixmap.width();
    setTransformOriginPoint(height/2, width/2);

    updatePixmap();
}

void CanvasCard::updatePixmap()
{
    if( !m_visible )
    {
        setPixmap( backPixmap );
        return;
    }
    if( m_forbidden )
    {
        setPixmap( forbiddenFrontPixmap );
        return;
    }
    setPixmap( frontPixmap );
    QGraphicsPixmapItem::update();
}

void CanvasCard::setFrontVisible( bool b )
{
    m_visible = b;
    updatePixmap();
}

void CanvasCard::forbidden()
{
    m_forbidden = true;
    QTimer::singleShot(std::chrono::seconds(1), this, &CanvasCard::disableForbidden);
    updatePixmap();
}

void CanvasCard::disableForbidden()
{
    m_forbidden = false;
    updatePixmap();
}

void CanvasCard::setDestination( int x, int y )
{
    tox = x;
    toy = y;
}

void CanvasCard::animatedMove()
{
    connect(timer, &QTimer::timeout, this, &CanvasCard::moveLoop);
    timer->start( 2 );
}

void CanvasCard::moveLoop()
{
    int speedx = 1, speedy = 1;
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
        disconnect(timer, &QTimer::timeout, this, &CanvasCard::moveLoop);
    }
}

void CanvasCard::cardDeckChanged()
{
    if( m_card )
    {
        m_card->cardDeckChanged();
        reloadPixmaps();
    }
}
