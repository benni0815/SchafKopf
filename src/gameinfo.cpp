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
#include "gameinfo.h"
#include "card.h"

GameInfo::GameInfo()
{
}


GameInfo::~GameInfo()
{
}


bool GameInfo::operator>( GameInfo info )
{
/*
  2.2.3. Hat ein Teilnehmer einen Wenz oder ein Solo und vorher schon ein Spiel angesagt worden, so muß der Einzelspieler erklären "Ich spiele auch". Wenn der Erstmeldende "Gut" sagt, kann der Einzelspieler sein Spiel bekanntgeben. Will der Voransitzende aber selber alleine spielen, so nennt er sein Einzelspiel. Der nachfolgende Teilnehmer kann nun nur noch mit einem höherrangigen Einzelspiel ans Spiel kommen. Denn: Bei gleichrangigen Spielen entscheidet die Sitzreihenfolge, wer zum Zuge kommt. Farbsoli sind untereinander alle gleichberechtigt. Herzsoli geht nicht vor.
*/  
    if( this->mode == info.mode )
    {
        // according to the above paragraph,
        // we have only to care about NOCOLOR
        // e.g. Farblos Wenz > Herz Wenz 
        // but Eichel Wenz == Gras Wenz
        return ( this->color == Card::NOCOLOR );
    } else
        return this->mode < info.mode;
}
