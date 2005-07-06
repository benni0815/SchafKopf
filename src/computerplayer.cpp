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

#include "card.h"
#include "cardlist.h"
#include "game.h"
#include "gameinfo.h"
#include "openbook.h"

#include <kapplication.h>

ComputerPlayer::ComputerPlayer(unsigned int id,Game* game)
	: Player(id,game)
{
    int i;
	
    book = new OpenBook( this, game );
    for(i=0;i<PLAYERS;i++)
        m_playedCards[i]=new CardList();
    m_angespielt=new CardList();
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
    int i;
    const int anz = NUMCARDS/2;
    int start = ( isLast() ? anz : 0 );
    int ende = start + anz;
    int count = 0;
    GameInfo* info;
    m_geklopft = false;
    
    /** check 2 possibilities:
      * - all cards have the same color
      * - all cards trump in herz sticht oder sau
      */
      
    // are all cards in the same color?
    for( i=start;i<ende;i++ )
        if( m_cards->at( i )->color() == m_cards->at( start )->color() )
            count++;
            
    if( count == anz )
    {
        m_geklopft = true;
        Player::klopfen();
        return;
    }
    
    // are all cards trump?
    info = new GameInfo;
    info->setMode( GameInfo::STICHT );
    info->setColor( Card::HERZ );
    count = 0;
    for( i=start;i<ende;i++ )
        if( m_cards->at( i )->card() == Card::SAU || info->istTrumpf( m_cards->at( i ) ) )
            count++;
    
    delete info;
    
    if( count == anz )
    {
        m_geklopft = true;
        Player::klopfen();
        return;
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
    else if(m_game->gameInfo()->mode()!=GameInfo::RUFSPIEL && 
            m_game->gameInfo()->mode()!=GameInfo::RAMSCH &&
            m_game->gameInfo()->spieler()!=this)
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
        qDebug("%s: allowed->count() : %i", name().latin1(), allowed->count() );
    }
    /*
      // The AI is more intelligent, let the AI decide
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
    */
    if(!allowed)
        allowed=allowedCards();
    
//    if( allowed->count() == 1 )
//        return allowed->first();

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
        Card* highestTrump = highestTrumpfInGame();

	if(ownStich())
	{
            // nur schmieren wenn :
            // -man letzter ist,
            // -der partner mit dem hoechsten trumpf im spiel 
            //  abzueglich der eigenen truempfe gestochen hat 
            // -oder wenn der darauf folgende spieler der selben
            //  partei angehoert wie man selbst (wichtig fuer solos)
            if( m_game->currStich()->count() == PLAYERS - 1 ||
                ( highestTrump && 
                  findHighestCard( m_game->currStich() )->id() == highestTrump->id() ) ||
                ( m_game->currStich()->count() == PLAYERS - 2 && 
                  m_game->nextPlayer()->isPlayer() == this->isPlayer() ) )
            {
                qDebug("%s: schmiere!", name().latin1() );
                return findSchmiere(cards);
            }
	}
        else
        {
            // Bei Solos wird geschmiert wenn:
            // -man nicht spieler und nicht letzter oder erster ist
            // -wenn jemand anders den stich noch machen kann
            if( m_game->gameInfo()->mitspieler() == NULL &&      // Solo ?
                m_game->currStich()->count() < PLAYERS - 1 &&    // letzter ?
                m_game->currStich()->count() &&                  // erster ?
                !this->isPlayer() &&                             // selber spieler ?
                ( highestTrump && 
                  findHighestCard( m_game->currStich() )->id() != highestTrump->id()) && // Kann der Stich ueberhaupt gestochen werden
                !canMakeStich(cards) )                           // kann ihn jemand ausser mir machen?
            {
                qDebug("%s: schmiere!", name().latin1() );
                return findSchmiere(cards);
            }
        }
	
        if(canMakeStich(cards))
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
            qDebug("%s: versuche so niedrig wie möglich zu stechen", name().latin1());
            return findLowestPossibleCard(findHighestCard(stich),cards);
        }
        else
        {
		  qDebug("%s: versuche zu stechen", name().latin1());
		  return findHighestCard(cards);
        }
	}
	qDebug("%s: spiele billigste karte", name().latin1());
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
    Card* highestCard= m_game->currStich()->at(m_game->highestCard());

    /* The stich cannot belong to us if we are to play
     * the first card! 
     */
    if( !m_game->currStich()->count() )
	return false;

    /* This looks butt ugly but it works!
     * We can only check if the card belongs to the player party, so we check afterwars
     * if we are part of the player part!! Think about it... it does really work :)
     */
    qDebug("%s ist %s Spieler.", name().latin1(), ( this->isPlayer() ? "":"nicht") );
    if( highestCard->owner()->isPlayer() )
        return ( this->isPlayer() ? true : false );
    else
        return ( this->isPlayer() ? false : true );
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
    // TODO: was called by signal before the thread change
    // has to be implemented in another way
	m_playedCards[player]->append(c);
	if(mitspieler!=-1)
		return;
	if(m_game->gameInfo()->mode()==GameInfo::RUFSPIEL && c->card()==Card::SAU && c->color()==m_game->gameInfo()->color())
	{
		if(m_game->gameInfo()->spieler()==this)
			mitspieler=player;
		else
			mitspieler=0+1+2+3-(id()+m_game->gameInfo()->spieler()->id()+player);
		qDebug("%s: Mitspieler gefunden: %s", name().latin1(), m_game->findIndex(mitspieler)->name().latin1());
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

