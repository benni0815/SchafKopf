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
#ifndef CANVASCARD_H
#define CANVASCARD_H

#include <qcanvas.h>

#define CANVASCARD 1000

class Card;
class QPainter;

class CanvasCard : public QObject, public QCanvasRectangle
{
	Q_OBJECT
    public:
        CanvasCard(QCanvas*c);
        CanvasCard(Card* card,QCanvas*c);
        ~CanvasCard();

        void setCard(Card* card);
        Card* card() const { return m_card; }
        
        void setFrontVisible( bool b );
        void setRotation( int d );
        
        int rtti() const { return CANVASCARD; }
        
        /** mark this card as forbidden to play
          * in the UI
          */
        void forbidden();
    private:
        void draw( QPainter & p );
        
        QPoint m_point;
        
        bool m_visible;
        bool m_forbidden;
        int m_rotation;
        
        Card* m_card;
	
	private slots:
		void disableForbidden();
};

#endif
