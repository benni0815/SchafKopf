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

#ifndef _SCHAFKOPF_H_
#define _SCHAFKOPF_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>

class QCanvas;
class GameCanvas;
class Game;
class KAction;
/**
 * @short Application Main Window
 * @author Dominik Seichter <domseichter@web.de>
 * @version 0.1
 */
class SchafKopf : public KMainWindow
{
    Q_OBJECT
    public:
        SchafKopf();
        ~SchafKopf();
        
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
        
    private:
        void setupActions();
        
        Game* m_game;
        GameCanvas* m_canvasview;
        QCanvas* m_canvas;    
        
        KAction* m_actEnd;
};

#endif // _SCHAFKOPF_H_
