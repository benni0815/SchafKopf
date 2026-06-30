// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "types.h"

#include <QWizard>


class QWidget;
class QLineEdit;
class QLabel;
class QRadioButton;
class SelectGameTypeBox;
class SelectGameColorBox;

/**
@author Christian Kern
*/
class SelectGameWizard : public QWizard
{
    Q_OBJECT

public:
    SelectGameWizard(bool force, const std::vector<CardId>& cards, QWidget* parent = nullptr);
    ~SelectGameWizard() override;
    const std::vector<CardId>& getCardIds();
    GameVariant game();

    void reject() override;

public slots:
    void pageChanged( int id );

protected:
    GameVariant m_game;
    std::vector<CardId> m_cards;
    bool m_closing{false};
    bool m_force{false};
    SelectGameTypeBox *box1{};
    SelectGameColorBox *box2{};
};
