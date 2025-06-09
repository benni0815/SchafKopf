// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "player.h"
#include "card.h"
#include "gameinfo.h"

class OpenBook;
class CardList;

class ComputerPlayer : public Player
{
    public:
        ComputerPlayer(unsigned int id,Game* game);
        ~ComputerPlayer();

        void init();
        void klopfen();
        Card *play();
        GameInfo* gameInfo( bool force = false );

        int rtti() const { return COMPUTER; }

        Card *findHighestCard(CardList *cards);
        Card *findSchmiere(CardList *cards);        // Nice name, isn't it? :)

        Card *findCheapestCard(CardList *cards);

        /** Finde die niedrigste Karte aus @p cards um @p highest zu stechen
          */
        Card *findLowestPossibleCard(Card* highest, CardList *cards);

        /** gibt zurueck wievile karten eines typs noch im
          * spiel sind.
          * Dabei wird die farbe als farbe im Spiel betrachtet:
          * I.e. der EichelOber zaehlt als trumpf und nicht als eichel.
          *
          * Um truempfte zu zaehlen bitte NOCOLOR angeben
          */
        int cardsStillInGame( int c );

    private:
        Card *findCardToPlay(CardList *cards);
        bool canMakeStich(CardList *cards);

        /** Prueft ob der aktuelle Stich der eigenen Spieler Partei
         *  gehoert oder nicht. Dabei werden nur die aktuell
         *  gespielten Karten gespielt bewertet und nicht evtl. hoehere
         *  Karten die die Gegenspielerpartei spaeter noch spielen koennte
         *
         *  @returns true  wenn der stich der eigenen Partei gehoert
         *  @returns false wenn der stich dem gegner gehoert oder
         *                 noch keine Karte gespielt wurde
         */
        bool ownStich();

        /** returns the highest trump that is still
         *  in the game not counting the own cards
         *  of the player.
         *  NULL is returned if there is no trump in
         *  the game anymore.
         */
        Card *highestTrumpfInGame();

        int myTrumpfs();

    //private slots:
        void cardPlayed(unsigned int player, Card *c);

    private:
        struct game_data {
            int trumpf;
            int fehlfarbe;
            int weight;
            GameInfo info;
        };

        CardList *m_playedCards[4];
        CardList *m_angespielt;
        OpenBook* book;
        int mitspieler;
};
