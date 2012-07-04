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

#include "carddeckinfo.h"
#include "carddeckinfo_p.h"

#include <QFileInfo>
#include <QDir>

#include <klocale.h>
#include <kstandarddirs.h>
#include <krandom.h>
#include <kdebug.h>
#include <kconfiggroup.h>
#include <kglobal.h>

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
        KGlobal::dirs()->addResourceType( "cards", "data", "carddecks/" );
        KGlobal::locale()->insertCatalog( QLatin1String(  "libkdegames" ) );
        readDecks();
    }
    ~KCardThemeInfoStatic()
    {
    }

    void readDecks()
    {
        // Empty data
        themeNameMap.clear();

        QStringList svg;
        // Add SVG card sets
        svg = KGlobal::dirs()->findAllResources( "cards", QLatin1String( "svg*/index.desktop" ), KStandardDirs::NoDuplicates );
        const QStringList list = svg + KGlobal::dirs()->findAllResources( "cards", QLatin1String( "card*/index.desktop" ), KStandardDirs::NoDuplicates );

        if ( list.isEmpty() ) return;

        for ( QStringList::ConstIterator it = list.begin(); it != list.end(); ++it )
        {
            KConfig cfg( *it, KConfig::SimpleConfig );
            KConfigGroup cfgcg( &cfg, "KDE Backdeck" );
            QString path = ( *it ).left(( *it ).lastIndexOf( QLatin1Char( '/' ) ) + 1 );
            Q_ASSERT( path[path.length() - 1] == QLatin1Char( '/' ) );
            QPixmap pixmap( path + cfgcg.readEntry( "Preview", "12c.png" ) );
            if ( pixmap.isNull() ) continue;

            QString idx  = cfgcg.readEntryUntranslated( "Name", i18n( "unnamed" ) );
            QString name = cfgcg.readEntry( "Name", i18n( "unnamed" ) );
            KCardThemeInfo info;
            info.name         = name;
            info.noi18Name    = idx;
            info.comment      = cfgcg.readEntry( "Comment", QString() );
            info.preview      = pixmap;
            info.path         = path;
            info.back         = cfgcg.readEntry( "Back", QString() );
            // if (!info.back.isNull()) kDebug() << "FOUND BACK " << info.back;
            info.isDefault    = cfgcg.readEntry( "Default", false );

            QString svg    = cfgcg.readEntry( "SVG", QString() );
            if ( !svg.isEmpty() )
            {
                QFileInfo svgInfo( QDir( path ), svg );
                info.svgfile = svgInfo.filePath();
                themeNameMap[idx] = info;
            }
        }
    }

    /** The card front sides for SVG decks.
     */
    QMap<QString, KCardThemeInfo> themeNameMap;

    /** The default front side name.
     */
    QString defaultDeck;
};

K_GLOBAL_STATIC( KCardThemeInfoStatic, deckinfoStatic )

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
    QMapIterator<QString, KCardThemeInfo> it( temp );
    while ( it.hasNext() )
    {
        it.next();
        KCardThemeInfo v = it.value();
        // Filter
        if ( v.isDefault ) return v.noi18Name;
        // Collect any deck if no default is stored
        noDefault = v.noi18Name;
    }
    if ( noDefault.isNull() ) kError() << "Could not find default card name";
    return noDefault;
}


// Retrieve a random card name
QString randomDeckName()
{
    // Collect matching items
    QStringList list = deckinfoStatic->themeNameMap.keys();
    // Draw random one
    int d = KRandom::random() % list.count();
    return list.at( d );
}


// Retrieve the SVG file belonging to the given card fronts.
QString svgFilePath( const QString& name )
{
    if ( !deckinfoStatic->themeNameMap.contains( name ) ) return QString();
    const KCardThemeInfo &v = deckinfoStatic->themeNameMap[name];
    return v.svgfile;
}

QStringList deckNames()
{
    return deckinfoStatic->themeNameMap.keys();
}

KCardThemeInfo deckInfo( const QString& name )
{
    if ( deckinfoStatic->themeNameMap.contains( name ) )
        return deckinfoStatic->themeNameMap.value( name );
    return KCardThemeInfo();
}

QString deckName( const KConfigGroup& group, const QString& defaultTheme )
{
    QString theme = group.readEntry( CONF_CARD, defaultTheme );
    if (!deckNames().contains(theme))
        return defaultTheme;
    return theme;
}

void writeDeckName( KConfigGroup& group, const QString& theme )
{
  group.writeEntry( CONF_CARD, theme );
}

}

