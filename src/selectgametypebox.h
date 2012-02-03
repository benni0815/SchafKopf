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

#include <q3hbox.h>
//Added by qt3to4:
#include <QLabel>

class QWidget;
class Q3HBox;
class Q3VBox;
class QLineEdit;
class QLabel;
class QRadioButton;
class Q3ButtonGroup;
class GameInfo;

struct t_AllowedGames;

/**
@author Christian Kern
*/
class SelectGameTypeBox : public Q3HBox
{
	Q_OBJECT
public:
	SelectGameTypeBox(QWidget *parent = 0, const char *name = 0);

	~SelectGameTypeBox();
	GameInfo* gameInfo() const;
    
private:
    t_AllowedGames* m_allowed;
    
protected:
	QRadioButton* checkRufspiel;
	QRadioButton* checkSolo;
	QRadioButton* checkGeier;
	QRadioButton* checkWenz;
	QRadioButton* checkDachs;
	Q3HBox *page;
	Q3VBox *row1;
	Q3ButtonGroup* group_TypeSelect;
	QLabel* preview;
	QLabel* infoLabel;
	
	void setInfoText(QString Text);
protected slots:
	void updatePreview();
	void typeChanged();

};

#endif
