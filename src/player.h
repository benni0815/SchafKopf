#pragma once

#include <QString>


class Card;
class CardList;
class Game;
class GameInfo;

class Player
{
    public:
        enum { HUMAN = 0, COMPUTER, NETHUMAN };

        Player(unsigned int id, Game* game);
        
        // this destructor should be virtual
        // but setting it to virtual makes everything
        // crash. Seems to be a good lenz-job ;) ...
        virtual ~Player();

        virtual void init() {};
        /** set m_geklopft to true in this function if the player wants to double
          * and to false if not. 
          */
        virtual void klopfen();
        virtual Card *play() = 0;
        /** if @p force is true the player has to play wether he wants or not
          * a valid GameInfo object has to be returned in this case!
          */
        virtual GameInfo* gameInfo( bool force = false ) = 0;
        
        void sortCards();
        
        Game* game() const { return m_game; }

        const QString & name() const;
        void setName( const QString & n );
        CardList *cards() const;
        void setCards( CardList *cards);
        CardList* stiche() const;
        void addStich( CardList stich );
        inline bool isLast() const { return m_last; }
        inline void setLast( bool b ) { m_last = b; }
        
        double points() const { return m_points; }
        void setPoints( double p ) { m_points = p; }
        
        virtual int rtti() const = 0;
        unsigned int id() const { return m_id; }

        /** returns wether this player want this game to
          * count twice or not
          */
        bool geklopft() const { return m_geklopft; }

        /** returns wether this player is part of the player party or not
          */
        bool isPlayer() const;

        CardList *allowedCards();
        void removeTrumpf(CardList* liste);
        bool hasTrumpf(CardList* liste);

    protected:
        CardList *m_cards;
        CardList *m_stiche;
        Game *m_game;

        QString m_name;

        // is this the last player in the current round
        // needed to make sure that this player sees the last
        // four cards to double
        bool m_last;
        bool m_geklopft;
        // is set to true as soon as klopfed()
        // was called
        bool m_has_doubled;
        unsigned int m_id;

        CardList* cardsOfSameType(Card* card);
        Card* firstPlayedCard();
        bool istTrumpf(Card* card);

    private:
        double m_points;

        CardList* PlayerCards();
};
