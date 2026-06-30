// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "card.h"

class CardList;
class Player;
class QString;

/**
Keeps all information on the current game (e.g. wenz or rufspiel)

@author Dominik Seichter
*/

class GameInfo{
    public:
        enum Mode { STICHT, WENZ, GEIER, RUFSPIEL, RAMSCH, DACHS };
        GameInfo() = default;
        ~GameInfo() = default;

        void setValid( bool b ) { m_valid = b; }
        bool isValid() const { return m_valid; }

        Card::Color color() const { return m_color; }
        void setColor(Card::Color c) { m_color = c; }

        Mode mode() const { return m_mode; }
        void setMode( int m ) { m_mode = static_cast<Mode>(m); }

        Player* spieler() const { return m_spieler; }
        void setSpieler( Player* p ) { m_spieler = p; }

        Player* mitspieler() const { return m_mitspieler; }
        void setMitspieler( Player* p ) { m_mitspieler = p; }

        const QString toString() const;
        int weight( Card* card ) const;
        bool istTrumpf(Card *card ) const;     

        /** returns a number between 1 and 32 according to the worth of the card. 
          *I.e. the highest card will get a value of 32 assigned
          * be sure that autodelete is turned of when using this function!
          */
        int evalCard(Card *card);


        /** calculate the number of "laufende" ticks
          */
        int laufende();

        /** Is a player with the cards @p cards allowed to playe
          * the game @p mode with the color @p color.
          */
        static bool isAllowed(const CardList& cards, int mode, Card::Color color);

        bool operator>(const GameInfo& info);

    private:
        bool m_valid{false};
        Card::Color m_color{Card::NoColor};
        Mode m_mode{GameInfo::STICHT};
        Player* m_spieler{};
        Player* m_mitspieler{};
};
