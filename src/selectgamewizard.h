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
#ifndef SELECTGAMEWIZARD_H
#define SELECTGAMEWIZARD_H

#include <kwizard.h>
class QWidget;
class QHBox;
class QLineEdit;
class QLabel;
class QRadioButton;
class SelectGameTypeBox;
class SelectGameColorBox;
class CardList;
class GameInfo;

/**
@author Christian Kern
*/
class SelectGameWizard : public KWizard
{
	Q_OBJECT

public:

	SelectGameWizard(bool force, CardList* list, QWidget *parent = 0, const char *name = 0 );
	~SelectGameWizard();
	 void showPage(QWidget* page);
	 CardList* getCardList();
	GameInfo* gameInfo();
	void canFinish(bool fin);
	
    void reject();
    
protected:
	CardList* m_list;
	bool m_closing;
    bool m_force;
	SelectGameTypeBox *box1;
	SelectGameColorBox *box2;


protected:


};

#endif
