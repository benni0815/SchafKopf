// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "cardlist.h"

#include "card.h"

#include "schafkopfdef.h"

#include <QRandomGenerator>


CardList::CardList(const std::vector<CardId>& cardIds)
{
  for (const auto id : cardIds)
  {
    append(new Card(id));
  }
}

void CardList::init()
{
    m_autoDelete = true;
    while( !isEmpty() )
        delete takeFirst();

    using Type = CardId::Type;
    using Color = CardId::Color;

    for (const auto type : {Type::Sau, Type::Koenig, Type::Ober, Type::Unter, Type::Zehn, Type::Neun, Type::Acht, Type::Sieben})
      for (const auto color : {Color::Eichel, Color::Gras, Color::Herz, Color::Schellen})
        append(new Card(type, color));
}

int CardList::points() const
{
  auto add = [] (int points, Card* card) { return points + card->points(); };
  return std::accumulate(begin(), end(), 0, add);
}

void CardList::appendList(const CardList& list)
{
  append(list);
}

void CardList::randomize()
{
    int rnd[CARD_CNT];
    int i, a, rval;
        
    init();
    for(i=0;i<CARD_CNT;i++)
    {
        rval = QRandomGenerator::global()->bounded(RAND_MAX) % CARD_CNT;
        rnd[i]=rval;
        for(a=0;a<i;a++)
        {
            if(rnd[a]==rval)
            {
                i--;
                break; //break does not work here,
                // as rnd[i-1] get rval assigned
                // as a reason this goto solution
                // Lenz says it does =:-)
                }
        }
    }
    for(i=0;i<CARD_CNT;i++)
        append(at(rnd[i]));
    while(count() > CARD_CNT )
        (void)removeAt(0); // we always remove the first item
                       // Do not use removeCard(), as autoDeletion is
                       // enabled!!! 
}

/*
    typedef bool (*__isTrumpf)(Card* Karte);
    CardList* FindCards(int color, int type, __isTrumpf _trumpf_func);*/

CardList CardList::FindCards(CardId::Color color, CardId::Type type)
{
  auto match = [&] (Card* card)
  {
    return (color == CardId::NoColor || card->color() == color) && (type == CardId::NoStich || card->card() == type);
  };
  CardList found;
  std::copy_if(begin(), end(), std::back_inserter(found), match);
  return found;
}

bool CardList::contains(CardId cardid) const
{
  return std::any_of(begin(), end(),
                     [&] (Card* c) { return c->id() == cardid; });
}

void CardList::RemoveCards(const CardList& itemsToRem)
{
    Card *card;
    Card *thiscard;
    for (int i = 0; i < itemsToRem.size(); ++i)
    {
        card = itemsToRem.at(i);
        for( int t = 0; t < this->count(); t++ )
        {
            thiscard = this->at( t );
            if(thiscard->isEqual(card))
                removeCard( thiscard );
        }
    }
}

void CardList::sort(EvalFunc eval)
{
  auto compare = [eval] (Card* lhs, Card* rhs) { return eval(lhs) < eval(rhs); };
  std::sort(begin(), end(), compare);
}

std::vector<CardId> CardList::toIds() const
{
  std::vector<CardId> ids;
  std::transform(std::begin(*this), std::end(*this), std::back_inserter(ids), [] (Card* card) { return card->id(); });
  return ids;
}

void CardList::removeCard( uint index )
{
    Card* card = this->takeAt( index );
    if( m_autoDelete ) delete card;
}

void CardList::removeCard( Card* card )
{
    int i = this->indexOf( card );
    if (i != -1)
    {
        card = this->takeAt( i );
        if( m_autoDelete ) delete card;
    }
}
