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
#include "selectgametypebox.h"

#include "card.h"
#include "cardlist.h"
#include "game.h"
#include "gameinfo.h"
#include "settings.h"

#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qapplication.h>
#include <qradiobutton.h>
#include <QButtonGroup>
#include <QGroupBox>
#include <qlayout.h>
#include <klocale.h>
#include <k3wizard.h>
#include <qpainter.h>
#include <qpixmap.h>

// DS: moved those from the header into the source file so that these messages are found during make messages-merge
#define HELP_GAMETYPE_SOLO i18n("When you play a \"Solo\", you have to additionally choose a color. Trumps are all \"Ober\", \"Unter\" and the color you chose. You will play alone against the other three players.")
#define HELP_GAMETYPE_DACHS i18n("When you play a \"Dachs\", the only trumps are aces and tenners. You will play alone against the other three players.")
#define HELP_GAMETYPE_RUFSPIEL i18n("When you play a callgame, you will have to choose a color. You will then play in team with the player which has the ace of the color you chose. Hearts are trumps.")
#define HELP_GAMETYPE_WENZ i18n("When you play a \"Wenz\", you can choose a color. If you don't choose a color, only all \"Unter\" will be trumps. If you do, the cards of the chosen color will also be trumps. You will play alone against the other three players.")
#define HELP_GAMETYPE_GEIER i18n("When you play a \"Geier\", you can choose a color. If you don't choose a color, only all \"Ober\" will be trumps. If you do, the cards of the chosen color will also be trumps. You will play alone against the other three players.")

SelectGameTypeBox::SelectGameTypeBox( QWidget *parent )
    : QWidget( parent )
{
    m_allowed = Settings::instance()->allowedGames();

    QWidget* row1 = new QWidget;

    preview = new QLabel;
    infoLabel = new QLabel;
    infoLabel->setMargin( 11 );
    infoLabel->setAlignment( Qt::AlignBottom | Qt::AlignLeft );
    infoLabel->setWordWrap( true );
    infoLabel->setMinimumHeight(100);

    checkRufspiel = new QRadioButton( i18n("&Callgame") );
    checkSolo = new QRadioButton( i18n("&Solo") );
    checkGeier = new QRadioButton( i18n("&Geier") );
    checkWenz = new QRadioButton( i18n("&Wenz") );
    checkDachs = new QRadioButton( i18n("&Dachs") );

    checkRufspiel->setChecked(TRUE);

    checkGeier->setEnabled( m_allowed->geier );
    checkWenz->setEnabled( m_allowed->wenz );
    checkDachs->setEnabled( m_allowed->dachs );

    QButtonGroup *myGroup = new QButtonGroup;
    myGroup->addButton( checkRufspiel );
    myGroup->addButton( checkSolo );
    myGroup->addButton( checkGeier );
    myGroup->addButton( checkWenz );
    myGroup->addButton( checkDachs );
    connect( myGroup, SIGNAL(buttonClicked(int)), this, SLOT(typeChanged()));

    QGroupBox* group_TypeSelect = new QGroupBox( i18n("Game") );
    QVBoxLayout *button_layout = new QVBoxLayout;
    button_layout->addWidget( checkRufspiel );
    button_layout->addWidget( checkSolo );
    button_layout->addWidget( checkGeier );
    button_layout->addWidget( checkWenz );
    button_layout->addWidget( checkDachs );
    group_TypeSelect->setLayout( button_layout );

    QVBoxLayout *row_layout = new QVBoxLayout;
    row_layout->addWidget( preview );
    row_layout->addWidget( infoLabel );
    row1->setLayout( row_layout );

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(8);
    layout->addWidget( row1 );
    layout->addWidget( group_TypeSelect );
    this->setLayout( layout );

    typeChanged();
    infoLabel->setMaximumWidth(preview->size().width());
}

SelectGameTypeBox::~SelectGameTypeBox()
{
    delete m_allowed;
}

void SelectGameTypeBox::typeChanged()
{
    GameInfo *info=gameInfo();
    updatePreview();
    switch(info->mode())
    {
    case (GameInfo::RUFSPIEL):
        setInfoText(HELP_GAMETYPE_RUFSPIEL);
    break;
    case (GameInfo::WENZ):
        setInfoText(HELP_GAMETYPE_WENZ);
    break;
    case (GameInfo::STICHT):
        setInfoText(HELP_GAMETYPE_SOLO);
    break;
    case (GameInfo::GEIER):
        setInfoText(HELP_GAMETYPE_GEIER);
    break;
    case (GameInfo::DACHS):
        setInfoText(HELP_GAMETYPE_DACHS);
    break;
    // DS: Fix a compiler warning
    default:
    break;
    }
    delete info;
}

void SelectGameTypeBox::setInfoText(QString Text)
{
    infoLabel->setText(Text);
}

void SelectGameTypeBox::updatePreview()
{
    int i = 0;
    int x=0, y = 0;
    CardList list;
    CardList trumpf;
    GameInfo* info = gameInfo();

    list.init();
    for(i=0;i<(int)list.count();i++)
        if( info->istTrumpf( list.at(i) ) )
            trumpf.append( list.at(i) );
    delete info;
    trumpf.sort((eval_func)info->evalCard, (void *)info);

    Card c( Card::SAU, Card::EICHEL );
    QPixmap pix(c.pixmap()->width()+(c.pixmap()->width()/2)*7,c.pixmap()->height()*(int(trumpf.count()/4)+(trumpf.count()%4)));
    x=c.pixmap()->width()/2*(trumpf.count()-1);
    pix.fill( Qt::darkGreen );
    QPainter p( &pix );
    for(i=0;i<=(int)trumpf.count()-1;i++)
    {
        QPixmap* pixmap = trumpf.at(i)->pixmap();
        p.drawPixmap( x, y, *pixmap );
        x -= pixmap->width()/2;

        int pos = trumpf.count() + i;
        if(!(pos+1)%4)
        {
            x = 0;
            y += pixmap->height();
        }
    }

    p.end();
    pix = pix.copy( 0, 0, pix.width(), y + ( x == 0 ? 0 : c.pixmap()->height() ) );
    preview->setPixmap( pix );
    preview->setMinimumWidth(pix.width()+10);
}


GameInfo* SelectGameTypeBox::gameInfo() const
{
    GameInfo* info = new GameInfo();

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
