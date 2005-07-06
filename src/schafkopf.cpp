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
#include "game.h"
#include "stichdlg.h"
#include "player.h"
#include "newgamewizard.h"
#include "preferencesdlg.h"
#include "selectgamewizard.h"
#include "timer.h"

// for pow()
#include <math.h>

#include <qfontmetrics.h>
#include <qheader.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qsemaphore.h>
#include <qsplitter.h>
#include <qtable.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include <kaction.h>
#include <kapplication.h>
#include <kgame/kgame.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kmenubar.h>
#include <ktoolbar.h>
#include <kpushbutton.h>
#include <kpopupmenu.h>
#include <kstdgameaction.h>

SchafKopf::SchafKopf()
        : KMainWindow( 0, "SchafKopf" )
{
    sem_init( &m_sem, 0, 0 );
    
    split = new QSplitter( QSplitter::Horizontal, this );
#if QT_VERSION >= 0x030200
    split->setChildrenCollapsible( true );
#endif // QT_VERSION

    setCentralWidget( split );
    // do not start with a too tiny window at the first start
    resize( 800, 550 );
    // save window size automatically
    setAutoSaveSettings( "SchafKopf", true );

    m_canvas = new QCanvas( this, "canvas" );
    m_canvasview = new GameCanvas( m_canvas, split, "canvasview" );

    m_game = new Game( &m_sem, this );
    m_canvasview->setHScrollBarMode(QScrollView::AlwaysOff);
    m_canvasview->setVScrollBarMode(QScrollView::AlwaysOff);
    
    QVBox * leftBox = new QVBox( split );
    new QLabel( i18n("Results:"), leftBox );

    m_table = new QTable( leftBox );
    m_table->setReadOnly( true );
    m_table->setNumCols( PLAYERS );
    m_table->setLeftMargin( 0 );
    updateTableNames();
    //QFontMetrics fm( m_table->horizontalHeader()->font() );
    for( unsigned int i = 0; i < PLAYERS; i++ )
    {
        /*int w = fm.width( m_table->horizontalHeader()->label(i) ) + 10;
        if(w<45) w=45;
        m_table->setColumnWidth( i, w );*/
        m_table->setColumnStretchable( i, true );
    }
    
    QVGroupBox* groupInfo = new QVGroupBox( i18n("Game Information:"), leftBox );
    lblCurGame = new QLabel( groupInfo );
    lblDoubled = new QLabel( groupInfo );

    btnLastTrick = new KPushButton( groupInfo );
    btnLastTrick->setFlat( true );

    split->setSizes( Settings::instance()->splitterSizes( width() ) );
    setupActions();

    connect(kapp, SIGNAL(lastWindowClosed()), this, SLOT(saveConfig()));
    connect(kapp, SIGNAL(lastWindowClosed()), this, SLOT(endGame()));
    //connect(kapp, SIGNAL(aboutToQuit()), this, SLOT(endGame()));

    connect(btnLastTrick,SIGNAL(clicked()),this,SLOT(showStich()));
    connect(Settings::instance(),SIGNAL(resultsTypeChanged()),this,SLOT(clearTable()));
    connect(Settings::instance(),SIGNAL(playerNamesChanged()),this,SLOT(updateTableNames()));
    connect( Settings::instance(), SIGNAL( cardChanged() ), this, SLOT( updateInfo() ) );
    
    QToolTip::add
        ( btnLastTrick, i18n("Show the last trick that was made.") );

    m_stichdlg = new StichDlg( this );            
    
    m_terminated = true;

    updateInfo();
}

SchafKopf::~SchafKopf()
{
    saveConfig();
    // make sure the thread is really not running
    // and does not wait for the semaphore
    endGame();
    sem_destroy( &m_sem );

    delete m_stichdlg;
}

void SchafKopf::customEvent( QCustomEvent* e )
{
    if( e->type() == SCHAFKOPF_EVENT )
    {
        int* a;
        bool force_select = false;
        
        t_EventData* data = (t_EventData*)e->data();
        switch( data->type )
        {
            case GameEnded:
                //EXIT_LOOP();
                m_canvasview->resetPlayerCards();
                // fall through!

            case RedrawPlayers:
                m_canvasview->redrawPlayers();
                enableControls();
                break;
            
            case GameStarted:
                m_canvasview->resetPlayers();
                m_canvasview->redrawPlayers();
                enableControls();
                break;
                        
            case GameInfoSetup:
                updateInfo();
                break;
                
            case PlayerIsLast:
                m_canvasview->playerIsLast( data->playerid );
                break;
                
            case PlayerDoubled:
                updateInfo();
                m_canvasview->information( data->data );
                break;
                
            case PlayerHasDoubled:
                m_canvasview->playerHasDoubled( data->playerid, true );
                break;
                
            case CardPlayed:
                m_canvasview->slotPlayerPlayedCard( data->playerid, *(data->cardids) );
                break;
            
            case PlayerMadeStich:
                m_canvasview->slotPlayerMadeStich( data->playerid );
                m_stichdlg->changed( data->data, data->cardids, data->playernames );
                break;
            
            case PlayerResults:
                this->slotPlayerResult( data->playerid, data->data );
                break;
                
            case InfoMessage:
                m_canvasview->information( data->data );
                break;
                
            case Bubble:
                m_canvasview->Bubble( data->data, data->playerid );
                break;
                
            case QuestionYesNo:
                a = new int;
                *a = m_canvasview->questionYesNo( data->data ) ? YES : NO;
                data->returncode = (void*)a;
                break;
            
            case HumanPlayerGetCard:
                a = new int;
                *a = m_canvasview->getCard();
                data->returncode = (void*)a;
                break;
            
            case ForbiddenCard:
                m_canvasview->cardForbidden( *(data->cardids) );
                break;
            
            case ForcedSelectGame:
                force_select = true;
                // fall through!
            case SelectGame:
                data->returncode = (void*)selectGame( force_select, data->cardids );
                break;
                
            case PlayerNameChanged:
                m_canvasview->setPlayerName( data->playerid, data->data );
                break;
                
            case PlayerGotCards:
                m_canvasview->setPlayerCards( data->playerid, data->cardids );
                break;
                
            default:
                break;
        }
        
        data->quitgame = m_terminated;
        
        if( data->wait )
        {
            sem_post( &m_sem );
        }
        else        
        {
            // compiler warns that deleting void is undefined....
            // data->returncode should be NULL anyways...
            // so why is it there???
            // stupid comments...
            if( data->returncode )
                delete data->returncode;
                
            if( data->cardids )
                delete [] data->cardids;

            if( data->playernames )
                delete data->playernames;
                
            delete data;
        }
    }
}

void SchafKopf::saveConfig()
{
    Settings::instance()->setSplitterSizes( split->sizes() );
}

void SchafKopf::setupActions()
{
    KAction *m_actNew, *m_actQuit;
    KPopupMenu* mnuGame = new KPopupMenu( this );
    KPopupMenu* mnuSettings = new KPopupMenu( this );

    menuBar()->insertItem( i18n("&Game"), mnuGame );
    menuBar()->insertItem( i18n("&Settings"), mnuSettings );
    menuBar()->insertItem( i18n("&Help"), helpMenu() );

    m_actNew = KStdGameAction::gameNew( this, SLOT( newGame() ) );
    m_actEnd = KStdGameAction::end( this, SLOT( endGame() ) );
    m_actStich = new KAction( i18n( "&Last Trick" ), 0, 0, this, SLOT( showStich() ), actionCollection() );

    m_actNew->plug( mnuGame );
    m_actEnd->plug( mnuGame );
    m_actStich->plug( mnuGame );

    m_actNew->plug( toolBar() );
    m_actEnd->plug( toolBar() );

    m_actQuit = KStdGameAction::quit( this, SLOT( endGame() ) );
    connect(m_actQuit, SIGNAL( activated() ), kapp, SLOT( quit() ));
    m_actQuit->plug(mnuGame);

    //connect(kapp, SIGNAL(lastWindowClosed()), this, SLOT(endGame()));

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
    newgamewizard* ng = new newgamewizard(this);

    if ( ng->exec() == QDialog::Accepted )
    {
        int game=ng->getGame();
        switch(game)
        {
        case newgamewizard::GAME_LOCAL:
            endGame();
            QTimer::singleShot(0, this, SLOT(realNewGame()));
            break;
        }

    }

    delete ng;
}

void SchafKopf::realNewGame()
{
    //connect(m_game,SIGNAL(playerResult(const QString &,const QString &)),this,SLOT(slotPlayerResult(const QString &,const QString &)));

    // entering the game loop is the last thing
    // we want to do!
    m_terminated = false;
    m_game->start();
    //m_game->gameLoop();
    //endGame();    
}

void SchafKopf::endGame()
{
    //disconnect(m_game,SIGNAL(playerResult(const QString &,const QString &)),this,SLOT(slotPlayerResult(const QString &,const QString &)));

    //m_game->endGame();
    if( !m_terminated )
    {
        m_terminated = true;
        EXIT_LOOP();
        //KApplication::postEvent( m_game, new QCustomEvent( (QEvent::Type)SCHAFKOPF_EVENT_QUIT ) );
    }
    
    clearTable();
    updateInfo();
}

void SchafKopf::showStich()
{
    if( m_terminated )
        return;

    m_stichdlg->show();
}

void SchafKopf::enableControls()
{
    m_actEnd->setEnabled( !m_game->isTerminated() );
    m_actStich->setEnabled( !m_game->isTerminated()  );
    btnLastTrick->setEnabled( !m_game->isTerminated() );
}

void SchafKopf::slotPlayerResult( unsigned int col, const QString & result )
{
    if( !m_table->numRows() || !m_table->text( m_table->numRows()-1, col ).isEmpty() )
        m_table->insertRows( m_table->numRows() );

    m_table->setText( m_table->numRows()-1, col, result );
    m_table->ensureCellVisible( m_table->numRows()-1, col );
}

void SchafKopf::updateInfo()
{
    int timesDoubled = 0, timesThrownTogether = 0;
    int valuation;
    
    btnLastTrick->setPixmap( *(Card::backgroundPixmap()) );
        
    if( !m_terminated && m_game->gameInfo()->isValid() )
        lblCurGame->setText( i18n("<qt>Current Game:<br><b>") + m_game->gameInfo()->toString() + "</b></qt>" );
    else
    {
        lblCurGame->setText( QString::null );
        lblDoubled->setText( QString::null );
        return;
    }


    QString sDoubled;
    for(unsigned int i=0;i<PLAYERS;i++)
    {
        Player* player = m_game->findIndex( i );
        if( player->geklopft() )
        {
            sDoubled.append( i18n("<qt><b>%1</b> has doubled.</qt>").arg( player->name() ) );
            timesDoubled++;
        }
    }
    timesThrownTogether = m_game->timesThrownTogether();
    if(timesThrownTogether>0)
        sDoubled.append( i18n("<qt>Times thrown together: <b>%1</b></qt>").arg(timesThrownTogether) );
    valuation=(int)pow(2, timesDoubled);
    if( Settings::instance()->doubleNextGame() )
        valuation = valuation * (int)pow(2, timesThrownTogether );
    sDoubled.append( i18n("<qt>Game counts <b>%1-fold</b>.</qt>").arg(valuation) );
    
    lblDoubled->setText( sDoubled );
}

void SchafKopf::clearTable()
{
    QMemArray<int> cols(m_table->numRows());
    int i;
    
    for(i=0;i<m_table->numRows();i++)
        cols[i]=i;
    m_table->removeRows(cols);
}

void SchafKopf::configure()
{
    PreferencesDlg prefs( this, "prefs");
    if( prefs.exec() == QDialog::Accepted )
    {
        m_canvasview->updateBackground();
    }
}

void SchafKopf::updateTableNames()
{
    m_table->setColumnLabels( Settings::instance()->playerNames() );
}

GameInfo* SchafKopf::selectGame( bool force, int* cardids )
{
    CardList list( cardids );
    list.setAutoDelete( true );
    
    SelectGameWizard sgw( force, &list );    
    if( sgw.exec() == QDialog::Accepted )
        return sgw.gameInfo();
    else
        return NULL;
}

#include "schafkopf.moc"
