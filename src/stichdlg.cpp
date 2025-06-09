// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "stichdlg.h"

#include "card.h"
#include "cardlist.h"

#include <KLocalizedString>

#include <QLabel>
#include <QLayout>
#include <QGridLayout>
#include <QPushButton>


StichDlg::StichDlg(QWidget *parent )
    : QDialog( parent )
{
    setWindowTitle(i18n("Last Trick"));
    QGridLayout* layout = new QGridLayout( this );
    trick = new QLabel();
    layout->addWidget( trick, 0, 0, 1, 4);

    for( unsigned int i = 0; i < PLAYERS; i++ )
    {
        cards[i] = new QLabel();
        layout->addWidget( cards[i], 1, i );

        players[i] = new QLabel();
        layout->addWidget( players[i], 2, i );
    }

    QPushButton* close = new QPushButton(i18n("&Close"));
    layout->addWidget( close, 3, 0, 1, 4, Qt::AlignRight );
    connect(close, &QPushButton::clicked, this, &StichDlg::close);

    reset();
}

StichDlg::~StichDlg()
{
}

void StichDlg::changed( const QString & name, int* list, QStringList* playerlist )
{
    CardList stich( list );
    stich.setAutoDelete( true );

    if( stich.count() )
        trick->setText(i18n("Trick was made by: <b>") + name + "</b>");
    else
        trick->setText(i18n("No trick was made."));

    for( int i = 0; i < PLAYERS; i++ )
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
            players[i]->setText( QString() );
        }
    }
}

void StichDlg::reset()
{
    changed( 0, NULL, NULL );
}
