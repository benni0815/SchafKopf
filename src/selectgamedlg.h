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
#ifndef SELECTGAMEDLG_H
#define SELECTGAMEDLG_H

#include <kdialogbase.h>

class CardList;
class GameInfo;
class QLabel;
class QRadioButton;
/**
Allows the user to select a game he wants to play.

@author Dominik Seichter
*/

class SelectGameDlg : public KDialogBase
{
    Q_OBJECT
    public:
        SelectGameDlg(CardList* list,QWidget *parent = 0, const char *name = 0);
        ~SelectGameDlg();

        GameInfo* gameInfo() const;
        
    private slots:
        void enableControls();
        void updatePreview();
        
    private:
        CardList* m_list;
        
        QRadioButton* checkRufspiel;
        QRadioButton* checkSolo;
        QRadioButton* checkGeier;
        QRadioButton* checkWenz;
        QRadioButton* checkDachs;
        
        QRadioButton* checkFarblos;
        QRadioButton* checkEichel;
        QRadioButton* checkGras;
        QRadioButton* checkHerz;
        QRadioButton* checkSchellen;
        
        QLabel* preview;
};

#endif
