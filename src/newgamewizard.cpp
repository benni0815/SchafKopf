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
#include "newgamewizard.h"

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

newgamewizard::newgamewizard( QWidget *parent, const char *name )
        : KWizard( parent, name, TRUE )
{
    
    setup_page_selectgame();
    //this->setSize(300,300,300,300);
}

void newgamewizard::setup_page_selectgame()
{
    page_selectgame = new QHBox( this );
    page_selectgame->setSpacing(8);

    info = new QLabel( page_selectgame );
    info->setMargin( 11 );
  //  info->setPalette( yellow );
    info->setMinimumWidth( 220);

    QVBox *page = new QVBox( page_selectgame );

    QHBox *row1 = new QHBox( page );

    QButtonGroup* group_TypeSelect  = new QButtonGroup( 1, Qt::Horizontal, i18n("Select Type:"), row1 );
    checkLocalGame = new QRadioButton( i18n("&Start local Game"), group_TypeSelect );
    checkNetworkServer = new QRadioButton( i18n("Start network Game as &Server"), group_TypeSelect );
    checkNetworkClient = new QRadioButton( i18n("Start network Game as &Client"), group_TypeSelect );
    checkLocalGame->setChecked(TRUE);

    connect( group_TypeSelect, SIGNAL(clicked(int)), this, SLOT(dataChanged()));

    addPage( page_selectgame, "Select Game Type" );

    setNextEnabled( page_selectgame, FALSE );
    setHelpEnabled( page_selectgame, FALSE );
    dataChanged();
}


void newgamewizard::showPage( QWidget* page )
{
    QWizard::showPage(page);
}

int newgamewizard::getGame()
{
    if(checkLocalGame->isChecked())
        return GAME_LOCAL;
    else
    {
        if(checkNetworkServer->isChecked())
            return GAME_NETWORK_SERVER;
        else
        {
            if(checkNetworkClient->isChecked())
                return GAME_NETWORK_CLIENT;
            else
                return -1;
        }
    }

}

QString newgamewizard::getGameInfoString(int gameType)
{
    switch(gameType)
    {
    case GAME_LOCAL:
        return
            i18n("Start a local Game:\nYou will play against\n3 Computer Players");
        break;
    case GAME_NETWORK_SERVER:
        return
            i18n("Start a Game Server\nYou will be the host\nof a new Network game");
        break;
    case GAME_NETWORK_CLIENT:
        return
            i18n("Connect to a Game Server\nYou will connect to a Computer,\nrunning a Schafkopf server");
        break;
    }
}

void newgamewizard::setInfoText(int gameType)
{
    info->setText(getGameInfoString(gameType));

}

void newgamewizard::dataChanged(  )
{
    int game=getGame();
    setInfoText(game);
    switch(game)
    {
    case GAME_LOCAL:
        setFinishEnabled( page_selectgame, TRUE );
        break;
    case GAME_NETWORK_SERVER:
        setFinishEnabled( page_selectgame, FALSE );
        nextButton()->setEnabled(FALSE);
        break;
    case GAME_NETWORK_CLIENT:
        setFinishEnabled( page_selectgame, FALSE );
        nextButton()->setEnabled(FALSE);
        break;
    }
}
