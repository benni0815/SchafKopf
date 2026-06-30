// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "types.h"

#include <QGraphicsPixmapItem>
#include <QPixmap>


class CanvasCard : public QObject, public QGraphicsPixmapItem
{
  Q_OBJECT

public:
  static QPixmap frontsidePixmap(CardId id);

  /// Returns a pixmap for the background of a card.
  static const QPixmap& backsidePixmap();

  CanvasCard();
  ~CanvasCard() override = default;

  void setCardId(CardId cardId);
  CardId cardId() const { return m_cardId; }

  void setFrontVisible(bool b);

  void animatedMove();
  void setDestination(int x, int y);

  /// Marks this card as forbidden to play in the UI.
  void forbidden();

private:
  void updatePixmap();
  void reloadPixmaps();

  QPixmap frontPixmap;
  QPixmap forbiddenFrontPixmap;

  bool m_visible{false};
  bool m_forbidden{false};

  int tox{0};
  int toy{0};
  QTimer* timer{};

  CardId m_cardId{CardId::NoStich, CardId::NoColor};

private slots:
  void disableForbidden();
  void moveLoop();
  void cardDeckChanged();
};
