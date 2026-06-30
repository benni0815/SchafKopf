// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "card.h"

#include <QGraphicsPixmapItem>
#include <QPixmap>

#define CANVASCARD 1000

class CanvasCard : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    public:
        static QPixmap frontsidePixmap(Card::Id id);

        /// return a pixmap for the background of a card.
        static const QPixmap& backsidePixmap();

        CanvasCard();
        ~CanvasCard() override = default;

        void setCard(Card::Id cardId);
        Card::Id card() const { return m_card; }

        void setFrontVisible( bool b );

        void animatedMove();
        void setDestination( int x, int y );

        int type() const override { return CANVASCARD; }

        /** mark this card as forbidden to play
          * in the UI
          */
        void forbidden();

    private:
        void updatePixmap();
        void reloadPixmaps();

        QPixmap frontPixmap;
        QPixmap forbiddenFrontPixmap;

        bool m_visible{false};
        bool m_forbidden{false};

        int tox{0};
        int toy{0};
        QTimer* timer{};

        Card::Id m_card{Card::NoStich, Card::NoColor};

    private slots:
        void disableForbidden();
        void moveLoop();
        void cardDeckChanged();
};
