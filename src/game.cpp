// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "game.h"

#include "cards.h"
#include "computerplayer.h"
#include "gameinfo.h"
#include "humanplayer.h"
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
      player = std::make_shared<ComputerPlayer>(playerId++, *this);
    }
  }
  else
  {
    m_players[0] = std::make_shared<HumanPlayer>(playerId++, *this);

    for (size_t i = 1; i < m_players.size(); ++i)
    {
      m_players[i] = std::make_shared<ComputerPlayer>(playerId++, *this);
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
  m_allcards = Cards::randomize();

  auto it = std::begin(m_allcards);
  for (auto& player : m_players)
  {
    CardList cards;
    it = std::for_each_n(it, NUMCARDS, [&](Card& c) { c.owner = player.get(); cards.push_back(c); });
    player->setCards(cards);
    player->clearStiche();
  }

  m_playedcards.clear();

  emit gameStarted();
}

// Wer hat den ganzen rekursiven code in gameLoop zu veantworten? Bitte sofort erschießen :)
void Game::gameLoop()
{
  std::array<Player*, PLAYERS> tmp;
  int gamecnt = 0;

  terminated = false;

  resetGameResults();

  while (!terminated)
  {
    startGame();

    // setLast() has to be done after startGame(),
    // so that CanvasPlayer draws cards correctly for
    // doubling when gameStarted is emited.
    for (int i = 0; i < PLAYERS; ++i)
    {
      tmp[i] = m_players[(i + gamecnt) % PLAYERS].get();
      if (i == PLAYERS - 1)
      {
        emit playerIsLast(tmp[i]->id());
      }
    }

    for (auto& player : tmp)
    {
      player->klopfen();
      player->sortCards();
      if (player->geklopft() && !isHuman(player))
      {
        if constexpr (!SIMULATION_MODE)
        {
          emit playerDoubled(i18n("%1 has doubled.", player->name()));
        }
      }
      if (terminated)
      {
        return;
      }
    }

    emit redrawPlayers();

    // find a player you can playercards
    // and setup m_gameinfo
    if (!setupGameInfo(tmp))
    {
      continue;
    }

    for (auto& player : tmp)
    {
      player->sortCards();
      player->init();
    }

    emit redrawPlayers();

    for (int i = 0; i < TURNS; ++i)
    {
      m_currstich.clear();

      for (auto it = std::begin(tmp); it != std::end(tmp); ++it)
      {
        Player* player = *it;
        m_nextPlayer = (std::next(it) != std::end(tmp)) ? *std::next(it) : nullptr;
        const Card c = player->play();

        if (terminated || c.id == CardId{})
        {
          return;
        }

        player->removeCard(c.id);
        m_currstich.push_back(c);

        m_playedcards.push_back(c);
        emit cardPlayed(player->id(), c.id);

        if constexpr (!SIMULATION_MODE)
        {
          sleep(1);
        }

        if (terminated)
        {
          return;
        }
      }
      auto highestCard = highestCardInStich();
      highestCard.owner->addStich(m_currstich);

      QStringList playernames;
      for (const Card c : m_currstich)
      {
        playernames.append(c.owner->name());
      }

      emit playerMadeStich(highestCard.owner->id(), Cards::toIds(m_currstich), highestCard.owner->name(), playernames);

      // Sortiere so, dass der stecher nächste karte spielt
      int realindex{0};
      for (; realindex < PLAYERS; ++realindex)
      {
        if (m_players[realindex].get() == highestCard.owner)
        {
          break;
        }
      }
      for (int a = 0; a < PLAYERS; a++)
      {
        tmp[a] = m_players[(a + realindex) % PLAYERS].get();
      }
    }

    if (!terminated)
    {
      gameResults();
    }
    gamecnt++;

    if constexpr (SIMULATION_MODE)
    {
      if (gamecnt >= SIMULATION_MODE_MAX)
      {
        terminated = true;
        qDebug("Terminating Simulation after %i games.", SIMULATION_MODE_MAX);
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

const GameInfo& Game::gameInfo() const
{
  return m_gameinfo;
}

std::vector<Player*> Game::players() const
{
  std::vector<Player*> ps;
  std::transform(std::begin(m_players), std::end(m_players), std::back_inserter(ps), [] (const auto& p) { return p.get(); });
  return ps;
}

Card Game::highestCardInStich() const
{
  return m_gameinfo.highestCard(m_currstich);
}

void Game::gameResults()
{
  Results r(*this);

  for (const auto& player : m_players)
  {
    emit playerResults(player->id(), r.formatedPoints(player.get()));
  }

  emit infoMessage(r.result());
  m_timesThrownTogether = 0;

  // the game is over, so the GameInfo structure is not
  // valid. Results in a cleared updateInfo() field in the UI
  m_gameinfo.setValid(false);
}

bool Game::setupGameInfo(const std::array<Player*, PLAYERS>& players)
{
  // list of games the players want to playercards
  // maximum 4 entries
  int humanPlayer = 0;
  for (unsigned int i = 0; i < PLAYERS; ++i)
  {
    if (isHuman(players[i]))
    {
      humanPlayer = i;
    }
  }

  std::vector<GameInfo> games;
  for (unsigned int i = 0; i < PLAYERS; ++i)
  {
    int playerInstance = 4 - humanPlayer + i;
    if (playerInstance > 3)
    {
      playerInstance = playerInstance - 4;
    }
    auto game = players[i]->gameInfo();
    if (terminated)
    {
      return false;
    }
    if (game)
    {
      auto gameInfo = GameInfo(game.value());
      gameInfo.setSpieler(players[i]);
      games.push_back(gameInfo);
      if (!isHuman(players[i]))
      {
        emit bubble(playerInstance, i18n("%1: \"I want to play.\"", players[i]->name()));
        sleep(1);
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

  if (terminated)
  {
    return false;
  }

  if (games.empty())
  {
    if (!setupGameInfoForced(players))
    {
      return false;
    }
  }
  else
  {
    // find the highest game in the list
    GameInfo& best = games.front();
    for (size_t t = 0; t < games.size(); ++t)
    {
      if (games.at(t) > best)
      {
        best = games.at(t);
      }
    }

    m_gameinfo = best;
  }

  // finde den mitspieler:
  if (m_gameinfo.mode() == GameVariant::RUFSPIEL)
  {
    for (auto& player : players)
    {
      const CardId sau{CardId::Sau, m_gameinfo.color()};
      if (Cards::find(player->cards(), sau))
      {
        m_gameinfo.setMitspieler(player);
        break;
      }
    }
  }

  emit infoMessage(m_gameinfo.toString());

  m_gameinfo.setValid(true);
  emit gameInfoSetup();
  return true;
}

bool Game::setupGameInfoForced(const std::array<Player*, PLAYERS>& players)
{
  int i;

  if (Settings::instance()->doublerHasToPlay())
  {
    // if someone has doubled he is forced to player
    // if more players have doubled the last one has to play
    for (i = PLAYERS - 1; i >= 0; i--)
    {
      if (players[i]->geklopft())
      {
        emit infoMessage(i18n("%1 has doubled last\nand has to play now.", players[i]->name()));

        m_gameinfo = *players[i]->gameInfo(true);
        m_gameinfo.setSpieler(players[i]);
        return true;
      }
    }
  }

  if (Settings::instance()->noGame() == Settings::NOGAME_NEUGEBEN)
  {
    emit infoMessage(i18n("No one wants to play.\nCards will be thrown together."));
    m_timesThrownTogether++;
    m_gameinfo.setValid(false);
    emit gameInfoSetup();
    return false;
  }
  else if (Settings::instance()->noGame() == Settings::NOGAME_ALTERSPIELT)
  {
    // find player with eichel ober
    for (auto& player : players)
    {
      const auto eichelOber = CardId{CardId::Ober, CardId::Eichel};
      if (Cards::find(player->cards(), eichelOber))
      {
        emit infoMessage(i18n("%1 has got the Eichel Ober\nand has to play.", player->name()));

        m_gameinfo = *player->gameInfo(true);
        m_gameinfo.setSpieler(player);
        return true;
      }
    }
  }
  else if (Settings::instance()->noGame() == Settings::NOGAME_RAMSCH)
  {
    emit infoMessage(i18n("No one wants to play.\nRamsch will be played."));
    m_gameinfo = GameInfo({GameVariant::RAMSCH, CardId::NoColor});
    return true;
  }

  return false;
}

void Game::resetGameResults()
{
  // reset points (i.e. results) from a previous game
  for (auto& player : m_players)
  {
    player->setPoints(0);
  }
  m_timesThrownTogether = 0;
}

int Game::timesDoubled() const
{
  int d = std::count_if(std::begin(m_players), std::end(m_players), [](const auto& p) { return p->geklopft(); });

  if (Settings::instance()->doubleNextGame())
  {
    d = d + m_timesThrownTogether;
  }
  return d;
}

int Game::timesThrownTogether() const
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
