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
 *   along with this program; if not, writ:e to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CARD_H
#define CARD_H

class QPixmap;
class Player;

/**
This class represents one card.

@author Dominik Seichter
*/
class Card {
    public:
        // TODO: find better names, Edit bei lenz: done =;-)
        // DS: renamed to avoid name clashes
        enum EType { /*ASS*/NOSTICH=-1, SAU = 1, ZEHN = 17, KOENIG = 5, OBER = 9, UNTER = 13, NEUN = 21, ACHT = 25, SIEBEN = 29 };
        enum EColor { NOCOLOR = -1, EICHEL = 0, GRAS = 1, HERZ = 2, SCHELLEN = 3 };

        Card( const int id );
        Card( const enum EType t, const enum EColor c );
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

	/** set the owner of the card.
	  * only do this when the cards are given,
	  * not after a trick was made!
	  */
        const Player* owner() const { return m_owner; }
      	
        /** return the owner of this card.
          * we need this to be able to know who
          * played this card of the trick was made.
          */
        void setOwner( Player* owner ) { m_owner = owner; }

        /** this operator< is not quite correct,
          * it just tells wether ass is bigger than 10 or 10 
          * is bigger than king, but does not care
          * for color or what is currently played.
          * So it does not care for Wenz or Solo's.
          * it is used in Game::highesCard
          */
        bool operator<( Card* c );

        bool isEqual(Card* othercard);

        /** a id(). Using this id the card can be quickly identified
          * as the id() is calculated using color and type.
          */
        int id() const;
        
        /** Call this whenever the carddeck has changed and
          * the internal pixmap should be reloaded.
          */
        void cardDeckChanged();
        
    private:
        void init( const enum EType t, const enum EColor c  );
        
    private:
        Player* m_owner;
        
        int m_points;
        int m_color;
        int m_card;

        QPixmap* m_pixmap;
        static QPixmap* m_background;
};

#endif
