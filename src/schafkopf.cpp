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
#include "settings.h"
#include "gamecanvas.h"
// TODO: either move the game object into
// GameCanvas as member variable or
// maybe make GameCanvas a subclasse of Game
#include "game.h"
#include "stichdlg.h"

#include <qlabel.h>
#include <qpixmap.h>
#include <qvbox.h>
#include <qtimer.h>

#include <kaction.h>
#include <kapplication.h>
#include <kgame/kgame.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kmenubar.h>
#include <ktoolbar.h>
#include <kpopupmenu.h>
#include <kstdgameaction.h>

SchafKopf::SchafKopf()
    : KMainWindow( 0, "SchafKopf" )
{
    QVBox* w = new QVBox( this );
    setCentralWidget( w );
    // save window size automatically
    setAutoSaveSettings( "SchafKopf", true );
    
    m_game = NULL;
    
    m_canvas = new QCanvas( this, "canvas" );
    m_canvasview = new GameCanvas( m_canvas, w, "canvasview" );
        
    setupActions();
}

SchafKopf::~SchafKopf()
{
    if( m_game )
        endGame();
        
    if( m_stichdlg )
        delete m_stichdlg;
}

void SchafKopf::setupActions()
{
    KAction* m_actNew;
	KPopupMenu* mnuGame = new KPopupMenu( this );
    KPopupMenu* mnuSettings = new KPopupMenu( this );
    
    menuBar()->insertItem( i18n("&Game"), mnuGame );
    menuBar()->insertItem( i18n("&Settings"), mnuSettings );
    menuBar()->insertItem( i18n("&Help"), helpMenu() );

    m_actNew = KStdGameAction::gameNew( this, SLOT( newGame() ) );
    m_actEnd = KStdGameAction::end( this, SLOT( endGame() ) );
    // TODO: DOM: translate me correctly!!!    
    m_actStich = new KAction( i18n( "&Last Stich" ), 0, 0, this, SLOT( showStich() ), actionCollection() );
    
    m_actNew->plug( mnuGame );
    m_actEnd->plug( mnuGame );
    m_actStich->plug( mnuGame );
    
    m_actNew->plug( toolBar() );
    m_actEnd->plug( toolBar() );

    KStdGameAction::quit( kapp, SLOT( quit() ) )->plug( mnuGame );
    
    KStdGameAction::carddecks( this, SLOT( carddecks() ) )->plug( mnuSettings );
    KStdAction::preferences( this, SLOT( configure() ), actionCollection() )->plug( mnuSettings );
    
    enableControls();
}

void SchafKopf::carddecks()
{
    Settings::instance()->configureCardDecks( this );
}

void SchafKopf::newGame()
{
    endGame();
	QTimer::singleShot(1, this, SLOT(realNewGame()));
}

void SchafKopf::realNewGame()
{
	m_game = new Game();
    m_game->setCanvas( m_canvasview );
    m_canvasview->setGame( m_game );    
    enableControls();
   
    // entering the game loop is the last thing
    // we want to do!
    m_game->gameLoop();
	endGame();
}

void SchafKopf::endGame()
{
	m_canvasview->setGame( NULL );
	if(m_game)
		m_game->endGame();
	delete m_game;
    m_game = NULL;
    
    enableControls();
}

void SchafKopf::showStich()
{
    if( !m_game )
        return;
        
    if( !m_stichdlg )
        m_stichdlg = new StichDlg( m_game, 0 );

    m_stichdlg->show();
}

void SchafKopf::enableControls()
{
    m_actEnd->setEnabled( m_game );
    m_actStich->setEnabled( m_game );
}

#include "schafkopf.moc"

