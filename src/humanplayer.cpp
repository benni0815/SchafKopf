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

#include "humanplayer.h"

#include "game.h"
#include "gamecanvas.h"

#include <kapplication.h>
#if QT_VERSION >= 0x030100
    #include <qeventloop.h>
#else
    #include <qapplication.h>
#endif
#include "cardlist.h"

HumanPlayer::HumanPlayer(CardList *cards,Game* game)
 : QObject( 0, 0 ), Player(cards,game)
{
    m_card = NULL;
	m_cards->sort();
}

HumanPlayer::~HumanPlayer()
{
}

void HumanPlayer::klopfen()
{

}

Card *HumanPlayer::play()
{
    connect( m_game->canvas(), SIGNAL(playCard(Card*)), this, SLOT(getCard(Card*)));
#if QT_VERSION >= 0x030100
    kapp->eventLoop()->enterLoop();
#else
    kapp->enter_loop();
#endif
    qDebug("Human Clicked on Card: %i, %i", m_card->card(), m_card->color());
    return m_card;
}

void HumanPlayer::getCard(Card* card)
{
    // TODO: check if card is valid here
    m_card = card;
    disconnect(m_game->canvas(), SIGNAL(playCard(Card*)), this, SLOT(getCard(Card*)));
#if QT_VERSION >= 0x030100
    kapp->eventLoop()->exitLoop();
#else
    kapp->exit_loop();
#endif
}

void HumanPlayer::setCards( CardList *cards)
{
	Player::setCards(cards);
    m_cards->sort();
}

void HumanPlayer::isValid( const int index,  bool &valid )
{
	valid=false;
}		

