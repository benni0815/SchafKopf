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

#include "schafkopfdef.h"

#include <qcanvas.h>

class CanvasCard;
class Card;
class Game;
class CanvasPlayer;
class QCanvasItem;
class QCanvasItemList;
class QCanvasText;
class QMouseEvent;
class QResizeEvent;

class GameCanvas : public QCanvasView
{
    Q_OBJECT
    
    enum { YES, NO };
    
    public:
        GameCanvas(QCanvas* c,QWidget *parent = 0, const char *name = 0);
        ~GameCanvas();
        
        void setGame( Game* game );
   
        /** the user played card, which
          * is forbidden to play!
          */
        void cardForbidden( Card* card );

        /** ask the user a question and allow him to make a decision
          */
        bool questionYesNo( const QString & message );
        
        /** display an information message to the user
          */
        void information( const QString & message );
          
    public slots:
        void redrawPlayers();
        
    signals:
        void clicked( QCanvasItem* item );
        void playCard( Card* card );
        
    private slots:
        /** Position the cards on the screen correctly
          */
        void positionObjects(bool redraw=true);
        
        void cardClicked( QCanvasItem* item );
        void yesNoClicked( QCanvasItem* item );
        
        void slotPlayerPlayedCard( unsigned int player, Card *c );
        void slotPlayerMadeStich(unsigned int);
        
    protected:
        void resizeEvent( QResizeEvent *r );
        
        void contentsMousePressEvent(QMouseEvent*);
        void contentsMouseReleaseEvent(QMouseEvent*);
                
    private:
        /** Create QCanvasItem's for all Cards 
          */
        void createObjects();
        int m_result;
        QPoint getStichPosition( int player );
        
        CanvasPlayer* m_players[PLAYERS];
        CanvasCard* m_stich[PLAYERS];
        QCanvasItem* m_item; // currently clicked item

        QCanvasText* m_message;
        QCanvasText* m_yes;
        QCanvasText* m_no;
        QCanvasText* m_ok;
                
        Game* m_game;
};

#endif
