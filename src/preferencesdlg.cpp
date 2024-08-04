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

#include <KUrlRequester>
#include <KPageWidgetItem>

#include <QRadioButton>
#include <QSpinBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>


PreferencesDlg::PreferencesDlg( QWidget *parent )
    : KPageDialog( parent )
{
    setWindowTitle( tr("Preferences") );
    setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
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
    ResultValues r;
    AllowedGames allowed;
    Settings* s = Settings::instance();

    s->setResultsType( m_radioMoney->isChecked() ? Settings::MONEY: Settings::POINTS );

    r.rufspiel = m_money_call->value();
    r.solo = m_money_solo->value();
    r.laufende = m_money_lauf->value();
    r.schneider = m_money_schneider->value();
    r.schwarz = m_money_notrick->value();
    r.ramsch = m_money_ramsch->value();
    s->setMoneyResults(r);

    r.rufspiel = m_point_call->value();
    r.solo = m_point_solo->value();
    r.laufende = m_point_lauf->value();
    r.schneider = m_point_schneider->value();
    r.schwarz = m_point_notrick->value();
    r.ramsch = m_point_ramsch->value();
    s->setPointResults(r);

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
    KPageWidgetItem *item = addPage( page, tr( "Player Settings" ) );
    item->setIcon( QIcon::fromTheme( "user-identity" ) );

    QStringList names = Settings::instance()->playerNames();

    box->addWidget( new QLabel( tr("Human Player:") ), 0, 0 );
    m_p1_name = new QLineEdit( names[0] );
    m_p1_name->setObjectName( "m_p1_name" );
    box->addWidget( m_p1_name, 0, 1 );

    box->addWidget( new QLabel( tr("Computer Player 1:") ), 1, 0 );
    m_p2_name = new QLineEdit( names[1] );
    m_p2_name->setObjectName( "m_p2_name" );
    box->addWidget( m_p2_name, 1, 1 );

    box->addWidget( new QLabel( tr("Computer Player 2:") ), 2, 0 );
    m_p3_name = new QLineEdit( names[2] );
    m_p3_name->setObjectName( "m_p3_name" );
    box->addWidget( m_p3_name, 2, 1 );

    box->addWidget( new QLabel( tr("Computer Player 3:") ), 3, 0 );
    m_p4_name = new QLineEdit( names[3] );
    m_p4_name->setObjectName( "m_p4_name" );
    box->addWidget( m_p4_name, 3, 1 );
}

void PreferencesDlg::addPageRules()
{
    QFrame* box = new QFrame();
    KPageWidgetItem *item = addPage( box, tr( "Rules" ) );
    item->setIcon( QIcon::fromTheme( "schafkopf" ) );

    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );

    m_radioThrowAway = new QRadioButton( tr("&Throw cards together and give new ones.") );
    m_radioForcedGame = new QRadioButton( tr("The player who has the Eichel &Ober has to play.") );
    m_radioRamsch = new QRadioButton( tr("Play Ramsch") );
    m_radioRamsch->setToolTip( tr( "The aim when playing a Ramsch is to make no tricks. The player who made the most points is the sole loser. If you make all tricks you can win, too." ) );
    m_checkDoublerPlays = new QCheckBox ( tr("The &last player who has doubled has to play.") );
    m_checkDoublerPlays->setToolTip( tr( "The last player who has doubled has to play. If no one has doubled the above rule takes effect." ) );
    m_checkDoubleNextGame = new QCheckBox ( tr("&Double next game when cards were thrown together.") );

    QGroupBox* group = new QGroupBox( tr("No one wants to play:") );
    QVBoxLayout *button_layout = new QVBoxLayout( group );
    button_layout->addWidget( m_radioThrowAway );
    button_layout->addWidget( m_radioForcedGame );
    button_layout->addWidget( m_radioRamsch );
    button_layout->addWidget( m_checkDoublerPlays );
    button_layout->addWidget( m_checkDoubleNextGame );

    QGridLayout* layout = new QGridLayout( box );
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing( 6 );
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
    connect(m_radioThrowAway, &QRadioButton::clicked, this, &PreferencesDlg::enableControls);
    connect(m_radioForcedGame, &QRadioButton::clicked, this, &PreferencesDlg::enableControls);
    connect(m_radioRamsch, &QRadioButton::clicked, this, &PreferencesDlg::enableControls);
}

void PreferencesDlg::addPageResults()
{
    QFrame* box = new QFrame();
    KPageWidgetItem *item = addPage( box, tr( "Results" ) );
    item->setIcon( QIcon::fromTheme( "applications-education" ) );

    ResultValues rm = Settings::instance()->moneyResults();
    ResultValues rp = Settings::instance()->pointResults();

    m_radioMoney = new QRadioButton( tr("count &money") );
    m_radioPoints = new QRadioButton( tr("count &points") );

    QGroupBox* group = new QGroupBox( tr("Results:") );
    QVBoxLayout *button_layout = new QVBoxLayout( group );
    button_layout->addWidget( m_radioMoney );
    button_layout->addWidget( m_radioPoints );

    stackMoney = new QWidget;

    QLabel* callgame_label = new QLabel( tr("Callgame:") );
    QLabel* solo_label = new QLabel( tr("Solo:") );
    QLabel* lauf_label = new QLabel( tr("Cards in a row:") );
    QLabel* notrick_label = new QLabel( tr("Notrick:") );
    QLabel* schneider_label = new QLabel( tr("Schneider:") );
    QLabel* ramsch_label = new QLabel( tr("Ramsch:") );

    const auto currencySymbol = QLocale().currencySymbol();

    m_money_call = new QDoubleSpinBox();
    m_money_call->setRange( 0., 100. );
    m_money_call->setValue( rm.rufspiel );
    m_money_call->setDecimals( 2 );
    m_money_call->setSingleStep( 0.1 );
    m_money_call->setSuffix( currencySymbol );

    m_money_solo = new QDoubleSpinBox();
    m_money_solo->setRange( 0., 100. );
    m_money_solo->setValue( rm.solo );
    m_money_solo->setDecimals( 2 );
    m_money_solo->setSingleStep( 0.1 );
    m_money_solo->setSuffix( currencySymbol );

    m_money_lauf = new QDoubleSpinBox();
    m_money_lauf->setRange( 0., 100. );
    m_money_lauf->setValue( rm.laufende );
    m_money_lauf->setDecimals( 2 );
    m_money_lauf->setSingleStep( 0.1 );
    m_money_lauf->setSuffix( currencySymbol );

    m_money_notrick = new QDoubleSpinBox();
    m_money_notrick->setRange( 0., 100. );
    m_money_notrick->setValue( rm.schwarz );
    m_money_notrick->setDecimals( 2 );
    m_money_notrick->setSingleStep( 0.1 );
    m_money_notrick->setSuffix( currencySymbol );

    m_money_schneider = new QDoubleSpinBox();
    m_money_schneider->setRange( 0., 100. );
    m_money_schneider->setValue( rm.schneider );
    m_money_schneider->setDecimals( 2 );
    m_money_schneider->setSingleStep( 0.1 );
    m_money_schneider->setSuffix( currencySymbol );

    m_money_ramsch = new QDoubleSpinBox();
    m_money_ramsch->setRange( 0., 100. );
    m_money_ramsch->setValue( rm.ramsch );
    m_money_ramsch->setDecimals( 2 );
    m_money_ramsch->setSingleStep( 0.1 );
    m_money_ramsch->setSuffix( currencySymbol );

    QGridLayout* stackMoney_layout = new QGridLayout( stackMoney );
    stackMoney_layout->addWidget( m_money_call, 0, 1 );
    stackMoney_layout->addWidget( m_money_solo, 1, 1 );
    stackMoney_layout->addWidget( m_money_lauf, 2, 1 );
    stackMoney_layout->addWidget( m_money_notrick, 3, 1 );
    stackMoney_layout->addWidget( m_money_schneider, 4, 1 );
    stackMoney_layout->addWidget( m_money_ramsch, 5, 1 );
    stackMoney_layout->addWidget( callgame_label, 0, 0 );
    stackMoney_layout->addWidget( solo_label, 1, 0 );
    stackMoney_layout->addWidget( lauf_label, 2, 0 );
    stackMoney_layout->addWidget( notrick_label, 3, 0 );
    stackMoney_layout->addWidget( schneider_label, 4, 0 );
    stackMoney_layout->addWidget( ramsch_label, 5, 0 );

    stackPoints = new QWidget;

    QLabel* callgame_label2 = new QLabel( tr("Callgame:") );
    QLabel* solo_label2 = new QLabel( tr("Solo:") );
    QLabel* lauf_label2 = new QLabel( tr("Cards in a row:") );
    QLabel* notrick_label2 = new QLabel( tr("Notrick:") );
    QLabel* schneider_label2 = new QLabel( tr("Schneider:") );
    QLabel* ramsch_label2 = new QLabel( tr("Ramsch:") );

    m_point_call = new QSpinBox();
    m_point_call->setRange( 0, 100 );
    m_point_call->setValue( (int)rp.rufspiel );

    m_point_solo = new QSpinBox();
    m_point_solo->setRange( 0, 100 );
    m_point_solo->setValue( (int)rp.solo );

    m_point_lauf = new QSpinBox();
    m_point_lauf->setRange( 0, 100 );
    m_point_lauf->setValue( (int)rp.laufende );

    m_point_notrick = new QSpinBox();
    m_point_notrick->setRange( 0, 100 );
    m_point_notrick->setValue( (int)rp.schwarz );

    m_point_schneider = new QSpinBox();
    m_point_schneider->setRange( 0, 100 );
    m_point_schneider->setValue( (int)rp.schneider );

    m_point_ramsch = new QSpinBox();
    m_point_ramsch->setRange( 0, 100 );
    m_point_ramsch->setValue( (int)rp.ramsch );

    QGridLayout* stackPoints_layout = new QGridLayout( stackPoints );
    stackPoints_layout->addWidget( m_point_call, 0, 1 );
    stackPoints_layout->addWidget( m_point_solo, 1, 1 );
    stackPoints_layout->addWidget( m_point_lauf, 2, 1 );
    stackPoints_layout->addWidget( m_point_notrick, 3, 1 );
    stackPoints_layout->addWidget( m_point_schneider, 4, 1 );
    stackPoints_layout->addWidget( m_point_ramsch, 5, 1 );
    stackPoints_layout->addWidget( callgame_label2, 0, 0 );
    stackPoints_layout->addWidget( solo_label2, 1, 0 );
    stackPoints_layout->addWidget( lauf_label2, 2, 0 );
    stackPoints_layout->addWidget( notrick_label2, 3, 0 );
    stackPoints_layout->addWidget( schneider_label2, 4, 0 );
    stackPoints_layout->addWidget( ramsch_label2, 5, 0 );

    stack = new QStackedWidget( box );
    stack->addWidget( stackMoney );
    stack->addWidget( stackPoints );

    QGridLayout* layout = new QGridLayout( box );
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing( 6 );
    layout->addWidget( group, 0, 0 );
    layout->addWidget( stack, 1, 0 );

    // load data from configuration
    if( Settings::instance()->resultsType() == Settings::MONEY )
        m_radioMoney->setChecked( true );
    else
        m_radioPoints->setChecked( true );

    connect(m_radioPoints, &QRadioButton::clicked, this, &PreferencesDlg::enableControls);
    connect(m_radioMoney, &QRadioButton::clicked, this, &PreferencesDlg::enableControls);
}

void PreferencesDlg::addPageView()
{
    QFrame* box = new QFrame();
    KPageWidgetItem *item = addPage( box, tr( "View" ) );
    item->setIcon( QIcon::fromTheme( "games-config-background" ) );

    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );

    QLabel* label3 = new QLabel( tr("&Background image:") );
    m_linePathBackground = new KUrlRequester( Settings::instance()->backgroundImage() );
    m_pushURLClear = new QPushButton( tr("&Clear background image") );
    label3->setBuddy( m_linePathBackground );

    QGroupBox* group2 = new QGroupBox( tr("Game Canvas:") );
    QVBoxLayout *button_layout = new QVBoxLayout( group2 );
    button_layout->addWidget( label3 );
    button_layout->addWidget( m_linePathBackground );
    button_layout->addWidget( m_pushURLClear );

    m_checkRearrangeCards = new QCheckBox ( tr("Rearrange cards after each trick.") );
    m_checkRearrangeCards->setToolTip( tr( "Cards will be rearranged after each trick." ) );

    QGroupBox* group = new QGroupBox( tr("Card Arrangement:") );
    QVBoxLayout *button_layout2 = new QVBoxLayout( group );
    button_layout2->addWidget( m_checkRearrangeCards );

    QGridLayout* layout = new QGridLayout( box );
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing( 6 );
    layout->addWidget( group2, 0, 0 );
    layout->addWidget( group, 1, 0 );
    layout->addItem( spacer, 2, 0 );
    disableClearButton(m_linePathBackground->text());

    connect(m_pushURLClear, &QRadioButton::clicked, m_linePathBackground, &KUrlRequester::clear);
    connect(m_linePathBackground, &KUrlRequester::textChanged, this, &PreferencesDlg::disableClearButton);

    // load data from configuration
    m_checkRearrangeCards->setChecked( Settings::instance()->rearrangeCards() );
}

void PreferencesDlg::addPageGames()
{
    QFrame* box = new QFrame();
    KPageWidgetItem *item = addPage( box, tr( "Games" ) );
    item->setIcon( QIcon::fromTheme( "schafkopf" ) );


    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );
    QSpacerItem* smallSpace = new QSpacerItem( 0, 0, QSizePolicy::Maximum, QSizePolicy::Maximum );
    QSpacerItem* smallSpace2 = new QSpacerItem( 0, 0, QSizePolicy::Maximum, QSizePolicy::Maximum );

    m_games_wenz = new QCheckBox( tr("&Wenz") );
    m_games_farbwenz = new QCheckBox( tr("Colored &Wenz"), box );
    m_games_geier = new QCheckBox( tr("&Geier"), box );
    m_games_farbgeier = new QCheckBox( tr("Colored &Geier"), box );
    m_games_dachs = new QCheckBox( tr("&Badger"), box );

    QLabel* infoLabel = new QLabel( tr("<qt>You can configure which games are allowed to play."
                                        " You cannot disable certain games, such as Callgames and Solos as they are always "
                                        "enabled by default.</qt>") );
    infoLabel->setWordWrap( true );

    QGridLayout* farbWenzLayout = new QGridLayout();
    farbWenzLayout->setContentsMargins(6, 6, 6, 6);
    farbWenzLayout->setSpacing( 6 );
    farbWenzLayout->addItem( smallSpace, 0, 0 );
    farbWenzLayout->addWidget( m_games_farbwenz, 0, 1 );

    QGridLayout* farbGeierLayout = new QGridLayout();
    farbGeierLayout->setContentsMargins(6, 6, 6, 6);
    farbGeierLayout->setSpacing( 6 );
    farbGeierLayout->addItem( smallSpace2, 0, 0 );
    farbGeierLayout->addWidget( m_games_farbgeier, 0, 1 );

    QGridLayout* layout = new QGridLayout( box );
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing( 6 );
    layout->addWidget( infoLabel );
    layout->addWidget( m_games_wenz, 1, 0 );
    layout->addLayout( farbWenzLayout, 2, 0 );
    layout->addWidget( m_games_geier, 3, 0 );
    layout->addLayout( farbGeierLayout, 4, 0 );
    layout->addWidget( m_games_dachs, 5, 0 );
    layout->addItem( spacer, 6, 0 );

    // load settings
    AllowedGames* allowed = Settings::instance()->allowedGames();
    m_games_wenz->setChecked( allowed->wenz );
    m_games_farbwenz->setChecked( allowed->farb_wenz );
    m_games_geier->setChecked( allowed->geier );
    m_games_farbgeier->setChecked( allowed->farb_geier );
    m_games_dachs->setChecked( allowed->dachs );
    delete allowed;

    // connections
    connect(m_games_wenz, &QCheckBox::clicked, this, &PreferencesDlg::enableControls);
    connect(m_games_geier, &QCheckBox::clicked, this, &PreferencesDlg::enableControls);

    // tool tips
    m_games_dachs->setToolTip( tr( "<qt>The badger is no official game in Schafkopf and is therefore not played at tournaments.</qt>" ) );
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
