// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "schafkopfdef.h"

#include <QTimer>

#include <memory>


class Card;
class CardList;
class CanvasCard;
class Player;
class QLabel;
class QGraphicsScene;
class QGraphicsView;
class QGraphicsSimpleTextItem;
class QGraphicsProxyWidget;

/**
Represents a player on the canvas

@author Dominik Seichter
*/
class CanvasPlayer{
    public:
        CanvasPlayer( int i, QGraphicsScene* scene );
        ~CanvasPlayer();
        
        void position();
        void init();
        
        unsigned int id() const;
        bool isHuman() const;
        
        void setHasDoubled( bool h );
        void setLast( bool l );
        void setName( const QString & name );
        void setCards( CardList* cards );
        void say( const QString & message, unsigned int playerid );
        void hideBubble();
        
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
        
        QGraphicsScene* m_scene;
        QGraphicsSimpleTextItem* m_name;
        std::unique_ptr<QLabel> m_bubbleLabel;
        QGraphicsProxyWidget* m_bubble;
        QTimer m_bubbleTimer;
        CanvasCard* m_items[NUMCARDS];
        CardList* m_cards;
};
