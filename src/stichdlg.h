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
#ifndef STICHDLG_H
#define STICHDLG_H

#include <kdialogbase.h>

#include "game.h"

class QLabel;
/**
Show the last trick which was made

@author Dominik Seichter
*/

class StichDlg : public KDialogBase
{
    Q_OBJECT
    public:
        StichDlg(Game* g, QWidget *parent = 0, const char *name = 0);
        ~StichDlg();
        
    private slots:
        void changed(unsigned int id);
        
    private:
        QLabel* cards[PLAYERS];
        QLabel* players[PLAYERS];
            
        Game* m_game;
};

#endif
