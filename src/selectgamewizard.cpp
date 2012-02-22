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
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qapplication.h>
#include <qradiobutton.h>
#include <klocale.h>
#include "selectgametypebox.h"
#include "selectgamecolorbox.h"
#include "cardlist.h"
#include "card.h"

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
    connect( this, SIGNAL( currentIdChanged( int ) ), this, SLOT( pageChanged( int ) ) );
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
