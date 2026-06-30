// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "types.h"

#include <functional>


namespace Cards
{

QString toString(const Card& card);
int getPoints(const Card& card);
int calcPoints(const CardList& cards);

std::vector<CardId> initIds();
std::vector<CardId> toIds(const CardList& cards);
CardList initCards();
CardList randomize();

void remove(CardList& cards, CardId id);
void remove(CardList& cards, const std::vector<CardId>& itemsToRem);

std::optional<Card> find(const CardList& cards, CardId id);
CardList findByColor(const CardList& cards, CardId::Color color);

using EvalFunc = std::function<int(const Card&)>;
void sort(CardList& cards, EvalFunc eval);

} // namespace Cards
