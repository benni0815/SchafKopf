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
    {Card::Sau,    11},
    {Card::Zehn,   10},
    {Card::Koenig, 4},
    {Card::Ober,   3},
    {Card::Unter,  2}
  };
  const auto it = map.find(t);
  if (it == std::end(map))
  {
    return 0;
  }
  return it->second;
}

} // namespace

Card::Card(const Id& id)
: m_id(id)
{}

Card::Card(const Type t, const Color c)
: m_id({t, c})
{}

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
    case Eichel: suit = KCardInfo::Club; break;
    case Gras: suit = KCardInfo::Spade; break;
    case Herz: suit = KCardInfo::Heart; break;
    case Schellen: suit = KCardInfo::Diamond; break;
    default: suit = KCardInfo::None;
    }

    switch(m_id.type)
    {
    case Sau: card = KCardInfo::Ace; break;
    case Koenig: card = KCardInfo::King; break;
    case Ober: card = KCardInfo::Queen; break;
    case Unter: card = KCardInfo::Jack; break;
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
    int a[] = {Card::Sau, Card::Zehn, Card::Koenig, Card::Ober, Card::Unter, Card::Neun, Card::Acht, Card::Sieben};
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

Card::Id Card::id() const
{
  return m_id;
}


QString Card::toString() const
{
    QString res;

    switch (m_id.color) {
    case Eichel: res += "Eichel"; break;
    case Gras: res += "Gras"; break;
    case Herz: res += "Herz"; break;
    case Schellen: res += "Schellen"; break;
    default: res += "Nocolor"; break;
    }

    res += ' ';

    switch (m_id.type) {
    case Sau: res += "Sau"; break;
    case Zehn: res += "Zehn"; break;
    case Koenig: res += "Koenig"; break;
    case Ober: res += "Ober"; break;
    case Unter: res += "Unter"; break;
    case Neun: res += "Neun"; break;
    case Acht: res += "Acht"; break;
    case Sieben: res += "Sieben"; break;
    default: res += "Unknown"; break;
    }

    return res;
}
