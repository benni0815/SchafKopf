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
class KLineEdit;
class QRadioButton;
class QLabel;
class QSpinBox;
class QCheckBox;
class QPushButton;
class KURLRequester;

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
    
    private slots:
        void enableControls();
        
    private:
        void addPagePlayer();
        void addPageResults();
        void addPageResultsMoney();
        void addPageResultsPoints();
        void addPageRules();
        void addPageView();
        void addPageGames();
        
    private:
        QRadioButton* m_radioMoney;
        QRadioButton* m_radioPoints;
        QRadioButton* m_radioThrowAway;
        QRadioButton* m_radioForcedGame;
        QRadioButton* m_radioRamsch;
        
        QCheckBox* m_checkDoublerPlays;
        QCheckBox* m_checkDoubleNextGame;
        QCheckBox* m_checkRearrangeCards;

        QCheckBox* m_games_wenz;
        QCheckBox* m_games_farbwenz;
        QCheckBox* m_games_geier;
        QCheckBox* m_games_farbgeier;
        QCheckBox* m_games_dachs;
        
        KLineEdit* m_p1_name;
        KLineEdit* m_p2_name;
        KLineEdit* m_p3_name;
        KLineEdit* m_p4_name;

        KURLRequester* m_linePathBackground;

        QPushButton* m_pushURLClear;
        
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
