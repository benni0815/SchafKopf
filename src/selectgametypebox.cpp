// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "selectgametypebox.h"

#include "card.h"
#include "cardlist.h"
#include "gameinfo.h"
#include "settings.h"

#include <KLocalizedString>

#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPainter>


SelectGameTypeBox::SelectGameTypeBox( QWidget *parent )
    : QWizardPage( parent )
{
    setTitle(i18n("Step 1/2: Select Game"));

    QWidget* row1 = new QWidget;

    preview = new QLabel;
    infoLabel = new QLabel;
    infoLabel->setMargin( 11 );
    infoLabel->setAlignment( Qt::AlignBottom | Qt::AlignLeft );
    infoLabel->setWordWrap( true );
    infoLabel->setMinimumHeight(100);

    checkRufspiel = new QRadioButton(i18n("&Callgame"));
    checkSolo = new QRadioButton(i18n("&Solo"));
    checkGeier = new QRadioButton(i18n("&Geier"));
    checkWenz = new QRadioButton(i18n("&Wenz"));
    checkDachs = new QRadioButton(i18n("&Dachs"));

    checkRufspiel->setChecked(true);

    const AllowedGames allowed = Settings::instance()->allowedGames();
    checkGeier->setEnabled(allowed.geier);
    checkWenz->setEnabled(allowed.wenz);
    checkDachs->setEnabled(allowed.dachs);

    QButtonGroup *myGroup = new QButtonGroup;
    myGroup->addButton( checkRufspiel );
    myGroup->addButton( checkSolo );
    myGroup->addButton( checkGeier );
    myGroup->addButton( checkWenz );
    myGroup->addButton( checkDachs );
    connect(myGroup, &QButtonGroup::buttonClicked, this, &SelectGameTypeBox::typeChanged);

    QGroupBox* group_TypeSelect = new QGroupBox(i18n("Game"));
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

void SelectGameTypeBox::typeChanged()
{
    const auto HelpGametypeSolo = i18n("When you play a \"Solo\", you have to additionally choose a color. Trumps are all \"Ober\", \"Unter\" and the color you chose. You will play alone against the other three players.");
    const auto HelpGametypeDachs = i18n("When you play a \"Dachs\", the only trumps are aces and tenners. You will play alone against the other three players.");
    const auto HelpGametypeRufspiel = i18n("When you play a callgame, you will have to choose a color. You will then play in team with the player which has the ace of the color you chose. Hearts are trumps.");
    const auto HelpGametypeWenz = i18n("When you play a \"Wenz\", you can choose a color. If you don't choose a color, only all \"Unter\" will be trumps. If you do, the cards of the chosen color will also be trumps. You will play alone against the other three players.");
    const auto HelpGametypeGeier = i18n("When you play a \"Geier\", you can choose a color. If you don't choose a color, only all \"Ober\" will be trumps. If you do, the cards of the chosen color will also be trumps. You will play alone against the other three players.");

    auto info = std::unique_ptr<GameInfo>(gameInfo());
    updatePreview();
    switch(info->mode())
    {
    case (GameInfo::RUFSPIEL):
        setInfoText(HelpGametypeRufspiel);
        break;
    case (GameInfo::WENZ):
        setInfoText(HelpGametypeWenz);
        break;
    case (GameInfo::STICHT):
        setInfoText(HelpGametypeSolo);
        break;
    case (GameInfo::GEIER):
        setInfoText(HelpGametypeGeier);
        break;
    case (GameInfo::DACHS):
        setInfoText(HelpGametypeDachs);
        break;
    // DS: Fix a compiler warning
    default:
        break;
    }
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
    auto info = std::unique_ptr<GameInfo>(gameInfo());

    list.init();
    for(i=0;i<(int)list.count();i++)
        if( info->istTrumpf( list.at(i) ) )
            trumpf.append( list.at(i) );
    trumpf.sort((eval_func)info->evalCard, (void *)info.get());

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
