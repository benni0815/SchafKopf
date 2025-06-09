// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

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
