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
#include "selectgamecolorbox.h"

#include "card.h"
#include "cardlist.h"
#include "game.h"
#include "gameinfo.h"
#include "settings.h"

#include <qwidget.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qapplication.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <klocale.h>
#include <kwizard.h>
#include <qpainter.h>
#include <qpixmap.h>
#include "card.h"
#include "selectgamewizard.h"

SelectGameColorBox::SelectGameColorBox( QWidget *parent, const char *name ):QHBox(parent, name, 0)
{
    m_allowed = Settings::instance()->allowedGames();
    
	m_gameinfo=0;
	setSpacing(8);
	page = new QVBox( this );
	//row1 = new QVBox( page );

	this->parent=(SelectGameWizard*)parent;
	color_group = new QButtonGroup( 1, Qt::Vertical, i18n("Color:"), page );
	checkEichel = new QRadioButton( i18n("&Eichel"), color_group );
	checkGras = new QRadioButton( i18n("&Gras"), color_group );
	checkHerz = new QRadioButton( i18n("&Herz"), color_group );
	checkSchellen = new QRadioButton( i18n("&Schellen"), color_group );
	checkFarblos = new QRadioButton( i18n("&Farblos"), color_group );

	m_Herz=new Card(Card::SAU, Card::HERZ);
	m_Schelle=new Card(Card::SAU, Card::SCHELLEN);
	m_Eichel=new Card(Card::SAU, Card::EICHEL);
	m_Gras=new Card(Card::SAU, Card::GRAS);
	
	
	connect( color_group, SIGNAL(clicked(int)), this, SLOT(colorChanged()));

	checkHerz->setPixmap(*m_Herz->pixmap());
	checkGras->setPixmap(*m_Gras->pixmap());
	checkEichel->setPixmap(*m_Eichel->pixmap());
	checkSchellen->setPixmap(*m_Schelle->pixmap());

}


SelectGameColorBox::~SelectGameColorBox()
{
	delete m_Herz;
	delete m_Schelle;
	delete m_Eichel;
	delete m_Gras;
    delete m_allowed;
}

int SelectGameColorBox::getColor()
{
	if(checkEichel->isChecked()) return (Card::EICHEL);
	else if(checkGras->isChecked()) return (Card::GRAS);
	else if(checkHerz->isChecked()) return (Card::HERZ);
	else if(checkSchellen->isChecked()) return (Card::SCHELLEN);
	return (Card::NOCOLOR);
}


void SelectGameColorBox::colorChanged()
{
	m_gameinfo->setColor(getColor());
}


void SelectGameColorBox::statusChanged()
{
	if(m_gameinfo!=0)
	{
		switch(m_gameinfo->mode())
		{
				case (GameInfo::RUFSPIEL):
							setStatus(
							    GameInfo::isAllowed( parent->getCardList(),GameInfo::RUFSPIEL, Card::EICHEL ),
							    GameInfo::isAllowed( parent->getCardList(),GameInfo::RUFSPIEL, Card::GRAS ),
							    false,
							    GameInfo::isAllowed( parent->getCardList(),GameInfo::RUFSPIEL, Card::SCHELLEN ) ,
							    false
							);
				break;
				case (GameInfo::WENZ):
							setStatus(
							    true,
							    true,
							    true,
							    true,
							    true
							);
				break;
				case (GameInfo::STICHT):
							setStatus(
							    true,
							    true,
							    true,
							    true,
							    false
							);
				break;
				case (GameInfo::GEIER):
							setStatus(
							    true,
							    true,
							    true,
							    true,
							    true
							);
				break;
				case (GameInfo::DACHS):
							setStatus(
							    false,
							    false,
							    false,
							    false,
							    true
							);
				break;
		}
	}
	checkFirstVisible();
	colorChanged();

}



void SelectGameColorBox::checkFirstVisible()
{
	if(!checkEichel->isHidden()) checkEichel->setChecked(true);
	else if(!checkHerz->isHidden()) checkHerz->setChecked(true);
	else if(!checkGras->isHidden()) checkGras->setChecked(true);
	else if(!checkSchellen->isHidden()) checkSchellen->setChecked(true);
	else if(!checkFarblos->isHidden()) checkFarblos->setChecked(true);
}

void SelectGameColorBox::setStatus(bool Eichel, bool Gras, bool Herz, bool Schellen, bool Farblos)
{
    bool allow_colors = true;
    
    if( m_gameinfo->mode() == GameInfo::WENZ )
        allow_colors = allow_colors & m_allowed->farb_wenz;
        
    if( m_gameinfo->mode() == GameInfo::GEIER )
        allow_colors = allow_colors & m_allowed->farb_geier;
    
	if(allow_colors&&Eichel==true) checkEichel->show();
	else checkEichel->hide();
	if(allow_colors&&Gras==true) checkGras->show();
	else checkGras->hide();
	if(allow_colors&&Herz==true) checkHerz->show();
	else checkHerz->hide();
	if(allow_colors&&Schellen==true) checkSchellen->show();
	else checkSchellen->hide();
	if(Farblos==true) checkFarblos->show();
	else checkFarblos->hide();

	if(!Eichel && !Gras && !Herz && !Schellen && !Farblos) m_finish=false;
	else m_finish=true;
}

bool SelectGameColorBox::getFinish()
{
	return m_finish;
}

void SelectGameColorBox::setGameInfo(GameInfo* info)
{

	m_gameinfo=info;
	statusChanged();
}

GameInfo* SelectGameColorBox::getGameInfo()
{
	return m_gameinfo;
}

void SelectGameColorBox::cleanGameInfo()
{
	if(m_gameinfo!=0)
	{
		delete m_gameinfo;
		m_gameinfo=0;
	}


}


