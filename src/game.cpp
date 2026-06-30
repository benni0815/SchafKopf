// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "game.h"

#include "humanplayer.h"
#include "computerplayer.h"
#include "cardlist.h"
#include "gameinfo.h"
#include "results.h"
#include "settings.h"

#include <KLocalizedString>

#include <QCoreApplication>
#include <QEvent>


namespace
{

bool isHuman(const Player* player)
{
  return dynamic_cast<const HumanPlayer*>(player);
}

} // namespace

Game::Game(QObject* parent)
    : QThread(parent)
{
  int playerId{0};

  if constexpr (SIMULATION_MODE)
  {
    for (auto& player : m_players)
    {
      player = std::make_shared<ComputerPlayer>(playerId++, this);
    }
  }
  else
  {
    m_players[0] = std::make_shared<HumanPlayer>(playerId++, this);

    for (size_t i = 1; i < m_players.size(); ++i)
    {
      m_players[i] = std::make_shared<ComputerPlayer>(playerId++, this);
    }
  }

  updatePlayerNames();
    
    // make sure that results get cleaned up, when the results type is changed
    /*
      connect( Settings::instance(), SIGNAL( resultsTypeChanged() ), this, SLOT( resetGameResults() ) );
      connect( Settings::instance(), SIGNAL( playerNamesChanged() ), this, SLOT( updatePlayerNames() ) );
    */
}

void Game::run()
{
    gameLoop();
}

void Game::startGame()
{
    unsigned int i = 0;
    CardList playercards[PLAYERS];

    m_allcards.randomize();

    for( i=0; i<CARD_CNT; i++)
        playercards[i%PLAYERS].append(m_allcards.at(i));

    for( i=0;i<PLAYERS;i++)
    {
        m_players[i]->setCards( playercards[i] );
        m_players[i]->clearStiche();
    }

    m_playedcards.clear();

    emit gameStarted();
}

// Wer hat den ganzen rekursiven code in gameLoop zu veantworten? Bitte sofort erschießen :)
void Game::gameLoop()
{
    int i, a, index, realindex;
    Player *tmp[PLAYERS];
    Card* c{};
    int gamecnt=0;

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
            tmp[i] = m_players[(i + gamecnt) % PLAYERS].get();
            if (i == PLAYERS - 1)
                emit playerIsLast(tmp[i]->id());
        }

        for(i=0;i<PLAYERS;i++)
        {
            tmp[i]->klopfen();
            tmp[i]->sortCards();
            if (tmp[i]->geklopft() && !isHuman(tmp[i]))
            {
              if constexpr (!SIMULATION_MODE)
              {
                emit playerDoubled(i18n("%1 has doubled.", tmp[i]->name()));
              }
            }
            if(terminated)
                return;
        }

        emit redrawPlayers();

        // find a player you can playercards
        // and setup m_gameinfo
        if( !setupGameInfo(tmp) )
            continue;

        for(i=0;i<PLAYERS;i++)
        {
            tmp[i]->sortCards();
            tmp[i]->init();
        }

        emit redrawPlayers();

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
                    m_nextPlayer = (a + 1 < PLAYERS ? tmp[a + 1] : nullptr);
                    c = tmp[a]->play();
                }

                if(terminated || c == nullptr)
                    return;

                tmp[a]->removeCard(c);
                m_currstich.push_back(c);

                m_playedcards.push_back(c);
                emit cardPlayed(tmp[a]->id(), c->id());

                if constexpr (!SIMULATION_MODE) { sleep(1); }

                if( terminated )
                    return;
            }
            index = highestCard();
            tmp[index]->addStich( m_currstich );

            QStringList playernames;
            for( a = 0; a < PLAYERS; a++ )
                playernames.append(m_currstich.at(a)->owner()->name());

            emit playerMadeStich(tmp[index]->id(), m_currstich.toInts(), tmp[index]->name(), playernames);

            // Sortiere so, dass der stecher nächste karte spielt
            for( realindex = 0; realindex < PLAYERS; realindex++ )
            {
                if (m_players[realindex].get() == tmp[index])
                    break;
            }
            for( a = 0; a < PLAYERS; a++ )
                tmp[a] = m_players[(a + realindex) % PLAYERS].get();
        }

        if(!terminated)
            gameResults();
        gamecnt++;

        if constexpr (SIMULATION_MODE)
        {
          if (gamecnt >= SIMULATION_MODE_MAX)
          {
            terminated = true;
            qDebug("Terminating Simulation after %i games.", SIMULATION_MODE_MAX );
          }
        }
    }
}

const CardList& Game::currStich() const
{
  return m_currstich;
}

void Game::endGame()
{
  terminated = true;
  emit gameEnded();
}

/*const*/ GameInfo *Game::gameInfo() //const
{
    return &m_gameinfo;
}

Player* Game::findIndex( unsigned int index ) const
{
    return (index < PLAYERS ? m_players[index].get() : nullptr);
}

std::array<std::shared_ptr<Player>, PLAYERS> Game::players()
{
  return m_players;
}

int Game::highestCard( CardList* list )
{
    if( !list )
        list = &m_currstich;

    Card* high{};
    if (!list->empty()) { high = list->front(); }
    Card* card{};

    for (int t = 0; t < list->size(); ++t)
    {
        card = list->at( t );
        if( isHigher( card, high ) )
        {
            high = card;
            continue;
        }
    }
    
    for (int i = 0; i < (int)list->size(); ++i)
    {
        if( list->at(i) == high )
            return i;
    }
    return 0;
}

bool Game::isHigher( Card* card, Card* high )
{
    int cardVal, highVal;
    int colcnt;

    cardVal = m_gameinfo.evalCard(card);
    highVal = m_gameinfo.evalCard(high);
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
      emit playerResults(m_players[i]->id(), r.formatedPoints(m_players[i].get()));
    }

    emit infoMessage(r.result());
    m_timesThrownTogether = 0;

    // the game is over, so the GameInfo structure is not
    // valid. Results in a cleared updateInfo() field in the UI
    gameInfo()->setValid( false );
}

bool Game::setupGameInfo(Player *players[])
{
    // list of games the players want to playercards
    // maximum 4 entries
    unsigned int i = 0;
    int humanPlayer = 0, playerInstance;
    for( i=0;i<PLAYERS;i++)
    {
        if (isHuman(players[i]))
            humanPlayer=i;
    }

    std::vector<GameInfo> games;
    for( i=0;i<PLAYERS;i++)
    {
        playerInstance=4-humanPlayer+i;
        if(playerInstance>3)
             playerInstance=playerInstance-4;
        auto info = players[i]->gameInfo();
        if(terminated)
            return false;
        if( info )
        {
            info->setSpieler( players[i] );
            games.push_back(info.value());
            if (!isHuman(players[i]))
            {
                emit bubble(playerInstance, i18n("%1: \"I want to play.\"", players[i]->name()));
                sleep (1);
            }
        }
        else
        {
            if (!isHuman(players[i]))
            {
              emit bubble(playerInstance, i18n("%1: \"I don't want to play.\"", players[i]->name()));
              sleep(1);
            }
        }
    }

    if(terminated)
        return false;

    if (games.empty())
    {
        if( !setupGameInfoForced(players) )
            return false;
    } 
    else
    {
        // find the highest game in the list
        GameInfo& best = games.front();
        for (size_t t = 0; t < games.size(); ++t)
        {
            if (games.at(t) > best)
                best = games.at(t);
        }

        m_gameinfo = best;
    }

    // finde den mitspieler:
    if( m_gameinfo.mode()==GameInfo::RUFSPIEL ) 
    {
        Card sau(Card::SAU, static_cast<Card::Color>(m_gameinfo.color()));
        for( i=0;i<PLAYERS || !m_gameinfo.mitspieler();i++ )
        {
            for( unsigned int z=0; z < CARD_CNT/PLAYERS; z++ )
            {
                if (players[i]->cards().at(z)->isEqual(&sau))
                {
                    m_gameinfo.setMitspieler(players[i]);
                    break;
                }
            }
        }
    }

    m_laufende = m_gameinfo.laufende();
    emit infoMessage(m_gameinfo.toString());

    m_gameinfo.setValid( true );
    emit gameInfoSetup();
    return true;
}

bool Game::setupGameInfoForced(Player *players[])
{
    int i;

    if( Settings::instance()->doublerHasToPlay() )
    {
        // if someone has doubled he is forced to player
        // if more players have doubled the last one has to play
        for( i=PLAYERS-1;i>=0;i-- )
        {
            if( players[i]->geklopft() )
            {
                emit infoMessage(i18n("%1 has doubled last\nand has to play now.", players[i]->name()));

                m_gameinfo = *players[i]->gameInfo(true);
                m_gameinfo.setSpieler(players[i]);
                return true;
            }
        }
    }

    if( Settings::instance()->noGame() == Settings::NOGAME_NEUGEBEN )
    {
        emit infoMessage(i18n("No one wants to play.\nCards will be thrown together."));
        m_timesThrownTogether++;
        m_gameinfo.setValid( false );
        emit gameInfoSetup();
        return false;
    }
    else if( Settings::instance()->noGame() == Settings::NOGAME_ALTERSPIELT )
    {
        // find player with eichel ober
        for( i=0;i<PLAYERS;i++ )
        {
            if (players[i]->cards().contains(Card::EICHEL, Card::OBER))
            {
                emit infoMessage(i18n("%1 has got the Eichel Ober\nand has to play.", players[i]->name()));
                
                m_gameinfo = *players[i]->gameInfo(true);
                m_gameinfo.setSpieler(players[i]);
                return true;
            }
        }
    }
    else if( Settings::instance()->noGame() == Settings::NOGAME_RAMSCH )
    {
        emit infoMessage(i18n("No one wants to play.\nRamsch will be played."));
        m_gameinfo.setSpieler(nullptr);
        m_gameinfo.setMitspieler(nullptr);
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
  const auto names = Settings::instance()->playerNames();

  for (size_t i = 0; i < m_players.size(); ++i)
  {
    m_players[i]->setName(names.at(i));
  }

  emit redrawPlayers();
}
