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
#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include <qobject.h>
#include "player.h"

class HumanPlayer : public QObject, public Player
{
    Q_OBJECT
	public:
        HumanPlayer(Game* game);
        ~HumanPlayer();
        
		void klopfen();
        Card *play();
        GameInfo* gameInfo();
        
		int rtti() const { return HUMAN; }        

    private slots:
        void getCard( Card* card );
        
    private:
        Card* m_card;
        CardList* m_allowed;
};

#endif
