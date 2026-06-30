// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "card.h"
#include "player.h"

#include <kcarddialog.h>

#include <QPixmap>

namespace
{

int getPoints(CardId::Type t)
{
  static const auto map = std::unordered_map<CardId::Type, int>{
    {CardId::Sau,    11},
    {CardId::Zehn,   10},
    {CardId::Koenig, 4},
    {CardId::Ober,   3},
    {CardId::Unter,  2}
  };
  const auto it = map.find(t);
  if (it == std::end(map))
  {
    return 0;
  }
  return it->second;
}

} // namespace

Card::Card(const CardId& id)
: m_id(id)
{}

Card::Card(const CardId::Type t, const CardId::Color c)
: m_id({t, c})
{}

int Card::points() const
{
  return getPoints(m_id.type);
}

KCardInfo Card::kdeCardInfo(CardId id)
{
  static const auto colorMap = std::unordered_map<CardId::Color, KCardInfo::Suit>{
    {CardId::Eichel, KCardInfo::Club},
    {CardId::Gras, KCardInfo::Spade},
    {CardId::Herz, KCardInfo::Heart},
    {CardId::Schellen, KCardInfo::Diamond}
  };

  static const auto typeMap = std::unordered_map<CardId::Type, KCardInfo::Card>{
    {CardId::Sau,    KCardInfo::Ace},
    {CardId::Koenig, KCardInfo::King},
    {CardId::Ober,   KCardInfo::Queen},
    {CardId::Unter,  KCardInfo::Jack},
    {CardId::Zehn,   KCardInfo::Ten},
    {CardId::Neun,   KCardInfo::Nine},
    {CardId::Acht,   KCardInfo::Eight},
    {CardId::Sieben, KCardInfo::Seven}
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
    int a[] = {CardId::Sau, CardId::Zehn, CardId::Koenig, CardId::Ober, CardId::Unter, CardId::Neun, CardId::Acht, CardId::Sieben};
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

CardId Card::id() const
{
  return m_id;
}


QString Card::toString() const
{
    QString res;

    switch (m_id.color) {
    case CardId::Eichel: res += "Eichel"; break;
    case CardId::Gras: res += "Gras"; break;
    case CardId::Herz: res += "Herz"; break;
    case CardId::Schellen: res += "Schellen"; break;
    default: res += "Nocolor"; break;
    }

    res += ' ';

    switch (m_id.type) {
    case CardId::Sau: res += "Sau"; break;
    case CardId::Zehn: res += "Zehn"; break;
    case CardId::Koenig: res += "Koenig"; break;
    case CardId::Ober: res += "Ober"; break;
    case CardId::Unter: res += "Unter"; break;
    case CardId::Neun: res += "Neun"; break;
    case CardId::Acht: res += "Acht"; break;
    case CardId::Sieben: res += "Sieben"; break;
    default: res += "Unknown"; break;
    }

    return res;
}
