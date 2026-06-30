// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "types.h"

#include <cardcache.h>

#include <numeric>
#include <vector>


class Player;

///This class represents one card.
///
///@author Dominik Seichter
class Card
{
public:
  static KCardInfo kdeCardInfo(CardId id);

  Card(const CardId& id);
  Card(const CardId::Type t, const CardId::Color c);
  ~Card() = default;

  /// return how much points the card has.
  /// e.g. king = 4 points
  int points() const;

  CardId id() const;

  /// return the color of the card.
  /// e.g. heart.
  CardId::Color color() const { return m_id.color; }

  /// return the type of the card, e.g. ASS
  CardId::Type card() const { return m_id.type; }

  /// set the owner of the card.
  /// only do this when the cards are given,
  /// not after a trick was made!
  void setOwner( Player* owner ) { m_owner = owner; }

  /// return the owner of this card.
  /// we need this to be able to know who
  /// played this card of the trick was made.
  const Player* owner() const { return m_owner; }

  /// this operator< is not quite correct,
  /// it just tells wether ass is bigger than 10 or 10
  /// is bigger than king, but does not care
  /// for color or what is currently played.
  /// So it does not care for Wenz or Solo's.
  /// it is used in Game::highesCard
  bool operator<(const Card* const c) const;

  bool isEqual(const Card* const othercard) const;

  /// Returns a non-localized textual representation of the card,
  ///  for example "Herz Ass"
  QString toString() const;

private:
  CardId m_id{CardId::NoStich, CardId::Color::NoColor};
  Player* m_owner{};
};

