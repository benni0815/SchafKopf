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
#ifndef RESULTS_H
#define RESULTS_H

#include <qptrlist.h>
#include <qstring.h>

class Game;
class GameInfo;
class Player;

/**
This interface implements the basic functions, 
to get game results and find out how many points 
or money who has to give to whom.

@author Dominik Seichter
*/

class Results{
    public:
        Results();
        ~Results() {}
        
        void setLaufende( int l ) { m_laufende = l; }
        void setGame( Game* game );
        
        /** get a message of the game results, which can be displayed
          * to the user.
          */
        QString result();
        double points( Player* player );
        QString formatedPoints( Player* player );
        
        double klopfen(int anzahl, double betrag);

    private:
        void init();
        void parse();
    
    private:        

        QPtrList<Player> m_winners;
        QPtrList<Player> m_losers;
        
        GameInfo* m_gameinfo;
        Game* m_game;
        
        int m_laufende;
        
        int m_points;
        bool m_schneider;
        bool m_schwarz;
};

#endif
