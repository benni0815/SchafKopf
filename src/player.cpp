
#include "player.h"
#include "game.h"
#include "cardlist.h"

#include <string>
#include <iostream>
#include <cstdlib>
using namespace std;

unsigned int Player::def_id=0;

Player::Player(Game* game)
    : m_game( game )
{
    m_stiche = new CardList();
    m_points = 0.0;
    m_id=def_id++;
    m_cards = NULL;
    
    m_has_doubled = false;
    m_geklopft = false;
}

Player::~ Player()
{
	delete m_cards;
	delete m_stiche;
}

const QString & Player::name() const
{
    return m_name;
}

void Player::setName( const QString & n )
{
    m_name = n;
}

CardList *Player::cards() const
{
    return m_cards;
}

void Player::setCards( CardList *cards)
{
    unsigned int i = 0;
    delete m_cards;
    
    m_has_doubled = false;
    m_geklopft = false;
    
    m_cards=cards;
    // tell the card WHO owns them and WHOM they should serve! ;-)
    for(i=0;i<cards->count();i++)
        m_cards->at(i)->setOwner( this );
}

void Player::sortCards()
{
    m_cards->sort((eval_func)m_game->gameInfo()->evalCard, (void *)m_game->gameInfo());
}

void Player::klopfen()
{
    m_has_doubled = true;
}

void Player::DebugCardOutput(Card* card)
{
	char* c;
	char* s;

	switch (card->color())
	{
		case Card::SCHELLEN:
			c="Schellen";
			break;
		case Card::HERZ:
			c="Herz";
			break;
		case Card::EICHEL:
			c="Eichel";
			break;
		case Card::GRAS:
			c="Gras";
			break;
	}
	switch (card->card())
	{
		case Card::SAU:
			s="SAU";
			break;
		case Card::ZEHN:
			s="ZEHN";
			break;
		case Card::KOENIG:
			s="KOENIG";
			break;
		case Card::OBER:
			s="OBER";
			break;
		case Card::UNTER:
			s="UNTER";
			break;
		case Card::NEUN:
			s="NEUN";
			break;
		case Card::ACHT:
			s="ACHT";
			break;
		case Card::SIEBEN:
			s="SIEBEN";
			break;
	}
	 qDebug("Entferne: %s, %s", c, s);
}

CardList* Player::stiche() const
{
    return m_stiche;
}

void Player::addStich( CardList stich )
{
    for( unsigned int i = 0; i < stich.count(); i++ )
        m_stiche->append( stich.at( i ) );
}

void Player::removeTrumpf(CardList* liste)
{
	Card *card;
	CardList* entfernen=new CardList;
	for ( card = liste->first(); card; card = liste->next() )
    	{
			if(m_game->gameInfo()->istTrumpf(card))	entfernen->append(card);
	}
	liste->RemoveCards(entfernen);
	delete entfernen;
}

bool Player::hasTrumpf(CardList* liste)
{
	Card* card;
	for ( card = liste->first(); card; card = liste->next() )
    	{
			if(m_game->gameInfo()->istTrumpf(card))	return true;
	}
	return false;
}

bool Player::istTrumpf(Card* card)
{
	return m_game->gameInfo()->istTrumpf(card);
}

CardList* Player::PlayerCards()
{
	return  m_cards->FindCards(Card::NOCOLOR, Card::NOSTICH);
}

Card* Player::firstPlayedCard()
{
	if(m_game->currStich()->isEmpty()==true) 
		return NULL;
	else 
		return (const_cast<CardList *>(m_game->currStich())->first());
}

CardList* Player::cardsOfSameType(Card* card)
{
	CardList* SpielerKarten=PlayerCards();
	CardList* SpielerFarbe;
	CardList* AntiMaske;
	
	if(card==NULL) 
		return SpielerKarten; //mssen alle Karten zurckgegeben werden
	else
	{
		if(istTrumpf(card)) //mssen alle trmpfe zurckgegeben werden
		{
			AntiMaske=PlayerCards();
			removeTrumpf(AntiMaske);
			SpielerKarten->RemoveCards(AntiMaske);
			delete AntiMaske;
			return SpielerKarten;
		}
		else	//muss die gleiche Farbe ohne Trmpfe zurckgegeben werden
		{
			removeTrumpf(SpielerKarten);
			SpielerFarbe=SpielerKarten->FindCards(firstPlayedCard()->color(), Card::NOSTICH);
			delete SpielerKarten;
			return SpielerFarbe;
		}
	}
}

CardList* Player::allowedCards()
{
	CardList* allowed=cardsOfSameType(firstPlayedCard());
	if(allowed->isEmpty())
	{
		delete allowed;
		allowed=PlayerCards();
	}

	if(m_game->gameInfo()->mode()==GameInfo::RUFSPIEL) //wei�nicht wieso dass Game::RUFSPIEL nimmer geht, seit letzter aktualisierung... naja, wird so schon gehn (mit GameInfo)
	{
		CardList* Sau=allowed->FindCards(m_game->gameInfo()->color(), Card::SAU);
		CardList* Spielfarbe=	allowed->FindCards(m_game->gameInfo()->color(), Card::NOSTICH);
		// entferne alle trumpfe aus Spielfarbe
        for(unsigned int i=0;i<Spielfarbe->count();i++)
            if( istTrumpf( Spielfarbe->at(i) ) )
            {
                Spielfarbe->removeRef( Spielfarbe->at(i) );
                qDebug("Entferne trumpf aus Spielfarbe");
            }
                
        if(!Sau->isEmpty())	//muss nur was machen wenn ich die Sau habe
		{
			if(firstPlayedCard()&&!istTrumpf(firstPlayedCard())&&firstPlayedCard()->color()==m_game->gameInfo()->color())
			{
				delete allowed;
				allowed=Sau;
				delete Spielfarbe;
			}
			else if(Spielfarbe->count()>=4)
			{
				Spielfarbe->RemoveCards(Sau);
				allowed->RemoveCards(Spielfarbe);
				delete Sau;
				delete Spielfarbe;
			}
		}
		else
		{
			delete Sau;
			delete Spielfarbe;
		}
	}
	return allowed;
}

