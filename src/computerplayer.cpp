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
	m_angespielt=new CardList();
	connect( m_game, SIGNAL( playerPlayedCard( unsigned int, Card* ) ), this, SLOT( cardPlayed(unsigned int, Card*) ) );
}

ComputerPlayer::~ComputerPlayer()
{
	int i;
	
	for(i=0;i<PLAYERS;i++)
		delete m_playedCards[i];
	delete m_angespielt;
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
	m_angespielt->clear();
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
    else if( m_game->currStich()->count() == 3 )
    {
        // last one to play a card
        // don't know if this will be helpfull
        // as it does merely the same the KI does already
        // just for a special case.
        // So it takes some work away from the real KI.
        StrategyBook strategy( this, m_game );
        allowed = strategy.possibleCards();
        
        
    }
	if(!allowed)
		allowed=allowedCards();
        
	ToPlay=findCardToPlay(allowed);
	if(m_game->currStich()->isEmpty())
		m_angespielt->append(ToPlay);
	else
		m_angespielt->append(m_game->currStich()->first());
	delete allowed;
	return ToPlay;
}

GameInfo* ComputerPlayer::gameInfo( bool force )
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
    
    if( force && lst.isEmpty() )
    {
        // Play eichel sticht for now. Optimize later
        GameInfo* gi = new GameInfo;
        gi->setColor( Card::EICHEL );
        gi->setMode( GameInfo::STICHT );
        return gi;
    }
    
    return NULL;
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
        CardList* stich = m_game->currStich();
        bool gestochen = false;
        int i;
        
        // sicher stellen das noch niemand gestochen hat
        for( i=0;i<stich->count();i++)
            if( m_game->gameInfo()->istTrumpf( stich->at(i) ) )
            {
                gestochen = true;
                break;
            }
        
        if( !stich->isEmpty() && !gestochen && cardsStillInGame( m_game->currStich()->at( 0 )->color() ) >= 3 )
        {
            // falls der computer spieler farbfrei ist und noch genügend karten von dieser farbe im spieler
            // sind versucht er so niedrig wie möglich zu stechen
            qDebug("versuche so niedrig wie möglich zu stechen");
            return findLowestPossibleCard(findHighestCard(stich),cards);
        }
        else
        {
		  qDebug("versuche zu stechen");
		  return findHighestCard(cards);
        }
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

Card *ComputerPlayer::findLowestPossibleCard(Card* highest, CardList *cards)
{
    Card* high = findHighestCard( cards );
    Card* card = cards->first();

    while( (card = cards->next() ) )    
    {
        if( m_game->isHigher( card, highest ) && !m_game->isHigher( card, high ) )
            high = card;
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
	} while( (card=cards->next()) );
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

bool ComputerPlayer::istTrumpfFrei(int playerId)
{
	int i;
	
	for(i=0;i<m_angespielt->count();i++)
	{
		if(m_game->gameInfo()->istTrumpf(m_angespielt->at(i)) && !m_game->gameInfo()->istTrumpf(m_playedCards[playerId]->at(i)))
			return true;
	}
	return false;
}
bool ComputerPlayer::istFarbFrei(int playerId, int c)
{
	int i;
	
	for(i=0;i<m_angespielt->count();i++)
	{
		if(!m_game->gameInfo()->istTrumpf(m_angespielt->at(i)) && m_angespielt->at(i)->color()==c && !m_playedCards[playerId]->at(i)->color()==c)
			return true;
	}
	return false;
}

float ComputerPlayer::gehtDurch(Card *card)
{
}

float ComputerPlayer::gegnerSticht(Card *card)
{
}

Card *ComputerPlayer::highestTrumpfInGame()
{
	CardList *trumpfs=new CardList();
	Card *c;
	int i;
	
	//trumpfs.setAutoDelete(false);
	for(c=m_game->allCards()->first();c;c=m_game->allCards()->next())
		if(m_game->gameInfo()->istTrumpf(c))
			trumpfs->append(c);
	for(i=0;i<PLAYERS;i++)
	{
		for(c=trumpfs->first();c;c=trumpfs->next())
			if(m_playedCards[i]->containsRef(c))
				trumpfs->removeRef(c);
	}
	for(c=m_cards->first();c;c=m_cards->next())
		trumpfs->removeRef(c);
	trumpfs->sort((eval_func)m_game->gameInfo()->evalCard, (void *)m_game->gameInfo());
	c=trumpfs->at(trumpfs->count()-1);
	delete trumpfs;
	return c;
}

int ComputerPlayer::myTrumpfs()
{
	int trumpfs=0;
	Card *c;
	
	for(c=m_cards->first();c;c=m_cards->next())
		if(m_game->gameInfo()->istTrumpf(c))
			trumpfs++;
	return trumpfs;
}

int ComputerPlayer::trumpfsInGame()
{
	Card *c;
	int trumpfs=0;
	int i;
	
	for(c=m_game->allCards()->first();c;c=m_game->allCards()->next())
		if(m_game->gameInfo()->istTrumpf(c))
			trumpfs++;
	for(i=0;i<PLAYERS;i++)
	{
		for(c=m_playedCards[i]->first();c;c=m_playedCards[i]->next())
			if(m_game->gameInfo()->istTrumpf(c))
				trumpfs--;
	}
	return trumpfs-myTrumpfs();
}

void ComputerPlayer::cardPlayed(unsigned int player, Card *c)
{
	m_playedCards[player]->append(c);
	if(mitspieler!=-1)
		return;
	if(m_game->gameInfo()->mode()==GameInfo::RUFSPIEL && c->card()==Card::SAU && c->color()==m_game->gameInfo()->color())
	{
		if(m_game->gameInfo()->spieler()==this)
			mitspieler=player;
		else
			mitspieler=0+1+2+3-(id()+m_game->gameInfo()->spieler()->id()+player);
		qDebug("Mitspieler gefunden: %s",m_game->findIndex(mitspieler)->name().latin1());
	}
}

int ComputerPlayer::cardsStillInGame( int c )
{
    int n = 0, n2 = 0;
    int i;
    CardList list;
    CardList* all = m_game->allCards();
    
    list.appendList( m_game->playedCards() );
    list.appendList( m_game->currStich() );
    for( i=0; i<list.count();i++ )
    {
        if( m_game->gameInfo()->istTrumpf( list.at( i ) ) )
        {
            if( Card::NOCOLOR == c )
                n++;
        }
        else
        {
            if( list.at( i )->color() == c )
                n++;
        }
    }
    
    for( i=0; i<all->count();i++ )
    {
        if( m_game->gameInfo()->istTrumpf( all->at( i ) ) )
        {
            if( Card::NOCOLOR == c )
                n2++;
        }
        else
        {
            if( all->at( i )->color() == c )
                n2++;
        }
    }
    
    return n2 - n;
}
    