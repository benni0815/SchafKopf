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
    int tmp;
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
        for(a=0;a<i;a++)
        {
            if(rnd[a]==rval)
            {
                i--;
                break;
            }
        }
        rnd[i]=rval;
    }
    for(i=0;i<CARD_CNT;i++)
        append(at(rnd[i]));
    while(count() > CARD_CNT )
        (void)take(0); // we always remove the first item
                       // Do not use remove(), as autoDeletion is
                       // enabled!!! 
}

