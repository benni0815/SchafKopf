// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "card.h"
#include "settings.h"
#include "player.h"

#include <kcarddialog.h>

#include <QPixmap>


QPixmap* Card::m_background = 0;

Card::Card( const int id )
{
    enum EType t = NOSTICH;
    enum EColor c = NOCOLOR;
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
    if( !m_pixmap ) m_pixmap = new QPixmap( Settings::instance()->cardCache()->frontside( kdeCardInfo() ) );

    return m_pixmap;
}

QPixmap* Card::backgroundPixmap()
{
    if( !m_background ) m_background = new QPixmap( Settings::instance()->cardCache()->backside() );

    return m_background;
}

KCardInfo Card::kdeCardInfo()
{
    KCardInfo::Suit suit;
    KCardInfo::Card card;

    switch( m_color )
    {
    case EICHEL: suit = KCardInfo::Club; break;
    case GRAS: suit = KCardInfo::Spade; break;
    case HERZ: suit = KCardInfo::Heart; break;
    case SCHELLEN: suit = KCardInfo::Diamond; break;
    default: suit = KCardInfo::None;
    }

    switch( m_card )
    {
    case SAU: card = KCardInfo::Ace; break;
    case KOENIG: card = KCardInfo::King; break;
    case OBER: card = KCardInfo::Queen; break;
    case UNTER: card = KCardInfo::Jack; break;
    default: card = KCardInfo::Card( ( m_card/4 ) + 1 );
    }

    KCardInfo cardInfo = KCardInfo( suit, card );
    return cardInfo;
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
    uint i = 0;
    uint pos = 0;
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
