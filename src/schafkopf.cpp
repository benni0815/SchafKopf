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
#include "card.h"
#include "settings.h"

#include <qlabel.h>
#include <qpixmap.h>
#include <qvbox.h>

#include <kaction.h>
#include <kapplication.h>
#include <kgame/kgame.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kmenubar.h>
#include <kpopupmenu.h>
#include <kstdgameaction.h>

SchafKopf::SchafKopf()
    : KMainWindow( 0, "SchafKopf" )
{
    QVBox* w = new QVBox( this );
    QLabel* label = new QLabel( "HI", w );
    
    setCentralWidget( w );
    Card c( Card::SIEBEN, Card::SCHELLEN );
    label->setPixmap( *c.pixmap() );
    
    setupActions();
}

SchafKopf::~SchafKopf()
{
}

void SchafKopf::setupActions()
{
    KPopupMenu* mnuGame = new KPopupMenu( this );
    KPopupMenu* mnuSettings = new KPopupMenu( this );
    
    menuBar()->insertItem( i18n("&Game"), mnuGame );
    menuBar()->insertItem( i18n("&Settings"), mnuSettings );
    menuBar()->insertItem( i18n("&Help"), helpMenu() );

    KAction* newAct = KStdGameAction::gameNew( this, SLOT( newGame() ) );

    newAct->plug( mnuGame );

    KStdGameAction::quit( kapp, SLOT( quit() ) )->plug( mnuGame );
    
    KStdGameAction::carddecks( this, SLOT( carddecks() ) )->plug( mnuSettings );
    KStdAction::preferences( this, SLOT( configure() ), actionCollection() )->plug( mnuSettings );
}

void SchafKopf::carddecks()
{
    Settings::instance()->configureCardDecks( this );
}

#include "schafkopf.moc"

