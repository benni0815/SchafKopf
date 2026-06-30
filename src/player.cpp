#include "player.h"

#include "game.h"
#include "cards.h"

#include <QDebug>

#include <string>

Player::Player(unsigned int id, const Game& game)
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

    emit gotCards(id(), Cards::toIds(m_cards));
}

void Player::removeCard(const CardId& id)
{
  const auto it = std::find_if(std::begin(m_cards), std::end(m_cards), [&] (const Card& card) { return card.id == id; });
  if (it == std::end(m_cards))
  {
    return;
  }
  m_cards.erase(it);
}

void Player::sortCards()
{
  Cards::sort(m_cards, [this] (const Card& card) { return m_game.gameInfo().evalCard(card.id); });
  emit gotCards(id(), Cards::toIds(m_cards));
}

bool Player::isPlayer() const
{
  return (m_game.gameInfo().spieler() == this || m_game.gameInfo().mitspieler() == this);
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
  auto eraseFrom = std::remove_if(std::begin(liste), std::end(liste), [this] (const Card& card) { return istTrumpf(card); });
  liste.erase(eraseFrom, std::end(liste));
}

bool Player::istTrumpf(const Card& card)
{
    return m_game.gameInfo().istTrumpf(card.id);
}

CardList Player::PlayerCards()
{
  return m_cards;
}

std::optional<Card> Player::firstPlayedCard()
{
    const auto& currStich = m_game.currStich();
    if (currStich.empty())
      return {};
    else
        return currStich.front();
}

CardList Player::cardsOfSameType(const std::optional<Card>& card)
{
    auto SpielerKarten = PlayerCards();

    if (!card)
        return SpielerKarten; // müssen alle Karten zurückgegeben werden
    else
    {
        if (istTrumpf(card.value())) // müssen alle trümpfe zurückgegeben werden
        {
            auto AntiMaske = PlayerCards();
            removeTrumpf(AntiMaske);
            Cards::remove(SpielerKarten, Cards::toIds(AntiMaske));
            return SpielerKarten;
        }
        else // muss die gleiche Farbe ohne Trümpfe zurückgegeben werden
        {
            removeTrumpf(SpielerKarten);
            return Cards::findByColor(SpielerKarten, firstPlayedCard()->id.color);
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

    const auto& gameInfo = m_game.gameInfo();
    if (gameInfo.mode() == GameVariant::RUFSPIEL)
    {
        CardList Spielfarbe = Cards::findByColor(allowed, gameInfo.color());

        // entferne alle trümpfe aus Spielfarbe
        for (int i = 0; i < Spielfarbe.size(); ++i)
        {
            if (istTrumpf(Spielfarbe.at(i)))
            {
                Cards::remove(Spielfarbe, Spielfarbe.at(i).id);
                qDebug() << name() << ": Entferne trumpf aus Spielfarbe";
            }
        }

        auto Sau = Cards::find(allowed, {CardId::Sau, gameInfo.color()});
        if (Sau) //muss nur was machen wenn ich die Sau habe
        {
            if (firstPlayedCard() && !istTrumpf(firstPlayedCard().value()) && firstPlayedCard()->id.color == gameInfo.color())
            {
                allowed = {Sau.value()};
            }
            else if (m_game.currStich().empty())
            {
                if (Spielfarbe.size() < 4)
                {
                    Cards::remove(Spielfarbe, Sau->id);
                    Cards::remove(allowed, Cards::toIds(Spielfarbe));
                }
            }
            else if (m_game.currStich().size() > 0 && m_cards.size() > 1)
            {
                Cards::remove(allowed, Sau->id);      // Rufsau darf nicht geschmiert werden
            }
        }
    }
    return allowed;
}
