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
#ifndef GAME_H
#define GAME_H

#include "cardlist.h"
#include "gameinfo.h"

#include <qobject.h>

#define PLAYERS 4
#define TURNS 8

class GameCanvas;
class Player;

/**
@author Dominik Seichter
*/
class Game : public QObject
{
    Q_OBJECT
    public:
        Game(QObject *parent = 0, const char *name = 0);
        ~Game();
        void gameLoop();
        const CardList *currStich() const;
        const GameInfo *gameInfo() const;
        
        void setCanvas( GameCanvas* c );
        GameCanvas* canvas() const { return m_canvas; }
        
        Player* findId( unsigned int id ) const;
        Player* findIndex( unsigned int index ) const;

		bool isTerminated() const { return terminated; }
		
		bool isHigher( Card* card, Card* high );
		
		/** returns a number between 1 and 32 according to the worth of the card
		*/
		static int evalCard(Card *card, GameInfo *gameinfo);
        void stateChanged()
		{
			emit gameStateChanged();
		}
		
	signals:        
		void gameStateChanged();
        void signalKlopfen();
        void playerPlayedCard( unsigned int player, Card* );
        void playerMadeStich( unsigned int player );
        void playerResult( const QString & name, const QString & result );
        
    public slots:
        void endGame(void);
        
    private:
        /** give cards to the player and begin a new gameinfo
          */
        void start();
        
        bool terminated;
        Player *m_players[PLAYERS];
        CardList m_allcards;
        CardList m_playedcards;
        CardList m_currstich;
        GameInfo m_gameinfo;
        
        GameCanvas *m_canvas;
        
        int highestCard();
        void gameResults();
		
        /** find a player who wants to playerPlayedCard 
          * and setup m_gameinfo according to this
          */
       void setupGameInfo();
};

#endif
