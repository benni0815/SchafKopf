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

#include "schafkopfevent.h"
#include "settings.h"
#include "gamecanvas.h"
#include "game.h"
#include "stichdlg.h"
#include "player.h"
#include "newgamewizard.h"
#include "preferencesdlg.h"
#include "selectgamewizard.h"

#include <QLabel>
#include <QApplication>
#include <QMenuBar>
#include <QSplitter>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QTimer>

#include <KActionCollection>
#include <KHelpMenu>
#include <KToolBar>
#include <KStandardGameAction>

// for pow()
#include <cmath>


namespace
{
  static const char version[] = "0.7.91";
}

SchafKopf::SchafKopf(QWidget *parent) : KXmlGuiWindow(parent)
{
    sem_init( &m_sem, 0, 0 );
    
    split = new QSplitter( Qt::Horizontal, this );
    split->setChildrenCollapsible( true );

    setCentralWidget( split );
    // do not start with a too tiny window at the first start
    resize( 800, 550 );
    // save window size automatically
    setAutoSaveSettings( "SchafKopf", true );

    m_scene = new QGraphicsScene( this );
    m_view = new GameCanvas( m_scene, split );

    m_game = new Game( &m_sem, this );
    m_view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    
    QLabel* label = new QLabel( tr("Results:") );
    label->setAlignment( Qt::AlignHCenter );

    m_table = new QTableWidget;
    m_table->setColumnCount( PLAYERS );
    //m_table->setLeftMargin( 0 );
    updateTableNames();
    m_table->horizontalHeader()->setResizeMode( QHeaderView::Stretch );

    lblCurGame = new QLabel;
    lblDoubled = new QLabel;
    lblCurGame->setWordWrap( true );
    lblDoubled->setWordWrap( true );

    btnLastTrick = new QPushButton;
    btnLastTrick->setFlat( true );
    split->setSizes( Settings::instance()->splitterSizes( width() ) );
    setupActions();

    QGroupBox* groupInfo = new QGroupBox( tr("Game Information:") );
    QVBoxLayout *groupInfo_layout = new QVBoxLayout( groupInfo );
    groupInfo_layout->addWidget( lblCurGame );
    groupInfo_layout->addWidget( lblDoubled );
    groupInfo_layout->addWidget( btnLastTrick );

    QWidget* leftBox = new QWidget( split );
    QVBoxLayout* leftBox_layout = new QVBoxLayout( leftBox );
    leftBox_layout->addWidget( label );
    leftBox_layout->addWidget( m_table );
    leftBox_layout->addWidget( groupInfo );

    const auto app = QApplication::instance();
    connect(app, SIGNAL(lastWindowClosed()), this, SLOT(saveConfig()));
    connect(app, SIGNAL(lastWindowClosed()), this, SLOT(endGame()));
    //connect(kapp, SIGNAL(aboutToQuit()), this, SLOT(endGame()));

    connect(btnLastTrick,SIGNAL(clicked()),this,SLOT(showStich()));
    connect(Settings::instance(),SIGNAL(resultsTypeChanged()),this,SLOT(clearTable()));
    connect(Settings::instance(),SIGNAL(playerNamesChanged()),this,SLOT(updateTableNames()));
    connect( Settings::instance(), SIGNAL( cardChanged() ), this, SLOT( updateInfo() ) );

    btnLastTrick->setToolTip( tr( "Show the last trick that was made." ) );

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

KAboutData SchafKopf::aboutData()
{
  // if you want copyright, then add you to the list:
  KAboutData about("schafkopf", "SchafKopf", version);
  about.setLicense(KAboutLicense::GPL);
  about.setShortDescription(tr("\"Schafkopf\" is a popular card game in Bavaria. SchafKopf ist the KDE version of this game."));
  about.setHomepage("https://github.com/benni0815/SchafKopf/");
  about.setCopyrightStatement(tr("(C) 2004-2019 Dominik Seichter (and others)"));
  about.addAuthor( tr("Christian Kern"), "", "kernch@in.tum.de" );
  about.addAuthor( tr("Lorenz Moesenlechner"), "", "moesenle@in.tum.de" );
  about.addAuthor( tr("Benjamin Löwe"), "", "benjamin.loewe@freenet.de" );
  about.addAuthor( tr("Dominik Seichter"), "", "domseichter@web.de", "http://www.krename.net" );

  return about;
}

void SchafKopf::customEvent( QEvent* e )
{
    if( e->type() == QEvent::Type( QEvent::User + SCHAFKOPF_EVENT ) )
    {
        int* a;
        bool force_select = false;

        t_EventData* data = ((SchafKopfEvent*) e)->data();
        switch( data->type )
        {
            case GameEnded:
                //EXIT_LOOP();
                m_view->resetPlayerCards();
                // fall through!

            case RedrawPlayers:
                m_view->redrawPlayers();
                enableControls();
                break;
            
            case GameStarted:
                m_view->resetPlayers();
                m_view->redrawPlayers();
                enableControls();
                break;
                        
            case GameInfoSetup:
                updateInfo();
                break;
                
            case PlayerIsLast:
                m_view->playerIsLast( data->playerid );
                break;
                
            case PlayerDoubled:
                updateInfo();
                m_view->information( data->data );
                break;
                
            case PlayerHasDoubled:
                m_view->playerHasDoubled( data->playerid, true );
                break;
                
            case CardPlayed:
                m_view->slotPlayerPlayedCard( data->playerid, *(data->cardids) );
                break;
            
            case PlayerMadeStich:
                m_view->slotPlayerMadeStich( data->playerid );
                m_stichdlg->changed( data->data, data->cardids, data->playernames );
                break;
            
            case PlayerResults:
                this->slotPlayerResult( data->playerid, data->data );
                break;
                
            case InfoMessage:
                m_view->information( data->data );
                break;
                
            case Bubble:
                m_view->Bubble( data->data, data->playerid );
                break;
                
            case QuestionYesNo:
                a = new int;
                *a = m_view->questionYesNo( data->data ) ? YES : NO;
                data->returncode = (void*)a;
                break;
            
            case HumanPlayerGetCard:
                a = new int;
                *a = m_view->getCard();
                data->returncode = (void*)a;
                break;
            
            case ForbiddenCard:
                m_view->cardForbidden( *(data->cardids) );
                break;
            
            case ForcedSelectGame:
                force_select = true;
                // fall through!
            case SelectGame:
                data->returncode = (void*)selectGame( force_select, data->cardids );
                break;
                
            case PlayerNameChanged:
                m_view->setPlayerName( data->playerid, data->data );
                break;
                
            case PlayerGotCards:
                m_view->setPlayerCards( data->playerid, data->cardids );
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
            // Benni: tried to fix this warning by casting the pointer back to what it was
            if( data->returncode )
            {
                switch( data->type )
                {
                case QuestionYesNo:
                case HumanPlayerGetCard:
                    delete (int*)data->returncode;
                case SelectGame:
                    delete (GameInfo*)data->returncode;
                default:;
                }
            }

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
    QAction *m_actNew, *m_actQuit;
    KMenu* mnuGame = new KMenu( this );
    KMenu* mnuSettings = new KMenu( this );

    mnuGame->setTitle( tr("&Game") );
    mnuSettings->setTitle( tr("&Settings") );

    menuBar()->addMenu( mnuGame );
    menuBar()->addMenu( mnuSettings );
    menuBar()->addMenu( helpMenu() );

    m_actNew = KStandardGameAction::gameNew( this, SLOT( newGame() ), this );
    m_actEnd = KStandardGameAction::end( this, SLOT( endGame() ), this );

    m_actStich = new QAction( this );
    m_actStich->setText( tr( "&Last Trick" ) );
    actionCollection()->addAction( "Last Trick", m_actStich );
    connect( m_actStich, SIGNAL( triggered( bool ) ), this, SLOT( showStich() ) );


    m_actQuit = KStandardAction::quit( this, SLOT( endGame() ), actionCollection() );
    const auto app = QApplication::instance();
    connect( m_actQuit, SIGNAL( triggered() ), app, SLOT( quit() ));
    connect( app, SIGNAL( lastWindowClosed() ), this, SLOT( endGame() ) );

    mnuGame->addAction( m_actStich);
    mnuGame->addAction( m_actNew );
    mnuGame->addAction( m_actEnd );
    mnuGame->addAction( m_actQuit );

    mnuSettings->addAction( KStandardGameAction::carddecks( this, SLOT( carddecks() ), this ) );
    mnuSettings->addAction( KStandardAction::preferences( this, SLOT( configure() ), this ) );

    toolBar()->addAction( m_actNew );
    toolBar()->addAction( m_actEnd );

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
        m_view->exitLoop();
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
    int lastRow = m_table->rowCount() - 1;
    if( lastRow == -1 || ( m_table->item( lastRow, 0 ) && m_table->item( lastRow, 1 ) && m_table->item( lastRow, 2 ) && m_table->item( lastRow, 3 ) ) )
        m_table->insertRow( m_table->rowCount() );

    QTableWidgetItem* item = new QTableWidgetItem();
    item->setText( result );
    item->setFlags( Qt::NoItemFlags | Qt::ItemIsEnabled );
    m_table->setItem( m_table->rowCount() - 1, col, item );
    m_table->scrollToItem( item );
}

void SchafKopf::updateInfo()
{
    int timesDoubled = 0, timesThrownTogether = 0;
    int valuation;
    
    btnLastTrick->setIconSize( Card::backgroundPixmap()->size() );
    btnLastTrick->setIcon( QIcon( *(Card::backgroundPixmap()) ) );

    if( !m_terminated && m_game->gameInfo()->isValid() )
        lblCurGame->setText( tr("<qt>Current Game:<br><b>") + m_game->gameInfo()->toString() + "</b></qt>" );
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
            sDoubled.append( tr("<qt><b>%1</b> has doubled.</qt>").arg( player->name() ) );
            timesDoubled++;
        }
    }
    timesThrownTogether = m_game->timesThrownTogether();
    if(timesThrownTogether>0)
        sDoubled.append( tr("<qt>Times thrown together: <b>%1</b></qt>").arg(timesThrownTogether) );
    valuation=(int)pow(2, timesDoubled);
    if( Settings::instance()->doubleNextGame() )
        valuation = valuation * (int)pow(2, timesThrownTogether );
    sDoubled.append( tr("<qt><br>Game counts <b>%1-fold</b>.</qt>").arg(valuation) );
    
    lblDoubled->setText( sDoubled );
}

void SchafKopf::clearTable()
{
    for( int i = 0; i < m_table->rowCount(); i++ )
        m_table->removeRow( i );
}

void SchafKopf::configure()
{
    PreferencesDlg prefs( this );
    if( prefs.exec() == QDialog::Accepted )
    {
        m_view->updateBackground();
    }
}

void SchafKopf::updateTableNames()
{
    m_table->setHorizontalHeaderLabels( Settings::instance()->playerNames() );
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
