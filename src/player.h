
#ifndef PLAYER_H
#define PLAYER_H

#include <qstring.h>

class Card;
class CardList;
class Game;

class Player
{
    public:
        enum { HUMAN = 0, COMPUTER, NETHUMAN };
        
        Player(CardList *cards,Game* game);
        ~Player();
        
        virtual void klopfen() = 0;
        virtual Card *play() = 0;

        const QString & name() const;
        void setName( const QString & n );
        CardList *cards() const;
        virtual void setCards( CardList *cards);
        CardList* stiche() const;
        void addStich( CardList stich );
        
        virtual int rtti() const = 0;
        const unsigned int id() const { return m_id; }
        
        bool geklopft() const { return m_geklopft; };


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

        unsigned int m_id;
    private:
        static unsigned int def_id;
};

#endif /* PLAYER_H */
