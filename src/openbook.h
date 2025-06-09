// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>


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
