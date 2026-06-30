// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "player.h"
#include "types.h"


class HumanPlayer : public Player
{
  Q_OBJECT

    public:
        HumanPlayer(unsigned int id, Game* game);
        ~HumanPlayer() override = default;
        
        void klopfen() override;
        Card *play() override;
        std::optional<GameVariant> gameInfo(bool force = false) override;
        
    signals:
        void forbiddenCard(CardId cardId);
        void questionYesNo(const QString& question, int* result);
        void getCard(CardId* card);
        void selectGame(GameVariant* gameInfo, const std::vector<CardId>& cardIds, bool force = false);

    private:
        Card* m_card{};
};
