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

#include <qframe.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qvbox.h>
#include <qvbuttongroup.h>
#include <qhbuttongroup.h>
#include <qlabel.h>
#include <qspinbox.h>

PreferencesDlg::PreferencesDlg(QWidget *parent, const char *name)
    : KDialogBase( IconList, i18n("Preferences"), KDialogBase::Ok|KDialogBase::Cancel,
      KDialogBase::Ok, parent, name, true, true )
{
    addPagePlayer();
    // addPageGame(); // erlaubte spiele
    // addPageSchafkopf(); // ton an aus .....
    addPageRules();
    addPageResults();
    addPageResultsMoney();
    addPageResultsPoints();
}


PreferencesDlg::~PreferencesDlg()
{
}

void PreferencesDlg::accept()
{
    t_ResultValues r;
    
    Settings::instance()->setResultsType( m_radioMoney->isChecked() ? Settings::MONEY: Settings::POINTS );
    
    r.rufspiel = m_money_call->value();
    r.solo = m_money_solo->value();
    r.laufende = m_money_lauf->value();
    r.schneider = m_money_schneider->value();
    r.schwarz = m_money_notrick->value();
    Settings::instance()->setMoneyResults( &r );
    
    r.rufspiel = m_point_call->value();
    r.solo = m_point_solo->value();
    r.laufende = m_point_lauf->value();
    r.schneider = m_point_schneider->value();
    r.schwarz = m_point_notrick->value();
    Settings::instance()->setPointResults( &r );
   
    QStringList names; 
    names << m_p1_name->text();
    names << m_p2_name->text();
    names << m_p3_name->text();
    names << m_p4_name->text();
    Settings::instance()->setPlayerNames( names );
    
    Settings::instance()->setNoGame( m_radioThrowAway->isChecked() ? Settings::NOGAME_NEUGEBEN : Settings::NOGAME_ALTERSPIELT );
    
    KDialogBase::accept();
}

void PreferencesDlg::addPagePlayer()
{
    QFrame* box2 = addPage( i18n("Player Settings"), QString::null, BarIcon("identity") );
    QVBoxLayout* layout2 = new QVBoxLayout( box2, 6, 6  );
    QStringList names = Settings::instance()->playerNames();

    QVButtonGroup* group2 = new QVButtonGroup( i18n("Your Player"), box2, "group2" );
    QLabel* label3 = new QLabel( i18n("Name:"), group2, "label3" );
    m_p1_name = new KLineEdit( names[0], group2, "m_p1_name" );
    
    QVButtonGroup* group3 = new QVButtonGroup( i18n("Computer Players"), box2, "group3" );
    QLabel* label4 = new QLabel( i18n("Computer Player 1:"), group3, "label4" );
    m_p2_name = new KLineEdit( names[1], group3, "m_p2_name" );
    QLabel* label5 = new QLabel( i18n("Computer Player 2:"), group3, "label5" );
    m_p3_name = new KLineEdit( names[2], group3, "m_p3_name" );
    QLabel* label6 = new QLabel( i18n("Computer Player 3:"), group3, "label6" );
    m_p4_name = new KLineEdit( names[3], group3, "m_p4_name" );
    
    layout2->addWidget( group2 );
    layout2->addWidget( group3 );
      
    // TODO: move into an own function  
    //QFrame* box3 = addPage( i18n("Rules"), "" );
}

void PreferencesDlg::addPageRules()
{
    QFrame* box = addPage( i18n("Rules"), QString::null, BarIcon("schafkopf") );
    QVBoxLayout* layout = new QVBoxLayout( box, 6, 6  );
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );
    
    QVButtonGroup* group = new QVButtonGroup( i18n("No one wants to play:"), box, "group" );
    m_radioThrowAway = new QRadioButton( i18n("&Throw cards away and give new ones"), group );
    m_radioForcedGame = new QRadioButton( i18n("&Forced game (Eichel Ober plays)"), group );
 
    layout->addWidget( group );
    layout->addItem( spacer );
    
    // load data from configuration
    if( Settings::instance()->noGame() == Settings::NOGAME_ALTERSPIELT )
        m_radioForcedGame->setChecked( true );
    else
        m_radioThrowAway->setChecked( true );
}

void PreferencesDlg::addPageResults()
{    
    QFrame* box = addPage( i18n("Results"), QString::null );
    QVBoxLayout* layout = new QVBoxLayout( box, 6, 6  );
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );
    
    QVButtonGroup* group = new QVButtonGroup( "", box, "group" );
    m_radioMoney = new QRadioButton( i18n("count &money"), group );
    m_radioPoints = new QRadioButton( i18n("count &points"), group );
    
    layout->addWidget( group );
    layout->addWidget( new QLabel( i18n("<qt>SchafKopf can count in its results view either points for each game or a monetrary value for each game.<br>You can also configure how much points/money a game costs when won or lost.</qt>"), box ) );
    layout->addItem( spacer );
    
    // load data from configuration
    if( Settings::instance()->resultsType() == Settings::MONEY )
        m_radioMoney->setChecked( true );
    else
        m_radioPoints->setChecked( true );
}

void PreferencesDlg::addPageResultsMoney()
{
    QFrame* box = addPage( i18n("Configure Money Values"), QString::null, BarIcon("edit") );
    QVBoxLayout* layout = new QVBoxLayout( box, 6, 6  );
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );
    t_ResultValues* r = Settings::instance()->moneyResults();
    
    KLocale* locale = KGlobal::locale();

    m_money_call = new KDoubleNumInput( 0.0, 100.00, r->rufspiel, 0.10, 2, box, "m_money_call" );
    m_money_call->setLabel( i18n("Callgame:"), AlignLeft | AlignVCenter );
    m_money_call->setSuffix( locale->currencySymbol() );
    
    m_money_solo = new KDoubleNumInput( m_money_call, 0.0, 100.00, r->solo, 0.10, 2, box, "m_money_solo" );
    m_money_solo->setLabel( i18n("Solo:"), AlignLeft | AlignVCenter );
    m_money_solo->setSuffix( locale->currencySymbol() );
    
    m_money_lauf = new KDoubleNumInput( m_money_solo, 0.0, 100.00, r->laufende, 0.10, 2, box, "m_money_lauf" );
    m_money_lauf->setLabel( i18n("Cards in a row:"), AlignLeft | AlignVCenter );
    m_money_lauf->setSuffix( locale->currencySymbol() );
    
    m_money_notrick = new KDoubleNumInput( m_money_lauf, 0.0, 100.00, r->schwarz, 0.10, 2, box, "m_money_notrick" );
    m_money_notrick->setLabel( i18n("Notrick:"), AlignLeft | AlignVCenter );
    m_money_notrick->setSuffix( locale->currencySymbol() );
    
    m_money_schneider = new KDoubleNumInput( m_money_notrick, 0.0, 100.00, r->schneider, 0.10, 2, box, "m_money_schneider" );
    m_money_schneider->setLabel( i18n("Schneider:"), AlignLeft | AlignVCenter );
    m_money_schneider->setSuffix( locale->currencySymbol() );
    
    layout->addWidget( m_money_call );    
    layout->addWidget( m_money_solo );
    layout->addWidget( m_money_lauf );
    layout->addWidget( m_money_notrick );
    layout->addWidget( m_money_schneider );
    layout->addItem( spacer );
    
    delete r;
}

void PreferencesDlg::addPageResultsPoints()
{
    QFrame* box = addPage( i18n("Configure Point Values"), QString::null, BarIcon("edit") );
    QVBoxLayout* layout = new QVBoxLayout( box, 6, 6  );
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );
    t_ResultValues* r = Settings::instance()->pointResults();

    m_point_call = new KIntNumInput( (int)r->rufspiel, box, 10, "m_point_call" );
    m_point_call->setLabel( i18n("Callgame:"), AlignLeft | AlignVCenter );
    m_point_call->setMinValue( 0 );
    m_point_call->setMaxValue( 100 );
    
    m_point_solo = new KIntNumInput( m_point_call, (int)r->solo, box, 10, "m_point_solo" );
    m_point_solo->setLabel( i18n("Solo:"), AlignLeft | AlignVCenter );
    m_point_solo->setMinValue( 0 );
    m_point_solo->setMaxValue( 100 );
    
    m_point_lauf = new KIntNumInput( m_point_solo, (int)r->laufende, box, 10, "m_point_lauf" );
    m_point_lauf->setLabel( i18n("Cards in a row:"), AlignLeft | AlignVCenter );
    m_point_lauf->setMinValue( 0 );
    m_point_lauf->setMaxValue( 100 );
    
    m_point_notrick = new KIntNumInput( m_point_lauf, (int)r->schwarz, box, 10, "m_point_notrick" );
    m_point_notrick->setLabel( i18n("Notrick:"), AlignLeft | AlignVCenter );
    m_point_notrick->setMinValue( 0 );
    m_point_notrick->setMaxValue( 100 );
    
    m_point_schneider = new KIntNumInput( m_point_notrick, (int)r->schneider, box, 10, "m_point_schneider" );
    m_point_schneider->setLabel( i18n("Schneider:"), AlignLeft | AlignVCenter );
    m_point_schneider->setMinValue( 0 );
    m_point_schneider->setMaxValue( 100 );
    
    layout->addWidget( m_point_call );    
    layout->addWidget( m_point_solo );
    layout->addWidget( m_point_lauf );
    layout->addWidget( m_point_notrick );
    layout->addWidget( m_point_schneider );
    layout->addItem( spacer );
    
    delete r;
}

#include "preferencesdlg.moc"
