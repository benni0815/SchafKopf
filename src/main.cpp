/***************************************************************************
 *   Copyright (C) 2004 by Dominik Seichter                                *
 *   domseichter@web.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "schafkopf.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char version[] = "0.8-devel";

int main(int argc, char **argv)
{
    // if you want copyright, then add you to the list:
    KLocalizedString description = ki18n("\"Schafkopf\" is a popular card game in Bavaria. SchafKopf ist the KDE version of this game.");
    KAboutData about("schafkopf", "SchafKopf", ki18n("SchafKopf"), version, description,
                     KAboutData::License_GPL, ki18n("(C) 2004-2005 Dominik Seichter (and others)"), ki18n(""), "http://schafkopf.berlios.de", "domseichter@web.de");
    about.addAuthor( ki18n("Christian Kern"), ki18n(""), "kernch@in.tum.de" );
    about.addAuthor( ki18n("Lorenz Moesenlechner"), ki18n(""), "moesenle@in.tum.de" );
    about.addAuthor( ki18n("Benjamin Loewe"), ki18n(""), "benjamin.loewe@freenet.de" );
    about.addAuthor( ki18n("Dominik Seichter"), ki18n(""), "domseichter@web.de", "http://www.krename.net" );

    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;

    // get KDE games translations
    KGlobal::locale()->insertCatalog("libkdegames");

    SchafKopf* mainWin = new SchafKopf();
    mainWin->show();

    return app.exec();
}
