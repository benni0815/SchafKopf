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
#ifndef CANVASPLAYER_H
#define CANVASPLAYER_H

// DISTANCE from the border
#define DIST 20

class Card;
class CanvasCard;
class Player;
class QCanvas;
class QCanvasText;

/**
Represents a player on the canvas

@author Dominik Seichter
*/
class CanvasPlayer{
    public:
        CanvasPlayer( QCanvas* canvas );
        CanvasPlayer( int i, Player* player, QCanvas* canvas );
        ~CanvasPlayer();
        
        void position( int i );
		void init(int i);
        
        void setPlayer( int, Player* player );
        
        void cardPlayed( Card* c );
        CanvasCard* hasCard( Card* c ) const; 
        Player* player() const { return m_player; }
        
    private:
		QCanvas* m_canvas;
        QCanvasText* m_name;
        CanvasCard* m_items[8];
        
        Player* m_player;
};

#endif
