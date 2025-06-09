// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "selectgamewizard.h"

#include "selectgametypebox.h"
#include "selectgamecolorbox.h"
#include "cardlist.h"

#include <QAbstractButton>


SelectGameWizard::SelectGameWizard( bool force, CardList* list, QWidget *parent )
    : QWizard( parent )
{
    m_force = force;
    m_list=list;
    m_closing=false;
    box1 = new SelectGameTypeBox( this );
    box2 = new SelectGameColorBox( this );
    addPage( box1 );
    addPage( box2 );
    button( QWizard::CancelButton )->setEnabled( !m_force );
    connect(this, &SelectGameWizard::currentIdChanged, this, &SelectGameWizard::pageChanged);
}

CardList* SelectGameWizard::getCardList()
{
    return m_list;
}

GameInfo* SelectGameWizard::gameInfo()
{
    if(box2)
       return(box2->getGameInfo());
    return NULL;
}

void SelectGameWizard::pageChanged( int id )
{
    if( id == 1 && !m_closing )
    {
        box2->cleanGameInfo();
        box2->setGameInfo( box1->gameInfo() );
        button( QWizard::FinishButton )->setEnabled( box2->getFinish() );
    }
}

SelectGameWizard::~SelectGameWizard()
{
    m_closing = true;
}

void SelectGameWizard::reject()
{
    // only allow reject if player is not forced to play something
    if( !m_force )
        QWizard::reject();
}
