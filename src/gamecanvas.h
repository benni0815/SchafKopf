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
#include "game.h"

class QCanvasItemList;
class QResizeEvent;

class GameCanvas : public QCanvasView
{
    Q_OBJECT
    public:
        GameCanvas(QCanvas* c,QWidget *parent = 0, const char *name = 0);
        ~GameCanvas();
        
        void setGame( Game* game );
    
    private slots:
        void redrawAll();
        
    private:
        /** Create QCanvasItem's for all Cards 
          */
        void createCards();
        /** Position the cards on the screen correctly
          */
        void lineupCards();

        void resizeEvent( QResizeEvent *r );
        
        QCanvasItemList* m_items[PLAYERS];
        
        Game* m_game;
};

#endif
