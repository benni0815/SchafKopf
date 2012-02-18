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
#include "preferencesdlg.h"
#include "settings.h"

#include <kiconloader.h>
#include <klocale.h>
#include <knuminput.h>
#include <klineedit.h>
#include <kurlrequester.h>
#include <KPageWidgetItem>

#include <qlayout.h>
#include <qradiobutton.h>
#include <q3buttongroup.h>
#include <Q3HButtonGroup>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qtooltip.h>
#include <QGridLayout>

PreferencesDlg::PreferencesDlg( QWidget *parent )
    : KPageDialog( parent )
{
    setCaption( i18n("Preferences") );
    setButtons( KDialog::Ok | KDialog::Cancel );
    addPagePlayer();
    addPageGames(); // erlaubte spiele
    // addPageSchafkopf(); // ton an aus .....
    addPageRules();
    addPageResults();
    addPageView();

    enableControls();
}

PreferencesDlg::~PreferencesDlg()
{
}

void PreferencesDlg::accept()
{
    t_ResultValues r;
    t_AllowedGames allowed;
    Settings* s = Settings::instance();

    s->setResultsType( m_radioMoney->isChecked() ? Settings::MONEY: Settings::POINTS );

    r.rufspiel = m_money_call->value();
    r.solo = m_money_solo->value();
    r.laufende = m_money_lauf->value();
    r.schneider = m_money_schneider->value();
    r.schwarz = m_money_notrick->value();
    r.ramsch = m_money_ramsch->value();
    s->setMoneyResults( &r );

    r.rufspiel = m_point_call->value();
    r.solo = m_point_solo->value();
    r.laufende = m_point_lauf->value();
    r.schneider = m_point_schneider->value();
    r.schwarz = m_point_notrick->value();
    r.ramsch = m_point_ramsch->value();
    s->setPointResults( &r );

    QStringList names; 
    names << m_p1_name->text();
    names << m_p2_name->text();
    names << m_p3_name->text();
    names << m_p4_name->text();
    s->setPlayerNames( names );

    if( m_radioThrowAway->isChecked() )
        s->setNoGame( Settings::NOGAME_NEUGEBEN );
    else if ( m_radioForcedGame->isChecked() )
        s->setNoGame( Settings::NOGAME_ALTERSPIELT );
    else if( m_radioRamsch->isChecked() )
        s->setNoGame( Settings::NOGAME_RAMSCH );

    s->setDoublerHasToPlay( m_checkDoublerPlays->isChecked() );

    s->setDoubleNextGame( m_checkDoubleNextGame->isChecked() );

    s->setRearrangeCards( m_checkRearrangeCards->isChecked() );
    s->setBackgroundImage( m_linePathBackground->text() );

    // allowed games
    allowed.wenz = m_games_wenz->isChecked();
    allowed.farb_wenz = m_games_farbwenz->isChecked();
    allowed.geier = m_games_geier->isChecked();
    allowed.farb_geier = m_games_farbgeier->isChecked();
    allowed.dachs = m_games_dachs->isChecked();
    s->setAllowedGames( &allowed );

    KPageDialog::accept();
}

void PreferencesDlg::addPagePlayer()
{
    QWidget *page = new QWidget( this );
    QGridLayout* box = new QGridLayout( page );
    KPageWidgetItem *item = addPage( page, i18n( "Player Settings" ) );
    item->setIcon( KIcon( "identity" ) );

    QStringList names = Settings::instance()->playerNames();

    box->addWidget( new QLabel( i18n("Human Player:") ), 0, 0 );
    m_p1_name = new KLineEdit( names[0] );
    m_p1_name->setObjectName( "m_p1_name" );
    box->addWidget( m_p1_name, 0, 1 );

    box->addWidget( new QLabel( i18n("Computer Player 1:") ), 1, 0 );
    m_p2_name = new KLineEdit( names[1] );
    m_p2_name->setObjectName( "m_p2_name" );
    box->addWidget( m_p2_name, 1, 1 );

    box->addWidget( new QLabel( i18n("Computer Player 2:") ), 2, 0 );
    m_p3_name = new KLineEdit( names[2] );
    m_p3_name->setObjectName( "m_p3_name" );
    box->addWidget( m_p3_name, 2, 1 );

    box->addWidget( new QLabel( i18n("Computer Player 3:") ), 3, 0 );
    m_p4_name = new KLineEdit( names[3] );
    m_p4_name->setObjectName( "m_p4_name" );
    box->addWidget( m_p4_name, 3, 1 );
}

void PreferencesDlg::addPageRules()
{
    QFrame* box = new QFrame();
    KPageWidgetItem *item = addPage( box, i18n( "Rules" ) );
    item->setIcon( KIcon( "schafkopf" ) );

    QGridLayout* layout = new QGridLayout( box );
    layout->setMargin( 6 );
    layout->setSpacing( 6 );
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );

    Q3VButtonGroup* group = new Q3VButtonGroup( i18n("No one wants to play:"), box, "group" );
    m_radioThrowAway = new QRadioButton( i18n("&Throw cards together and give new ones."), group );
    m_radioForcedGame = new QRadioButton( i18n("The player who has the Eichel &Ober has to play."), group );
    m_radioRamsch = new QRadioButton( i18n("Play Ramsch"), group );

    m_radioRamsch->setToolTip( i18n( "The aim when playing a Ramsch is to make no tricks. The player who made the most points is the sole loser. If you make all tricks you can win, too." ) );
    m_checkDoublerPlays = new QCheckBox ( i18n("The &last player who has doubled has to play."), group );
    m_checkDoublerPlays->setToolTip( i18n( "The last player who has doubled has to play. If no one has doubled the above rule takes effect." ) );
    m_checkDoubleNextGame = new QCheckBox ( i18n("&Double next game when cards were thrown together."), group );

    layout->addWidget( group, 0, 0 );
    layout->addItem( spacer, 1, 0 );

    // load data from configuration
    if( Settings::instance()->noGame() == Settings::NOGAME_ALTERSPIELT )
        m_radioForcedGame->setChecked( true );
    else if( Settings::instance()->noGame() == Settings::NOGAME_NEUGEBEN )
        m_radioThrowAway->setChecked( true );
    else if( Settings::instance()->noGame() == Settings::NOGAME_RAMSCH )
        m_radioRamsch->setChecked( true );

    m_checkDoublerPlays->setChecked( Settings::instance()->doublerHasToPlay() );

    m_checkDoubleNextGame->setChecked( Settings::instance()->doubleNextGame() );

    // connections
    connect( m_radioThrowAway, SIGNAL( clicked() ), this, SLOT( enableControls() ) );
    connect( m_radioForcedGame, SIGNAL( clicked() ), this, SLOT( enableControls() ) );
    connect( m_radioRamsch, SIGNAL( clicked() ), this, SLOT( enableControls() ) );
}

void PreferencesDlg::addPageResults()
{
    QFrame* box = new QFrame();
    KPageWidgetItem *item = addPage( box, i18n( "Results" ) );
    item->setIcon( KIcon( "edit" ) );

    QGridLayout* layout = new QGridLayout( box );
    layout->setMargin( 6 );
    layout->setSpacing( 6 );

    t_ResultValues* rm = Settings::instance()->moneyResults();
    t_ResultValues* rp = Settings::instance()->pointResults();
    KLocale* locale = KGlobal::locale();

    Q3VButtonGroup* group = new Q3VButtonGroup( i18n("Results:"), box, "group" );
    m_radioMoney = new QRadioButton( i18n("count &money"), group );
    m_radioPoints = new QRadioButton( i18n("count &points"), group );

    stack = new QStackedWidget( box );

    stackMoney = new Q3VBox( stack );
    stackPoints = new Q3VBox( stack );

    m_money_call = new KDoubleNumInput( 0.0, 100.00, rm->rufspiel, stackMoney, 0.10, 2 );
    m_money_call->setLabel( i18n("Callgame:"), Qt::AlignLeft | Qt::AlignVCenter );
    m_money_call->setSuffix( locale->currencySymbol() );

    m_money_solo = new KDoubleNumInput( 0.0, 100.00, rm->solo, stackMoney, 0.10, 2);
    m_money_solo->setLabel( i18n("Solo:"), Qt::AlignLeft | Qt::AlignVCenter );
    m_money_solo->setSuffix( locale->currencySymbol() );

    m_money_lauf = new KDoubleNumInput( 0.0, 100.00, rm->laufende, stackMoney, 0.10, 2 );
    m_money_lauf->setLabel( i18n("Cards in a row:"), Qt::AlignLeft | Qt::AlignVCenter );
    m_money_lauf->setSuffix( locale->currencySymbol() );

    m_money_notrick = new KDoubleNumInput( 0.0, 100.00, rm->schwarz, stackMoney, 0.10, 2 );
    m_money_notrick->setLabel( i18n("Notrick:"), Qt::AlignLeft | Qt::AlignVCenter );
    m_money_notrick->setSuffix( locale->currencySymbol() );

    m_money_schneider = new KDoubleNumInput( 0.0, 100.00, rm->schneider, stackMoney, 0.10, 2 );
    m_money_schneider->setLabel( i18n("Schneider:"), Qt::AlignLeft | Qt::AlignVCenter );
    m_money_schneider->setSuffix( locale->currencySymbol() );

    m_money_ramsch = new KDoubleNumInput( 0.0, 100.00, rm->ramsch, stackMoney, 0.10, 2 );
    m_money_ramsch->setLabel( i18n("Ramsch:"), Qt::AlignLeft | Qt::AlignVCenter );
    m_money_ramsch->setSuffix( locale->currencySymbol() );

    m_point_call = new KIntNumInput( (int)rp->rufspiel, stackPoints, 10 );
    m_point_call->setLabel( i18n("Callgame:"), Qt::AlignLeft | Qt::AlignVCenter );
    m_point_call->setMinimum( 0 );
    m_point_call->setMaximum( 100 );

    m_point_solo = new KIntNumInput( (int)rp->solo, stackPoints, 10 );
    m_point_solo->setLabel( i18n("Solo:"), Qt::AlignLeft | Qt::AlignVCenter );
    m_point_solo->setMinimum( 0 );
    m_point_solo->setMaximum( 100 );

    m_point_lauf = new KIntNumInput( (int)rp->laufende, stackPoints, 10 );
    m_point_lauf->setLabel( i18n("Cards in a row:"), Qt::AlignLeft | Qt::AlignVCenter );
    m_point_lauf->setMinimum( 0 );
    m_point_lauf->setMaximum( 100 );

    m_point_notrick = new KIntNumInput( (int)rp->schwarz, stackPoints, 10 );
    m_point_notrick->setLabel( i18n("Notrick:"), Qt::AlignLeft | Qt::AlignVCenter );
    m_point_notrick->setMinimum( 0 );
    m_point_notrick->setMaximum( 100 );

    m_point_schneider = new KIntNumInput( (int)rp->schneider, stackPoints, 10 );
    m_point_schneider->setLabel( i18n("Schneider:"), Qt::AlignLeft | Qt::AlignVCenter );
    m_point_schneider->setMinimum( 0 );
    m_point_schneider->setMaximum( 100 );

    m_point_ramsch = new KIntNumInput( (int)rp->ramsch, stackPoints, 10 );
    m_point_ramsch->setLabel( i18n("Ramsch:"), Qt::AlignLeft | Qt::AlignVCenter );
    m_point_ramsch->setMinimum( 0 );
    m_point_ramsch->setMaximum( 100 );

    stack->addWidget( stackMoney );
    stack->addWidget( stackPoints );

    delete rm;
    delete rp;

    layout->addWidget( group, 0, 0 );
    layout->addWidget( stack, 1, 0 );

    // load data from configuration
    if( Settings::instance()->resultsType() == Settings::MONEY )
        m_radioMoney->setChecked( true );
    else
        m_radioPoints->setChecked( true );

    connect( m_radioPoints, SIGNAL( clicked() ), this, SLOT( enableControls() ) );
    connect( m_radioMoney, SIGNAL( clicked() ), this, SLOT( enableControls() ) );
}

void PreferencesDlg::addPageView()
{
    QFrame* box = new QFrame();
    KPageWidgetItem *item = addPage( box, i18n( "View" ) );
    item->setIcon( KIcon( "background" ) );

    QGridLayout* layout = new QGridLayout( box );
    layout->setMargin( 6 );
    layout->setSpacing( 6 );
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );

    Q3VButtonGroup* group2 = new Q3VButtonGroup( i18n("Game Canvas:"), box, "group2" );
    QLabel* label3 = new QLabel( i18n("&Background image:"), group2 );
    m_linePathBackground = new KUrlRequester( Settings::instance()->backgroundImage(), group2 );
    m_pushURLClear = new QPushButton( i18n("&Clear background image"), group2 );
    label3->setBuddy( m_linePathBackground );

    Q3VButtonGroup* group = new Q3VButtonGroup( i18n("Card Arrangement:"), box, "group" );
    m_checkRearrangeCards = new QCheckBox ( i18n("Rearrange cards after each trick."), group );
    m_checkRearrangeCards->setToolTip( i18n( "Cards will be rearranged after each trick." ) );

    layout->addWidget( group2, 0, 0 );
    layout->addWidget( group, 1, 0 );
    layout->addItem( spacer, 2, 0 );
    disableClearButton(m_linePathBackground->text());

    connect( m_pushURLClear, SIGNAL( clicked() ), m_linePathBackground, SLOT( clear() ) );
    connect( m_linePathBackground, SIGNAL( textChanged(const QString &) ), this, SLOT( disableClearButton(const QString &) ) );

    // load data from configuration
    m_checkRearrangeCards->setChecked( Settings::instance()->rearrangeCards() );
}

void PreferencesDlg::addPageGames()
{
    QFrame* box = new QFrame();
    KPageWidgetItem *item = addPage( box, i18n( "Games" ) );
    item->setIcon( KIcon( "joystick" ) );

    QGridLayout* layout = new QGridLayout( box );
    layout->setMargin( 6 );
    layout->setSpacing( 6 );

    QGridLayout* farbWenzLayout = new QGridLayout( box );
    farbWenzLayout->setMargin( 6 );
    farbWenzLayout->setSpacing( 6 );

    QGridLayout* farbGeierLayout = new QGridLayout( box );
    farbGeierLayout->setMargin( 6 );
    farbGeierLayout->setSpacing( 6 );

    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );
    QSpacerItem* smallSpace = new QSpacerItem( 0, 0, QSizePolicy::Maximum, QSizePolicy::Maximum );
    QSpacerItem* smallSpace2 = new QSpacerItem( 0, 0, QSizePolicy::Maximum, QSizePolicy::Maximum );

    m_games_wenz = new QCheckBox( i18n("&Wenz"), box );
    m_games_farbwenz = new QCheckBox( i18n("Colored &Wenz"), box );
    m_games_geier = new QCheckBox( i18n("&Geier"), box );
    m_games_farbgeier = new QCheckBox( i18n("Colored &Geier"), box );
    m_games_dachs = new QCheckBox( i18n("&Badger"), box );

    layout->addWidget( new QLabel( i18n("<qt>You can configure which games are allowed to play."
                                        " You cannot disable certain games, such as Callgames and Solos as they are always "
                                        "enabled by default.</qt>"), box ), 0, 0 );

    farbWenzLayout->addItem( smallSpace, 0, 0 );
    farbWenzLayout->addWidget( m_games_farbwenz, 0, 1 );
    farbGeierLayout->addItem( smallSpace2, 0, 0 );
    farbGeierLayout->addWidget( m_games_farbgeier, 0, 1 );

    layout->addWidget( m_games_wenz, 1, 0 );
    layout->addLayout( farbWenzLayout, 2, 0 );
    layout->addWidget( m_games_geier, 3, 0 );
    layout->addLayout( farbGeierLayout, 4, 0 );
    layout->addWidget( m_games_dachs, 5, 0 );
    layout->addItem( spacer, 6, 0 );

    // load settings
    t_AllowedGames* allowed = Settings::instance()->allowedGames();
    m_games_wenz->setChecked( allowed->wenz );
    m_games_farbwenz->setChecked( allowed->farb_wenz );
    m_games_geier->setChecked( allowed->geier );
    m_games_farbgeier->setChecked( allowed->farb_geier );
    m_games_dachs->setChecked( allowed->dachs );
    delete allowed;

    // connections
    connect( m_games_wenz, SIGNAL( clicked() ), this, SLOT( enableControls() ) );
    connect( m_games_geier, SIGNAL( clicked() ), this, SLOT( enableControls() ) );

    // tool tips
    m_games_dachs->setToolTip( i18n( "<qt>The badger is no official game in Schafkopf and is therefore not played at tournaments.</qt>" ) );
}

void PreferencesDlg::enableControls()
{
    if( m_radioPoints->isChecked() )
        stack->setCurrentWidget( stackPoints );
    else if( m_radioMoney->isChecked() )
            stack->setCurrentWidget( stackMoney );

    m_checkDoubleNextGame->setEnabled( m_radioThrowAway->isChecked() );

    m_games_farbwenz->setEnabled( m_games_wenz->isChecked() );
    m_games_farbgeier->setEnabled( m_games_geier->isChecked() );
}

void PreferencesDlg::disableClearButton(const QString & url)
{
    if(url=="")
        m_pushURLClear->setEnabled( false );
    else
        m_pushURLClear->setEnabled( true );
}
