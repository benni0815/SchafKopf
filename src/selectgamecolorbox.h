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
#ifndef SELECTGAMECOLORBOX_H
#define SELECTGAMECOLORBOX_H


#include <qhbox.h>

class QWidget;
class QHBox;
class QVBox;
class QLineEdit;
class QLabel;
class QRadioButton;
class QButtonGroup;
class GameInfo;
class Card;
class SelectGameWizard;
class GameInfo;

/**
@author Christian Kern
*/
class SelectGameColorBox : public QHBox
{
	Q_OBJECT
public:
	SelectGameColorBox(QWidget *parent = 0, const char *name = 0);
	void cleanGameInfo();
	
	~SelectGameColorBox();
	void setGameInfo(GameInfo* info);
	GameInfo* getGameInfo();
	bool getFinish();
protected:
	GameInfo* m_gameinfo;
	QButtonGroup* color_group;
	QRadioButton* checkFarblos;
        QRadioButton* checkEichel;
        QRadioButton* checkGras;
        QRadioButton* checkHerz;
        QRadioButton* checkSchellen;
	QHBox* page;
	QVBox* row1;
	Card* m_Herz;
	Card* m_Schelle;
	Card* m_Eichel;
	Card* m_Gras;
	SelectGameWizard* parent;
	void setStatus(bool Eichel, bool Gras, bool Herz, bool Schellen, bool Farblos);
	void checkFirstVisible();
	bool m_finish;
	void statusChanged();
	int getColor();
protected slots:
	void colorChanged();


};

#endif
