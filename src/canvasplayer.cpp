// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "canvasplayer.h"

#include "canvascard.h"
#include "settings.h"

#include <QFont>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include <QLabel>


CanvasPlayer::CanvasPlayer(int i, QGraphicsScene* scene)
: m_position(i)
, m_scene(scene)
{
  create();
  // TODO: get a correct id! especially important for networking!
  m_id = i;

  m_bubbleLabel = new QLabel();
  m_bubbleLabel->setStyleSheet("background: lightgrey; font: 12pt; border: 1px solid black;");
  m_bubble = m_scene->addWidget(m_bubbleLabel);
  m_bubble->setParent(m_scene);
  m_bubble->hide();

  if constexpr (SIMULATION_MODE)
  {
    m_is_human = false;
  }
  else
  {
    // TODO: This should be passed as flag depending on Player::isHuman();
    m_is_human = (m_id == 0);
  }
}

void CanvasPlayer::create()
{
  for (auto& card : m_items)
  {
    card = new CanvasCard();
    m_scene->addItem(card);
  }

  m_cards.clear();

  m_name = new QGraphicsSimpleTextItem();
  m_name->setBrush(QBrush(Qt::white));
  m_name->setFont(QFont("Helvetica", 24));
  m_name->hide();
  m_scene->addItem(m_name);

  m_player_name = QString();
}

void CanvasPlayer::position()
{
  int x = 0, y = 0;
  int num = 0;
  int w = m_scene->width();
  int h = m_scene->height();
  int offsetl = 0; //Hiermit kann man am linken Rand Platz schaffen fuer z.B. ein Bild des Spielers (Netzwerkmodus).
  int availw = m_scene->width() - 2 * DIST - offsetl;
  const auto& pix = CanvasCard::backsidePixmap();
  int cardw = pix.width();
  int cardh = pix.height();
  float cardoverlap = 1. / 8.; //Ueberlapp der Karten im Verhaeltnis zur Kartenbreite

  for (unsigned int z = 0; z < NUMCARDS; z++)
  {
    CanvasCard* card = m_items[z];
    if (card->isVisible())
    {
      num++;
    }
  }

  if (!Settings::instance()->rearrangeCards())
  {
    num = NUMCARDS;
  }

  if (m_position == 1 || m_position == 3)
  {
    qSwap(cardw, cardh);
  }

  switch (m_position)
  {
  case 0:
    if (availw > num * cardw + (num - 1))
    {
      x = (w - cardw * num) / 2 - (num - 1) / 2 - offsetl;
    }
    else
    {
      x = DIST + offsetl;
    }
    y = h - cardh - DIST;

    m_name->setPos((w - m_name->sceneBoundingRect().width()) / 2, y - m_name->sceneBoundingRect().height());
    break;

  case 1:
    x = DIST;
    y = (h - ((cardh * cardoverlap) * (num - 1) + cardh)) / 2;

    m_name->setPos(x, y - m_name->sceneBoundingRect().height());
    break;

  case 2:
    x = (w - ((cardw * cardoverlap) * (num - 1) + cardw)) / 2; //berechnet die Position der linken Seite des Kartenstapels
    y = DIST;

    m_name->setPos((w - m_name->sceneBoundingRect().width()) / 2, y + cardh);
    break;

  case 3:
  default:
    x = w - cardw - DIST;
    y = (h - ((cardh * cardoverlap) * (num - 1) + cardh)) / 2;

    m_name->setPos(x, y - m_name->sceneBoundingRect().height());
    break;
  }

  for (unsigned int z = 0; z < NUMCARDS; z++)
  {
    CanvasCard* card = m_items[z];
    // only move if necessary!
    if (card->isVisible() || !Settings::instance()->rearrangeCards())
    {
      if (x != card->x() || y != card->y())
      {
        if (num == NUMCARDS)
        {
          card->setPos(x, y);
        }
        else
        {
          card->setDestination(x, y);
          card->animatedMove();
        }
      }
      if (m_position == 0)
      {
        if (availw > num * cardw + (num - 1))
        {
          x += cardw + 1;
        }
        else
        {
          x += (availw - cardw) / (num - 1);
        }
      }
      else
      {
        if (m_position == 2)
        {
          x += (cardw * cardoverlap);
        }
        else
        {
          y += (cardh * cardoverlap);
        }
      }
    }
  }

  // swap them back
  if (m_position == 1 || m_position == 3)
  {
    qSwap(cardw, cardh);
  }
}

void CanvasPlayer::init()
{
  m_name->setText(m_player_name);

  if (!m_cards.empty())
  {
    for (size_t z = 0; z < m_cards.size(); ++z)
    {
      CanvasCard* c = m_items[z];
      c->setCardId(m_cards.at(z));
      c->setZValue(-1 - static_cast<int>(z));
      c->show();

      if (m_position == 1)
      {
        c->setRotation(270);
      }
      else if (m_position == 3)
      {
        c->setRotation(90);
      }

      if constexpr (DebugMode)
      {
        c->setFrontVisible(true);
      }
      else
      {
        if (!m_is_human)
        {
          c->setFrontVisible(false);
          continue;
        }

        auto visible = m_has_doubled || (m_is_last ? z >= NUMCARDS / 2
                                                   : z < NUMCARDS / 2);
        c->setFrontVisible(visible);
      }
    }
    m_name->show();
  }
  else
  {
    for (unsigned int z = 0; z < NUMCARDS; z++)
    {
      m_items[z]->hide();
    }

    m_name->hide();
  }
}

CanvasCard* CanvasPlayer::canvasCard(int index)
{
  if (index < 0 || index >= NUMCARDS)
  {
    return nullptr;
  }

  CanvasCard* card = m_items[index];
  return card->isVisible() ? card
                           : nullptr;
}

CanvasCard* CanvasPlayer::hasCard(CardId cardid) const
{
  for (unsigned int i = 0; i < NUMCARDS; i++)
  {
    CanvasCard* card = m_items[i];
    if (card->cardId() == cardid)
    {
      return card;
    }
  }
  return nullptr;
}

void CanvasPlayer::cardPlayed(CardId id)
{
  if (CanvasCard* card = hasCard(id))
  {
    card->hide();
  }
}

void CanvasPlayer::setCards(const std::vector<CardId>& cards)
{
  for (int i = 0; i < NUMCARDS; i++)
  {
    m_items[i]->setCardId({});
  }

  m_cards = cards;
}

void CanvasPlayer::say(const QString& message, unsigned int)
{
  m_bubbleLabel->setText(message);
  m_bubble->setPos(-10000, -10000);
  m_bubble->show();
  QPointF p;
  switch (m_position)
  {
  case 1:
    p = QPointF(m_name->x(), m_name->y() - 40);
    break;

  case 2:
    p = QPointF(m_name->x() + m_name->sceneBoundingRect().width() / 2 - m_bubble->size().width() / 2, m_name->y() + 40);
    break;

  case 3:
  default:
    p = QPointF(m_scene->width() - m_bubble->size().width() - 40, m_name->y() - 40);
    break;
  }
  m_bubble->setPos(p);
  QTimer::singleShot(std::chrono::seconds(1), m_scene, [this] { m_bubble->hide(); });
}

void CanvasPlayer::hideBubble()
{
  m_bubble->hide();
}

void CanvasPlayer::setHasDoubled(bool h)
{
  m_has_doubled = h;
}

void CanvasPlayer::setLast(bool l)
{
  m_is_last = l;
}

void CanvasPlayer::setName(const QString& name)
{
  m_player_name = name;
}

bool CanvasPlayer::isHuman() const
{
  return m_is_human;
}

unsigned int CanvasPlayer::id() const
{
  return m_id;
}
