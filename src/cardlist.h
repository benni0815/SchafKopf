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

#ifndef CARDLIST_H
#define CARDLIST_H

#include <qptrlist.h>
#include "card.h"

typedef int (*eval_func)(Card *card, void *param);

/**
 * By default CardList does not delete its children
 * on destruction!
 * @author Dominik Seichter
 */

class CardList : public QPtrList<Card>
{
    public:
        CardList();

        /** initialize this card list with a default card
          * deck of 32 cards.
          * The CardList does now own all elements, i.e. they
          * are deleted on destruction.
          */
        void init();

		/** generate a randomized list of 32 cards
		*/
        void randomize();

		/** Evaluate the all cards in this list
		*/
        int points();
        
        /** append CardList @p list to this CardList
          */
        void appendList( CardList* list );

        /** returns wether the card color and typedef
          * is in the list
          */
        bool contains( int color, int type );
        
		CardList* FindCards(int color, int type);
		void RemoveCards(CardList* itemsToRem);
	
		void sort(eval_func eval, void *param);

	signals:
		void changeEvent(void);
};

#endif
