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

#include "cardlist.h"
#include "game.h"
#include "gamecanvas.h"
#include "timer.h"

HumanPlayer::HumanPlayer(CardList *cards,Game* game)
 : QObject( 0, 0 ), Player(cards,game)
{
    m_allowed = NULL;
    m_card = NULL;
	m_cards->sort();
}

HumanPlayer::~HumanPlayer()
{
    delete m_allowed;
}

void HumanPlayer::klopfen()
{

}

Card *HumanPlayer::play()
{
    m_allowed = allowedCards();
	m_card=NULL;
    connect( m_game->canvas(), SIGNAL(playCard(Card*)), this, SLOT(getCard(Card*)));
    ENTER_LOOP();
	if(m_card)
		qDebug("Human Clicked on Card: %i, %i", m_card->card(), m_card->color());
	delete m_allowed;
    m_allowed = 0;
    disconnect(m_game->canvas(), SIGNAL(playCard(Card*)), this, SLOT(getCard(Card*)));
    return m_card;
}

void HumanPlayer::getCard(Card* card)
{
    if( m_allowed->containsRef( card ) )
    {
        m_card = card;
        EXIT_LOOP();
    } 
	else
        m_game->canvas()->cardForbidden(card);
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

