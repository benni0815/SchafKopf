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

#include <schafkopf_version.h>

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
#include <KLocalizedString>
#include <KToolBar>

// for pow()
#include <cmath>


SchafKopf::SchafKopf(QWidget *parent) : KXmlGuiWindow(parent)
{
    KAboutData::setApplicationData(aboutData());

    split = new QSplitter( Qt::Horizontal, this );
    split->setChildrenCollapsible( true );

    setCentralWidget( split );
    // do not start with a too tiny window at the first start
    resize( 800, 550 );
    // save window size automatically
    setAutoSaveSettings( "SchafKopf", true );

    m_scene = new QGraphicsScene( this );
    m_view = new GameCanvas( m_scene, split );

    m_game = new Game( &m_semaphore, this );
    m_view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    m_view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    
    QLabel* label = new QLabel(i18n("Results:"));
    label->setAlignment( Qt::AlignHCenter );

    m_table = new QTableWidget;
    m_table->setColumnCount( PLAYERS );
    //m_table->setLeftMargin( 0 );
    updateTableNames();
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    lblCurGame = new QLabel;
    lblDoubled = new QLabel;
    lblCurGame->setWordWrap( true );
    lblDoubled->setWordWrap( true );

    btnLastTrick = new QPushButton;
    btnLastTrick->setFlat( true );
    split->setSizes( Settings::instance()->splitterSizes( width() ) );
    setupActions();

    QGroupBox* groupInfo = new QGroupBox(i18n("Game Information:"));
    QVBoxLayout *groupInfo_layout = new QVBoxLayout( groupInfo );
    groupInfo_layout->addWidget( lblCurGame );
    groupInfo_layout->addWidget( lblDoubled );
    groupInfo_layout->addWidget( btnLastTrick );

    QWidget* leftBox = new QWidget( split );
    QVBoxLayout* leftBox_layout = new QVBoxLayout( leftBox );
    leftBox_layout->addWidget( label );
    leftBox_layout->addWidget( m_table );
    leftBox_layout->addWidget( groupInfo );

    const auto app = dynamic_cast<QGuiApplication*>(QApplication::instance());
    connect(app, &QGuiApplication::lastWindowClosed, this, &SchafKopf::saveConfig);
    connect(app, &QGuiApplication::lastWindowClosed, this, &SchafKopf::endGame);
    //connect(kapp, SIGNAL(aboutToQuit()), this, SLOT(endGame()));

    connect(btnLastTrick, &QPushButton::clicked, this, &SchafKopf::showStich);
    connect(Settings::instance(), &Settings::resultsTypeChanged, this, &SchafKopf::clearTable);
    connect(Settings::instance(), &Settings::playerNamesChanged, this, &SchafKopf::updateTableNames);
    connect(Settings::instance(), &Settings::cardChanged, this, &SchafKopf::updateInfo);

    btnLastTrick->setToolTip(i18n("Show the last trick that was made."));

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

    delete m_stichdlg;
}

KAboutData SchafKopf::aboutData()
{
  // if you want copyright, then add you to the list:
  KAboutData about("schafkopf", "SchafKopf", QStringLiteral(SCHAFKOPF_VERSION_STRING));
  about.setLicense(KAboutLicense::GPL);
  about.setShortDescription(i18n("\"Schafkopf\" is a popular card game in Bavaria. SchafKopf ist the KDE version of this game."));
  about.setHomepage("https://github.com/benni0815/SchafKopf/");
  about.setCopyrightStatement(i18n("(C) 2004-2019 Dominik Seichter (and others)"));
  about.addAuthor(i18n("Christian Kern"), "", "kernch@in.tum.de");
  about.addAuthor(i18n("Lorenz Moesenlechner"), "", "moesenle@in.tum.de");
  about.addAuthor(i18n("Benjamin Löwe"), "", "benjamin.loewe@freenet.de");
  about.addAuthor(i18n("Dominik Seichter"), "", "domseichter@web.de", "http://www.krename.net");
  about.setDesktopFileName("org.schafkopf.SchafKopf");

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
            m_semaphore.release();
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
                    delete static_cast<int*>(data->returncode);
                    break;

                case SelectGame:
                    delete static_cast<GameInfo*>(data->returncode);
                    break;

                default:
                    break;
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
    auto mnuGame = new QMenu( this );
    auto mnuSettings = new QMenu( this );

    mnuGame->setTitle(i18n("&Game"));
    mnuSettings->setTitle(i18n("&Settings"));

    menuBar()->addMenu( mnuGame );
    menuBar()->addMenu( mnuSettings );
    auto help = new KHelpMenu(this, aboutData());
    menuBar()->addMenu( help->menu() );

    {
      m_actStich = new QAction(this);
      m_actStich->setText(i18n("&Last Trick"));
      actionCollection()->addAction("Last Trick", m_actStich);
      connect(m_actStich, &QAction::triggered, this, &SchafKopf::showStich);
      mnuGame->addAction( m_actStich);
    }

    {
      auto actNew =KStandardAction::openNew(this, &SchafKopf::newGame, this);
      mnuGame->addAction(actNew);
      toolBar()->addAction(actNew);
    }

    {
      m_actEnd = new QAction(QIcon::fromTheme("window-close"), i18n("End Game"), this);
      connect(m_actEnd, &QAction::triggered, this, &SchafKopf::endGame);
      mnuGame->addAction(m_actEnd);
      toolBar()->addAction(m_actEnd);
    }

    {
      auto actQuit = KStandardAction::quit(this, &SchafKopf::endGame, actionCollection());
      const auto app = dynamic_cast<QGuiApplication*>(QApplication::instance());
      connect(actQuit, &QAction::triggered, app, &QApplication::quit);
      connect(app, &QGuiApplication::lastWindowClosed, this, &SchafKopf::endGame);
      mnuGame->addAction( actQuit );
    }

    {
      auto actCarddecks = new QAction(i18n("Configure Carddecks"), this);
      connect(actCarddecks, &QAction::triggered, this, &SchafKopf::carddecks);
      mnuSettings->addAction(actCarddecks);
    }

    mnuSettings->addAction(KStandardAction::preferences(this, &SchafKopf::configure, this));

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
            QTimer::singleShot(0, this, &SchafKopf::realNewGame);
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
        lblCurGame->setText(i18n("<qt>Current Game:<br><b>") + m_game->gameInfo()->toString() + "</b></qt>");
    else
    {
        lblCurGame->setText( QString() );
        lblDoubled->setText( QString() );
        return;
    }


    QString sDoubled;
    for(unsigned int i=0;i<PLAYERS;i++)
    {
        Player* player = m_game->findIndex( i );
        if( player->geklopft() )
        {
            sDoubled.append(i18n("<qt><b>%1</b> has doubled.</qt>", player->name()));
            timesDoubled++;
        }
    }
    timesThrownTogether = m_game->timesThrownTogether();
    if(timesThrownTogether>0)
        sDoubled.append(i18n("<qt>Times thrown together: <b>%1</b></qt>", timesThrownTogether));
    valuation=(int)pow(2, timesDoubled);
    if( Settings::instance()->doubleNextGame() )
        valuation = valuation * (int)pow(2, timesThrownTogether );
    sDoubled.append(i18n("<qt><br>Game counts <b>%1-fold</b>.</qt>", valuation));
    
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
