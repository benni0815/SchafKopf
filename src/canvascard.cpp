// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "canvascard.h"

#include "settings.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QTimer>

namespace
{

QPixmap& backside()
{
  static QPixmap pix;
  return pix;
}

KCardInfo kdeCardInfo(CardId id)
{
  static const auto colorMap = std::unordered_map<CardId::Color, KCardInfo::Suit>{
    {CardId::Eichel, KCardInfo::Club},
    {CardId::Gras, KCardInfo::Spade},
    {CardId::Herz, KCardInfo::Heart},
    {CardId::Schellen, KCardInfo::Diamond}
  };

  static const auto typeMap = std::unordered_map<CardId::Type, KCardInfo::Card>{
    {CardId::Sau,    KCardInfo::Ace},
    {CardId::Koenig, KCardInfo::King},
    {CardId::Ober,   KCardInfo::Queen},
    {CardId::Unter,  KCardInfo::Jack},
    {CardId::Zehn,   KCardInfo::Ten},
    {CardId::Neun,   KCardInfo::Nine},
    {CardId::Acht,   KCardInfo::Eight},
    {CardId::Sieben, KCardInfo::Seven}
  };

  const auto colorIt = colorMap.find(id.color);
  const auto color = (colorIt == std::end(colorMap)) ? KCardInfo::Suit::None
                                                     : colorIt->second;
  const auto typeIt = typeMap.find(id.type);
  const auto type = (typeIt == std::end(typeMap)) ? KCardInfo::Card::Joker
                                                  : typeIt->second;

  return KCardInfo(color, type);
}

} // namespace

QPixmap CanvasCard::frontsidePixmap(CardId id)
{
  return Settings::instance()->cardCache()->frontside(kdeCardInfo(id));
}

const QPixmap& CanvasCard::backsidePixmap()
{
  return backside();
}

CanvasCard::CanvasCard()
: QGraphicsPixmapItem()
{
  setFlag(QGraphicsItem::ItemIsSelectable);
  setTransformationMode(Qt::SmoothTransformation);

  auto dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setOffset(2, 2);
  dropShadowEffect->setBlurRadius(5);
  setGraphicsEffect(dropShadowEffect);

  timer = new QTimer(this);
  connect(Settings::instance(), &Settings::cardChanged, this, &CanvasCard::cardDeckChanged);
}

void CanvasCard::setCard(CardId cardId)
{
  m_card = cardId;
  if (m_card != CardId{})
  {
    reloadPixmaps();
  }
}

void CanvasCard::reloadPixmaps()
{
  frontPixmap = frontsidePixmap(m_card);

  QImage forbiddenFrontImage = frontPixmap.toImage();

  {
    QPainter p(&forbiddenFrontImage);
    p.setBrush(QBrush(QColor::fromRgbF(1, 1, 1, 0.5)));
    p.drawRect(QRect(QPoint(0, 0), forbiddenFrontImage.size()));
  }

  forbiddenFrontPixmap.convertFromImage(forbiddenFrontImage);

  backside() = QPixmap(Settings::instance()->cardCache()->backside());

  float height = frontPixmap.height();
  float width = frontPixmap.width();
  setTransformOriginPoint(height / 2, width / 2);

  updatePixmap();
}

void CanvasCard::updatePixmap()
{
  if (!m_visible)
  {
    setPixmap(backsidePixmap());
    return;
  }
  if (m_forbidden)
  {
    setPixmap(forbiddenFrontPixmap);
    return;
  }
  setPixmap(frontPixmap);
  QGraphicsPixmapItem::update();
}

void CanvasCard::setFrontVisible(bool b)
{
  m_visible = b;
  updatePixmap();
}

void CanvasCard::forbidden()
{
  m_forbidden = true;
  QTimer::singleShot(std::chrono::seconds(1), this, &CanvasCard::disableForbidden);
  updatePixmap();
}

void CanvasCard::disableForbidden()
{
  m_forbidden = false;
  updatePixmap();
}

void CanvasCard::setDestination(int x, int y)
{
  tox = x;
  toy = y;
}

void CanvasCard::animatedMove()
{
  connect(timer, &QTimer::timeout, this, &CanvasCard::moveLoop);
  timer->start(2);
}

void CanvasCard::moveLoop()
{
  if (tox == x() && toy == y())
  {
    timer->stop();
    disconnect(timer, &QTimer::timeout, this, &CanvasCard::moveLoop);
    return;
  }

  auto getStep = [] (int dest, int pos) -> int
  {
    if (dest == pos) { return 0; }
    const auto diff = dest - pos;
    const auto absSpeed = std::abs(diff) > 60 ? 20 : 1;
    return std::copysign(absSpeed, diff);
  };

  moveBy(getStep(tox, x()), getStep(toy, y()));
}

void CanvasCard::cardDeckChanged()
{
  if (m_card != CardId{})
  {
    reloadPixmaps();
  }
}
