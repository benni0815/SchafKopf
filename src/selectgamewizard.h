// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "cardlist.h"

#include <QWizard>


class QWidget;
class QLineEdit;
class QLabel;
class QRadioButton;
class SelectGameTypeBox;
class SelectGameColorBox;
class GameInfo;

/**
@author Christian Kern
*/
class SelectGameWizard : public QWizard
{
    Q_OBJECT

public:
    SelectGameWizard(bool force, const CardList& list, QWidget* parent = nullptr);
    ~SelectGameWizard() override;
    const CardList& getCardList();
    GameInfo* gameInfo();

    void reject() override;

public slots:
    void pageChanged( int id );

protected:
    CardList m_list;
    bool m_closing{false};
    bool m_force{false};
    SelectGameTypeBox *box1{};
    SelectGameColorBox *box2{};
};
