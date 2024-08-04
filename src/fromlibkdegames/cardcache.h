/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2008 Andreas Pakulat <apaku@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef CARDCACHE_H
#define CARDCACHE_H

#include <QFlags>

class QPixmap;
class QString;
class QSize;
class QSizeF;

/**
 * \class KCardInfo cardcache.h <KCardCache>
 */
class KCardInfo
{
public:
    enum Suit { None, Diamond, Heart, Club, Spade };
    enum Card { Joker, Ace, King, Queen, Jack, Ten, Nine, Eight, Seven, Six, Five, Four, Three, Two };

    KCardInfo(Suit s, Card c);

    void setCard(Card c);
    Card card() const;

    void setSuit(Suit s);
    Suit suit() const;

    QString svgName() const;
    bool operator==(KCardInfo c) const;
private:
    Suit m_suit;
    Card m_card;
};

/**
 * \class KCardCache cardcache.h <KCardCache>
 *
 * This class implements a kdegames wide cache for cards.
 *
 * Card games such as lskat or kpat should use this cache
 * to load the various decks into QPixmaps instead of inventing
 * their own. It uses KImageCache behind the scenes, set up to
 * use disk and memory caching. Thus a SVG card deck that was loaded
 * by kpat for the size 100x200 does not need re-rendering when
 * requested from lskat.
 *
 * Usage is quite simple. During initialization of the game the
 * cache object should be created and it should be told to load the
 * currently selected theme.
 * <code>
 * myCache = new KCardCache();
 * myCache->loadTheme(myTheme);
 * </code>
 *
 * Later when actually drawing the cards the getter methods can be used to
 * get the pixmap of a specific card at a specific size from a given theme.
 * <code>
 * myCache->getCard(myTheme, KCardCache::Club, KCardCache::Ace, calculatedSize);
 * </code>
 *
 */
class KCardCache
{
public:
    /**
     * Can be used to load only parts of a theme, in case
     * the user chose to have front and backside from different
     * themes
     */
    enum LoadInfo
    {
        LoadFrontSide = 1 << 0 /**< Load only the front sides of the theme. */,
        LoadBackSide = 1 << 2 /**< Load only the back side of the theme. */,
        Load52Cards = 1 << 3 /**< Load a 52 card deck, ranges from Ace down to two */,
        Load32Cards = 1 << 4 /**< Load a 32 card deck, ranges from Ace down to seven */,
        Load53Cards = 1 << 5 /**< Load a 52 card deck as above, but include Jolly Joker */
    };
    Q_DECLARE_FLAGS(LoadInfos, LoadInfo)

    /**
     * Constructor creates and initializes a KImageCache
     * card games
     */
    KCardCache();

    /**
     * Cleans up the cache
     */
    ~KCardCache();

    /**
     * Set the size of rendered pixmaps.
     *
     * Make sure to set a reasonable size, before fetching pixmaps from the
     * cache.
     *
     * @param size the new size for rendering cards and backsides
     */
    void setSize(const QSize &size);

    /**
     * Returns the currently used size to render cards and backsides.
     *
     * @returns the size of pixmaps for rendering.
     */
    QSize size() const;

    /**
     * Set the theme to be used to render the frontside of cards.
     *
     * Make sure to set a proper theme before fetching frontside pixmaps from
     * the cache.
     *
     * @param theme the name of the theme to be use for rendering frontsides
     */
    void setDeckName(const QString &theme);

    /**
     * Return the currently used frontside theme
     * @returns the name of the frontside theme
     */
    QString deckName() const;

    /**
     * Retrieve the backside of the given theme @p theme at the specified
     * size @p size
     *
     * Make sure to set a reasonable size and theme, before calling this
     * function.
     *
     * @param variant which variant, like a back with another color,
     * to use for rendering. Defaults to -1 which means no variant.
     *
     * @returns a QPixmap with the card backside rendered
     *
     * @see setBackTheme
     * @see setSize
     */
    QPixmap backside() const;

    /**
     * Invalidates all cached images in the current size for the current
     * backside theme
     */
    void invalidateBackside();

    /**
     * Retrieve the frontside pixmap.
     *
     * The @p info parameter is used to determine which frontside to load.
     * Make sure to set a reasonable size and theme, before calling this
     * function.
     *
     * @param info A combination of CardInfo flags to identify what type of card
     * to load. There are of course only certain combinations that make sense,
     * like King | Heart, some flags are used standalone, like Joker.
     *
     * @returns a QPixmap with the card frontside rendered
     *
     * @see setBackTheme
     * @see setSize
     * @see CardInfo
     */
    QPixmap frontside(KCardInfo info) const;

    /**
     * Invalidates all cached images in the current size for the current
     * frontside theme
     */
    void invalidateCache();

    /**
     * Loads a whole theme in the background.
     *
     * Depending on the value of @p type only parts may be rendered.
     *
     * @param infos whether to load all entries in the theme or just the front
     * or back sides. Also its possible to specify a different deck, like a
     * 32 card deck.
     */
    void loadTheme(LoadInfos infos = LoadInfos(LoadFrontSide | LoadBackSide | Load53Cards));
private:
    class KCardCachePrivate *const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KCardCache::LoadInfos)

#endif
