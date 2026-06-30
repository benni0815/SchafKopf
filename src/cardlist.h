// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "card.h"


using EvalFunc = std::function<int(Card*)>;

/**
 * By default CardList does not delete its children
 * on destruction!
 * @author Dominik Seichter
 */

class CardList : public QList<Card*>
{
    public:
        CardList() = default;

        CardList(const std::vector<Card::Id>& cardIds);

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
        int points() const;

        /** append CardList @p list to this CardList
          */
        void appendList(const CardList& list);

        bool contains(Card::Id cardid) const;

        void removeCard( uint index );
        void removeCard( Card* card );

        CardList FindCards(Card::Color color, Card::Type type);
        void RemoveCards(const CardList& itemsToRem);

        void sort(EvalFunc eval);

        std::vector<Card::Id> toIds() const;

        void setAutoDelete( bool autoDelete ) { m_autoDelete = autoDelete; }

    private:
        bool m_autoDelete{false};
};
