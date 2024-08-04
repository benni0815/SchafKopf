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
#pragma once

#include <QWizardPage>


class QWidget;
class QLineEdit;
class QPushButton;
class QRadioButton;
class GameInfo;
class Card;
class SelectGameWizard;
class GameInfo;


/**
@author Christian Kern
*/
class SelectGameColorBox : public QWizardPage
{
    Q_OBJECT

public:
    SelectGameColorBox( QWidget *parent = 0 );
    void cleanGameInfo();

    ~SelectGameColorBox() = default;
    void setGameInfo(GameInfo* info);
    GameInfo* getGameInfo();
    bool getFinish();

protected:
    GameInfo* m_gameinfo;
    QRadioButton* checkFarblos;
    QRadioButton* checkEichel;
    QRadioButton* checkGras;
    QRadioButton* checkHerz;
    QRadioButton* checkSchellen;
    QPushButton* labelEichel;
    QPushButton* labelGras;
    QPushButton* labelHerz;
    QPushButton* labelSchellen;

    SelectGameWizard* parent;
    void setStatus(bool Eichel, bool Gras, bool Herz, bool Schellen, bool Farblos);
    void checkFirstVisible();
    bool m_finish;
    void statusChanged();
    int getColor();

protected slots:
    void colorChanged();
    void colorChangedToEichel();
    void colorChangedToGras();
    void colorChangedToHerz();
    void colorChangedToSchellen();
};
