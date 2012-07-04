/*
    This file is part of the KDE games library

    Copyright 2008 Andreas Pakulat <apaku@gmx.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef __CARDCACHE_P_H_
#define __CARDCACHE_P_H_

#include <QImage>
#include <QThread>
#include <QString>
#include <QStringList>
#include <QSize>
#include "cardcache.h"

class KPixmapCache;
class QMutex;
class QSvgRenderer;
class LoadThread;

class KCardCachePrivate : public QObject
{
    Q_OBJECT
public:
    KPixmapCache* cache;
    QMutex* cacheMutex;
    QMutex* rendererMutex;
    LoadThread* loadThread;
    QSize size;
    QString deckName;
    QSvgRenderer* svgRenderer;

    QSvgRenderer* renderer();
    QPixmap renderSvg( const QString& element );
    void ensureNonNullPixmap( QPixmap& pix );
public slots:
    void submitRendering( const QString& key, const QImage& image );
};

class LoadThread : public QThread
{
    Q_OBJECT
signals:
    void renderingDone( const QString& key, const QImage& image );
public:
    LoadThread( KCardCachePrivate* d );
    void setSize( const QSize& s );
    void setDeckName( const QString& frontTheme );
    void setElementsToLoad( const QStringList& elements );
    void run();
    void kill();
private:
    KCardCachePrivate* d;
    QString frontTheme;
    QSize size;
    bool doKill;
    QMutex* killMutex;
    QStringList elementsToRender;
};

#endif
