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
class Player;
class QString;
/**
Keeps all information on the current game (e.g. wenz or rufspiel)

@author Dominik Seichter
*/

class GameInfo{
    public:
        enum __mode { STICHT, WENZ, GEIER, RUFSPIEL, RAMSCH };
        
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

        bool operator>( GameInfo info );
        
    private:
        int m_color;
        __mode m_mode;
        Player *m_spieler;
        Player *m_mitspieler;
};

#endif
