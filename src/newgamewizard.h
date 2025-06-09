// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

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
