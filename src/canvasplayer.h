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

#include "schafkopfdef.h"

#include <qstring.h>

class Card;
class CardList;
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
        CanvasPlayer( int i, QCanvas* canvas );
        ~CanvasPlayer();
        
        void position();
        void init();
        
        unsigned int id() const;
        bool isHuman() const;
        
        void setHasDoubled( bool h );
        void setLast( bool l );
        void setName( const QString & name );
        void setCards( CardList* cards );
        
        CanvasCard* canvasCard( int index );
        void cardPlayed( Card* c );
        CanvasCard* hasCard( int cardid ) const; 
        
    private:
        void create();
        
        unsigned int m_id;
        int m_position;
        bool m_has_doubled;
        bool m_is_last;
        bool m_is_human;
        QString m_player_name;
        
        QCanvas* m_canvas;
        QCanvasText* m_name;
        CanvasCard* m_items[NUMCARDS];
        CardList* m_cards;
};

#endif
