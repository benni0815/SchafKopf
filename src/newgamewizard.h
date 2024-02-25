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

#include <QWizard>


class QLineEdit;
class QLabel;
class QRadioButton;

/**
@author Christian Kern
*/
class newgamewizard : public QWizard
{
    Q_OBJECT

public:
    enum __gameType {GAME_LOCAL, GAME_NETWORK_CLIENT, GAME_NETWORK_SERVER};
    newgamewizard( QWidget *parent = 0 );
    int getGame();

protected:
    void setup_page_selectgame();
    QWizardPage *page_selectgame;
    QRadioButton *checkLocalGame, *checkNetworkServer, *checkNetworkClient;
    QString getGameInfoString(int gameType);
    QLabel *info;
    void setInfoText(int gameType);

protected:

protected slots:
    void dataChanged(  );

};
