// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <KXmlGuiWindow>
#include <KAboutData>

#include <QPointer>
#include <QSemaphore>


class CardList;
class GameCanvas;
class Game;
class GameInfo;
class QLabel;
class QSplitter;
class QTableWidget;
class QPushButton;
class QGraphicsScene;
class StichDlg;

/**
 * @short Application Main Window
 * @author Dominik Seichter <domseichter@web.de>
 * @version 0.1
 */
class SchafKopf : public KXmlGuiWindow
{
    Q_OBJECT
    public:
        SchafKopf(QWidget *parent=0);
        ~SchafKopf();

        static KAboutData aboutData();

    private slots:
        /** Configure the carddeck to be used
          */
        void carddecks();

        /** Start a new game
          */
        void newGame();

        /** abort current game
          */
        void endGame();

        /** show the last stich
          */
        void showStich();

        /** takes care of enabling and disabling actions
          */
        void enableControls();

        /** guess what? shows the preferences dialog to the user
          */
        void configure();

        void realNewGame();

        void slotPlayerResult( unsigned int id, const QString & result );

        void saveConfig();

        void updateInfo();

        /** remove all items from the table displaying the results
          */
        void clearTable();

        /** update table headings when the user changes the player names
          * in the preferences
          */
        void updateTableNames();

    protected:
        /** Event handler to receive thread events
          */
        void customEvent( QEvent* e );

    private:
        void setupActions();
        GameInfo* selectGame( bool force, int* cardids );

        bool m_terminated;
        QSemaphore m_semaphore;

        Game* m_game;
        QGraphicsScene* m_scene;
        GameCanvas* m_view;
        QTableWidget* m_table;
        QSplitter* split;

        QLabel* lblCurGame;
        QLabel* lblDoubled;
        QPushButton* btnLastTrick;

        QAction* m_actStich;
        QAction* m_actEnd;

        QPointer<StichDlg> m_stichdlg;
};
