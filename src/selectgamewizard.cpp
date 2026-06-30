// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "selectgamewizard.h"

#include "selectgametypebox.h"
#include "selectgamecolorbox.h"

#include <QAbstractButton>


SelectGameWizard::SelectGameWizard(bool force, const std::vector<CardId>& cards, QWidget* parent)
: QWizard(parent)
, m_cards(cards)
, m_force(force)
{
    box1 = new SelectGameTypeBox( this );
    box2 = new SelectGameColorBox(m_game, this);
    addPage( box1 );
    addPage( box2 );
    button( QWizard::CancelButton )->setEnabled( !m_force );
    connect(this, &SelectGameWizard::currentIdChanged, this, &SelectGameWizard::pageChanged);
}

const std::vector<CardId>& SelectGameWizard::getCardIds()
{
  return m_cards;
}

GameVariant SelectGameWizard::game()
{
  return m_game;
}

void SelectGameWizard::pageChanged( int id )
{
    if( id == 1 && !m_closing )
    {
        m_game.mode = box1->gameMode();
        box2->statusChanged();
        button( QWizard::FinishButton )->setEnabled( box2->getFinish() );
    }
}

SelectGameWizard::~SelectGameWizard()
{
    m_closing = true;
}

void SelectGameWizard::reject()
{
    // only allow reject if player is not forced to play something
    if( !m_force )
        QWizard::reject();
}
