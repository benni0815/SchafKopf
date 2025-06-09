// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

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
