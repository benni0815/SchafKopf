
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
			if(m_game->istTrumpf(card))	entfernen->append(card);
	}
	liste->RemoveCards(entfernen);
	delete entfernen;
}

bool Player::hasTrumpf(CardList* liste)
{
	Card* card;
	for ( card = liste->first(); card; card = liste->next() )
    	{
			if(m_game->istTrumpf(card))	return true;
	}
	return false;
}

CardList* Player::allowedCards()
{
	CardList* allowed= m_cards->FindCards(Card::NOCOLOR, Card::NOSTICH);
	if(m_game->gameInfo()->mode==Game::RUFSPIEL)
	{
	CardList* Sau=			m_cards->FindCards(m_game->gameInfo()->color, Card::SAU);
	CardList* Spielfarbe=	m_cards->FindCards(m_game->gameInfo()->color, Card::NOSTICH);
	removeTrumpf(Spielfarbe);
		if(m_game->currStich()->isEmpty()==true)
		{
			if(Sau->isEmpty())
			{
				delete Sau;
				delete Spielfarbe;
				return allowed;
			}
			else
			{
				if(Spielfarbe->count()<4&&Spielfarbe->count()>0)
				{
					allowed->RemoveCards(Spielfarbe);
					delete Spielfarbe;
					delete Sau;
					return allowed;
				}
				else
				{
					delete Spielfarbe;
					delete Sau;
					return allowed;
				}
			}
		}
		else
		{
			if(m_game->istTrumpf(const_cast<CardList *>(m_game->currStich())->first()))
			{
				if(hasTrumpf(m_cards))
				{
					CardList* alle=m_cards->FindCards(Card::NOCOLOR, Card::NOSTICH);
					removeTrumpf(alle);
					allowed->RemoveCards(alle);
					delete Sau;
					delete Spielfarbe;
					delete alle;
					return allowed;
				}
				else
				{
					if(Sau->isEmpty())
					{
						delete Sau;
						delete Spielfarbe;
						return allowed;
					}
					else
					{
						if(Spielfarbe->count()<4&&Spielfarbe->count()>0)
						{
							allowed->RemoveCards(Spielfarbe);
							delete Spielfarbe;
							delete Sau;
							return allowed;
						}
						else
						{
							delete Spielfarbe;
							delete Sau;
							return allowed;
						}
					}
				}

			}
			else
			{

				if( (const_cast<CardList *>(m_game->currStich())->first()->color()==m_game->gameInfo()->color) && (!Sau->isEmpty()) )
				{
						delete allowed;
						delete Spielfarbe;
						return Sau;
				}
				else
				{
				CardList *Farbe=m_cards->FindCards(const_cast<CardList *>(m_game->currStich())->first()->color(), Card::NOSTICH);
				removeTrumpf(Farbe);
					if(!Farbe->isEmpty())
					{
						delete Sau;
						delete allowed;
						delete Spielfarbe;
						return Farbe;
					}
					else
					{
						delete Spielfarbe;
						delete Sau;
						delete Farbe;
						return allowed;
					}
				}
			}
		}
	/*
		if(m_game->currStich()->isEmpty()==true) return allowed;
		else
			if(m_game->istTrumpf(m_game->currStich()->first()))
			{
				if(hasTrumpf(m_cards))
				{
					CardList* alle=m_cards->FindCards(Card::NOCOLOR, Card::NOSTICH);
					removeTrumpf(alle);
					allowed->RemoveCards(alle);
					delete alle;
					return allowed;
				}
				else
				{
					return allowed;
				}
			}
			else
			{
			CardList *Sau=m_cards->FindCards(m_game->gameInfo()->color, Card::SAU);
				if(!Sau->isEmpty()) //ich hab die Sau
				{
					CardList* Sau=m_
				}
				else
				{
				}
			}

	*/

	}
}

