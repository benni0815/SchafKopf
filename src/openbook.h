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
class Player;

/**
Our OpenBook implementation.

@author Dominik Seichter
*/

class OpenBook : public QObject {    
    Q_OBJECT
    public:
        OpenBook( Player* player, Game* game );
        ~OpenBook();

        CardList* possibleCards();
        
    private slots:
        void cardPlayed( unsigned int player, Card* card );
        
    private:
        void nichtSpieler( CardList* list );
        void spieler( CardList* list );
        
        Game* m_game;
        Player* m_self;
        bool m_player;
};

#endif
