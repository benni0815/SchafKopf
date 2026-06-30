// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "player.h"
#include "card.h"
#include "gameinfo.h"
#include "openbook.h"


class CardList;

class ComputerPlayer : public Player
{
    public:
        ComputerPlayer(unsigned int id,Game* game);
        ~ComputerPlayer() override = default;

        void init() override;
        void klopfen() override;
        Card *play() override;
        std::optional<GameInfo> gameInfo(bool force = false) override;

        Card* findHighestCard(const CardList& cards) const;
        Card* findSchmiere(const CardList& cards) const;

        Card* findCheapestCard(const CardList& cards) const;

        /** Finde die niedrigste Karte aus @p cards um @p highest zu stechen
          */
        Card* findLowestPossibleCard(Card* highest, const CardList& cards) const;

        /** gibt zurueck wievile karten eines typs noch im
          * spiel sind.
          * Dabei wird die farbe als farbe im Spiel betrachtet:
          * I.e. der EichelOber zaehlt als trumpf und nicht als eichel.
          *
          * Um truempfte zu zaehlen bitte NOCOLOR angeben
          */
        int cardsStillInGame( int c );

    private:
        Card* findCardToPlay(const CardList& cards);
        bool canMakeStich(const CardList& cards);

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

        void cardPlayed(unsigned int player, Card *c);

    private:
        void log(const QString& msg) const;

        struct game_data {
            int trumpf;
            int fehlfarbe;
            int weight;
            GameInfo info;
        };

        OpenBook m_openBook;
        int mitspieler{-1};
};
