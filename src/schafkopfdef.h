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
#define NUMCARDS  8
#define TURNS     8
#define CARD_CNT  32




/** Uncomment this line to make all cards
  * user visible, this is useful for debuggin
  * and improving the AI.
  * But it might cause problems in network mode for cheaters!!
  */
#define CHEAT

#ifdef CHEAT
    #warning "CHEATING ENABLED!!!"
#endif


// DISTANCE from the border used in CanvasPlayer
#define DIST 20

 
 
#endif // SCHAFKOPF_DEF_H
 

