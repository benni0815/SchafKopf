/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2008 Andreas Pakulat <apaku@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "cardcache.h"
#include "cardcache_p.h"

#include <QDateTime>
#include <QFileInfo>
#include <QMutexLocker>
#include <QPainter>
#include <QPixmap>
#include <QSizeF>
#include <QSvgRenderer>


#include "carddeckinfo.h"

#define DECKLIST_LENGTH 53

KCardInfo fullDeckList[DECKLIST_LENGTH] = {
    KCardInfo(KCardInfo::Club, KCardInfo::Ace),
    KCardInfo(KCardInfo::Heart, KCardInfo::Ace),
    KCardInfo(KCardInfo::Diamond, KCardInfo::Ace),
    KCardInfo(KCardInfo::Spade, KCardInfo::Ace),
    KCardInfo(KCardInfo::Club, KCardInfo::King),
    KCardInfo(KCardInfo::Heart, KCardInfo::King),
    KCardInfo(KCardInfo::Diamond, KCardInfo::King),
    KCardInfo(KCardInfo::Spade, KCardInfo::King),
    KCardInfo(KCardInfo::Club, KCardInfo::Queen),
    KCardInfo(KCardInfo::Heart, KCardInfo::Queen),
    KCardInfo(KCardInfo::Diamond, KCardInfo::Queen),
    KCardInfo(KCardInfo::Spade, KCardInfo::Queen),
    KCardInfo(KCardInfo::Club, KCardInfo::Jack),
    KCardInfo(KCardInfo::Heart, KCardInfo::Jack),
    KCardInfo(KCardInfo::Diamond, KCardInfo::Jack),
    KCardInfo(KCardInfo::Spade, KCardInfo::Jack),
    KCardInfo(KCardInfo::Club, KCardInfo::Ten),
    KCardInfo(KCardInfo::Heart, KCardInfo::Ten),
    KCardInfo(KCardInfo::Diamond, KCardInfo::Ten),
    KCardInfo(KCardInfo::Spade, KCardInfo::Ten),
    KCardInfo(KCardInfo::Club, KCardInfo::Nine),
    KCardInfo(KCardInfo::Heart, KCardInfo::Nine),
    KCardInfo(KCardInfo::Diamond, KCardInfo::Nine),
    KCardInfo(KCardInfo::Spade, KCardInfo::Nine),
    KCardInfo(KCardInfo::Club, KCardInfo::Eight),
    KCardInfo(KCardInfo::Heart, KCardInfo::Eight),
    KCardInfo(KCardInfo::Diamond, KCardInfo::Eight),
    KCardInfo(KCardInfo::Spade, KCardInfo::Eight),
    KCardInfo(KCardInfo::Club, KCardInfo::Seven),
    KCardInfo(KCardInfo::Heart, KCardInfo::Seven),
    KCardInfo(KCardInfo::Diamond, KCardInfo::Seven),
    KCardInfo(KCardInfo::Spade, KCardInfo::Seven),
    KCardInfo(KCardInfo::Club, KCardInfo::Six),
    KCardInfo(KCardInfo::Heart, KCardInfo::Six),
    KCardInfo(KCardInfo::Diamond, KCardInfo::Six),
    KCardInfo(KCardInfo::Spade, KCardInfo::Six),
    KCardInfo(KCardInfo::Club, KCardInfo::Five),
    KCardInfo(KCardInfo::Heart, KCardInfo::Five),
    KCardInfo(KCardInfo::Diamond, KCardInfo::Five),
    KCardInfo(KCardInfo::Spade, KCardInfo::Five),
    KCardInfo(KCardInfo::Club, KCardInfo::Four),
    KCardInfo(KCardInfo::Heart, KCardInfo::Four),
    KCardInfo(KCardInfo::Diamond, KCardInfo::Four),
    KCardInfo(KCardInfo::Spade, KCardInfo::Four),
    KCardInfo(KCardInfo::Club, KCardInfo::Three),
    KCardInfo(KCardInfo::Heart, KCardInfo::Three),
    KCardInfo(KCardInfo::Diamond, KCardInfo::Three),
    KCardInfo(KCardInfo::Spade, KCardInfo::Three),
    KCardInfo(KCardInfo::Club, KCardInfo::Two),
    KCardInfo(KCardInfo::Heart, KCardInfo::Two),
    KCardInfo(KCardInfo::Diamond, KCardInfo::Two),
    KCardInfo(KCardInfo::Spade, KCardInfo::Two),
    KCardInfo(KCardInfo::None, KCardInfo::Joker)
};

KCardInfo::KCardInfo(KCardInfo::Suit s, KCardInfo::Card c)
    : m_suit(s), m_card(c)
{
}

KCardInfo::Card KCardInfo::card() const
{
    return m_card;
}

KCardInfo::Suit KCardInfo::suit() const
{
    return m_suit;
}

void KCardInfo::setCard(KCardInfo::Card c)
{
    m_card = c;
}

void KCardInfo::setSuit(KCardInfo::Suit s)
{
    m_suit = s;
}

bool KCardInfo::operator==(KCardInfo c) const
{
    return (c.card() == card() && c.suit() == suit());
}

QString KCardInfo::svgName() const
{
    QString s;
    if (card() == KCardInfo::Ace)
        s += QLatin1String("1_");
    if (card() == KCardInfo::King)
        s += QLatin1String("king_");
    if (card() == KCardInfo::Queen)
        s += QLatin1String("queen_");
    if (card() == KCardInfo::Jack)
        s += QLatin1String("jack_");
    if (card() == KCardInfo::Ten)
        s += QLatin1String("10_");
    if (card() == KCardInfo::Nine)
        s += QLatin1String("9_");
    if (card() == KCardInfo::Eight)
        s += QLatin1String("8_");
    if (card() == KCardInfo::Seven)
        s += QLatin1String("7_");
    if (card() == KCardInfo::Six)
        s += QLatin1String("6_");
    if (card() == KCardInfo::Five)
        s += QLatin1String("5_");
    if (card() == KCardInfo::Four)
        s += QLatin1String("4_");
    if (card() == KCardInfo::Three)
        s += QLatin1String("3_");
    if (card() == KCardInfo::Two)
        s += QLatin1String("2_");
    if (suit() == KCardInfo::Club)
        s += QLatin1String("club");
    if (suit() == KCardInfo::Spade)
        s += QLatin1String("spade");
    if (suit() == KCardInfo::Diamond)
        s += QLatin1String("diamond");
    if (suit() == KCardInfo::Heart)
        s += QLatin1String("heart");
    return s;
}

QPixmap doRender(const QString &element, QSvgRenderer *r, QSize s)
{
    QPixmap pix = QPixmap(s);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    r->render(&p, element);
    p.end();
    return pix;
}

QString keyForPixmap(const QString &theme, const QString &element, QSize s)
{
    return theme + QLatin1Char('_') + element + QLatin1Char('_')
                 + QString::number(s.width()) + QLatin1Char('_')
                 + QString::number(s.height());
}

QSvgRenderer *KCardCachePrivate::renderer()
{
    if (!svgRenderer)
    {
        qDebug() << "Loading front SVG renderer";
        svgRenderer = new QSvgRenderer(CardDeckInfo::svgFilePath(deckName));
    }
    return svgRenderer;
}

void KCardCachePrivate::ensureNonNullPixmap(QPixmap &pix)
{
    if (pix.isNull())
    {
        qWarning() << "Could not produce a non-null pixmap, creating a red cross";
        pix = QPixmap(size);
        QPainter p(&pix);
        p.fillRect(QRect(0, 0, pix.width(), pix.height()), QBrush(Qt::white));
        QPen pen = p.pen();
        pen.setWidth(4);
        pen.setColor(QColor(Qt::red));
        p.setPen(pen);
        p.drawLine(QPoint(2, 2), QPoint(pix.width()-2, pix.height()-2));
        p.drawLine(QPoint(pix.width()-2, 2), QPoint(2, pix.height()-2));
        p.end();
    }
}

QPixmap KCardCachePrivate::renderSvg(const QString &element)
{
    qDebug() << "Rendering" << element << "in main thread.";
    QMutexLocker l(rendererMutex);
    return doRender(element, renderer(), size);
}

void KCardCachePrivate::submitRendering(const QString &key, const QPixmap &pixmap)
{
    qDebug() << "Received render of" << key << "from rendering thread.";
    QMutexLocker l(cacheMutex);
    cache->insertPixmap(key, pixmap);
}

LoadThread::LoadThread(KCardCachePrivate *d_)
    : d(d_), doKill(false), killMutex(new QMutex)
{
}

LoadThread::~LoadThread()
{
    delete killMutex;
}

void LoadThread::setSize(const QSize &s)
{
    size = s;
}

void LoadThread::setDeckName(const QString &frontTheme_)
{
    frontTheme = frontTheme_;
}

void LoadThread::setElementsToLoad(const QStringList &elements_)
{
    elementsToRender = elements_;
}

void LoadThread::kill()
{
    QMutexLocker l(killMutex);
    doKill = true;
}

void LoadThread::run()
{
    {
        // Load the renderer even if we don't have any elements to render.
        QMutexLocker l(d->rendererMutex);
        d->renderer();
    }

    for (const QString &element : std::as_const(elementsToRender))
    {
        {
            QMutexLocker l(killMutex);
            if (doKill)
                return;
        }
        QPixmap img(size);
        img.fill(Qt::transparent);
        QPainter p(&img);
        {
            QMutexLocker l(d->rendererMutex);
            d->renderer()->render(&p, element);
        }
        p.end();

        QString key = keyForPixmap(frontTheme, element, size);
        Q_EMIT renderingDone(key, img);
    }
}

KCardCache::KCardCache()
    : d(new KCardCachePrivate)
{
    d->cache = nullptr;
    d->cacheMutex = new QMutex();
    d->rendererMutex = new QMutex();
    d->svgRenderer = nullptr;
    d->loadThread = nullptr;
}

KCardCache::~KCardCache()
{
    if (d->loadThread && d->loadThread->isRunning())
    {
        d->loadThread->kill();
    }
    delete d->loadThread;
    delete d->cache;
    delete d->cacheMutex;
    delete d->rendererMutex;
    delete d->svgRenderer;
    delete d;
}

QPixmap KCardCache::backside() const
{
    QPixmap pix;
    if (d->deckName.isEmpty() || d->size.isEmpty())
        return pix;
    QString element = QStringLiteral("back");
    QString key = keyForPixmap(d->deckName, element, d->size);

    {
        QMutexLocker l(d->cacheMutex);
        if (d->cache && (!d->cache->findPixmap(key, &pix) || pix.isNull()))
        {
            pix = d->renderSvg(element);
            d->cache->insertPixmap(key, pix);
        }
    }
    // Make sure we never return an invalid pixmap
    d->ensureNonNullPixmap(pix);
    return pix;
}

QPixmap KCardCache::frontside(KCardInfo info) const
{
    QPixmap pix;
    if (d->deckName.isEmpty() || d->size.isEmpty())
        return pix;
    QString key = keyForPixmap(d->deckName, info.svgName(), d->size);

    {
        QMutexLocker l(d->cacheMutex);
        if (d->cache && (!d->cache->findPixmap(key, &pix) || pix.isNull()))
        {
            pix = d->renderSvg(info.svgName());
            d->cache->insertPixmap(key, pix);
        }
    }
    // Make sure we never return an invalid pixmap
    d->ensureNonNullPixmap(pix);
    return pix;
}

void KCardCache::setSize(const QSize &s)
{
    if (s != d->size)
        d->size = s;
}

QSize KCardCache::size() const
{
    return d->size;
}

void KCardCache::setDeckName(const QString &theme)
{
    {
        QMutexLocker l(d->cacheMutex);
        delete d->cache;
        // The default size is arbitrary: it reflects the old KPixmapCache default
        // and it seems to match the real maximum size for the decks
        d->cache = new KImageCache(QStringLiteral("kdegames-cards_%1").arg(theme), 3*(1024<<10));
        QDateTime dt = QFileInfo(CardDeckInfo::svgFilePath(theme)).lastModified();
        if (d->cache->lastModifiedTime() < dt)
        {
            d->cache->clear();
        }
    }
    {
        QMutexLocker l(d->rendererMutex);
        delete d->svgRenderer;
        d->svgRenderer = nullptr;
    }
    d->deckName = theme;
}

QString KCardCache::deckName() const
{
    return d->deckName;
}

void KCardCache::loadTheme(LoadInfos infos)
{
    if (d->loadThread && d->loadThread->isRunning())
    {
        d->loadThread->kill();
        d->loadThread->wait();
    }
    delete d->loadThread;

    // We have to compile the list of elements to load here, because we can not
    // check the contents of the KImageCache from outside the GUI thread.
    QStringList elements;
    QPixmap pix;
    if (infos &KCardCache::LoadFrontSide)
    {
        int numCards;
        if (infos &KCardCache::Load53Cards)
            numCards = 53;
        else if (infos &KCardCache::Load52Cards)
            numCards = 52;
        else
            numCards = 32;

        for (int i = 0; i < numCards; i++)
        {
            QString element = fullDeckList[i].svgName();
            QString key = keyForPixmap(d->deckName, element, d->size);
            {
                QMutexLocker l(d->cacheMutex);
                if (d->cache && !d->cache->findPixmap(key, &pix))
                    elements << element;
            }
        }
    }

    d->loadThread = new LoadThread(d);
    d->loadThread->setDeckName(d->deckName);
    d->loadThread->setSize(d->size);
    d->loadThread->setElementsToLoad(elements);
    d->connect(d->loadThread, &LoadThread::renderingDone, d, &KCardCachePrivate::submitRendering, Qt::QueuedConnection);
    d->loadThread->start(QThread::IdlePriority);
}

void KCardCache::invalidateCache()
{
    QMutexLocker l(d->cacheMutex);
    if (d->cache)
        d->cache->clear();
}

#include "moc_cardcache_p.cpp"
