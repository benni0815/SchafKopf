/***************************************************************************
 *   Copyright (C) 2004 by Dominik Seichter                                *
 *   domseichter@web.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef SETTINGS_H
#define SETTINGS_H

#include <qobject.h>

/** a small datatype which contains configureable 
  * values to calculate how much a game was.
  */
struct t_ResultValues {
    double solo;
    double rufspiel;
    double laufende;
    double schwarz;
    double schneider;
    double ramsch;
};

/** a datatype defining which games are allowed to be 
  * played in this game according to the users preferences
  */
struct t_AllowedGames {
    bool wenz;
    bool farb_wenz;
    
    bool geier;
    bool farb_geier;
    
    bool dachs;
};

/* 
// Use as soon as the avatar is implemented
struct t_Players {
    QString name;
    QImage avatar;
};
*/


class Results;
class QMutex;
class QWidget;

/**
@author Dominik Seichter
*/
class Settings : public QObject
{
    Q_OBJECT
    public:
        enum { MONEY, POINTS };
        enum { NOGAME_NEUGEBEN, NOGAME_ALTERSPIELT, NOGAME_RAMSCH };
        
        static Settings* instance();
        
        const QString cardDeck() const;
        const QString cardBackground() const;
        
        const QStringList playerNames() const;
        void setPlayerNames( const QStringList & names );
        
        void configureCardDecks( QWidget* parent = 0 );
        
        QValueList<int> splitterSizes( int width );
        void setSplitterSizes( QValueList<int> list );
        
        void setResultsType( int r );
        int resultsType() const;
        
        void setNoGame( int e );
        /** returns what has to happen when no player wants to play
          */
        int noGame() const;
        
        void setDoublerHasToPlay( bool b );
        bool doublerHasToPlay() const;
        
        void setDoubleNextGame( bool b );
        bool doubleNextGame() const;
        
        void setMoneyResults( const t_ResultValues* r );
        void setPointResults( const t_ResultValues* r );

        void setBackgroundImage( QString b);
        QString backgroundImage() const;

        void setRearrangeCards( bool b);
        bool rearrangeCards() const;

        /** delete after usage
          */
        t_ResultValues* moneyResults() const;
        /** delete after usage
          */
        t_ResultValues* pointResults() const;

        /** delete after usage
          */
        t_AllowedGames* allowedGames() const;
        void setAllowedGames( const t_AllowedGames* allowed );
        
    signals:
        void cardChanged();
        void resultsTypeChanged();
        void playerNamesChanged();
        
    private:
        
        Settings(QObject *parent = 0, const char *name = 0);
        ~Settings();
        
        static Settings* m_instance;
        QString getCardDir() const;
        
        QMutex* m_mutex;
};

#endif
