
#include "player.h"
#include "game.h"
#include "cardlist.h"


#include <string>
#include <iostream>
#include <cstdlib>
using namespace std;

unsigned int Player::def_id=0;

Player::Player(CardList *cards,Game* game)
    : m_cards( cards ), m_game( game )
{
    m_stiche = new CardList();
    m_points = 0.0;
    m_id=def_id++;
    
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
    delete m_cards;
    m_cards=cards;

    klopfen();
    // sortCards();
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
	return m_cards->FindCards(Card::NOCOLOR, Card::NOSTICH);
}

Card* Player::firstPlayedCard()
{
	if(m_game->currStich()->isEmpty()==true) return NULL;
	else return (const_cast<CardList *>(m_game->currStich())->first());
}

CardList* Player::cardsOfSameType(Card* card)
{
	CardList* SpielerKarten=PlayerCards();
	if(card==NULL) return SpielerKarten; //müssen alle Karten zurückgegeben werden
	else
	{
		if(istTrumpf(card)) //müssen alle trümpfe zurückgegeben werden
		{
		CardList* AntiMaske=PlayerCards();
		removeTrumpf(AntiMaske);
		SpielerKarten->RemoveCards(AntiMaske);
		delete AntiMaske;
		return SpielerKarten;
		}
		else	//muss die gleiche Farbe ohne Trümpfe zurückgegeben werden
		{
		removeTrumpf(SpielerKarten);
		CardList* SpielerFarbe=	SpielerKarten->FindCards(firstPlayedCard()->color(), Card::NOSTICH);
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

	if(m_game->gameInfo()->mode()==GameInfo::RUFSPIEL) //weiß nicht wieso dass Game::RUFSPIEL nimmer geht, seit letzter aktualisierung... naja, wird so schon gehn (mit GameInfo)
	{
		CardList* Sau=allowed->FindCards(m_game->gameInfo()->color(), Card::SAU);
		CardList* Spielfarbe=	allowed->FindCards(m_game->gameInfo()->color(), Card::NOSTICH);
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

