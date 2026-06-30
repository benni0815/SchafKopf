// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "player.h"

class HumanPlayer : public Player
{
  Q_OBJECT

    public:
        HumanPlayer(unsigned int id, Game* game);
        ~HumanPlayer() override = default;
        
        void klopfen() override;
        Card *play() override;
        GameInfo* gameInfo(bool force = false) override;
        
    signals:
        void forbiddenCard(unsigned int cardId);
        void questionYesNo(const QString& question, int* result);
        void getCard(int* card);
        void selectGame(GameInfo* gameInfo, const std::vector<int> cardIds, bool force = false);

    private:
        Card* m_card{};
};
