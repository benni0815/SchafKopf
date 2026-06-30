// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "card.h"
#include "schafkopfdef.h"

#include <QGraphicsView>
#include <QEventLoop>


class CanvasCard;
class Game;
class CanvasPlayer;
class CanvasText;
class QMouseEvent;
class QResizeEvent;

class GameCanvas : public QGraphicsView
{
    Q_OBJECT
    
    public:
        GameCanvas(QGraphicsScene* gs, QWidget *parent = nullptr);
        ~GameCanvas() override;
        
        /** the user has to click a card to play
          */
        void getCard(Card::Id* card);

        /** the user played card, which
          * is forbidden to play!
          */
        void cardForbidden(Card::Id cardid);

        /** ask the user a question and allow him to make a decision
          */
        void questionYesNo(const QString& message, int* result);
        
        /** display an information message to the user
          */
        void information( const QString & message );

        /** displays a bubble with the text the computerplayers say
          */
        void Bubble(unsigned int playerid, const QString& message);
        
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
        void updateBackground();

        void exitLoop();

    public slots:
        void setPlayerCards(unsigned int id, const std::vector<Card::Id>& cardIds);
        void setPlayerName(unsigned int id, const QString& name);
        void redrawPlayers();
        void slotPlayerPlayedCard(unsigned int player, Card::Id cardid);
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
        void resizeEvent(QResizeEvent* r) override;
        void resizeBackground();

        void mousePressEvent(QMouseEvent*) override;
        void mouseReleaseEvent(QMouseEvent*) override;
        void keyPressEvent(QKeyEvent*) override;
        void focusInEvent(QFocusEvent*) override;
        void focusOutEvent(QFocusEvent*) override;
        QPoint getStichPosition( int player );
        int getStichRotation( int player );
        CanvasPlayer* humanPlayer() const;
        
    private:
        int m_result{0};
        Card::Id m_resultCard{};
        
        QList<QGraphicsItem*> m_focus_list;
        CanvasPlayer* m_players[PLAYERS];
        CanvasCard* m_stich[PLAYERS];
        
        // currently clicked item
        QGraphicsItem* m_item{};

        QGraphicsSimpleTextItem* m_message{};
        CanvasText* m_yes{};
        CanvasText* m_no{};
        CanvasText* m_ok{};

        QImage ImgBack;
        bool loadOK{false};

        QEventLoop m_loop;
};
