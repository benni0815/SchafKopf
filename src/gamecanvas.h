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
#ifndef GAMECANVAS_H
#define GAMECANVAS_H

#include <qcanvas.h>

#ifndef PLAYERS
    #define PLAYERS 4
#endif

class Card;
class Game;
class QCanvasItem;
class QCanvasItemList;
class QMouseEvent;
class QResizeEvent;

class GameCanvas : public QCanvasView
{
    Q_OBJECT
    public:
        GameCanvas(QCanvas* c,QWidget *parent = 0, const char *name = 0);
        ~GameCanvas();
        
        void setGame( Game* game );
    
    signals:
        void clicked( QCanvasItem* item );
        void playCard( Card* card );
        
    private slots:
        void redrawAll();
        /** Position the cards on the screen correctly
          */
        void positionObjects();
        
        void cardClicked( QCanvasItem* item );

    protected:
        void resizeEvent( QResizeEvent *r );
        
        void contentsMousePressEvent(QMouseEvent*);
        void contentsMouseReleaseEvent(QMouseEvent*);
                
    private:
        /** Create QCanvasItem's for all Cards 
          */
        void createObjects();
        void clearObjects();
      
        QCanvasItemList* m_items[PLAYERS];
        QCanvasItem* m_item; // currently clicked item
        
        Game* m_game;
};

#endif
