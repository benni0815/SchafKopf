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

StichDlg::StichDlg(QWidget *parent, const char *name)
    : KDialogBase( KDialogBase::Plain, i18n("Last Trick"),
      KDialogBase::Close, KDialogBase::Close, parent,name, false)
{
    QGridLayout* layout = new QGridLayout( plainPage(), 4, 2 );
    trick = new QLabel( plainPage() );
    layout->addMultiCellWidget( trick, 0, 0, 0, 3);
    
    for( unsigned int i = 0; i < PLAYERS; i++ ) {
        cards[i] = new QLabel( plainPage() );
        layout->addWidget( cards[i], 1, i );
        
        players[i] = new QLabel( plainPage() );
        layout->addWidget( players[i], 2, i );
    }
    
    reset();
}


StichDlg::~StichDlg()
{
}

void StichDlg::changed( const QString & name, int* list, QStringList* playerlist )
{
    CardList stich( list );
    
    if( stich.count() )
        trick->setText( i18n("Trick was made by: <b>") + name + "</b>" );
    else
        trick->setText( i18n("No trick was made.") );
        
    for( unsigned int i = 0; i < PLAYERS; i++ )
    {
        if( stich.count() > i ) 
        {
	    Card* c = stich.at( stich.count() - PLAYERS + i );
	    if( c )
		cards[i]->setPixmap( *(c->pixmap()) );
            players[i]->setText( "<qt><b>"+(*playerlist)[i]+"</b></qt>" );
        }   
        else
        {
            cards[i]->setPixmap( *Card::backgroundPixmap() );
            players[i]->setText( QString::null );
        }
    }
}

void StichDlg::reset()
{
    changed( 0, NULL, NULL );
}

#include "stichdlg.moc"
