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

#include <iostream>

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

void CardList::sort()
{
	int i, n, cmp;
	Card *tmp;
	
	for(n=count()-1;n>0;n--)
	{
		for(i=0;i<n;i++)
		{
			cmp=compareItems( at(i), at(i+1) );
			if(cmp>0)
			{
				tmp=at(i);
				replace(i, at(i+1));
				replace(i+1, tmp);	
			}
		}
	}
}

int CardList::compareItems ( Card* Item1, Card* Item2 )
{
	if(Item1->card()==Card::OBER)
	{
		if(Item2->card()==Card::OBER)
		{
			if(Item1->color()<=Item2->color())
				return 1;
			else
				return -1;
		}
		else
			return 1;
	}
	else if(Item2->card()==Card::OBER)
		return -1;
	if(Item1->card()==Card::UNTER)
	{
		if(Item2->card()==Card::UNTER)
		{
			if(Item1->color()<=Item2->color())
				return 1;
			else
				return -1;
		}
		else
			return 1;
	}
	else if(Item2->card()==Card::UNTER)
		return -1;
	else if(Item1->color()==Card::HERZ)
	{
		if(Item2->color()==Card::HERZ)
		{
			if(Item1->card()<=Item2->card())
				return 1;
			else
				return -1;
		}
		else
			return 1;
	}
	else if(Item2->color()==Card::HERZ)
		return -1;
	else if(Item1->color()==Item2->color())
	{
		if(Item1->card()<=Item2->card())
			return 1;
		else
			return -1;
	}
	else if(Item1->color()<Item2->color())
	{
		return 1;
	}
	else
	{
		return -1;
	}
	return 0;
	/*
	Card *tmp;

	if(Item2->card()==Card::OBER || Item2->card()==Card::UNTER)
	{
		tmp=Item1;
		Item1=Item2;
		Item2=tmp;
	}
	if(Item1->card()!=Card::OBER && Item1->card()!=Card::UNTER &&  Item2->color()==Card::HERZ)
	{
		tmp=Item1;
		Item1=Item2;
		Item2=tmp;
	}
	// Sortierung abhängig vom spiel implementier ich morgen. Lass dich überraschen =:-)
	if(Item1->card()==Card::OBER)
	{
		if(Item2->card()==Card::OBER && Item1->color()>Item2->color())
		{
			std::cout << "Ober"  << std::endl;
			return 1;
		}
		else
			return -1;
	}
	 else if(Item1->card()==Card::UNTER)
	{
		if(Item2->card()==Card::OBER)
		{
			std::cout << "Ober 2"  << std::endl;
			return 1;
		}
		else if(Item2->card()==Card::UNTER && Item1->color()>Item2->color())
		{
			std::cout << "Unter"  << std::endl;
			return 1;
		}
		else
			return -1;
	}
	else if(Item1->color()==Card::HERZ)
	{
		if(Item2->color()==Card::HERZ && Item1->card()>Item2->card())
		{
			std::cout << "Herz"  << std::endl;
			return 1;
		}
		return -1;
	}
	else if(Item1->color()==Item2->color())
	{
		if(Item1->card()>Item2->card())
		{
			std::cout << "Karte"  << std::endl;
			return 1;
		}
		return -1;
	}
	else if(Item1->color()>Item2->color())
	{
		std::cout << "Farbe"  << std::endl;
		return -1;
	}
	return 1;
	*/
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
