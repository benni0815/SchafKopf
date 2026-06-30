// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "card.h"
#include "schafkopfdef.h"

#include <QTimer>

#include <memory>


class Card;
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
        ~CanvasPlayer() = default;
        
        void position();
        void init();
        
        unsigned int id() const;
        bool isHuman() const;
        
        void setHasDoubled( bool h );
        void setLast( bool l );
        void setName( const QString & name );
        void setCards(const std::vector<Card::Id>& cards);
        void say( const QString & message, unsigned int playerid );
        void hideBubble();
        
        CanvasCard* canvasCard( int index );
        void cardPlayed(Card::Id id);
        CanvasCard* hasCard(Card::Id cardid) const;

    private:
        void create();

        unsigned int m_id{0};
        int m_position{0};
        bool m_has_doubled{false};
        bool m_is_last{false};
        bool m_is_human{false};
        QString m_player_name;
        
        QGraphicsScene* m_scene{};
        QGraphicsSimpleTextItem* m_name{};
        std::unique_ptr<QLabel> m_bubbleLabel;
        QGraphicsProxyWidget* m_bubble{};
        CanvasCard* m_items[NUMCARDS]{};
        std::vector<Card::Id> m_cards;
};
