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
#ifndef SELECTGAMETYPEBOX_H
#define SELECTGAMETYPEBOX_H

#define HELP_GAMETYPE_SOLO i18n("When you Play a Solo, you have additionally to choose a color. Trumpf are all Ober, Unter, and the Color you Choose. You will play alone against the other 3 Players!")
#define HELP_GAMETYPE_DACHS i18n("When you Play a Dachs, you have to choose a color. Trumpf are all Ass and 10 cards, and cards from the color you choose. You will play alone against the other 3 Players!")
#define HELP_GAMETYPE_RUFSPIEL i18n("When you Play a Callgame, you will have to choose a color. You will then play in team with the player which has the Ass of the color you choosed.")
#define HELP_GAMETYPE_WENZ i18n("When you Play a Wenz, you can choose a color. If you don't choose a color, all Unter will be Trumpf. If you do, the cards of the choosen color will also be Trumpf.")
#define HELP_GAMETYPE_GEIER i18n("When you Play a Geier, you can choose a color. If you don't choose a color, all Ober will be Trumpf. If you do, the cards of the choosen color will also be Trumpf.")

#include <qhbox.h>

class QWidget;
class QHBox;
class QVBox;
class QLineEdit;
class QLabel;
class QRadioButton;
class QButtonGroup;
class GameInfo;

/**
@author Christian Kern
*/
class SelectGameTypeBox : public QHBox
{
	Q_OBJECT
public:
	SelectGameTypeBox(QWidget *parent = 0, const char *name = 0);

	~SelectGameTypeBox();
	GameInfo* gameInfo() const;
protected:
	QRadioButton* checkRufspiel;
	QRadioButton* checkSolo;
	QRadioButton* checkGeier;
	QRadioButton* checkWenz;
	QRadioButton* checkDachs;
	QHBox *page;
	QVBox *row1;
	QButtonGroup* group_TypeSelect;
	QLabel* preview;
	QLabel* infoLabel;
	
	void setInfoText(QString Text);
protected slots:
	void updatePreview();
	void typeChanged();

};

#endif
