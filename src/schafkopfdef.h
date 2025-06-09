// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

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

#include <QString>
#include <QStringList>

class Card;
class QSemaphore;

enum { YES, NO };
    
enum EAction { NoAction, GameStarted, GameEnded, PlayerDoubled,
                       PlayerHasDoubled, PlayerIsLast, PlayerNameChanged, PlayerGotCards,
                       CardPlayed, PlayerMadeStich, PlayerResults, GameInfoSetup,
                       RedrawPlayers, InfoMessage, Bubble, QuestionYesNo, 
                       HumanPlayerGetCard, ForbiddenCard, SelectGame,
                       ForcedSelectGame };

struct t_EventData {
    EAction type;

    unsigned int playerid;
    void* returncode;
    int* cardids;

    bool wait;
    bool quitgame;

    QStringList* playernames;
    QString data;
};

struct t_PlayerData {
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
