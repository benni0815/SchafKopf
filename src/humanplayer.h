// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "player.h"

class HumanPlayer : public Player
{
    public:
        HumanPlayer(unsigned int id, Game* game);
        ~HumanPlayer();
        
        void klopfen();
        Card *play();
        GameInfo* gameInfo( bool force = false );
        
        int rtti() const { return HUMAN; }        
        
    private:
        Card* m_card;
        CardList* m_allowed;
};
