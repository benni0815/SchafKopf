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
#include <kglobal.h>
#include <klocale.h>

static const char description[] =
    I18N_NOOP("<qt>Schafkopf is a popular card game in Bavaria. "
              "SchafKopf ist the KDE version of this game.</qt>");

static const char version[] = "0.6";

static KCmdLineOptions options[] =
{
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    // if you want copyright, then add you to the list:
    KAboutData about("schafkopf", I18N_NOOP("SchafKopf"), version, description,
                     KAboutData::License_GPL, "(C) 2004-2005 Dominik Seichter (and others)", 0, 0, "domseichter@web.de");
    about.addAuthor( "Christian Kern", 0, "kernch@in.tum.de" );	
    about.addAuthor( "Lorenz Moesenlechner", 0, "moesenle@in.tum.de" );
    about.addAuthor( "Benjamin Loewe", 0, "benjamin.loewe@freenet.de" );
    about.addAuthor( "Dominik Seichter", 0, "domseichter@web.de", "http://www.krename.net" );

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;
    
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

    // get KDE games translations
    KGlobal::locale()->insertCatalogue("libkdegames");

    SchafKopf* mainWin = new SchafKopf();
    app.setMainWidget( mainWin );
    mainWin->show();

    // free some memory
    args->clear(); 
    
    return app.exec();
}

