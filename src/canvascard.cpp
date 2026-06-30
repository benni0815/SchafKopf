// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "canvascard.h"
#include "card.h"
#include "settings.h"

#include <QTimer>
#include <QPainter>
#include <QGraphicsDropShadowEffect>


namespace
{

QPixmap& backside()
{
  static QPixmap pix;
  return pix;
}

} // namespace

QPixmap CanvasCard::frontsidePixmap(CardId id)
{
  return Settings::instance()->cardCache()->frontside(Card::kdeCardInfo(id));
}

const QPixmap& CanvasCard::backsidePixmap()
{
  return backside();
}

CanvasCard::CanvasCard()
 :  QGraphicsPixmapItem()
{
    setFlag( QGraphicsItem::ItemIsSelectable );
    setTransformationMode( Qt::SmoothTransformation );

    auto dropShadowEffect = new QGraphicsDropShadowEffect(this);
    dropShadowEffect->setOffset(2, 2);
    dropShadowEffect->setBlurRadius(5);
    setGraphicsEffect(dropShadowEffect);

    timer = new QTimer( this );
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

    forbiddenFrontPixmap.convertFromImage( forbiddenFrontImage );

    backside() = QPixmap(Settings::instance()->cardCache()->backside());

    float height = frontPixmap.height();
    float width = frontPixmap.width();
    setTransformOriginPoint(height/2, width/2);

    updatePixmap();
}

void CanvasCard::updatePixmap()
{
    if( !m_visible )
    {
      setPixmap(backsidePixmap());
      return;
    }
    if( m_forbidden )
    {
        setPixmap( forbiddenFrontPixmap );
        return;
    }
    setPixmap( frontPixmap );
    QGraphicsPixmapItem::update();
}

void CanvasCard::setFrontVisible( bool b )
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

void CanvasCard::setDestination( int x, int y )
{
    tox = x;
    toy = y;
}

void CanvasCard::animatedMove()
{
    connect(timer, &QTimer::timeout, this, &CanvasCard::moveLoop);
    timer->start( 2 );
}

void CanvasCard::moveLoop()
{
    int speedx = 1, speedy = 1;
    if( tox-x()>60 || x()-tox>60 ) speedx=20;
    if( toy-y()>60 || y()-toy>60 ) speedy=20;
    if(tox>x())
        moveBy( speedx, 0 );
    else if(tox!=x())
        moveBy( -speedx, 0 );
    if(toy>y())
        moveBy( 0, speedy );
    else if(toy!=y())
        moveBy( 0, -speedy );
    if(tox==x() && toy==y())
    {
        timer->stop();
        disconnect(timer, &QTimer::timeout, this, &CanvasCard::moveLoop);
    }
}

void CanvasCard::cardDeckChanged()
{
  if (m_card != CardId{})
  {
    reloadPixmaps();
  }
}
