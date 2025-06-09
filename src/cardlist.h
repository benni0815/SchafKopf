// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "card.h"

typedef int (*eval_func)(Card *card, void *param);

/**
 * By default CardList does not delete its children
 * on destruction!
 * @author Dominik Seichter
 */

class CardList : public QList<Card*>
{
    public:
        CardList();

        /** Creates a card list from a zero terminated list of integers
          * as you can create it using toIntList()
          */
        CardList( int* cards );

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
        bool contains( int cardid );

        void removeCard( uint index );
        void removeCard( Card* card );

        CardList* FindCards(int color, int type);
        void RemoveCards(CardList* itemsToRem);

        void sort(eval_func eval, void *param);

        /** creates a zero terminated list of integers, every
          * int has the id of one card assigned.
          */
        int* toIntList();

        void setAutoDelete( bool autoDelete ) { m_autoDelete = autoDelete; }

    private:
        bool m_autoDelete;
};
