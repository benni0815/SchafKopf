// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "card.h"
#include "player.h"

#include <kcarddialog.h>

#include <QPixmap>

namespace
{

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

int Card::points() const
{
  return getPoints(m_id.type);
}

KCardInfo Card::kdeCardInfo(Id id)
{
  static const auto colorMap = std::unordered_map<Color, KCardInfo::Suit>{
    {Eichel, KCardInfo::Club},
    {Gras, KCardInfo::Spade},
    {Herz, KCardInfo::Heart},
    {Schellen, KCardInfo::Diamond}
  };

  static const auto typeMap = std::unordered_map<Type, KCardInfo::Card>{
    {Sau,    KCardInfo::Ace},
    {Koenig, KCardInfo::King},
    {Ober,   KCardInfo::Queen},
    {Unter,  KCardInfo::Jack},
    {Zehn,   KCardInfo::Ten},
    {Neun,   KCardInfo::Nine},
    {Acht,   KCardInfo::Eight},
    {Sieben, KCardInfo::Seven}
  };

  const auto colorIt = colorMap.find(id.color);
  const auto color = (colorIt == std::end(colorMap)) ? KCardInfo::Suit::None
                                                     : colorIt->second;
  const auto typeIt = typeMap.find(id.type);
  const auto type = (typeIt == std::end(typeMap)) ? KCardInfo::Card::Joker
                                                  : typeIt->second;

  return KCardInfo(color, type);
}

bool Card::isEqual(const Card* const othercard) const
{
  return (othercard->id() == id());
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
