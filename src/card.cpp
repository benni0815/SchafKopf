// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "card.h"
#include "settings.h"
#include "player.h"

#include <kcarddialog.h>

#include <QPixmap>

namespace
{

QPixmap& background()
{
  static QPixmap pix;
  return pix;
}

int getPoints(Card::Type t)
{
  static const auto map = std::unordered_map<Card::Type, int>{
    {Card::SAU,    11},
    {Card::ZEHN,   10},
    {Card::KOENIG, 4},
    {Card::OBER,   3},
    {Card::UNTER,  2}
  };
  const auto it = map.find(t);
  if (it == std::end(map))
  {
    return 0;
  }
  return it->second;
}

} // namespace

Card::Card( const int id )
{
    Type t = NOSTICH;
    Color c = NOCOLOR;
    int i;
    
    // try to calculate the type and color from the id
    for( i=Card::EICHEL; i <= Card::SCHELLEN; i++ )
        if( (id - i - 1) % 4 == 0 )
        {
            t = (Card::Type)(id - i);
            c = (Card::Color)i;
            
            break;
        }
    
    init( t, c );
}

Card::Card(const Type t, const Color c)
{
    init( t, c );
}

void Card::init(const Type t, const Color c)
{
  m_id.type = t;
  m_id.color = c;
  cardDeckChanged();
}

const QPixmap& Card::pixmap() const
{
  return m_pixmap;
}

const QPixmap& Card::backgroundPixmap()
{
  if (background().isNull())
  {
    background() = QPixmap(Settings::instance()->cardCache()->backside());
  }

  return background();
}

int Card::points() const
{
  return getPoints(m_id.type);
}

KCardInfo Card::kdeCardInfo() const
{
    KCardInfo::Suit suit;
    KCardInfo::Card card;

    switch(m_id.color)
    {
    case EICHEL: suit = KCardInfo::Club; break;
    case GRAS: suit = KCardInfo::Spade; break;
    case HERZ: suit = KCardInfo::Heart; break;
    case SCHELLEN: suit = KCardInfo::Diamond; break;
    default: suit = KCardInfo::None;
    }

    switch(m_id.type)
    {
    case SAU: card = KCardInfo::Ace; break;
    case KOENIG: card = KCardInfo::King; break;
    case OBER: card = KCardInfo::Queen; break;
    case UNTER: card = KCardInfo::Jack; break;
    default: card = KCardInfo::Card( ( m_id.type / 4 ) + 1 );
    }

    KCardInfo cardInfo = KCardInfo( suit, card );
    return cardInfo;
}

bool Card::isEqual(const Card* const othercard) const
{
  return (othercard->card() == card()) && (othercard->color() == color());
}

void Card::cardDeckChanged()
{
  m_pixmap = QPixmap(Settings::instance()->cardCache()->frontside(kdeCardInfo()));
  background() = QPixmap();
}

bool Card::operator<(const Card* const c) const
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
    return m_id.type + m_id.color;
}


QString Card::toString() const
{
    QString res;

    switch (m_id.color) {
    case EICHEL: res += "Eichel"; break;
    case GRAS: res += "Gras"; break;
    case HERZ: res += "Herz"; break;
    case SCHELLEN: res += "Schellen"; break;
    default: res += "Nocolor"; break;
    }

    res += ' ';

    switch (m_id.type) {
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
