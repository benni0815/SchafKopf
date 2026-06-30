// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QPixmap>

#include <cardcache.h>

class QPixmap;
class Player;
class QString;

///This class represents one card.
///
///@author Dominik Seichter
class Card
{
public:
  // TODO: find better names, Edit bei lenz: done =;-)
  // DS: renamed to avoid name clashes
  enum Type { /*ASS*/NOSTICH=-1, SAU = 1, ZEHN = 17, KOENIG = 5, OBER = 9, UNTER = 13, NEUN = 21, ACHT = 25, SIEBEN = 29 };
  enum Color { NOCOLOR = -1, EICHEL = 0, GRAS = 1, HERZ = 2, SCHELLEN = 3 };

  struct Id
  {
    Type type{NOSTICH};
    Color color{NOCOLOR};
  };

  Card( const int id );
  Card(const Type t, const Color c);
  ~Card() = default;

  /// return a pixmap of the card
  const QPixmap& pixmap() const;

  /// return a pixmap for the background of a card.
  static const QPixmap& backgroundPixmap();

  /// return how much points the card has.
  /// e.g. king = 4 points
  int points() const;

  /// return the color of the card.
  /// e.g. heart.
  Color color() const { return m_id.color; }

  /// return the type of the card, e.g. ASS
  Type card() const { return m_id.type; }

  KCardInfo kdeCardInfo() const;

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

  /// a id(). Using this id the card can be quickly identified
  /// as the id() is calculated using color and type.
  int id() const;

  /// Call this whenever the carddeck has changed and
  /// the internal pixmap should be reloaded.
  void cardDeckChanged();

  /// Returns a non-localized textual representation of the card,
  ///  for example "Herz Ass"
  QString toString() const;

private:
  void init(const Type t, const Color c);

  Id m_id{Type::NOSTICH, Color::NOCOLOR};

  Player* m_owner{};

  QPixmap m_pixmap;
};
