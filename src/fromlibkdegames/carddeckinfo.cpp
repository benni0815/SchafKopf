/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2008 Andreas Pakulat <apaku@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "carddeckinfo.h"
#include "carddeckinfo_p.h"

#include <QDir>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QStandardPaths>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>

#include <utility>

// KConfig entries
#define CONF_CARD QString::fromLatin1("Cardname")

/**
 * Local static information.
 */
class KCardThemeInfoStatic
{
public:
    KCardThemeInfoStatic()
    {
        readDecks();
    }
    ~KCardThemeInfoStatic()
    {
    }

    void readDecks()
    {
        // Empty data
        themeNameMap.clear();

        // Add SVG card sets
        QStringList list;
        const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("carddecks"), QStandardPaths::LocateDirectory);
        for (const QString& dir : dirs) {
            const QStringList deckFolderNames = QDir(dir).entryList({QStringLiteral("svg*")});
            list.reserve(list.size() + deckFolderNames.size());
            for (const QString& deck : deckFolderNames) {
                list.append(dir + QLatin1Char('/') + deck + QStringLiteral("/index.desktop"));
            }
        }

        for (const QString &deckPath : std::as_const(list)) {
            KConfig cfg(deckPath, KConfig::SimpleConfig);
            KConfigGroup cfgcg(&cfg, QStringLiteral("KDE Backdeck"));
            QString path = deckPath.left(deckPath.lastIndexOf(QLatin1Char('/')) + 1);
            Q_ASSERT(path[path.length() - 1] == QLatin1Char('/'));
            QPixmap pixmap(path + cfgcg.readEntry("Preview", "12c.png"));
            if (pixmap.isNull()) continue;

            QString idx  = cfgcg.readEntryUntranslated("Name", i18n("unnamed"));
            QString name = cfgcg.readEntry("Name", i18n("unnamed"));
            KCardThemeInfo info;
            info.name         = name;
            info.noi18Name    = idx;
            info.comment      = cfgcg.readEntry("Comment", QString());
            info.preview      = pixmap;
            info.path         = path;

            info.back         = cfgcg.readEntry("Back", QString());
            // if (!info.back.isNull()) qCDebug(LSKAT_LOG) << "FOUND BACK " << info.back;
            info.isDefault    = cfgcg.readEntry("Default", false);

            QString svg    = cfgcg.readEntry("SVG", QString());
            if (!svg.isEmpty())
            {
                QFileInfo svgInfo(QDir(path), svg);
                info.svgfile = svgInfo.filePath();
                themeNameMap[idx] = info;
            }
        }
    }

    /** The card front sides for SVG decks. */
    QMap<QString, KCardThemeInfo> themeNameMap;

    /** The default front side name. */
    QString defaultDeck;
};

Q_GLOBAL_STATIC(KCardThemeInfoStatic, deckinfoStatic)

QDebug operator<<(QDebug debug, const KCardThemeInfo &cn)
{
    return debug << "name: " << cn.name
                 << " noi18Name: " << cn.noi18Name
                 << " comment: " << cn.comment
                 << " path: " << cn.path
                 << " back: " << cn.back
                 << " preview: " << cn.preview
                 << " svgfile: " << cn.svgfile
                 << " isDefault: " << cn.isDefault;
}

namespace CardDeckInfo
{
// Retrieve default card set name
QString defaultDeckName()
{
    QString noDefault;
    // Count filtered cards
    QMap<QString, KCardThemeInfo> temp = deckinfoStatic->themeNameMap;
    QMapIterator<QString, KCardThemeInfo> it(temp);
    while (it.hasNext())
    {
        it.next();
        KCardThemeInfo v = it.value();
        // Filter
        if (v.isDefault) return v.noi18Name;
        // Collect any deck if no default is stored
        noDefault = v.noi18Name;
    }
    if (noDefault.isNull()) qCritical() << "Could not find default card name";
    return noDefault;
}

// Retrieve a random card name
QString randomDeckName()
{
    // Collect matching items
    QStringList list = deckinfoStatic->themeNameMap.keys();
    // Draw random one
    int d = QRandomGenerator::system()->bounded(list.count());
    return list.at(d);
}

// Retrieve the SVG file belonging to the given card fronts.
QString svgFilePath(const QString &name)
{
    if (!deckinfoStatic->themeNameMap.contains(name)) return QString();
    const KCardThemeInfo &v = deckinfoStatic->themeNameMap[name];
    return v.svgfile;
}

QStringList deckNames()
{
    return deckinfoStatic->themeNameMap.keys();
}

KCardThemeInfo deckInfo(const QString &name)
{
    if (deckinfoStatic->themeNameMap.contains(name))
        return deckinfoStatic->themeNameMap.value(name);
    return KCardThemeInfo();
}

QString deckName(const KConfigGroup &group, const QString &defaultTheme)
{
    QString theme = group.readEntry(CONF_CARD, defaultTheme);
    if (!deckNames().contains(theme))
        return defaultTheme;
    return theme;
}

void writeDeckName(KConfigGroup &group, const QString &theme)
{
    group.writeEntry(CONF_CARD, theme);
}
}
