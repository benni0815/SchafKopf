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
        std::optional<GameInfo> gameInfo(bool force = false) override;
        
    signals:
        void forbiddenCard(Card::Id cardId);
        void questionYesNo(const QString& question, int* result);
        void getCard(Card::Id* card);
        void selectGame(GameInfo* gameInfo, const std::vector<Card::Id>& cardIds, bool force = false);

    private:
        Card* m_card{};
};
