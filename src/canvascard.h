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

#include <QGraphicsPixmapItem>
#include <QPixmap>

#define CANVASCARD 1000

class Card;

class CanvasCard : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    public:
        CanvasCard();
        ~CanvasCard();

        void setCard(Card* card);
        Card* card() const { return m_card; }

        void setFrontVisible( bool b );

        void animatedMove();
        void setDestination( int x, int y );

        int type() const { return CANVASCARD; }

        /** mark this card as forbidden to play
          * in the UI
          */
        void forbidden();

    private:
        void updatePixmap();
        void reloadPixmaps();

        QPixmap frontPixmap;
        QPixmap forbiddenFrontPixmap;
        QPixmap backPixmap;

        bool m_visible;
        bool m_forbidden;

        int tox;
        int toy;
        QTimer *timer;

        Card* m_card;

    private slots:
        void disableForbidden();
        void moveLoop();
        void cardDeckChanged();
};

#endif
