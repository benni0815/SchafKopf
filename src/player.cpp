
#include "player.h"
#include "game.h"
#include "cardlist.h"

#include <QDebug>

#include <string>
#include <iostream>
#include <cstdlib>


Player::Player(unsigned int id, Game* game)
  : m_game(game)
  , m_id(id)
{}

const QString & Player::name() const
{
    return m_name;
}

void Player::setName(const QString& name)
{
  m_name = name;
  emit nameChanged(id(), m_name);
}

const CardList& Player::cards() const
{
    return m_cards;
}

void Player::setCards(const CardList& cards)
{
    m_geklopft = false;
    m_cards = cards;

    // tell the card WHO owns them and WHOM they should serve! ;-)
    for (auto& card : m_cards)
    {
      card->setOwner(this);
    }

    emit gotCards(id(), m_cards.toIds());
}

void Player::removeCard(const Card* card)
{
  const auto it = std::find(std::begin(m_cards), std::end(m_cards), card);
  if (it == std::end(m_cards))
  {
    return;
  }
  m_cards.erase(it);
}

void Player::sortCards()
{
    m_cards.sort([this] (Card* card) { return m_game->gameInfo()->evalCard(card); });
    emit gotCards(id(), m_cards.toIds());
}

void Player::klopfen()
{
  emit doubled(id());
}

bool Player::isPlayer() const
{
    if( m_game && m_game->gameInfo() ) 
        return ( m_game->gameInfo()->spieler() == this || m_game->gameInfo()->mitspieler() == this );
    else
        return false;
}

const CardList& Player::stiche() const
{
    return m_stiche;
}

void Player::clearStiche()
{
  m_stiche.clear();
}

void Player::addStich(const CardList& stich)
{
  std::copy(std::begin(stich), std::end(stich), std::back_inserter(m_stiche));
}

void Player::removeTrumpf(CardList& liste)
{
  auto eraseFrom = std::remove_if(std::begin(liste), std::end(liste), [this] (Card* card) { return istTrumpf(card); });
  liste.erase(eraseFrom, std::end(liste));
}

bool Player::istTrumpf(Card* card)
{
    return m_game->gameInfo()->istTrumpf(card);
}

CardList Player::PlayerCards()
{
    return m_cards.FindCards(Card::NoColor, Card::NoStich);
}

Card* Player::firstPlayedCard()
{
    const auto& currStich = m_game->currStich();
    if (currStich.empty())
        return nullptr;
    else
        return currStich.front();
}

CardList Player::cardsOfSameType(Card* card)
{
    auto SpielerKarten = PlayerCards();

    if (!card)
        return SpielerKarten; // müssen alle Karten zurückgegeben werden
    else
    {
        if(istTrumpf(card)) // müssen alle trümpfe zurückgegeben werden
        {
            auto AntiMaske = PlayerCards();
            removeTrumpf(AntiMaske);
            SpielerKarten.RemoveCards(AntiMaske);
            return SpielerKarten;
        }
        else // muss die gleiche Farbe ohne Trümpfe zurückgegeben werden
        {
            removeTrumpf(SpielerKarten);
            return SpielerKarten.FindCards(firstPlayedCard()->color(), Card::NoStich);
        }
    }
}

CardList Player::allowedCards()
{
    CardList allowed = cardsOfSameType(firstPlayedCard());
    if (allowed.empty())
    {
        allowed = PlayerCards();
    }

    if(m_game->gameInfo()->mode()==GameInfo::RUFSPIEL)
    {
        CardList Sau = allowed.FindCards(m_game->gameInfo()->color(), Card::Sau);
        CardList Spielfarbe = allowed.FindCards(m_game->gameInfo()->color(), Card::NoStich);

        // entferne alle trümpfe aus Spielfarbe
        for (int i = 0; i < Spielfarbe.size(); ++i)
        {
            if (istTrumpf(Spielfarbe.at(i)))
            {
                Spielfarbe.removeCard(Spielfarbe.at(i));
                qDebug() << name() << ": Entferne trumpf aus Spielfarbe";
            }
        }

        if (!Sau.empty()) //muss nur was machen wenn ich die Sau habe
        {
            if(firstPlayedCard()&&!istTrumpf(firstPlayedCard())&&firstPlayedCard()->color()==m_game->gameInfo()->color())
            {
                allowed=Sau;
            }
            else if (m_game->currStich().empty())
            {
                if (Spielfarbe.size() < 4)
                {
                    Spielfarbe.RemoveCards(Sau);
                    allowed.RemoveCards(Spielfarbe);
                }
            }
            else if (m_game->currStich().size() > 0 && m_cards.size() > 1)
            {
                allowed.RemoveCards(Sau);      // Rufsau darf nicht geschmiert werden
            }
        }
    }
    return allowed;
}
