// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWizard>


class QWidget;
class QLineEdit;
class QLabel;
class QRadioButton;
class SelectGameTypeBox;
class SelectGameColorBox;
class CardList;
class GameInfo;

/**
@author Christian Kern
*/
class SelectGameWizard : public QWizard
{
    Q_OBJECT

public:

    SelectGameWizard(bool force, CardList* list, QWidget *parent = 0 );
    ~SelectGameWizard();
    CardList* getCardList();
    GameInfo* gameInfo();

    void reject();

public slots:
    void pageChanged( int id );

protected:
    CardList* m_list;
    bool m_closing;
    bool m_force;
    SelectGameTypeBox *box1;
    SelectGameColorBox *box2;
};
