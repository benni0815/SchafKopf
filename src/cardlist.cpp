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
#include "cardlist.h"

#include "schafkopfdef.h"

#include <KRandom>

#include <iostream>

CardList::CardList()
{
    m_autoDelete = false;
}

CardList::CardList( int* cards )
{
    m_autoDelete = false;

    if( cards )
        while( *cards != 0 )
        {
            append( new Card( *cards ) );
            ++cards;
        }
}

void CardList::init()
{
    m_autoDelete = true;
    while( !isEmpty() )
        delete takeFirst();

    for( int i = 0; i < CARD_CNT ; i += 4  )
        for( int z = Card::EICHEL; z <= Card::SCHELLEN; z++ )
            append( new Card( (Card::EType)(i+1), (Card::EColor)z ) );
}

int CardList::points()
{
    int tmp = 0;
    for( int i = 0; i < this->count(); i++ )
        tmp += at( i )->points();
        
    return tmp;
}

void CardList::appendList( CardList* list )
{
    for( int i=0;i<list->count();i++)
        append( list->at( i ) );
}

void CardList::randomize()
{
    int rnd[CARD_CNT];
    int i, a, rval;
        
    init();
    for(i=0;i<CARD_CNT;i++)
    {
        rval=KRandom::random()%CARD_CNT;
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

CardList* CardList::FindCards(int color, int type)
{
    CardList* found=new CardList();

    for( int i = 0; i < this->count(); i++ )
    {
        if( (color==Card::NOCOLOR || at(i)->color()==color) && (type==Card::NOSTICH || at(i)->card()==type) )
            found->append(at(i));
    }

    return found;
}

bool CardList::contains( int color, int type )
{
    return contains( color + type );
}

bool CardList::contains( int cardid )
{
    for( int i = 0; i < this->count(); i++ )
    {
        if( at(i)->id() == cardid )
            return true;
    }
    return false;
}

void CardList::RemoveCards(CardList* itemsToRem)
{
    Card *card;
    Card *thiscard;
    for( int i = 0; i < itemsToRem->count(); i++ )
    {
        card = itemsToRem->at( i );
        for( int t = 0; t < this->count(); t++ )
        {
            thiscard = this->at( t );
            if(thiscard->isEqual(card))
                removeCard( thiscard );
        }
    }
}

void CardList::sort(eval_func eval, void *param)
{
    int i, n;
    int x, y;
    Card *tmp;

    for(n=count()-1;n>0;n--)
    {
        for(i=0;i<n;i++)
        {
            //cmp=compareItems( at(i), at(i+1) );
            //if(cmp>0)
            x=(*eval)(at(i), param);
            y=(*eval)(at(i+1), param);
//          qDebug("%d, %d: %d, %d\n", i, i+1, x, y);
            if(x>y)
            {
                tmp=at(i);
                replace(i, at(i+1));
                replace(i+1, tmp);
            }
        }
    }
}

int* CardList::toIntList()
{
    int* list = new int[count()+1];
    int i;

    for( i=0; i<count(); i++ )
        list[i] = at(i)->id();
    list[i] = 0;

    return list;
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
