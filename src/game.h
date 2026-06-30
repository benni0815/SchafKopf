// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "schafkopfdef.h"

#include "cardlist.h"
#include "gameinfo.h"

#include <QThread>


class Player;
class GameCanvas;

/**
@author Dominik Seichter
*/
class Game : public QThread
{
    Q_OBJECT

    public:
        Game(QSemaphore* semaphore, QObject *parent);
        ~Game() override;

        void gameLoop();
        const CardList& currStich() const;
        inline CardList *allCards() { return &m_allcards; }
        const CardList& playedCards() const { return m_playedcards; }
        GameInfo *gameInfo();

        Player* findId( unsigned int id ) const;
        Player* findIndex( unsigned int index ) const;

        /** @returns the player who is the next to play
         *  a card in this turn or NULL if there is no player left.
         *
         *  This should only be called from Player::play()
         */
        inline Player* nextPlayer() const { return m_nextPlayer; };

        bool isTerminated() const { return terminated; }
        int highestCard(CardList* list = nullptr);
        bool isHigher( Card* card, Card* high );

        /** returns how many players have doubled
          */
        int timesDoubled();
        int timesThrownTogether();

        /** post a event to the parent object
          */
        void* postEvent(EAction action, unsigned int playerid = 0, const std::vector<int>& cardids = {},
                        const QString& data = QString(), bool wait = false, QStringList* names = nullptr);

        void endGame(void);

    private:
        /** set the results of all players to 0. This is necessary
          * if for example the class for the results calculation was changed
          * or when a new game is started.
          */
        void resetGameResults();

        /** Called on initialization and when the user changed the names
          * of the players to represent the changes in the UI
          */
        void updatePlayerNames();

    private:
        /** give cards to the player and begin a new gameinfo
         */
        void startGame();
        /** Display the results of the game (winner/loser)
          * to the user. Called after each game.
          */
        void gameResults();

        /** find a player who wants to playerPlayedCard
          * and setup m_gameinfo according to this
          * returns true on succes
          * and false when no one wants to playerPlayedCard
          * and a new game should be started
          */
       bool setupGameInfo(Player *players[]);

       /** internal helper function for setupGameInfo().
         * handles all cases when no player wants to play.
         * It forces players in certain cases to play according
         * to the users settings.
         */
       bool setupGameInfoForced(Player *players[]);

    protected:
        /** inherited from QThread, calls gameLoop();
          */
        void run() override;

    private:
        static unsigned int def_id;

        QSemaphore* m_semaphore{};
        bool terminated{true};

        Player *m_players[PLAYERS];
        // player who is the next to play a card (for nextPlayer() )
        Player *m_nextPlayer{};

        CardList m_allcards;
        CardList m_playedcards;
        CardList m_currstich;
        GameInfo m_gameinfo;
        int m_laufende{0};
        int m_timesThrownTogether{0};

        QObject* m_parent{};
};
