// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "schafkopf.h"

#include "canvascard.h"
#include "game.h"
#include "gamecanvas.h"
#include "humanplayer.h"
#include "newgamewizard.h"
#include "player.h"
#include "preferencesdlg.h"
#include "selectgamewizard.h"
#include "settings.h"
#include "stichdlg.h"

#include <schafkopf_version.h>

#include <QApplication>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

#include <KActionCollection>
#include <KHelpMenu>
#include <KLocalizedString>
#include <KToolBar>

// for pow()
#include <cmath>


SchafKopf::SchafKopf(QWidget* parent)
: KXmlGuiWindow(parent)
{
  KAboutData::setApplicationData(aboutData());

  split = new QSplitter(Qt::Horizontal, this);
  split->setChildrenCollapsible(true);

  setCentralWidget(split);
  // do not start with a too tiny window at the first start
  resize(800, 550);
  // save window size automatically
  setAutoSaveSettings("SchafKopf", true);

  m_scene = new QGraphicsScene(this);
  m_view = new GameCanvas(m_scene, split);
  m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  m_game = new Game(this);
  for (auto* p : m_game->players())
  {
    connect(p, &Player::gotCards, m_view, &GameCanvas::setPlayerCards);
    connect(p, &Player::doubled, m_view, [this](unsigned int playerId) { m_view->playerHasDoubled(playerId, true); });
    connect(p, &Player::nameChanged, m_view, &GameCanvas::setPlayerName);
    if (auto* human = dynamic_cast<HumanPlayer*>(p))
    {
      connect(human, &HumanPlayer::forbiddenCard, m_view, &GameCanvas::cardForbidden, Qt::BlockingQueuedConnection);
      connect(human, &HumanPlayer::questionYesNo, m_view, &GameCanvas::questionYesNo, Qt::BlockingQueuedConnection);
      connect(human, &HumanPlayer::getCard, m_view, &GameCanvas::getCard, Qt::BlockingQueuedConnection);
      connect(human, &HumanPlayer::selectGame, this, &SchafKopf::selectGame, Qt::BlockingQueuedConnection);
    }
  }
  connect(m_game, &Game::redrawPlayers, m_view, [this]
          {
            m_view->redrawPlayers();
            enableControls();
          });
  connect(m_game, &Game::gameInfoSetup, this, &SchafKopf::updateInfo);
  connect(m_game, &Game::gameStarted, m_view, [this]
          {
            m_view->resetPlayers();
            m_view->redrawPlayers();
            enableControls();
          });
  connect(m_game, &Game::gameEnded, m_view, [this]
          {
            m_view->resetPlayerCards();
            m_view->resetPlayers();
            m_view->redrawPlayers();
            enableControls();
          });
  connect(m_game, &Game::playerIsLast, m_view, &GameCanvas::playerIsLast);
  connect(m_game, &Game::playerMadeStich, m_view, [this]
          (unsigned int playerId, const std::vector<CardId>& cardIds, const QString& playerName, const QStringList& playerNames)
          {
            m_view->slotPlayerMadeStich(playerId);
            m_stichdlg->changed(playerName, cardIds, playerNames);
          });
  connect(m_game, &Game::playerDoubled, m_view, [this] (const QString& message)
          {
            updateInfo();
            m_view->information(message);
          }, Qt::BlockingQueuedConnection);
  connect(m_game, &Game::cardPlayed, m_view, &GameCanvas::slotPlayerPlayedCard, Qt::BlockingQueuedConnection);
  connect(m_game, &Game::infoMessage, m_view, &GameCanvas::information, Qt::BlockingQueuedConnection);
  connect(m_game, &Game::bubble, m_view, &GameCanvas::Bubble, Qt::BlockingQueuedConnection);
  connect(m_game, &Game::playerResults, this, &SchafKopf::slotPlayerResult, Qt::BlockingQueuedConnection);

  QLabel* label = new QLabel(i18n("Results:"));
  label->setAlignment(Qt::AlignHCenter);

  m_table = new QTableWidget;
  m_table->setColumnCount(PLAYERS);
  //m_table->setLeftMargin( 0 );
  updateTableNames();
  m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  lblCurGame = new QLabel;
  lblDoubled = new QLabel;
  lblCurGame->setWordWrap(true);
  lblDoubled->setWordWrap(true);

  btnLastTrick = new QPushButton;
  btnLastTrick->setFlat(true);
  split->setSizes(Settings::instance()->splitterSizes(width()));
  setupActions();

  QGroupBox* groupInfo = new QGroupBox(i18n("Game Information:"));
  QVBoxLayout* groupInfo_layout = new QVBoxLayout(groupInfo);
  groupInfo_layout->addWidget(lblCurGame);
  groupInfo_layout->addWidget(lblDoubled);
  groupInfo_layout->addWidget(btnLastTrick);

  QWidget* leftBox = new QWidget(split);
  QVBoxLayout* leftBox_layout = new QVBoxLayout(leftBox);
  leftBox_layout->addWidget(label);
  leftBox_layout->addWidget(m_table);
  leftBox_layout->addWidget(groupInfo);

  const auto app = dynamic_cast<QGuiApplication*>(QApplication::instance());
  connect(app, &QGuiApplication::lastWindowClosed, this, &SchafKopf::saveConfig);
  connect(app, &QGuiApplication::lastWindowClosed, this, &SchafKopf::endGame);
  //connect(kapp, SIGNAL(aboutToQuit()), this, SLOT(endGame()));

  connect(btnLastTrick, &QPushButton::clicked, this, &SchafKopf::showStich);
  connect(Settings::instance(), &Settings::resultsTypeChanged, this, &SchafKopf::clearTable);
  connect(Settings::instance(), &Settings::playerNamesChanged, this, &SchafKopf::updateTableNames);
  connect(Settings::instance(), &Settings::cardChanged, this, &SchafKopf::updateInfo);

  btnLastTrick->setToolTip(i18n("Show the last trick that was made."));

  m_stichdlg = new StichDlg(this);

  updateInfo();
}

SchafKopf::~SchafKopf()
{
  saveConfig();
  // make sure the thread is really not running
  endGame();
  m_game->wait();
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

void SchafKopf::saveConfig()
{
  Settings::instance()->setSplitterSizes(split->sizes());
}

void SchafKopf::setupActions()
{
  auto mnuGame = new QMenu(this);
  auto mnuSettings = new QMenu(this);

  mnuGame->setTitle(i18n("&Game"));
  mnuSettings->setTitle(i18n("&Settings"));

  menuBar()->addMenu(mnuGame);
  menuBar()->addMenu(mnuSettings);
  auto help = new KHelpMenu(this, aboutData());
  menuBar()->addMenu(help->menu());

  {
    m_actStich = new QAction(this);
    m_actStich->setText(i18n("&Last Trick"));
    actionCollection()->addAction("Last Trick", m_actStich);
    connect(m_actStich, &QAction::triggered, this, &SchafKopf::showStich);
    mnuGame->addAction(m_actStich);
  }

  {
    auto actNew = KStandardAction::openNew(this, &SchafKopf::newGame, this);
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
    mnuGame->addAction(actQuit);
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
  Settings::instance()->configureCardDecks(this);
}

void SchafKopf::newGame()
{
  auto ng = newgamewizard();

  if (ng.exec() == QDialog::Accepted)
  {
    int game = ng.getGame();
    switch (game)
    {
    case newgamewizard::GAME_LOCAL:
      endGame();
      QTimer::singleShot(0, this, &SchafKopf::realNewGame);
      break;
    }
  }
}

void SchafKopf::realNewGame()
{
  // entering the game loop is the last thing
  // we want to do!
  m_game->start();
}

void SchafKopf::endGame()
{
  if (m_game->isRunning())
  {
    m_game->endGame();
    m_view->exitLoop();
  }

  clearTable();
  updateInfo();
}

void SchafKopf::showStich()
{
  if (m_game->isTerminated())
  {
    return;
  }

  m_stichdlg->show();
}

void SchafKopf::enableControls()
{
  m_actEnd->setEnabled(!m_game->isTerminated());
  m_actStich->setEnabled(!m_game->isTerminated());
  btnLastTrick->setEnabled(!m_game->isTerminated());
}

void SchafKopf::slotPlayerResult(unsigned int col, const QString& result)
{
  int lastRow = m_table->rowCount() - 1;
  if (lastRow == -1 || (m_table->item(lastRow, 0) && m_table->item(lastRow, 1) && m_table->item(lastRow, 2) && m_table->item(lastRow, 3)))
  {
    m_table->insertRow(m_table->rowCount());
  }

  QTableWidgetItem* item = new QTableWidgetItem();
  item->setText(result);
  item->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
  m_table->setItem(m_table->rowCount() - 1, col, item);
  m_table->scrollToItem(item);
}

void SchafKopf::updateInfo()
{
  int timesDoubled = 0, timesThrownTogether = 0;
  int valuation;

  const auto& pix = CanvasCard::backsidePixmap();
  btnLastTrick->setIconSize(pix.size());
  btnLastTrick->setIcon(QIcon(pix));

  if (!m_game->isTerminated() && m_game->gameInfo().isValid())
  {
    lblCurGame->setText(i18n("<qt>Current Game:<br><b>") + m_game->gameInfo().toString() + "</b></qt>");
  }
  else
  {
    lblCurGame->setText(QString());
    lblDoubled->setText(QString());
    return;
  }

  QString sDoubled;
  for (const auto* player : m_game->players())
  {
    if (player->geklopft())
    {
      sDoubled.append(i18n("<qt><b>%1</b> has doubled.</qt>", player->name()));
      timesDoubled++;
    }
  }
  timesThrownTogether = m_game->timesThrownTogether();
  if (timesThrownTogether > 0)
  {
    sDoubled.append(i18n("<qt>Times thrown together: <b>%1</b></qt>", timesThrownTogether));
  }
  valuation = (int) pow(2, timesDoubled);
  if (Settings::instance()->doubleNextGame())
  {
    valuation = valuation * (int) pow(2, timesThrownTogether);
  }
  sDoubled.append(i18n("<qt><br>Game counts <b>%1-fold</b>.</qt>", valuation));

  lblDoubled->setText(sDoubled);
}

void SchafKopf::clearTable()
{
  for (int i = 0; i < m_table->rowCount(); i++)
  {
    m_table->removeRow(i);
  }
}

void SchafKopf::configure()
{
  PreferencesDlg prefs(this);
  if (prefs.exec() == QDialog::Accepted)
  {
    m_view->updateBackground();
  }
}

void SchafKopf::updateTableNames()
{
  m_table->setHorizontalHeaderLabels(Settings::instance()->playerNames());
}

void SchafKopf::selectGame(std::optional<GameVariant>* gameInfo, const std::vector<CardId>& cardids, bool force)
{
  SelectGameWizard sgw(force, cardids);
  if (sgw.exec() == QDialog::Accepted)
  {
    *gameInfo = sgw.game();
  }
}
