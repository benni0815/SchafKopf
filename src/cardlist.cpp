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


#include <kapplication.h>

CardList::CardList()
{
    setAutoDelete( false );
}

void CardList::init()
{
    setAutoDelete( true );
    clear();

    for( int i = 0; i < CARD_CNT ; i += 4  )
        for( int z = Card::EICHEL; z <= Card::SCHELLEN; z++ )
            append( new Card( (enum Card::type)(i+1), (enum Card::color)z ) );
}

int CardList::points()
{
    int tmp = 0;
    for( unsigned int i = 0; i < this->count(); i++ )
        tmp += at( i )->points();
        
    return tmp;
}

void CardList::randomize()
{
    int rnd[CARD_CNT];
    int i, a, rval;
        
    init();
    for(i=0;i<CARD_CNT;i++)
    {
   		rval=KApplication::random()%CARD_CNT;
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
        (void)take(0); // we always remove the first item
                       // Do not use remove(), as autoDeletion is
                       // enabled!!! 
}

/*
	typedef bool (*__isTrumpf)(Card* Karte);
	CardList* FindCards(int color, int type, __isTrumpf _trumpf_func);*/

CardList* CardList::FindCards(int color, int type)
{
	CardList* found=new CardList();
    	for( unsigned int i = 0; i < this->count(); i++ )
	{
	if( (color==Card::NOCOLOR||at(i)->color()==color) && (type==Card::NOSTICH||at(i)->card()==type) )	found->append(at(i));
	}

	return found;
}


void CardList::RemoveCards(CardList* itemsToRem)
{
	Card *card;
	Card *thiscard;
	for ( card = itemsToRem->first(); card; card = itemsToRem->next() )
    	{
		for(thiscard = this->first(); thiscard; thiscard = this->next() )
		{
		if(thiscard->isEqual(card)) this->removeNode(this->currentNode());
		}
	}
}

int CardList::compareItems ( QPtrCollection::Item Item1, QPtrCollection::Item Item2 )
{
	Card *tmp, *item1=(Card *)Item1, *item2=(Card *)Item2;

	if(item2->card()==Card::OBER || item2->card()==Card::UNTER)
	{
		tmp=item1;
		item1=item2;
		item2=tmp;
	}
	if(item1->card()!=Card::OBER && item1->card()!=Card::UNTER &&  item2->color()==Card::HERZ)
	{
		tmp=item1;
		item1=item2;
		item2=tmp;
	}
	// Sortierung abhängig vom spiel implementier ich morgen. Lass dich überraschen =:-)
	if(item1->card()==Card::OBER)
	{
		if(item2->card()==Card::OBER && item1->color()>item2->color())
			return 1;
		else
			return -1;
	}
	 else if(item1->card()==Card::UNTER)
	{
		if(item2->card()==Card::OBER)
			return 1;
		else if(item2->card()==Card::UNTER && item1->color()>item2->color())
			return 1;
		else
			return -1;
	}
	else if(item1->color()==Card::HERZ)
	{
		if(item2->color()==Card::HERZ && item1->card()>item2->card())
			return 1;
		return -1;
	}
	else if(item1->color()==item2->color())
	{
		if(item1->card()>item2->card())
			return 1;
		return -1;
	}
	else if(item1->color()>item2->color())
		return -1;
	return 1;
	/*
	if(item1->card()==Card::OBER)
	{
		if(item2->card()==Card::OBER && item1->color()>item2->color())
			return -1;
		return 1;
	}
	else if(item1->card()==Card::UNTER)
	{
		if(item2->card()==Card::OBER)
			return -1;
		else if(item2->card()==Card::UNTER && item1->color()>item2->color())
			return -1;
		else return 1;
	}
	else if(item1->color()==Card::HERZ && item2->color()==Card::HERZ)
	{
		if(item1->card()>item2->card())
			return -1;
		return 1;
	}
	else if(item1->color()==Card::HERZ)
		return 1;
	else if(item1->card()>item2->card() && item1->color()<=item2->color())
		return -1;
	return 1;
	*/
}
