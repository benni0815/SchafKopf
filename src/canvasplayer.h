// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "schafkopfdef.h"
#include "types.h"

#include <QTimer>


class CanvasCard;
class Player;
class QLabel;
class QGraphicsScene;
class QGraphicsView;
class QGraphicsSimpleTextItem;
class QGraphicsProxyWidget;

/// Represents a player on the canvas
/// @author Dominik Seichter
class CanvasPlayer
{
public:
  CanvasPlayer() = default;
  CanvasPlayer(int i, QGraphicsScene* scene);
  ~CanvasPlayer() = default;

  void position();
  void init();

  unsigned int id() const;
  bool isHuman() const;

  void setHasDoubled(bool h);
  void setLast(bool l);
  void setName(const QString& name);
  void setCards(const std::vector<CardId>& cards);
  void say(const QString& message, unsigned int playerid);
  void hideBubble();

  CanvasCard* canvasCard(int index);
  void cardPlayed(CardId id);
  CanvasCard* hasCard(CardId cardid) const;

private:
  void create();

  unsigned int m_id{0};
  int m_position{0};
  bool m_has_doubled{false};
  bool m_is_last{false};
  bool m_is_human{false};
  QString m_player_name;

  QGraphicsScene* m_scene{};
  QGraphicsSimpleTextItem* m_name{};
  QLabel* m_bubbleLabel{};
  QGraphicsProxyWidget* m_bubble{};
  CanvasCard* m_items[NUMCARDS]{};
  std::vector<CardId> m_cards;
};
