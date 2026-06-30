// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "types.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <KAboutData>
#include <KXmlGuiWindow>

#include <QPointer>


class GameCanvas;
class Game;
class QLabel;
class QSplitter;
class QTableWidget;
class QPushButton;
class QGraphicsScene;
class StichDlg;

/// @short Application Main Window
/// @author Dominik Seichter <domseichter@web.de>
class SchafKopf : public KXmlGuiWindow
{
  Q_OBJECT

public:
  SchafKopf(QWidget* parent = nullptr);
  ~SchafKopf() override;

  static KAboutData aboutData();

private slots:
  /// Configures the carddeck to be used.
  void carddecks();

  /// Starts a new game.
  void newGame();

  /// Aborts the current game.
  void endGame();

  /// Shows the last stich.
  void showStich();

  /// Takes care of enabling and disabling actions.
  void enableControls();

  /// Shows the preferences dialog to the user.
  void configure();

  void realNewGame();

  void slotPlayerResult(unsigned int id, const QString& result);

  void saveConfig();

  void updateInfo();

  /// Removes all items from the table displaying the results.
  void clearTable();

  /// Updates table headings when the user changes the player names in the preferences.
  void updateTableNames();

private:
  void setupActions();
  void selectGame(std::optional<GameVariant>* gameInfo, const std::vector<CardId>& cardids, bool force = false);

  Game* m_game{};
  QGraphicsScene* m_scene{};
  GameCanvas* m_view{};
  QTableWidget* m_table{};
  QSplitter* split{};

  QLabel* lblCurGame{};
  QLabel* lblDoubled{};
  QPushButton* btnLastTrick{};

  QAction* m_actStich{};
  QAction* m_actEnd{};

  QPointer<StichDlg> m_stichdlg;
};
