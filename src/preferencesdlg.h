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
#ifndef PREFERENCESDLG_H
#define PREFERENCESDLG_H

#include <kdialogbase.h>

class KIntNumInput;
class KDoubleNumInput;
class QRadioButton;
class QLabel;
class QSpinBox;

/**
Preferences dialog for schafkopf

@author Dominik Seichter
*/
class PreferencesDlg : public KDialogBase
{
    Q_OBJECT
    public:
        PreferencesDlg(QWidget *parent = 0, const char *name = 0);
        ~PreferencesDlg();
       
    protected:
        void accept();
        
    private:
        void addPagePlayer();
        void addPageResults();
        void addPageResultsMoney();
        void addPageResultsPoints();
        
    private:
        QRadioButton* m_radioMoney;
        QRadioButton* m_radioPoints;
        
        KDoubleNumInput* m_money_solo;
        KDoubleNumInput* m_money_call;
        KDoubleNumInput* m_money_lauf;
        KDoubleNumInput* m_money_notrick;
        KDoubleNumInput* m_money_schneider;
        
        KIntNumInput* m_point_solo;
        KIntNumInput* m_point_call;
        KIntNumInput* m_point_lauf;
        KIntNumInput* m_point_notrick;
        KIntNumInput* m_point_schneider;
};

#endif
