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

#include "schafkopfdef.h"

#include "cardlist.h"
#include "gameinfo.h"

#include <qobject.h>

class Player;
class GameCanvas;

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
		CardList *currStich();
		CardList *allCards() { return &m_allcards; }
		GameInfo *gameInfo();
        
        void setCanvas( GameCanvas* c );
        GameCanvas* canvas() const { return m_canvas; }
        
        Player* findId( unsigned int id ) const;
        Player* findIndex( unsigned int index ) const;
		bool isTerminated() const { return terminated; }
		int highestCard();
		bool isHigher( Card* card, Card* high );
		
    signals:        
        void gameStarted();
        void gameEnded();
    
        void playerPlayedCard( unsigned int player, Card* );
        void playerMadeStich( unsigned int player );
        void playerResult( const QString & name, const QString & result );
        
        void signalSetupGameInfo();
        void signalDoubled();
        
    public slots:
        void endGame(void);
        
    private:
        bool terminated;
        Player *m_players[PLAYERS];
        CardList m_allcards;
        CardList m_playedcards;
        CardList m_currstich;
        GameInfo m_gameinfo;
        int m_laufende;
                
        GameCanvas *m_canvas;
        
		/** give cards to the player and begin a new gameinfo
		 */
		void start();
        void gameResults();
		
        /** find a player who wants to playerPlayedCard 
          * and setup m_gameinfo according to this
          * returns true on succes
          * and false when no one wants to playerPlayedCard
          * and a new game should be started
          */
       bool setupGameInfo(Player *players[]);
};

#endif
