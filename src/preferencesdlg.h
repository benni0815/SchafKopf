// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <KPageDialog>

#include <QStackedWidget>


class QRadioButton;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QPushButton;
class QLineEdit;
class KUrlRequester;

/**
Preferences dialog for schafkopf

@author Dominik Seichter
*/
class PreferencesDlg : public KPageDialog
{
    Q_OBJECT
    public:
        PreferencesDlg(QWidget *parent = 0 );
        ~PreferencesDlg();

    protected:
        void accept();

    private slots:
        void enableControls();
        void disableClearButton(const QString & url);

    private:
        void addPagePlayer();
        void addPageResults();
        void addPageRules();
        void addPageView();
        void addPageGames();

    private:
        QWidget* stackMoney;
        QWidget* stackPoints;
        QStackedWidget* stack;

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

        QLineEdit* m_p1_name;
        QLineEdit* m_p2_name;
        QLineEdit* m_p3_name;
        QLineEdit* m_p4_name;

        KUrlRequester* m_linePathBackground;

        QPushButton* m_pushURLClear;

        QDoubleSpinBox* m_money_solo;
        QDoubleSpinBox* m_money_call;
        QDoubleSpinBox* m_money_lauf;
        QDoubleSpinBox* m_money_notrick;
        QDoubleSpinBox* m_money_schneider;
        QDoubleSpinBox* m_money_ramsch;

        QSpinBox* m_point_solo;
        QSpinBox* m_point_call;
        QSpinBox* m_point_lauf;
        QSpinBox* m_point_notrick;
        QSpinBox* m_point_schneider;
        QSpinBox* m_point_ramsch;
};
