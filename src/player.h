
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
        
        virtual bool doppeln() = 0;
        virtual Card *play() = 0;

        const QString & name() const;
        void setName( const QString & n );
        CardList *cards() const;
        void setCards( CardList *cards);
        const CardList* stiche() const;
        void addStich( CardList stich );
        
        virtual int rtti() const = 0;
        const unsigned int id() const { return m_id; }
        
    protected:
        CardList *m_cards;
        CardList *m_stiche;
        Game *m_game;
        
        QString m_name;
        
        unsigned int m_id;
    private:
        static unsigned int def_id;
};

#endif /* PLAYER_H */
