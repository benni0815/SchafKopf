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
#include "computerplayer.h"

#include "cardlist.h"
#include "game.h"
#include "gameinfo.h"
#include "openbook.h"

#include <kapplication.h>

ComputerPlayer::ComputerPlayer(Game* game)
	: QObject(0, 0), Player(game)
{
	int i;
	
	book = new OpenBook( this, game );
	for(i=0;i<PLAYERS;i++)
		m_playedCards[i]=new CardList();
	connect( m_game, SIGNAL( playerPlayedCard( unsigned int, Card* ) ), this, SLOT( cardPlayed(unsigned int, Card*) ) );
}

ComputerPlayer::~ComputerPlayer()
{
	int i;
	
	for(i=0;i<PLAYERS;i++)
		delete m_playedCards[i];
	delete book;
}

void ComputerPlayer::klopfen()
{
    m_geklopft = false;
    unsigned int i=0;
	
    for(i=0;i<m_cards->count();i++)
    {
    
    }
    
    Player::klopfen();
}

void ComputerPlayer::init()
{
	int i;
	
	for(i=0;i<PLAYERS;i++)
		m_playedCards[i]->clear();
	if(m_game->gameInfo()->mitspieler()==this)
	{
		for(mitspieler=0;mitspieler<PLAYERS;mitspieler++)
			if(m_game->findIndex(mitspieler)==m_game->gameInfo()->spieler())
				break;
	}
	else if(m_game->gameInfo()->mode()!=GameInfo::RUFSPIEL && m_game->gameInfo()->spieler()!=this)
		mitspieler=m_game->gameInfo()->spieler()->id();
	else
		mitspieler=-1;
}

Card *ComputerPlayer::play()
{
	CardList* allowed=NULL;
	Card* ToPlay;
	Card *c;
	int i;
			
    if( m_game->currStich()->isEmpty() )
	{
        allowed=book->possibleCards();
		if(!allowed)	        
			allowed=allowedCards();
		i=0;
		for(;;)
		{
			if(!(i<allowed->count() && allowed->count()>1))
				break;
			if(allowed->at(i)->points()==10)
				allowed->remove(i);
			else
				i++;
		}
		i=0;
		for(;;)
		{
			if(!(i<allowed->count() && allowed->count()>1))
				break;
			if(allowed->at(i)->points()==4)
				allowed->remove(i);
			else
				i++;
		}
	}
	if(!allowed)
		allowed=allowedCards();
        
	ToPlay=findCardToPlay(allowed);
	
	delete allowed;
	return ToPlay;
}

GameInfo* ComputerPlayer::gameInfo()
{
    QValueList<game_data> lst;
        
    for(int i=GameInfo::STICHT;i<=GameInfo::RUFSPIEL;i++)
    {
        for( int z=Card::NOCOLOR;z<=Card::SCHELLEN;z++)
        {
            if( !GameInfo::isAllowed( m_cards, i, z ) )
                continue;
         
            game_data d;
            d.trumpf = d.fehlfarbe = d.weight = 0;
            d.info.setMode( i );
            d.info.setColor( z );        
            
            for( unsigned int c=0;c<m_cards->count();c++)
            {
                d.weight += d.info.weight( m_cards->at(c) );
                if( d.info.istTrumpf( m_cards->at(c) ) )
                    d.trumpf++;
                else if( !d.info.istTrumpf( m_cards->at(c) ) && m_cards->at(c)->card() != Card::SAU )
                {
                    CardList* list = m_cards->FindCards( m_cards->at(c)->color(), Card::SAU );
                    if( list->isEmpty() )
                        d.fehlfarbe++;
                    delete list;
                }
                    
            }

            if( i==GameInfo::RUFSPIEL && d.weight>=8 && ((d.fehlfarbe<=2 && d.trumpf>=5) || d.fehlfarbe<=1) )
                lst.append( d );
            else if( i!=GameInfo::RUFSPIEL && d.weight>=9 && d.fehlfarbe<=1 && d.trumpf>=5 )
                lst.append( d );
        }
    }
    
    // now we have all possible games in list,
    // let's find the best one;
    if( !lst.isEmpty() )
    {
        int best = 0;
        for( unsigned int i=0;i<lst.count();i++)
            if( lst[i].weight > lst[best].weight || (lst[i].trumpf > lst[best].trumpf && lst[i].fehlfarbe < lst[i].fehlfarbe) )
            {
                best = i;
                continue;
            }
        
        GameInfo* gi = new GameInfo;
        *gi = lst[best].info;
        return gi;
    }
    return 0;
}

Card *ComputerPlayer::findCardToPlay(CardList *cards)
{
	if(ownStich())
	{
		qDebug("schmiere");
		return findSchmiere(cards);
	}
	else if(canMakeStich(cards))
	{
		qDebug("versuche zu stechen");
		return findHighestCard(cards);
	}
	qDebug("spiele billigste karte");
	return findCheapestCard(cards);
}

Card *ComputerPlayer::findHighestCard(CardList *cards)
{
	Card* high = cards->first();
	Card* card = cards->first();
	
	while( (card = cards->next() ) )
	{
		if( m_game->isHigher( card, high ) )
		{
			high = card;
			continue;
		}
	}
	return high;
}

Card *ComputerPlayer::findSchmiere(CardList *cards)
{
	Card *schmiere=cards->first();
	Card *card=cards->first();
	
	while( (card = cards->next() ) )
	{
		if( card->points() > schmiere->points() )
		{
			schmiere = card;
			continue;
		}
	}
	return schmiere;
}

Card *ComputerPlayer::findCheapestCard(CardList *cards)
{
	Card* low = cards->first();
	Card* card = cards->first();
	
	while( (card = cards->next() ) )
	{
		if(card->points()<low->points() )
		{
			low = card;
			continue;
		}
	}
	return low;
}
		
bool ComputerPlayer::canMakeStich(CardList *cards)
{
	Card *highestCard, *card;
	
	highestCard=m_game->currStich()->at(m_game->highestCard());
	card=cards->first();
	if(!highestCard)
		return true;
	do
	{
		if(m_game->isHigher(card, highestCard))
				 return true;
	} while(card=cards->next());
	return false;
}

bool ComputerPlayer::ownStich()
{
	Card *highestCard=m_game->currStich()->at(m_game->highestCard());
	
	if(mitspieler==-1)
		return false;
	if(m_game->gameInfo()->mode()==GameInfo::RUFSPIEL)
	{
		if(m_playedCards[mitspieler]->containsRef(highestCard))
			return true;
	}
	else
	{
		if(!m_playedCards[mitspieler]->containsRef(highestCard))	
			return true;
	}
	return false;
}

void ComputerPlayer::cardPlayed(unsigned int player, Card *c)
{
	int self_index, spieler_index;
	
	m_playedCards[player]->append(c);
	if(mitspieler!=-1)
		return;
	if(m_game->gameInfo()->mode()==GameInfo::RUFSPIEL && c->card()==Card::SAU && c->color()==m_game->gameInfo()->color())
	{
		if(m_game->gameInfo()->spieler()==this)
			mitspieler=player;
		else
			mitspieler=0+1+2+3-(id()+m_game->gameInfo()->spieler()->id()+player);
		qDebug("Mitspieler gefunden:" + m_game->findIndex(mitspieler)->name());
	}
}
