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
#include "cardlist.h"
#include "game.h"
#include "gameinfo.h"

#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qradiobutton.h>

#include <klocale.h>


SelectGameDlg::SelectGameDlg(CardList* list,QWidget *parent, const char *name)
    : KDialogBase( KDialogBase::Plain, i18n("Select Game"),
      KDialogBase::Ok, KDialogBase::Ok, parent,name, true), m_list( list )
{
    QHBoxLayout* layout = new QHBoxLayout( plainPage() );    
    
    QButtonGroup* group1 = new QButtonGroup( 1, Qt::Horizontal, i18n("Game:"), plainPage() );
    checkRufspiel = new QRadioButton( i18n("&Rufspiel"), group1 );
    checkSolo = new QRadioButton( i18n("&Solo"), group1 );
    checkGeier = new QRadioButton( i18n("&Geier"), group1 );
    checkWenz = new QRadioButton( i18n("&Wenz"), group1 );
    checkDachs = new QRadioButton( i18n("&Dachs"), group1 );

    QButtonGroup* group2 = new QButtonGroup( 1, Qt::Horizontal, i18n("Color:"), plainPage() );
    checkEichel = new QRadioButton( i18n("&Eichel"), group2 );
    checkGras = new QRadioButton( i18n("&Gras"), group2 );
    checkHerz = new QRadioButton( i18n("&Herz"), group2 );
    checkSchellen = new QRadioButton( i18n("&Schellen"), group2 );
    checkFarblos = new QRadioButton( i18n("&Farblos"), group2 );
    
    preview = new QLabel( plainPage() );
    
    connect( group1, SIGNAL(clicked(int)), this, SLOT(enableControls()));
    connect( group2, SIGNAL(clicked(int)), this, SLOT(enableControls()));
    
    layout->addWidget( group1 );
    layout->addWidget( group2 );
    layout->addWidget( preview );
    
    checkSolo->setChecked( true );
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
        info->setColor(Card::NOCOLOR);
    else if( checkEichel->isChecked() )
        info->setColor(Card::EICHEL);
    else if( checkGras->isChecked() )
        info->setColor(Card::GRAS);
    else if( checkHerz->isChecked() )
        info->setColor(Card::HERZ);
    else if( checkSchellen->isChecked() )
        info->setColor(Card::SCHELLEN);
        
    if( checkRufspiel->isChecked() )
        info->setMode(GameInfo::RUFSPIEL);
    else if( checkSolo->isChecked() )
        info->setMode(GameInfo::STICHT);
    else if( checkGeier->isChecked() )
        info->setMode(GameInfo::GEIER);
    else if( checkWenz->isChecked() )
        info->setMode(GameInfo::WENZ);
    else if( checkDachs->isChecked() )
        info->setMode(GameInfo::DACHS);
        
    return info;
}


void SelectGameDlg::enableControls()
{
    checkHerz->setEnabled( !checkRufspiel->isChecked() );
    checkFarblos->setEnabled( !checkRufspiel->isChecked() && !checkSolo->isChecked() );

    if( checkRufspiel->isChecked() )
    {
        checkEichel->setEnabled( GameInfo::isAllowed( m_list, GameInfo::RUFSPIEL, Card::EICHEL ) );
        checkGras->setEnabled( GameInfo::isAllowed( m_list, GameInfo::RUFSPIEL, Card::GRAS ) );
        checkSchellen->setEnabled( GameInfo::isAllowed( m_list, GameInfo::RUFSPIEL, Card::SCHELLEN ) );
        
        if( !checkEichel->isEnabled() && !checkGras->isEnabled() && !checkSchellen->isEnabled() )
            checkRufspiel->setEnabled( false );
        else
            checkRufspiel->setEnabled( true );
    }
    else if( checkDachs->isChecked() )
    {
        checkHerz->setEnabled( false );
        checkGras->setEnabled( false );
        checkEichel->setEnabled( false );
        checkSchellen->setEnabled( false );
        checkFarblos->setEnabled( true );
    }
    else
    {
        checkEichel->setEnabled( true );
        checkGras->setEnabled( true );
        checkSchellen->setEnabled( true );
    }
    
    updatePreview();
}

void SelectGameDlg::updatePreview()
{
    int i = 0;
    int x = 0, y = 0;
    CardList list;
    CardList trumpf;
    GameInfo* info = gameInfo();
    
    list.init();
    for(i=0;i<list.count();i++)
        if( info->istTrumpf( list.at(i) ) )
            trumpf.append( list.at(i) );
    delete info;
    trumpf.sort((eval_func)info->evalCard, (void *)info);
    
    Card c( Card::SAU, Card::EICHEL );
    QPixmap pix(c.pixmap()->width()*4,c.pixmap()->height()*(int(trumpf.count()/4)+(trumpf.count()%4)));
    pix.fill( Qt::darkGreen );
    QPainter p( &pix );
    for(i=trumpf.count()-1;i>=0;i--)
    {
        QPixmap* pixmap = trumpf.at(i)->pixmap();
        p.drawPixmap( x, y, *pixmap );
        x += pixmap->width();
        
        int pos = trumpf.count() - i;
        if(!(pos+1)%4)
        {
            x = 0;
            y += pixmap->height();
        }
    }
        
    p.end();
    pix.resize( pix.width(), y + ( x == 0 ? 0 : c.pixmap()->height() ) );
    preview->setPixmap( pix );
    
}

#include "selectgamedlg.moc"
