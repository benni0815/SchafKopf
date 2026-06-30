// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cardcache.h>


class Player;

///This class represents one card.
///
///@author Dominik Seichter
class Card
{
public:
  enum Type { NoStich = -1, Sau = 1, Koenig = 5, Ober = 9, Unter = 13, Zehn = 17, Neun = 21, Acht = 25, Sieben = 29 };
  enum Color { NoColor = -1, Eichel = 0, Gras = 1, Herz = 2, Schellen = 3 };

  struct Id
  {
    Type type{NoStich};
    Color color{NoColor};
  };

  static KCardInfo kdeCardInfo(Id id);

  Card(const Id& id);
  Card(const Type t, const Color c);
  ~Card() = default;

  /// return how much points the card has.
  /// e.g. king = 4 points
  int points() const;

  Id id() const;

  /// return the color of the card.
  /// e.g. heart.
  Color color() const { return m_id.color; }

  /// return the type of the card, e.g. ASS
  Type card() const { return m_id.type; }

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
  Id m_id{Type::NoStich, Color::NoColor};
  Player* m_owner{};
};

inline bool operator==(const Card::Id& lhs, const Card::Id& rhs)
{
  return lhs.type == rhs.type && lhs.color == rhs.color;
}

inline bool operator!=(const Card::Id& lhs, const Card::Id& rhs)
{
  return !(lhs == rhs);
}
