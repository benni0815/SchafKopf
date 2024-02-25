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
        bool istTrumpfFrei(int playerId);
        bool istFarbFrei(int playerId, int c);
        float gehtDurch(Card *card);
        float gegnerSticht(Card *card);

        /** returns the highest trump that is still
         *  in the game not counting the own cards
         *  of the player.
         *  NULL is returned if there is no trump in
         *  the game anymore.
         */
        Card *highestTrumpfInGame();

        int myTrumpfs();
        int trumpfsInGame();

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
