
#include "player.h"
#include "game.h"
#include "cardlist.h"

#include <QDebug>

#include <string>
#include <iostream>
#include <cstdlib>


Player::Player(unsigned int id, Game* game)
    : m_game( game )
{
    m_stiche = new CardList();
    m_points = 0.0;
    m_id=id;
    m_cards = NULL;

    m_has_doubled = false;
    m_geklopft = false;
    m_last = false;
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

void Player::setCards( CardList *cards )
{
    int i = 0;

    delete m_cards;

    m_geklopft = false;

    m_cards=cards;
    // tell the card WHO owns them and WHOM they should serve! ;-)
    for(i=0;i<m_cards->count();i++)
        m_cards->at(i)->setOwner( this );

    m_game->postEvent( PlayerGotCards, id(), m_cards->toIntList() );
}

void Player::sortCards()
{
    m_cards->sort((eval_func)m_game->gameInfo()->evalCard, (void *)m_game->gameInfo());
    m_game->postEvent( PlayerGotCards, id(), m_cards->toIntList() );
}

void Player::klopfen()
{
    m_game->postEvent( PlayerHasDoubled, id() );
}

bool Player::isPlayer() const
{
    if( m_game && m_game->gameInfo() ) 
        return ( m_game->gameInfo()->spieler() == this || m_game->gameInfo()->mitspieler() == this );
    else
        return false;
}

CardList* Player::stiche() const
{
    return m_stiche;
}

void Player::addStich( CardList stich )
{
    for( int i = 0; i < stich.count(); i++ )
        m_stiche->append( stich.at( i ) );
}

void Player::removeTrumpf(CardList* liste)
{
    Card *card;
    CardList* entfernen=new CardList;
    for( int i = 0; i < liste->count(); i++ )
    {
        card = liste->at( i );
        if(m_game->gameInfo()->istTrumpf(card)) entfernen->append(card);
    }
    liste->RemoveCards(entfernen);
    delete entfernen;
}

bool Player::hasTrumpf(CardList* liste)
{
    Card* card;
    for( int i = 0; i < liste->count(); i++ )
    {
        card = liste->at( i );
        if(m_game->gameInfo()->istTrumpf(card)) return true;
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
        return SpielerKarten; // müssen alle Karten zurückgegeben werden
    else
    {
        if(istTrumpf(card)) // müssen alle trümpfe zurückgegeben werden
        {
            AntiMaske=PlayerCards();
            removeTrumpf(AntiMaske);
            SpielerKarten->RemoveCards(AntiMaske);
            delete AntiMaske;
            return SpielerKarten;
        }
        else // muss die gleiche Farbe ohne Trümpfe zurückgegeben werden
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

    if(m_game->gameInfo()->mode()==GameInfo::RUFSPIEL)
    {
        CardList* Sau=allowed->FindCards(m_game->gameInfo()->color(), Card::SAU);
        CardList* Spielfarbe= allowed->FindCards(m_game->gameInfo()->color(), Card::NOSTICH);
        // entferne alle trümpfe aus Spielfarbe
        for( int i = 0; i < Spielfarbe->count(); i++ )
        {
            if( istTrumpf( Spielfarbe->at(i) ) )
            {
                Spielfarbe->removeCard( Spielfarbe->at(i) );
                qDebug() << name() << ": Entferne trumpf aus Spielfarbe";
            }
        }

        if(!Sau->isEmpty()) //muss nur was machen wenn ich die Sau habe
        {
            if(firstPlayedCard()&&!istTrumpf(firstPlayedCard())&&firstPlayedCard()->color()==m_game->gameInfo()->color())
            {
                delete allowed;
                allowed=Sau;
            }
            else if(m_game->currStich()->count()==0)
            {
                if(Spielfarbe->count()<4)
                {
                    Spielfarbe->RemoveCards(Sau);
                    allowed->RemoveCards(Spielfarbe);
                    delete Sau;
                }
            }
            else if(m_game->currStich()->count()>0 && m_cards->count()>1)
            {
                allowed->RemoveCards(Sau);      // Rufsau darf nicht geschmiert werden
                delete Sau;
            }
        }
        //delete Sau;
        delete Spielfarbe;
    }
    return allowed;
}
