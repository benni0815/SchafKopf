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
#ifndef OPENBOOK_H
#define OPENBOOK_H

#include <qobject.h>

class Card;
class CardList;
class Game;
class ComputerPlayer;

class RuleBook : public QObject {
    Q_OBJECT
    public:
        RuleBook( ComputerPlayer* player, Game* game );
        virtual ~RuleBook();

        virtual CardList* possibleCards() = 0;

    protected:
        Game* m_game;
        ComputerPlayer* m_self;
        bool m_player;
};

/**
 * Our OpenBook implementation.
 * Finds a card which can be played as a first card.
 *
 * @author Dominik Seichter
 */

class OpenBook : public RuleBook {    
    public:
        OpenBook( ComputerPlayer* player, Game* game );
        
        CardList* possibleCards();
};

/**
 * Contains strategies when we are the last player
 * to a play a card
 * If this class finds a good card to player
 * it is really safe to play it.
 *
 * @author Dominik Seichter
 */
class StrategyBook : public RuleBook {
    public:
        StrategyBook( ComputerPlayer* player, Game* game );
        
        CardList* possibleCards();
};

#endif
