
#include "player.h"

#include "cardlist.h"

unsigned int Player::def_id=0;

Player::Player(CardList *cards,Game* game)
    : m_cards( cards ), m_game( game )
{
    m_stiche = new CardList();
    m_id=def_id++;
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
}
        
const CardList* Player::stiche() const
{
    return m_stiche;
}

void Player::addStich( CardList stich )
{
    for( unsigned int i = 0; i < stich.count(); i++ )
        m_stiche->append( stich.at( i ) );
}

