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
#include "stichdlg.h"

#include "card.h"
#include "cardlist.h"
#include "player.h"

#include <qlabel.h>
#include <qlayout.h>

#include <klocale.h>

StichDlg::StichDlg(Game* g,QWidget *parent, const char *name)
    : KDialogBase( KDialogBase::Plain, i18n("&Last Stich"),
      KDialogBase::Close, KDialogBase::Close, parent,name, false),
      m_game( g)
{
    QGridLayout* layout = new QGridLayout( plainPage(), 4, 2 );
    for( unsigned int i = 0; i < PLAYERS; i++ ) {
        cards[i] = new QLabel( plainPage() );
        layout->addWidget( cards[i], 0, i );
        
        players[i] = new QLabel( plainPage() );
        layout->addWidget( players[i], 1, i );
    }
    
    changed();
    
    // close if there is no active game
    connect( m_game, SIGNAL( destroyed() ), this, SLOT( deleteLater() ));
    connect( m_game, SIGNAL( playerMadeStich(unsigned int)), this, SLOT(changed()));
}


StichDlg::~StichDlg()
{
}

void StichDlg::changed()
{
    Player* player = m_game->findIndex( 0 );
    CardList* stich = player->stiche();
    for( unsigned int i = 0; i < PLAYERS; i++ )
    {
        if( stich->count() ) 
            cards[i]->setPixmap( *(stich->at( stich->count() - PLAYERS + i )->pixmap()) );
        else
            cards[i]->setPixmap( *Card::backgroundPixmap() );

        players[i]->setText( "<qt><b>"+m_game->findIndex( i )->name()+"</b></qt>" );
    }
}

#include "stichdlg.moc"
