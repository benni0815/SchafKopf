// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "types.h"

#include <vector>


class CardList;
class Player;
class QString;

/**
Keeps all information on the current game (e.g. wenz or rufspiel)

@author Dominik Seichter
*/

class GameInfo{
    public:
        static bool istTrumpf(CardId card, GameVariant game);

        GameInfo() = default;
        GameInfo(GameVariant game);
        ~GameInfo() = default;

        void setValid( bool b ) { m_valid = b; }
        bool isValid() const { return m_valid; }

        CardId::Color color() const { return m_game.color; }
        void setColor(CardId::Color c) { m_game.color = c; }

        GameVariant::Mode mode() const { return m_game.mode; }
        void setMode(GameVariant::Mode m) { m_game.mode = m; }

        Player* spieler() const { return m_spieler; }
        void setSpieler( Player* p ) { m_spieler = p; }

        Player* mitspieler() const { return m_mitspieler; }
        void setMitspieler( Player* p ) { m_mitspieler = p; }

        const QString toString() const;
        static int weight(CardId card, GameVariant game);

        bool istTrumpf(CardId card) const;

        /** returns a number between 1 and 32 according to the worth of the card.
          *I.e. the highest card will get a value of 32 assigned
          * be sure that autodelete is turned of when using this function!
          */
        int evalCard(CardId card) const;

        /** calculate the number of "laufende" ticks
          */
        int laufende();

        /** Is a player with the cards @p cards allowed to playe
          * the game @p mode with the color @p color.
          */
        static bool isAllowed(const CardList& cards, GameVariant game);
        static bool isAllowed(const std::vector<CardId>& cards, GameVariant game);

        bool operator>(const GameInfo& info);

    private:
        bool m_valid{false};
        GameVariant m_game{GameVariant::STICHT, CardId::NoColor};
        Player* m_spieler{};
        Player* m_mitspieler{};
};
