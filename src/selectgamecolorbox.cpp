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
#include "selectgamecolorbox.h"

#include "card.h"
#include "cardlist.h"
#include "gameinfo.h"
#include "settings.h"
#include "card.h"
#include "selectgamewizard.h"

#include <KLocalizedString>

#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QGroupBox>


SelectGameColorBox::SelectGameColorBox( QWidget *parent )
    : QWizardPage( parent )
{
    setTitle(i18n("Step 2/2: Select Color"));

    m_gameinfo=0;

    this->parent=(SelectGameWizard*)parent;

    labelEichel = new QPushButton( this );
    labelGras = new QPushButton( this );
    labelHerz = new QPushButton( this );
    labelSchellen = new QPushButton( this );

    labelEichel->setFlat(true);
    labelGras->setFlat(true);
    labelHerz->setFlat(true);
    labelSchellen->setFlat(true);
    connect(labelEichel, &QPushButton::clicked, this, &SelectGameColorBox::colorChangedToEichel);
    connect(labelGras, &QPushButton::clicked, this, &SelectGameColorBox::colorChangedToGras);
    connect(labelHerz, &QPushButton::clicked, this, &SelectGameColorBox::colorChangedToHerz);
    connect(labelSchellen, &QPushButton::clicked, this, &SelectGameColorBox::colorChangedToSchellen);


    checkEichel = new QRadioButton(i18n("&Eichel"), this);
    checkGras = new QRadioButton(i18n("&Gras"), this);
    checkHerz = new QRadioButton(i18n("&Herz"), this);
    checkSchellen = new QRadioButton(i18n("&Schellen"), this);
    checkFarblos = new QRadioButton(i18n("&Farblos"), this);

    Card* herz=new Card(Card::SAU, Card::HERZ);
    Card* schelle=new Card(Card::SAU, Card::SCHELLEN);
    Card* eichel=new Card(Card::SAU, Card::EICHEL);
    Card* gras=new Card(Card::SAU, Card::GRAS);

    QButtonGroup *myGroup = new QButtonGroup(this);
    myGroup->addButton( checkEichel );
    myGroup->addButton( checkGras );
    myGroup->addButton( checkHerz );
    myGroup->addButton( checkSchellen );
    myGroup->addButton( checkFarblos );
    connect(myGroup, &QButtonGroup::buttonClicked, this, &SelectGameColorBox::colorChanged);

    labelHerz->setIcon( ( *herz->pixmap() ) );
    labelGras->setIcon( ( *gras->pixmap() ) );
    labelEichel->setIcon( ( *eichel->pixmap() ) );
    labelSchellen->setIcon( ( *schelle->pixmap() ) );
    labelHerz->setIconSize( herz->pixmap()->size() );
    labelGras->setIconSize( gras->pixmap()->size() );
    labelEichel->setIconSize( eichel->pixmap()->size() );
    labelSchellen->setIconSize( schelle->pixmap()->size() );

    QGroupBox* color_group = new QGroupBox(i18n("Color:"), this);
    QVBoxLayout* eichelLayout = new QVBoxLayout;
    QVBoxLayout* grasLayout = new QVBoxLayout;
    QVBoxLayout* herzLayout = new QVBoxLayout;
    QVBoxLayout* schellenLayout = new QVBoxLayout;

    eichelLayout->addWidget( labelEichel );
    eichelLayout->addWidget( checkEichel );
    grasLayout->addWidget( labelGras );
    grasLayout->addWidget( checkGras );
    herzLayout->addWidget( labelHerz );
    herzLayout->addWidget( checkHerz );
    schellenLayout->addWidget( labelSchellen );
    schellenLayout->addWidget( checkSchellen );

    QHBoxLayout* button_layout = new QHBoxLayout;
    button_layout->addLayout( eichelLayout );
    button_layout->addLayout( grasLayout );
    button_layout->addLayout( herzLayout );
    button_layout->addLayout( schellenLayout );
    button_layout->addWidget( checkFarblos );
    color_group->setLayout( button_layout );

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(8);
    layout->addWidget( color_group );
    this->setLayout( layout );
}

int SelectGameColorBox::getColor()
{
    if(checkEichel->isChecked()) return (Card::EICHEL);
    else if(checkGras->isChecked()) return (Card::GRAS);
    else if(checkHerz->isChecked()) return (Card::HERZ);
    else if(checkSchellen->isChecked()) return (Card::SCHELLEN);
    return (Card::NOCOLOR);
}


void SelectGameColorBox::colorChanged()
{
    m_gameinfo->setColor(getColor());
}

void SelectGameColorBox::statusChanged()
{
    if(m_gameinfo!=0)
    {
        switch(m_gameinfo->mode())
        {
                case (GameInfo::RUFSPIEL):
                            setStatus(
                                GameInfo::isAllowed( parent->getCardList(),GameInfo::RUFSPIEL, Card::EICHEL ),
                                GameInfo::isAllowed( parent->getCardList(),GameInfo::RUFSPIEL, Card::GRAS ),
                                false,
                                GameInfo::isAllowed( parent->getCardList(),GameInfo::RUFSPIEL, Card::SCHELLEN ) ,
                                false
                            );
                break;
                case (GameInfo::WENZ):
                            setStatus(
                                true,
                                true,
                                true,
                                true,
                                true
                            );
                break;
                case (GameInfo::STICHT):
                            setStatus(
                                true,
                                true,
                                true,
                                true,
                                false
                            );
                break;
                case (GameInfo::GEIER):
                            setStatus(
                                true,
                                true,
                                true,
                                true,
                                true
                            );
                break;
                case (GameInfo::DACHS):
                            setStatus(
                                false,
                                false,
                                false,
                                false,
                                true
                            );
                break;
                case (GameInfo::RAMSCH):
                break;
        }
    }
    checkFirstVisible();
    colorChanged();
}

void SelectGameColorBox::checkFirstVisible()
{
    if(!checkEichel->isHidden()) checkEichel->setChecked(true);
    else if(!checkHerz->isHidden()) checkHerz->setChecked(true);
    else if(!checkGras->isHidden()) checkGras->setChecked(true);
    else if(!checkSchellen->isHidden()) checkSchellen->setChecked(true);
    else if(!checkFarblos->isHidden()) checkFarblos->setChecked(true);
}

void SelectGameColorBox::setStatus(bool Eichel, bool Gras, bool Herz, bool Schellen, bool Farblos)
{
    bool allow_colors = true;

    const AllowedGames allowed = Settings::instance()->allowedGames();

    if( m_gameinfo->mode() == GameInfo::WENZ )
        allow_colors = allow_colors & allowed.farb_wenz;

    if( m_gameinfo->mode() == GameInfo::GEIER )
        allow_colors = allow_colors & allowed.farb_geier;

    checkEichel->setVisible(allow_colors&&Eichel==true);
    labelEichel->setVisible(allow_colors&&Eichel==true);

    checkGras->setVisible(allow_colors&&Gras==true);
    labelGras->setVisible(allow_colors&&Gras==true);

    checkHerz->setVisible(allow_colors&&Herz==true);
    labelHerz->setVisible(allow_colors&&Herz==true);

    checkSchellen->setVisible(allow_colors&&Schellen==true);
    labelSchellen->setVisible(allow_colors&&Schellen==true);

    if(Farblos==true)
        checkFarblos->show();
    else
        checkFarblos->hide();

    if(!Eichel && !Gras && !Herz && !Schellen && !Farblos)
        m_finish=false;
    else
        m_finish=true;
}

bool SelectGameColorBox::getFinish()
{
    return m_finish;
}

void SelectGameColorBox::setGameInfo(GameInfo* info)
{
    m_gameinfo=info;
    statusChanged();
}

GameInfo* SelectGameColorBox::getGameInfo()
{
    return m_gameinfo;
}

void SelectGameColorBox::cleanGameInfo()
{
    if(m_gameinfo!=0)
    {
        delete m_gameinfo;
        m_gameinfo=0;
    }
}

void SelectGameColorBox::colorChangedToEichel()
{
    checkEichel->setChecked(true);
}

void SelectGameColorBox::colorChangedToGras()
{
    checkGras->setChecked(true);
}

void SelectGameColorBox::colorChangedToHerz()
{
    checkHerz->setChecked(true);
}

void SelectGameColorBox::colorChangedToSchellen()
{
    checkSchellen->setChecked(true);
}
