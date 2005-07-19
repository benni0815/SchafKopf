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

#ifndef SCHAFKOPF_DEF_H
#define SCHAFKOPF_DEF_H

#define PLAYERS   4

// If you do want to play with only 6 cards
// like it is done in Franken, please comment this line:
#define SCHAFKOPF_LANG
 
#ifdef SCHAFKOPF_LANG
#define NUMCARDS  8
#define NUMTRUMPF 4
#define TURNS     8
#define CARD_CNT  32
#else
#define NUMCARDS  6
#define NUMTRUMPF 4
#define TURNS     6
#define CARD_CNT  24
#endif // SCHAFKOPF_LANG

/** Identify events from schafkopf in the Qt event loop
  * using this ID.
  */
#define SCHAFKOPF_EVENT 1984

#include <qstring.h>

class Card;
class QSemaphore;
class QStringList;

enum { YES, NO };
    
typedef enum EAction { NoAction, GameStarted, GameEnded, PlayerDoubled, 
                       PlayerHasDoubled, PlayerIsLast, PlayerNameChanged, PlayerGotCards,
                       CardPlayed, PlayerMadeStich, PlayerResults, GameInfoSetup,
                       RedrawPlayers, InfoMessage, Bubble, QuestionYesNo, 
                       HumanPlayerGetCard, ForbiddenCard, SelectGame,
                       ForcedSelectGame };

typedef struct t_EventData {
    EAction type;
    
    unsigned int playerid;
    void* returncode;
    int* cardids;

    
    bool wait;
    bool quitgame;
    
    QStringList* playernames;
    QString data;
};

typedef struct t_PlayerData {
    unsigned int id1;
    int* cards1;
    
    unsigned int id2;
    int* cards2;

    unsigned int id3;
    int* cards3;

    unsigned int id4;
    int* cards4;

    unsigned int lastid;
};

/** Uncomment this line to make all cards
  * user visible, this is useful for debugging
  * and improving the AI.
  * But it might cause problems in network mode for cheaters!!
  */
//#define CHEAT

#ifdef CHEAT
    #warning "CHEATING ENABLED!!!"
#endif

/** 
 * Enable the simulation mode, might be useful for training 
 * the neuro-net we will surely get soon.
 */
//#define SIMULATION_MODE
/**
 * How many games should be simulated in SIMULATION_MODE?
 */
#define SIMULATION_MODE_MAX 10000

// DISTANCE from the border used in CanvasPlayer
#define DIST 20

 
 
#endif // SCHAFKOPF_DEF_H
 

