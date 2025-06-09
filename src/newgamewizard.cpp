// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "newgamewizard.h"

#include <KLocalizedString>

#include <QLabel>
#include <QRadioButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QButtonGroup>


newgamewizard::newgamewizard( QWidget *parent )
        : QWizard( parent )
{
    setWindowTitle(i18n("Start New Game"));
    setup_page_selectgame();
    //this->setSize(300,300,300,300);
}

void newgamewizard::setup_page_selectgame()
{
    page_selectgame = new QWizardPage;
    page_selectgame->setTitle(i18n("Select Game Type"));

    info = new QLabel;
    info->setMargin( 11 );
    info->setMinimumWidth( 220);

    checkLocalGame = new QRadioButton(i18n("&Start local Game"));
    checkNetworkServer = new QRadioButton(i18n("Start network Game as &Server"));
    checkNetworkClient = new QRadioButton(i18n("Start network Game as &Client"));

    checkLocalGame->setChecked(true);

    QGroupBox *group_TypeSelect = new QGroupBox;
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget( checkLocalGame );
    vbox->addWidget( checkNetworkServer );
    vbox->addWidget( checkNetworkClient );
    group_TypeSelect->setLayout( vbox );

    QButtonGroup *myGroup = new QButtonGroup;
    myGroup->addButton( checkLocalGame );
    myGroup->addButton( checkNetworkServer );
    myGroup->addButton( checkNetworkClient );
    connect(myGroup, &QButtonGroup::buttonClicked, this, &newgamewizard::dataChanged);

    //page_selectgame->setMinimumSize(QSize(400,200)) ;

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(8);
    layout->addWidget( group_TypeSelect );
    layout->addWidget( info );
    page_selectgame->setLayout( layout );
    addPage( page_selectgame );

    button( QWizard::NextButton )->setEnabled( false );
    button( QWizard::HelpButton )->setEnabled( false );
    dataChanged();
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
      default:
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
        button( QWizard::FinishButton )->setEnabled( true );
        break;
    case GAME_NETWORK_SERVER:
        button( QWizard::FinishButton )->setEnabled( false );
        button( QWizard::NextButton )->setEnabled( false );
        break;
    case GAME_NETWORK_CLIENT:
        button( QWizard::FinishButton )->setEnabled( false );
        button( QWizard::NextButton )->setEnabled( false );
        break;
    }
}
