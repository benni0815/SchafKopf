// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "schafkopfdef.h"

#include <QGraphicsView>
#include <QEventLoop>


class CanvasCard;
class Card;
class Game;
class CanvasPlayer;
class CanvasText;
class QMouseEvent;
class QResizeEvent;

class GameCanvas : public QGraphicsView
{
    Q_OBJECT
    
    public:
        GameCanvas(QGraphicsScene* gs, QWidget *parent = 0 );
        ~GameCanvas();
        
        /** the user has to click a card to play
          */
        int getCard();
        
        /** the user played card, which
          * is forbidden to play!
          */
        void cardForbidden( int cardid );

        /** ask the user a question and allow him to make a decision
          */
        bool questionYesNo( const QString & message );
        
        /** display an information message to the user
          */
        void information( const QString & message );

        /** displays a bubble with the text the computerplayers say
          */
        void Bubble( const QString & message, unsigned int playerid );
        
        /** set the canvas player with the @p id to have doubled in this game
          */
        void playerHasDoubled( unsigned int id, bool value );
        
        /** Reset all data of the canvas players to default values 
          */
        void resetPlayers();
        /** remove all cards from the canvas player cardlists
          * so that they are going to hide themselves
          */
        void resetPlayerCards();

        void playerIsLast( unsigned int id );
        void setPlayerName( unsigned int id, const QString & name );
        void setPlayerCards( unsigned int id, int* cards );
        
        void updateBackground();

        void exitLoop();

    public slots:
        void redrawPlayers();
        //void drawContents ( QPainter * p, int cx, int cy, int cw, int ch );
        void slotPlayerPlayedCard( unsigned int player, int cardid );
        void slotPlayerMadeStich(unsigned int);
                
    signals:
        void clicked( QGraphicsItem* item );
        void playCard( Card* card );
        
    private slots:
        /** Position the cards on the screen correctly
          */
        void positionObjects();
        
        void cardClicked( QGraphicsItem* item );
        void yesNoClicked( QGraphicsItem* item );
        
    protected:
        void resizeEvent( QResizeEvent *r );
        void resizeBackground();

        void mousePressEvent(QMouseEvent*);
        void mouseReleaseEvent(QMouseEvent*);
        void keyPressEvent(QKeyEvent*);
        void focusInEvent(QFocusEvent*);
        void focusOutEvent(QFocusEvent*);
        QPoint getStichPosition( int player );
        int getStichRotation( int player );
        CanvasPlayer* humanPlayer() const;
        
    private:        
        int m_result;
        
        QList<QGraphicsItem*> m_focus_list;
        CanvasPlayer* m_players[PLAYERS];
        CanvasCard* m_stich[PLAYERS];
        
        QGraphicsItem* m_item; // currently clicked item

        QGraphicsSimpleTextItem* m_message;
        CanvasText* m_yes;
        CanvasText* m_no;
        CanvasText* m_ok;

        QImage ImgBack;
        bool loadOK;

        QEventLoop m_loop;
};
