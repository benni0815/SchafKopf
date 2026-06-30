// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QList>
#include <QString>


class Game;
class GameInfo;
class Player;

/// This interface implements the basic functions, to get game results and find out how many points/money who has to give to whom.
///
/// @author Dominik Seichter
class Results
{
public:
  Results(const Game& game);
  ~Results() = default;

  /// Returns a message of the game results, which can be displayed to the user.
  QString result();

  double points(const Player* player);
  QString formatedPoints(Player* player);

private:
  void parse();
  int determineLaufende() const;

  QList<Player*> m_winners;
  QList<Player*> m_losers;

  const Game& m_game;

  int m_laufende{0};

  int m_points{0};
  bool m_schneider{false};
  bool m_schwarz{false};
};
