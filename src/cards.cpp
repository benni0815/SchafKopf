// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "cards.h"

#include "schafkopfdef.h"

#include <QRandomGenerator>

#include <unordered_map>


namespace Cards
{

QString toString(const Card& card)
{
  QString res;

  switch (card.id.color) {
  case CardId::Eichel: res += "Eichel"; break;
  case CardId::Gras: res += "Gras"; break;
  case CardId::Herz: res += "Herz"; break;
  case CardId::Schellen: res += "Schellen"; break;
  default: res += "Nocolor"; break;
  }

  res += ' ';

  switch (card.id.type) {
  case CardId::Sau: res += "Sau"; break;
  case CardId::Zehn: res += "Zehn"; break;
  case CardId::Koenig: res += "Koenig"; break;
  case CardId::Ober: res += "Ober"; break;
  case CardId::Unter: res += "Unter"; break;
  case CardId::Neun: res += "Neun"; break;
  case CardId::Acht: res += "Acht"; break;
  case CardId::Sieben: res += "Sieben"; break;
  default: res += "Unknown"; break;
  }

  return res;
}

int getPoints(const Card& card)
{
  static const auto map = QMap<CardId::Type, int>{
    {CardId::Sau,    11},
    {CardId::Zehn,   10},
    {CardId::Koenig, 4},
    {CardId::Ober,   3},
    {CardId::Unter,  2}
  };

  return map.value(card.id.type, 0);
}

std::vector<CardId> initIds()
{
  using Type = CardId::Type;
  using Color = CardId::Color;

  std::vector<CardId> ids;
  for (const auto type : {Type::Sau, Type::Koenig, Type::Ober, Type::Unter, Type::Zehn, Type::Neun, Type::Acht, Type::Sieben})
  {
    for (const auto color : {Color::Eichel, Color::Gras, Color::Herz, Color::Schellen})
    {
      ids.push_back({type, color});
    }
  }
  return ids;
}

CardList initCards()
{
  CardList cards;
  const auto ids = initIds();
  std::transform(std::begin(ids), std::end(ids), std::back_inserter(cards), [] (CardId id) { return Card{id}; });
  return cards;
}

CardList randomize()
{
  int rnd[CARD_CNT];

  for (int i = 0; i < CARD_CNT; ++i)
  {
    int rval = QRandomGenerator::global()->bounded(RAND_MAX) % CARD_CNT;
    rnd[i] = rval;
    for (int a = 0; a < i; ++a)
    {
      if (rnd[a] == rval)
      {
        i--;
        break;
      }
    }
  }

  const auto cards = initCards();
  CardList randomized;
  for (int i = 0; i < CARD_CNT; ++i)
    randomized.append(cards.at(rnd[i]));
  return randomized;
}

int calcPoints(const CardList& cards)
{
  auto add = [] (int points, const Card& card) { return points + getPoints(card); };
  return std::accumulate(std::begin(cards), std::end(cards), 0, add);
}

std::optional<Card> find(const CardList& cards, CardId id)
{
  auto it = std::find_if(std::begin(cards), std::end(cards), [&] (const Card& card) { return card.id == id; });
  if (it == std::end(cards))
  {
    return {};
  }
  return *it;
}

void remove(CardList& cards, CardId id)
{
  auto it = std::find_if(std::begin(cards), std::end(cards), [&] (const Card& c) { return c.id == id; });
  if (it != std::end(cards))
  {
    cards.erase(it);
  }
}

CardList findByColor(const CardList& cards, CardId::Color color)
{
  CardList found;
  std::copy_if(std::begin(cards), std::end(cards), std::back_inserter(found), [&] (const Card& card) { return card.id.color == color; });
  return found;
}

void remove(CardList& cards, const std::vector<CardId>& itemsToRem)
{
  for (const auto& cardToRem : itemsToRem)
  {
    for (const auto& card : cards)
    {
      if (card.id == cardToRem)
        remove(cards, card.id);
    }
  }
}

void sort(CardList& cards, EvalFunc eval)
{
  auto compare = [eval] (const Card& lhs, const Card rhs) { return eval(lhs) < eval(rhs); };
  std::sort(std::begin(cards), std::end(cards), compare);
}

std::vector<CardId> toIds(const CardList& cards)
{
  std::vector<CardId> ids;
  std::transform(std::begin(cards), std::end(cards), std::back_inserter(ids), [] (const Card& card) { return card.id; });
  return ids;
}

} // namespace Cards
