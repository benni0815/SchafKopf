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

#include "game.h"

#include "schafkopfevent.h"
#include "humanplayer.h"
#include "computerplayer.h"
#include "cardlist.h"
#include "gameinfo.h"
#include "results.h"
#include "settings.h"

#include <KLocalizedString>

#include <QCoreApplication>
#include <QEvent>
#include <QSemaphore>

#include <string.h>


unsigned int Game::def_id=0;

Game::Game(QSemaphore* semaphore, QObject *parent )
    : QThread()
{
    unsigned int i;
    terminated = true;
    m_semaphore = semaphore;
    
    m_parent = parent;
    m_laufende = 0;
    m_timesThrownTogether = 0;

#ifndef SIMULATION_MODE
    // Create our player objects
    // delete these only in the destructor    
    m_players[0] = new HumanPlayer( def_id++, this );

    for( i=1;i<PLAYERS;i++)
        m_players[i] = new ComputerPlayer( def_id++, this );
#else
    for( i=0;i<PLAYERS;i++)
        m_players[i] = new ComputerPlayer( def_id++, this );
#endif // SIMULATION_MODE    

    updatePlayerNames();
    
    // make sure that results get cleaned up, when the results type is changed
    /*
      connect( Settings::instance(), SIGNAL( resultsTypeChanged() ), this, SLOT( resetGameResults() ) );
      connect( Settings::instance(), SIGNAL( playerNamesChanged() ), this, SLOT( updatePlayerNames() ) );
    */
}

Game::~Game()
{
    unsigned int i;

    m_currstich.clear();
    for(i=0;i<PLAYERS;i++)
    {
        delete m_players[i];
        m_players[i]=NULL;
    }
}

void Game::run()
{
    gameLoop();
}

void Game::startGame()
{
    unsigned int i = 0;
    CardList *playercards[PLAYERS];

    m_allcards.randomize();
    for( i=0; i<PLAYERS; i++)
        playercards[i]=new CardList();

    for( i=0; i<CARD_CNT; i++)
        playercards[i%PLAYERS]->append(m_allcards.at(i));

    for( i=0;i<PLAYERS;i++)
    {
        m_players[i]->setCards( playercards[i] );
        m_players[i]->stiche()->clear();
    }

    m_playedcards.clear();

    postEvent( GameStarted );
    //emit gameStarted();
}

// Wer hat den ganzen rekursiven code in gameLoop zu veantworten? Bitte sofort erschießen :)
void Game::gameLoop()
{
    int i, a, index, realindex;
    Player *tmp[PLAYERS];
    Card *c=NULL;
    int gamecnt=0;
    QStringList* playernames = NULL;
    
    terminated = false;

    resetGameResults();

    while(!terminated)
    {
        startGame();

        // setLast() has to be done after startGame(),
        // so that CanvasPlayer draws cards correctly for
        // doubling when gameStarted is emited.
        for(i=0;i<PLAYERS;i++)
        {
            tmp[i]=m_players[(i+gamecnt)%PLAYERS];
            if (i == PLAYERS - 1)
                postEvent( PlayerIsLast, tmp[i]->id() );
        }

        for(i=0;i<PLAYERS;i++)
        {
            tmp[i]->klopfen();
            tmp[i]->sortCards();
            if( tmp[i]->geklopft() && tmp[i]->rtti() != Player::HUMAN )
            {
                postEvent(PlayerDoubled, tmp[i]->id(), 0, i18n("%1 has doubled.", tmp[i]->name()), true);
                // emit signalDoubled();
            }
            if(terminated)
                return;
        }

        postEvent( RedrawPlayers );

        // find a player you can playercards
        // and setup m_gameinfo
        if( !setupGameInfo(tmp) )
            continue;

        for(i=0;i<PLAYERS;i++)
        {
            tmp[i]->sortCards();
            tmp[i]->init();
        }

        postEvent( RedrawPlayers );

        for( i = 0; i < TURNS; i++ )
        {
            m_currstich.clear();

            for(a=0;a<PLAYERS;a++)
            {
                // TODO: if this check fails we have
                //       a problem anyways so we should
                //       maybe remove it
                if(tmp[a])
                {
                    m_nextPlayer = ( a+1 < PLAYERS ? tmp[a+1] : NULL );
                    c = tmp[a]->play();
                }

                if(terminated || c==NULL)
                    return;

                for( int z = 0; z<tmp[a]->cards()->count(); z++ )
                {
                    if(tmp[a]->cards()->at(z) == c)
                    {
                                tmp[a]->cards()->removeAt(z);
                                break;
                    }
                }

                m_currstich.append(c);
                int* cards = new int[2];
                cards[0] = c->id();
                cards[1] = 0;
                m_playedcards.append(c);
                postEvent( CardPlayed, tmp[a]->id(), cards, QString(), true );
                //emit playerPlayedCard(tmp[a]->id(),c);

#ifndef SIMULATION_MODE
               sleep( 1 );
#endif // SIMULATION_MODE

                if( terminated )
                    return;
            }
            index = highestCard();
            tmp[index]->addStich( m_currstich );

            playernames = new QStringList();
            for( a = 0; a < PLAYERS; a++ )
                playernames->append( m_currstich.at(a)->owner()->name() );

            postEvent( PlayerMadeStich, tmp[index]->id(), m_currstich.toIntList(), tmp[index]->name(), false, playernames );

            //emit playerMadeStich(tmp[index]->id());
            // Sortiere so, dass der stecher nächste karte spielt
            for( realindex = 0; realindex < PLAYERS; realindex++ )
            {
                if(m_players[realindex]==tmp[index])
                    break;
            }
            for( a = 0; a < PLAYERS; a++ )
                tmp[a]=m_players[(a+realindex)%PLAYERS];
        }

        if(!terminated)
            gameResults();
        gamecnt++;

#ifdef SIMULATION_MODE
        if( gamecnt >= SIMULATION_MODE_MAX )
        {
            terminated = true;
            qDebug("Terminating Simulation after %i games.", SIMULATION_MODE_MAX );
        }
#endif // SIMULATION_MODE
    }
}

/*const*/ CardList *Game::currStich() //const
{
    return &m_currstich;
}

void Game::endGame(void)
{
    terminated=true;
    postEvent( GameEnded );

    //emit gameEnded();
    //EXIT_LOOP();
}

/*const*/ GameInfo *Game::gameInfo() //const
{
    return &m_gameinfo;
}

Player* Game::findId( unsigned int id ) const
{
    for( unsigned int i = 0; i < PLAYERS; i++)
    {
        if( id == m_players[i]->id() )
            return m_players[i];
    }
    return 0;
}

Player* Game::findIndex( unsigned int index ) const
{
    return ( index < PLAYERS ? m_players[index] : 0 );
}

int Game::highestCard( CardList* list )
{
    if( !list )
        list = &m_currstich;
        
    Card* high = 0;
    if( !list->empty() ) high = list->first();
    Card* card = 0;
    int i = 0;

    for( int t = 0; t < list->count(); t++ )
    {
        card = list->at( t );
        if( isHigher( card, high ) )
        {
            high = card;
            continue;
        }
    }
    
    for( ; i < (signed int)list->count(); i++ )
    {
        if( list->at(i) == high )
            break;
    }
    return i;
}

bool Game::isHigher( Card* card, Card* high )
{
    int cardVal, highVal;
    int colcnt;

    cardVal=m_gameinfo.evalCard(card, &m_gameinfo);
    highVal=m_gameinfo.evalCard(high, &m_gameinfo);
    if( m_gameinfo.istTrumpf( card ) || m_gameinfo.istTrumpf( high ) )
        return highVal < cardVal;
    else
    {
        switch( m_gameinfo.mode() )
        {
            case GameInfo::GEIER:
            case GameInfo::WENZ:
                colcnt=NUMCARDS-1;
                break;
            case GameInfo::RAMSCH:
            case GameInfo::RUFSPIEL:
            case GameInfo::STICHT:
            case GameInfo::DACHS:
            default:
                colcnt=NUMCARDS-2;
                break;
        }
        if( ( highVal - 1 ) / colcnt == ( cardVal - 1 ) / colcnt )
            return highVal < cardVal;
    }
    return false;
}

void Game::gameResults()
{
    Results r;
    r.setLaufende( m_laufende );
    r.setGame( this );

    for( unsigned int i=0;i<PLAYERS;i++)
    {
        postEvent( PlayerResults, m_players[i]->id(), 0, r.formatedPoints(m_players[i]), true );
        // emit playerResult( m_players[i]->name(), r->formatedPoints(m_players[i])  );
    }

    postEvent( InfoMessage, 0, 0, r.result(), true );
    //m_canvas->information( r->result() );
    m_timesThrownTogether = 0;

    // the game is over, so the GameInfo structure is not
    // valid. Results in a cleared updateInfo() field in the UI
    gameInfo()->setValid( false );
}

bool Game::setupGameInfo(Player *players[])
{
    // list of games the players want to playercards
    // maximum 4 entries
    QList<GameInfo*> games;
    unsigned int i = 0;
    int humanPlayer = 0, playerInstance;
    for( i=0;i<PLAYERS;i++)
    {
        if( players[i]->rtti() == Player::HUMAN )
            humanPlayer=i;
    }

    for( i=0;i<PLAYERS;i++)
    {
        playerInstance=4-humanPlayer+i;
        if(playerInstance>3)
             playerInstance=playerInstance-4;
        GameInfo* info = players[i]->gameInfo();
        if(terminated)
            return false;
        if( info )
        {
            info->setSpieler( players[i] );
            games.append( info );
            if( players[i]->rtti() != Player::HUMAN )
            {
                postEvent(Bubble, playerInstance, 0, i18n("%1: \"I want to play.\"", players[i]->name()), true);
                sleep ( 1 );
            }
        }
        else
        {
            if( players[i]->rtti() != Player::HUMAN )
            {
                postEvent(Bubble, playerInstance, 0, i18n("%1: \"I don't want to play.\"", players[i]->name()), true);
                sleep ( 1 );
            }
        }
    }

    if(terminated)
        return false;

    if( games.isEmpty() )
    {
        if( !setupGameInfoForced(players) )
            return false;
    } 
    else
    {
        // find the highest game in the list
        GameInfo* best = games.first();
        for( int t = 0; t < games.count(); t++ )
        {
            if( *games.at( t ) > *best )
                best = games.at( t );
        }

        m_gameinfo = *best;
    }

    // finde den mitspieler:
    if( m_gameinfo.mode()==GameInfo::RUFSPIEL ) 
    {
        Card sau( Card::SAU, static_cast<Card::EColor>(m_gameinfo.color()) );
        for( i=0;i<PLAYERS || !m_gameinfo.mitspieler();i++ )
        {
            for( unsigned int z=0; z < CARD_CNT/PLAYERS; z++ )
            {
                if( players[i]->cards()->at(z)->isEqual( &sau ) )
                {
                    m_gameinfo.setMitspieler(players[i]);
                    break;
                }
            }
        }
    }

    m_laufende = m_gameinfo.laufende();
    postEvent( InfoMessage, 0, 0, m_gameinfo.toString(), true );
    //m_canvas->information( m_gameinfo.toString() );

    m_gameinfo.setValid( true );
    postEvent( GameInfoSetup );
    //emit signalSetupGameInfo();
    return true;
}

bool Game::setupGameInfoForced(Player *players[])
{
    int i;
    GameInfo* info;

    if( Settings::instance()->doublerHasToPlay() )
    {
        // if someone has doubled he is forced to player
        // if more players have doubled the last one has to play
        for( i=PLAYERS-1;i>=0;i-- )
        {
            if( players[i]->geklopft() )
            {
                postEvent(InfoMessage, 0, 0, i18n("%1 has doubled last\nand has to play now.", players[i]->name()), true);

                info = players[i]->gameInfo( true );
                info->setSpieler( players[i] );
                m_gameinfo = *info;
                delete info;
                return true;
            }
        }
    }

    if( Settings::instance()->noGame() == Settings::NOGAME_NEUGEBEN )
    {
        postEvent(InfoMessage, 0, 0, i18n("No one wants to play.\nCards will be thrown together."), true);
        m_timesThrownTogether++;
        m_gameinfo.setValid( false );
        postEvent( GameInfoSetup );
        //emit signalSetupGameInfo();
        return false;
    }
    else if( Settings::instance()->noGame() == Settings::NOGAME_ALTERSPIELT )
    {
        // find player with eichel ober
        for( i=0;i<PLAYERS;i++ )
        {
            if( players[i]->cards()->contains( Card::EICHEL, Card::OBER ) )
            {
                postEvent(InfoMessage, 0, 0, i18n("%1 has got the Eichel Ober\nand has to play.", players[i]->name()), true);
                
                info = players[i]->gameInfo( true );
                info->setSpieler( players[i] );
                m_gameinfo = *info;
                delete info;
                return true;
            }
        }
    }
    else if( Settings::instance()->noGame() == Settings::NOGAME_RAMSCH )
    {
        postEvent(InfoMessage, 0, 0, i18n("No one wants to play.\nRamsch will be played."), true);
        m_gameinfo.setSpieler( NULL );
        m_gameinfo.setMitspieler( NULL );
        m_gameinfo.setMode( GameInfo::RAMSCH );
        m_gameinfo.setColor( Card::NOCOLOR );
        return true;
    }

    return false;
}

void Game::resetGameResults()
{
    int i;
    // reset points (i.e. results) from a previous game
    for(i=0;i<PLAYERS;i++)
        m_players[i]->setPoints( 0.0 );    
    m_timesThrownTogether = 0;
}

int Game::timesDoubled()
{
    int i, d = 0;

    for( i=0;i<PLAYERS;i++)
        if( m_players[i]->geklopft() )
            d++;
    if( Settings::instance()->doubleNextGame() )
        d = d + m_timesThrownTogether;
    return d;
}

int Game::timesThrownTogether()
{
    return m_timesThrownTogether;
}

void Game::updatePlayerNames()
{
    int i;
    QStringList list = Settings::instance()->playerNames();
    m_players[0]->setName( list[0] );
    postEvent( PlayerNameChanged, m_players[0]->id(), 0, list[0] );

    for( i=1;i<PLAYERS;i++)
    {
        m_players[i]->setName( list[i] );
        postEvent( PlayerNameChanged, m_players[i]->id(), 0, list[i] );
    }

    postEvent( RedrawPlayers );
    //m_canvas->redrawPlayers();
}

void* Game::postEvent( EAction action, unsigned int playerid, int* cardids, QString d, bool wait, QStringList* names )
{
    t_EventData* data = new t_EventData;
    void* ret = NULL;

#ifdef SIMULATION_MODE
    // ignore all actions that require user 
    // interaction in simulation mode
    if( action == InfoMessage ||
        action == PlayerDoubled )
    {
        if( cardids )
            delete [] cardids;

        if( names )
            delete names;

        delete data;
        return NULL;
    }
#endif // SIMULATION_MODE

    data->type = action;
    data->playerid = playerid;
    data->cardids = cardids;
    data->data = d;
    data->wait = wait;
    data->playernames = names;
    data->returncode = NULL;
    data->quitgame = false;

    QCoreApplication::postEvent( m_parent, new SchafKopfEvent( data ) );
    if( wait )
    {
        m_semaphore->acquire();

        if( data->quitgame )
            endGame();

        ret = data->returncode;

        if( data->cardids )
            delete [] data->cardids;

        if( data->playernames )
            delete data->playernames;

        delete data;
    }

    return ret;
}
