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
        Game(QObject *parent);
        ~Game() override = default;

        void gameLoop();
        const CardList& currStich() const;
        const CardList& allCards() const { return m_allcards; }
        const CardList& playedCards() const { return m_playedcards; }
        GameInfo *gameInfo();

        Player* findIndex( unsigned int index ) const;

        /** @returns the player who is the next to play
         *  a card in this turn or NULL if there is no player left.
         *
         *  This should only be called from Player::play()
         */
        inline Player* nextPlayer() const { return m_nextPlayer; };

        std::array<std::shared_ptr<Player>, PLAYERS> players();

        bool isTerminated() const { return terminated; }
        int highestCard(CardList* list = nullptr);
        bool isHigher( Card* card, Card* high );

        /** returns how many players have doubled
          */
        int timesDoubled();
        int timesThrownTogether();

    public slots:
        void endGame(void);

    signals:
        void gameStarted();
        void gameEnded();
        void playerNameChanged(unsigned int playerId, const QString& name);
        void playerDoubled(const QString& message);
        void playerIsLast(unsigned int playerId);
        void playerMadeStich(unsigned int playerId, std::vector<int> cardIds, const QString& playerName, const QStringList& playerNames);
        void cardPlayed(unsigned int playerId, int cardId);
        void redrawPlayers();
        void gameInfoSetup();
        void playerResults(unsigned int playerId, const QString& formattedPoints);
        void infoMessage(const QString& message);
        void bubble(unsigned int playerId, const QString& message);

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
        bool terminated{true};

        std::array<std::shared_ptr<Player>, PLAYERS> m_players;
        // player who is the next to play a card (for nextPlayer() )
        Player *m_nextPlayer{};

        CardList m_allcards;
        CardList m_playedcards;
        CardList m_currstich;
        GameInfo m_gameinfo;
        int m_laufende{0};
        int m_timesThrownTogether{0};
};
