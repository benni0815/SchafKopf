/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2008 Andreas Pakulat <apaku@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef CARDDECKINFO_P_H
#define CARDDECKINFO_P_H

#include <QPixmap>
#include <QString>

/**
 * Stores the information for one card front or back side.
 */
class KCardThemeInfo
{
public:
    /** The translated name. */
    QString name;

    /** The untranslated name. */
    QString noi18Name;

    /** The comment (author and description). */
    QString comment;

    /** The full path information. */
    QString path;

    /** The translated name of the back side. */
    QString back;

    /** The preview image. */
    QPixmap preview;

    /** The full filename of the SVG file. */
    QString svgfile;

    /** Is this a default deck or set. */
    bool isDefault;
};

QDebug operator<<(QDebug debug, const KCardThemeInfo &cn);

namespace CardDeckInfo
{
KCardThemeInfo deckInfo(const QString &name);
KCardThemeInfo backInfo(const QString &name);
}

#endif
