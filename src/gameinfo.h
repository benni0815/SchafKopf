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
#ifndef GAMEINFO_H
#define GAMEINFO_H

class Card;
class CardList;
class Player;
class QString;
/**
Keeps all information on the current game (e.g. wenz or rufspiel)

@author Dominik Seichter
*/

class GameInfo{
    public:
        enum __mode { STICHT, WENZ, GEIER, RUFSPIEL, RAMSCH, DACHS };
        GameInfo();
        ~GameInfo();

        int color() const { return m_color; }
        void setColor( int c ) { m_color = c; }
        
        __mode mode() const { return m_mode; }
        void setMode( int m ) { m_mode = (__mode)m; }
        
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
        static int evalCard(Card *card, GameInfo *gameinfo);

        
        /** calculate the number of "laufende" ticks
          */
        int laufende();
        
        /** Is a player with the cards @p cards allowed to playe
          * the game @p mode with the color @p color.
          */
        static bool isAllowed( CardList* cards, int mode, int color );

        bool operator>( GameInfo info );
        
    private:
        int m_color;
        __mode m_mode;
        Player *m_spieler;
        Player *m_mitspieler;
};

#endif
