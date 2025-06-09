// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QString>
#include <QList>

class Game;
class GameInfo;
class Player;

/**
This interface implements the basic functions, 
to get game results and find out how many points 
or money who has to give to whom.

@author Dominik Seichter
*/

class Results{
    public:
        Results();
        ~Results() {}
        
        void setLaufende( int l ) { m_laufende = l; }
        void setGame( Game* game );
        
        /** get a message of the game results, which can be displayed
          * to the user.
          */
        QString result();
        double points( Player* player );
        QString formatedPoints( Player* player );
        
        double klopfen(int anzahl, double betrag);

    private:
        void init();
        void parse();
    
    private:        

        QList<Player*> m_winners;
        QList<Player*> m_losers;
        
        GameInfo* m_gameinfo;
        Game* m_game;
        
        int m_laufende;
        
        int m_points;
        bool m_schneider;
        bool m_schwarz;
};
