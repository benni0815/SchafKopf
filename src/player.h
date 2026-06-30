#pragma once

#include "types.h"

#include <QObject>
#include <QString>


struct Card;
class Game;

class Player : public QObject
{
  Q_OBJECT

public:
  Player(unsigned int id, const Game& game);

  ~Player() override = default;

  virtual void init() {}

  /// Sets m_geklopft to true in this function if the player wants to double and to false if not.
  virtual void klopfen() = 0;

  virtual Card play() = 0;

  /// If @p force is true the player has to play wether he wants or not. A valid GameVariant has to be returned in this case!
  virtual std::optional<GameVariant> gameInfo(bool force = false) = 0;

  void sortCards();

  const QString& name() const;
  void setName(const QString& name);
  const CardList& cards() const;
  void setCards(const CardList& cards);
  void removeCard(const CardId& id);
  const CardList& stiche() const;
  void clearStiche();
  void addStich(const CardList& stich);
  inline bool isLast() const { return m_last; }
  inline void setLast(bool b) { m_last = b; }

  double points() const { return m_points; }
  void setPoints(double p) { m_points = p; }

  unsigned int id() const { return m_id; }

  /// Returns wether this player want this game to count twice or not.
  bool geklopft() const { return m_geklopft; }

  /// Returns wether this player is part of the player party or not.
  bool isPlayer() const;

  CardList allowedCards();
  void removeTrumpf(CardList& liste);

signals:
  void gotCards(unsigned int playerId, const std::vector<CardId>& cardIds);
  void doubled(unsigned int playerId);
  void nameChanged(unsigned int playerId, const QString& name);

protected:
  CardList m_cards;
  CardList m_stiche;
  const Game& m_game;

  QString m_name;

  /// Is this the last player in the current round? Needed to make sure that this player sees the last four cards to double.
  bool m_last{false};

  bool m_geklopft{false};

  /// Is set to true as soon as klopfed() was called.
  bool m_has_doubled{false};

  unsigned int m_id{0};

  CardList cardsOfSameType(const std::optional<Card>& card);
  std::optional<Card> firstPlayedCard();
  bool istTrumpf(const Card& card);

private:
  double m_points{0};

  CardList PlayerCards();
};
