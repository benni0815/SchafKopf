// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once


constexpr int PLAYERS{4};

/// If you do want to play with only 6 cards
/// like it is done in Franken, please comment this line:
#define SCHAFKOPF_LANG

#ifdef SCHAFKOPF_LANG
constexpr int NUMCARDS{8};
constexpr int NUMTRUMPF{4};
#else
constexpr int NUMCARDS{6};
constexpr int NUMTRUMPF{4};
#endif // SCHAFKOPF_LANG

constexpr int CARD_CNT{PLAYERS * NUMCARDS};
constexpr int TURNS{NUMCARDS};

enum { YES, NO };

/// Uncomment this line to make all cards
/// user visible, this is useful for debugging
/// and improving the AI.
/// But it might cause problems in network mode for cheaters!!
//#define CHEAT

#ifdef CHEAT
    #warning "CHEATING ENABLED!!!"
#endif

/// Enable the simulation mode, might be useful for training
/// the neuro-net we will surely get soon.
constexpr bool SIMULATION_MODE{false};

/// How many games should be simulated in SIMULATION_MODE?
constexpr int SIMULATION_MODE_MAX{10000};

/// DISTANCE from the border used in CanvasPlayer
constexpr int DIST{20};
