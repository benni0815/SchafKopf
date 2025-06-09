// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cardcache.h>

#include <KSharedConfig>

#include <QObject>


/** a small datatype which contains configureable 
  * values to calculate how much a game was.
  */
struct ResultValues {
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
struct AllowedGames {
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

        KCardCache* cardCache();

        const QString cardDeck() const;

        const QStringList playerNames() const;
        void setPlayerNames( const QStringList & names );

        void loadCardDeck();
        void configureCardDecks( QWidget* parent = 0 );

        QList<int> splitterSizes( int width );
        void setSplitterSizes( QList<int> list );

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

        void setMoneyResults(const ResultValues& r);
        void setPointResults(const ResultValues& r);

        void setBackgroundImage( QString b);
        QString backgroundImage() const;

        void setRearrangeCards( bool b);
        bool rearrangeCards() const;

        ResultValues moneyResults() const;
        ResultValues pointResults() const;

        AllowedGames allowedGames() const;
        void setAllowedGames(const AllowedGames& allowed);

    signals:
        void cardChanged();
        void resultsTypeChanged();
        void playerNamesChanged();

    private:
        Settings( QObject *parent = 0 );
        ~Settings();

        KSharedConfig::Ptr m_config;

        QMutex* m_mutex;

        KCardCache m_cardCache;
};
