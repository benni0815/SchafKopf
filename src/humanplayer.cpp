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

#include <kapplication.h>
#if QT_VERSION >= 0x030100
    #include <qeventloop.h>
#else
    #include <qapplication.h>
#endif

#include "humanplayer.h"

HumanPlayer::HumanPlayer(CardList *cards,Game* game)
 : Player(cards,game)
{
	can_play=false;
	selected_card=-1;
	sortCards();
}


HumanPlayer::~HumanPlayer()
{
}

bool HumanPlayer::doppeln()
{
    return false;
}

Card *HumanPlayer::play()
{
    can_play=true;
#if QT_VERSION >= 0x030100
	//kapp->eventLoop()->enterLoop();
#else
	//kapp->enter_loop();
#endif
	can_play=false;
	// Now return the card indexed by selected_card
	sortCards();
	return 0;
}

void HumanPlayer::isValid( const int index,  bool &valid )
{
	valid=false;
}		
		
void HumanPlayer::playCard( const int index )
{
	if(!can_play)
			return;
	selected_card=index;
#if QT_VERSION >= 0x030100
	kapp->eventLoop()->exitLoop();
#else
    kapp->exit_loop();
#endif
}
	
void HumanPlayer::sortCards()
{
}
	