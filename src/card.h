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
#ifndef CARD_H
#define CARD_H

#include <qstring.h>

class QPixmap;
/**
This class represents one card.

@author Dominik Seichter
*/
class Card{
    public:
        // TODO: find better names
        enum type { ASS = 1, ZEHN = 17, KOENIG = 5, OBER = 9, UNTER = 13, NEUN = 21, ACHT = 25, SIEBEN = 29 };
        enum color { NOCOLOR = -1, EICHEL = 0, GRAS = 1, HERZ = 2, SCHELLEN = 3 };

        Card( const enum type t, const enum color c );
        ~Card();

        /** return a pixmap of the card 
          */
        QPixmap* pixmap();
        
        /** return a pixmap for the background of a card.
          */
        static QPixmap* backgroundPixmap();
        
        /** return how much points the card has.
          * e.g. king = 4 points
          */
        int points() const { return m_points; }
        
        /** return the color of the card.
          * e.g. heart.
          */
        int color() const { return m_color; }
        
        /** return the type of the card, e.g. ASS
          */
        int card() const { return m_card; }
        
    private:
        int m_points;
        int m_color;
        int m_card;
        
        static void getDecks( QString & dir, QString & deck );

        QPixmap* m_pixmap;
        static QPixmap* m_background;
};

#endif
