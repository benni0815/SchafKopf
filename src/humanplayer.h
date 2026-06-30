// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "player.h"

class HumanPlayer : public Player
{
    public:
        HumanPlayer(unsigned int id, Game* game);
        ~HumanPlayer() override = default;
        
        void klopfen() override;
        Card *play() override;
        GameInfo* gameInfo(bool force = false) override;
        
        int rtti() const override { return HUMAN; }
        
    private:
        Card* m_card{};
};
