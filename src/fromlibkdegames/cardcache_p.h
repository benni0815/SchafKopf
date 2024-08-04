/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2008 Andreas Pakulat <apaku@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef CARDCACHE_P_H
#define CARDCACHE_P_H

#include <KImageCache>

#include <QThread>
#include <QString>
#include <QStringList>
#include <QSize>
#include "cardcache.h"

class QMutex;
class QSvgRenderer;
class LoadThread;

class KCardCachePrivate : public QObject
{
    Q_OBJECT
public:
    KImageCache *cache;
    QMutex *cacheMutex;
    QMutex *rendererMutex;
    LoadThread *loadThread;
    QSize size;
    QString deckName;
    QSvgRenderer *svgRenderer;

    QSvgRenderer *renderer();
    QPixmap renderSvg(const QString &element);
    void ensureNonNullPixmap(QPixmap &pix);
public Q_SLOTS:
    void submitRendering(const QString &key, const QPixmap &pixmap);
};

class LoadThread : public QThread
{
    Q_OBJECT
Q_SIGNALS:
    void renderingDone(const QString &key, const QPixmap &pixmap);
public:
    explicit LoadThread(KCardCachePrivate *d);
    ~LoadThread() override;
    void setSize(const QSize &s);
    void setDeckName(const QString &frontTheme);
    void setElementsToLoad(const QStringList &elements);
    void run() override;
    void kill();
private:
    KCardCachePrivate *d;
    QString frontTheme;
    QSize size;
    bool doKill;
    QMutex *killMutex;
    QStringList elementsToRender;
};

#endif
