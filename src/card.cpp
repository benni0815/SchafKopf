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
#include "card.h"
#include "settings.h"
#include "player.h"

#include <qpixmap.h>

#include <kapplication.h>
#include <kcarddialog.h>

QPixmap* Card::m_background = 0;

Card::Card( const int id )
{
    enum EType t;
    enum EColor c;
    int i;
    
    // try to calculate the type and color from the id
    for( i=Card::EICHEL; i <= Card::SCHELLEN; i++ )
        if( (id - i - 1) % 4 == 0 )
        {
            t = (Card::EType)(id - i);
            c = (Card::EColor)i;
            
            break;
        }
    
    init( t, c );
}

Card::Card( const enum EType t, const enum EColor c )
{
    init( t, c );
}

Card::~Card()
{
    delete m_pixmap;
}

void Card::init( const enum EType t, const enum EColor c  )
{
    m_pixmap = NULL;
    m_owner = NULL;
    
    m_card = t;
    m_color = c;
    switch( m_card ) {
        case SAU:
            m_points = 11; break;
        case ZEHN:
            m_points = 10; break;
        case KOENIG:
            m_points = 4; break;
        case OBER:
            m_points = 3; break;
        case UNTER:
            m_points = 2; break;
        case NEUN:
        case ACHT:
        case SIEBEN:
        default:
            m_points = 0;
            break;
    };
}

QPixmap* Card::pixmap()
{
    if( !m_pixmap ) {
        QString dir = Settings::instance()->cardDeck();
        m_pixmap = new QPixmap( KCardDialog::getCardPath( dir, m_card + m_color ) );
    }
    
    return m_pixmap;
}
        
QPixmap* Card::backgroundPixmap()
{
    if( !m_background ) {
        QString deck = Settings::instance()->cardBackground();
        m_background = new QPixmap( deck );
    }
    
    return m_background;
}

bool Card::isEqual(Card *othercard)
{
	if(othercard->card()==this->card()&&othercard->color()==this->color()) return true;
	else 
		return false;
}

void Card::cardDeckChanged()
{
    delete m_pixmap;
    m_pixmap = NULL;
    
    if( m_background ) 
    {
        delete m_background;
        m_background = NULL;
    }
}

bool Card::operator< ( Card* c )
{
    // TODO: anyone a better idea?
    int a[] = {Card::SAU, Card::ZEHN, Card::KOENIG, Card::OBER, Card::UNTER, Card::NEUN, Card::ACHT, Card::SIEBEN};
    int i = 0;
    int pos = 0;
    for( ;i<(sizeof(a)/sizeof(int));i++)
        if( a[i] == this->card() )
        {
            pos = i;
            break;
        }
    
    for(i=0;i<(sizeof(a)/sizeof(int));i++)
        if( a[i] == c->card() )
            return i < pos ? false : true;

    return false;
}

int Card::id() const
{
    return m_card + m_color;
}


QString Card::toString() const
{
    QString res;

    switch (m_color) {
    case EICHEL: res += "Eichel"; break;
    case GRAS: res += "Gras"; break;
    case HERZ: res += "Herz"; break;
    case SCHELLEN: res += "Schellen"; break;
    default: res += "Nocolor"; break;
    }

    res += ' ';

    switch (m_card) {
    case SAU: res += "Sau"; break;
    case ZEHN: res += "Zehn"; break;
    case KOENIG: res += "Koenig"; break;
    case OBER: res += "Ober"; break;
    case UNTER: res += "Unter"; break;
    case NEUN: res += "Neun"; break;
    case ACHT: res += "Acht"; break;
    case SIEBEN: res += "Sieben"; break;
    default: res += "Unknown"; break;
    }

    return res;
}
