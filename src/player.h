
#ifndef PLAYER_H
#define PLAYER_H

#include <qstring.h>

class Card;
class CardList;
class Game;
class GameInfo;

class Player
{
    public:
        enum { HUMAN = 0, COMPUTER, NETHUMAN };

        Player(Game* game);
        ~Player();

        virtual void klopfen();
        virtual Card *play() = 0;
        virtual GameInfo* gameInfo() = 0;
        
        void sortCards();
        
        Game* game() const { return m_game; }

        const QString & name() const;
        void setName( const QString & n );
        CardList *cards() const;
        void setCards( CardList *cards);
        CardList* stiche() const;
        void addStich( CardList stich );

        double points() const { return m_points; }
        void setPoints( double p ) { m_points = p; }
        
        virtual int rtti() const = HUMAN;
        const unsigned int id() const { return m_id; }

        /** returns wether this player want this game to
          * count twice or not
          */
        bool geklopft() const { return m_geklopft; }
        bool hasDoubled() const { return m_has_doubled; }

	CardList *allowedCards();
	void removeTrumpf(CardList* liste);
	bool hasTrumpf(CardList* liste);
	void DebugCardOutput(Card* card);
    protected:
        CardList *m_cards;
        CardList *m_stiche;
        Game *m_game;

        QString m_name;

        bool m_geklopft;
        // is set to true as soon as klopfed()
        // was called
        bool m_has_doubled;
        unsigned int m_id;
    
    private:
        static unsigned int def_id;
        double m_points;
	bool istTrumpf(Card* card);
	CardList* cardsOfSameType(Card* card);
	Card* firstPlayedCard();
	CardList* PlayerCards();
};

#endif /* PLAYER_H */
