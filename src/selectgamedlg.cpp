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
#include "selectgamedlg.h"

#include "card.h"
#include "gameinfo.h"

#include <qbuttongroup.h>
#include <qlayout.h>
#include <qradiobutton.h>

#include <klocale.h>


SelectGameDlg::SelectGameDlg(QWidget *parent, const char *name)
    : KDialogBase( KDialogBase::Plain, i18n("Select Game"),
      KDialogBase::Ok, KDialogBase::Ok, parent,name, true)
{
    QHBoxLayout* layout = new QHBoxLayout( plainPage() );    
    
    QButtonGroup* group1 = new QButtonGroup( 1, Qt::Horizontal, i18n("Game:"), plainPage() );
    checkRufspiel = new QRadioButton( i18n("&Rufspiel"), group1 );
    checkSolo = new QRadioButton( i18n("&Solo"), group1 );
    checkGeier = new QRadioButton( i18n("&Geier"), group1 );
    checkWenz = new QRadioButton( i18n("&Wenz"), group1 );

    QButtonGroup* group2 = new QButtonGroup( 1, Qt::Horizontal, i18n("Color:"), plainPage() );
    checkFarblos = new QRadioButton( i18n("&Farblos"), group2 );
    checkEichel = new QRadioButton( i18n("&Eichel"), group2 );
    checkGras = new QRadioButton( i18n("&Gras"), group2 );
    checkHerz = new QRadioButton( i18n("&Herz"), group2 );
    checkSchellen = new QRadioButton( i18n("&Schellen"), group2 );
    
    connect( group1, SIGNAL(clicked(int)), this, SLOT(enableControls()));
    connect( group2, SIGNAL(clicked(int)), this, SLOT(enableControls()));
    
    layout->addWidget( group1 );
    layout->addWidget( group2 );
    
    checkRufspiel->setChecked( true );
    checkEichel->setChecked( true );
    
    enableControls();
}


SelectGameDlg::~SelectGameDlg()
{
}

GameInfo* SelectGameDlg::gameInfo() const
{
    GameInfo* info = new GameInfo();

    if( checkFarblos->isChecked() )        
        info->color=Card::NOCOLOR;
    else if( checkEichel->isChecked() )
        info->color=Card::EICHEL;
    else if( checkGras->isChecked() )
        info->color=Card::GRAS;
    else if( checkHerz->isChecked() )
        info->color=Card::HERZ;
    else if( checkSchellen->isChecked() )
        info->color=Card::SCHELLEN;
        
    if( checkRufspiel->isChecked() )
        info->mode=GameInfo::RUFSPIEL;
    else if( checkSolo->isChecked() )
        info->mode=GameInfo::STICHT;
    else if( checkGeier->isChecked() )
        info->mode=GameInfo::GEIER;
    else if( checkWenz->isChecked() )
        info->mode=GameInfo::WENZ;

    return info;
}


void SelectGameDlg::enableControls()
{
    checkHerz->setEnabled( !checkRufspiel->isChecked() );
    checkFarblos->setEnabled( !checkRufspiel->isChecked() && !checkSolo->isChecked() );
}

#include "selectgamedlg.moc"
