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
#include "selectgamewizard.h"



#include <qwidget.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qapplication.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <klocale.h>
#include <kwizard.h>
#include "selectgametypebox.h"
#include "selectgamecolorbox.h"
#include "cardlist.h"
#include "card.h"

SelectGameWizard::SelectGameWizard(CardList* list, QWidget *parent, const char *name )
		: KWizard( parent, name, TRUE )
{
	m_list=list;
	m_closing=false;
	box1 = new SelectGameTypeBox(this, 0);
	box2 = new SelectGameColorBox(this, 0);
	addPage( box1, "Step 1/2: Select Game Type" );
	addPage( box2, "Step 2/2: Select Game Color" );

	setNextEnabled( box1, TRUE );
	setFinishEnabled(box2, TRUE);
	setHelpEnabled( box1, FALSE );
	setHelpEnabled(box2, FALSE);
}

CardList* SelectGameWizard::getCardList()
{
return m_list;
}

GameInfo* SelectGameWizard::gameInfo()
{
if(box2)
	return(box2->getGameInfo());
}

void SelectGameWizard::showPage( QWidget* page )
{

	if(page==box2&&!m_closing)
	{
		box2->cleanGameInfo();
		box2->setGameInfo(box1->gameInfo());
		setFinishEnabled(box2, box2->getFinish());
	}
	KWizard::showPage(page);
	
}

void SelectGameWizard::canFinish(bool fin)
{
	if(box2) setFinishEnabled(box2,fin);
}

SelectGameWizard::~SelectGameWizard()
{
	m_closing=true;
}
