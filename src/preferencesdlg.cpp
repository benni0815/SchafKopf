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

#include <klocale.h>

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
    addPageResults();
    addPageResultsMoney();
    addPageResultsPoints();
}


PreferencesDlg::~PreferencesDlg()
{
}

void PreferencesDlg::accept()
{
    Settings::instance()->setResultsType( m_radioMoney->isChecked() ? Settings::MONEY: Settings::POINTS );
    
    KDialogBase::accept();
}

void PreferencesDlg::addPageResults()
{
    QFrame* box2 = addPage( i18n("Player Settings"), "" );
    QVBoxLayout* layout2 = new QVBoxLayout( box2, 6, 6  );
    
    QVButtonGroup* group2 = new QVButtonGroup( i18n("Own Player"), box2, "group2" );
    m_label3 = new QLabel( i18n("Name:"), group2, "m_label3" );
    QVButtonGroup* group3 = new QVButtonGroup( i18n("Other Players"), box2, "group3" );
    m_label4 = new QLabel( i18n("Player 1:"), group3, "m_label4" );
        
    QFrame* box3 = addPage( i18n("Rules"), "" );

    
    QFrame* box = addPage( i18n("Results"), "" );
    QVBoxLayout* layout = new QVBoxLayout( box, 6, 6  );
    QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding );

    QVButtonGroup* group = new QVButtonGroup( i18n("Results"), box, "group" );
    m_radioMoney = new QRadioButton( i18n("Count &money"), group );
    m_label1 = new QLabel( i18n("Cost of a call game:"), group, "m_label1" );
    m_spinCallGame = new QSpinBox( group, "m_spinCallGame" );
    m_label2 = new QLabel( i18n("Cost of a solo game:"), group, "m_label2" );
    m_spinSoloGame = new QSpinBox( group, "m_spinSoloGame" );
    m_radioPoints = new QRadioButton( i18n("Count &points"), group );
    

    
    layout->addWidget( group );
    layout2->addWidget( group2 );
    layout2->addWidget( group3 );
    layout->addItem( spacer );
    
    
    // load data from configuration
    if( Settings::instance()->resultsType() == Settings::MONEY )
        m_radioMoney->setChecked( true );
    else
        m_radioPoints->setChecked( true );
}

void PreferencesDlg::addPageResultsMoney()
{

}

void PreferencesDlg::addPageResultsPoints()
{

}

#include "preferencesdlg.moc"
