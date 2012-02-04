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
#include "settings.h"

#include "schafkopfdef.h"

#include <kapplication.h>
#include <kcarddialog.h>
#include <kconfig.h>
#include <kuser.h>
#include <kstandarddirs.h>

#include <qmutex.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <kglobal.h>

Settings* Settings::m_instance = 0;

Settings* Settings::instance() 
{
    if( !m_instance )
        m_instance = new Settings();

    return m_instance;
}

Settings::Settings(QObject *parent, const char *name)
 : QObject(parent, name)
{
    // the mutex causes more problems right now than be useful...
    m_mutex = NULL; // new QMutex();

    //find the directory for the card frontsides
    QString ourCards="cards-OpenTarock";
    cardFrontsideDir=getCardDir()+ourCards+"/";
    if( !KStandardDirs::exists ( cardFrontsideDir ) )
    {
        cardFrontsideDir="/usr/share/apps/carddecks/"+ourCards+"/";
#warning "AUSKOMMENTIERT"
        //if(!KStandardDirs::exists ( cardFrontsideDir ))
            //cardFrontsideDir = KCardDialog::getDefaultCardDir();
    }

    //find the card deck
    QString ourDeck="bavaria_tux2.png";
    cardDeckFile=getCardDir()+"decks/"+ourDeck;
    if( !KStandardDirs::exists ( cardDeckFile ) )
    {
        cardDeckFile="/usr/share/apps/carddecks/decks/"+ourDeck;
#warning "AUSKOMMENTIERT"
        //if(!KStandardDirs::exists ( cardDeckFile ))
            //cardDeckFile = KCardDialog::getDefaultDeck();
    }
}


Settings::~Settings()
{
    delete m_mutex;
    delete m_instance;
}

QString Settings::getCardDir() const
{
    // do not lock private members
#warning "AUSKOMMENTIERT"
    QString dir;// = KCardDialog::getDefaultCardDir();
    int k;
    dir = dir.remove( dir.length()-1, 1 );
    k = dir.findRev( '/', -1 );
    dir = dir.remove( k+1, dir.length()-k );
    return dir;
}

const QString Settings::cardDeck() const
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("CardDeck");
    return config.readEntry("Cards", cardFrontsideDir );
}

const QString Settings::cardBackground() const
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("CardDeck");
    return config.readEntry("Deck", cardDeckFile );
}

void Settings::configureCardDecks( QWidget* parent )
{
    // no mutex locker here as we would lock cardDeck and cardBackground
    
    QString dir = cardDeck();
    QString deck = cardBackground();
#warning "AUSKOMMENTIERT"
   // if (KCardDialog::getCardDeck(deck, dir, parent, KCardDialog::Both ) == QDialog::Accepted)
    {
        //m_mutex->lock();
        KConfigGroup config = KGlobal::config()->group("CardDeck");
        config.writeEntry( "Cards", dir );
        config.writeEntry( "Deck", deck );
        config.sync();
        //m_mutex->unlock();
        emit cardChanged();
    }
}

const QStringList Settings::playerNames() const
{
    QMutexLocker locker( m_mutex );
    
    QString username;
    QStringList list;
    #if KDE_VERSION >= 0x30200
    KUser user;
    username=user.loginName();
    #else
    username="Player 1";
    #endif
    KConfigGroup config = KGlobal::config()->group("PlayerNames");
    list.append( config.readEntry( "Player1", username ) );
    list.append( config.readEntry( "Player2", "Dom" ) );
    list.append( config.readEntry( "Player3", "Lenz" ) );
    list.append( config.readEntry( "Player4", "G.J." ) );
    return list;
}

void Settings::setPlayerNames( const QStringList & names )
{
    QMutexLocker locker( m_mutex );

    int i;
    KConfigGroup config = KGlobal::config()->group("PlayerNames");
    QStringList old = this->playerNames();
    for( i=0;i<PLAYERS;i++ )
        config.writeEntry( QString("Player%1").arg(i+1), names[i] );
    config.sync();
    
    if( old != names )
        emit playerNamesChanged();
}

QList<int> Settings::splitterSizes( int width )
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    QList<int> list;

    config.readEntry( "Splitter", list );
    if( list.isEmpty() )
    {
        list.append( int((width/4)*3) );
        list.append( int(width/4) );
    }

    return list;
}

void Settings::setSplitterSizes( QList<int> list )
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("");
    config.writeEntry( "Splitter", list );
    config.sync();
}

void Settings::setResultsType( int r )
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    config.writeEntry("ResultMode", r );
    config.sync();
    
    emit resultsTypeChanged();
}

int Settings::resultsType() const
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    return config.readEntry("ResultMode", 0 );
}

void Settings::setMoneyResults( const t_ResultValues* r )
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("MoneyValues");
    config.writeEntry( "Solo", r->solo );
    config.writeEntry( "Rufspiel", r->rufspiel );
    config.writeEntry( "Laufende", r->laufende );
    config.writeEntry( "Schneider", r->schneider );
    config.writeEntry( "Schwarz", r->schwarz );
    config.writeEntry( "Ramsch", r->ramsch );
    config.sync();
}

void Settings::setPointResults( const t_ResultValues* r )
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("PointValues");
    config.writeEntry( "Solo", (int)r->solo );
    config.writeEntry( "Rufspiel", (int)r->rufspiel );
    config.writeEntry( "Laufende", (int)r->laufende );
    config.writeEntry( "Schneider", (int)r->schneider );
    config.writeEntry( "Schwarz", (int)r->schwarz );
    config.writeEntry( "Ramsch", (int)r->ramsch );
    config.sync();
}

t_ResultValues* Settings::moneyResults() const
{
    QMutexLocker locker( m_mutex );

    t_ResultValues* r = new t_ResultValues;
    KConfigGroup config = KGlobal::config()->group("MoneyValues");
    r->solo = config.readEntry( "Solo", 0.2 );
    r->rufspiel = config.readEntry( "Rufspiel", 0.1 );
    r->laufende = config.readEntry( "Laufende", 0.1 );
    r->schneider = config.readEntry( "Schneider", 0.1 );
    r->schwarz = config.readEntry( "Schwarz", 0.1 );
    r->ramsch = config.readEntry( "Ramsch", 0.2 );
    return r;
}

t_ResultValues* Settings::pointResults() const
{
    QMutexLocker locker( m_mutex );

    t_ResultValues* r = new t_ResultValues;
    KConfigGroup config = KGlobal::config()->group("PointValues");
    r->solo = config.readEntry( "Solo", 2 );
    r->rufspiel = config.readEntry( "Rufspiel", 1 );
    r->laufende = config.readEntry( "Laufende", 1 );
    r->schneider = config.readEntry( "Schneider", 1 );
    r->schwarz = config.readEntry( "Schwarz", 1 );
    r->ramsch = config.readEntry( "Ramsch", 1 );
    return r;    
}

void Settings::setNoGame( int e )
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    config.writeEntry( "KeinSpiel", e );
    config.sync();
}

int Settings::noGame() const
{
    QMutexLocker locker( m_mutex );
    
    int e = NOGAME_NEUGEBEN;
    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    e = config.readEntry( "KeinSpiel", e );
    
    return e;
}

void Settings::setDoublerHasToPlay( bool b )
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    config.writeEntry( "DoublerHasToPlay", b );
    config.sync();
}

bool Settings::doublerHasToPlay() const
{
    QMutexLocker locker( m_mutex );

    bool b = true;
    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    b = config.readEntry( "DoublerHasToPlay", b );
    return b;
}

void Settings::setDoubleNextGame( bool b )
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    config.writeEntry( "DoubleNextGame", b );
    config.sync();
}

bool Settings::doubleNextGame() const
{
    QMutexLocker locker( m_mutex );

    bool b = true;
    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    b = config.readEntry( "DoubleNextGame", b );
    return b;
}

void Settings::setRearrangeCards( bool b)
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    config.writeEntry( "RearrangeCards", b );
    config.sync();
}

bool Settings::rearrangeCards() const
{
    QMutexLocker locker( m_mutex );

    bool b = true;
    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    b = config.readEntry( "RearrangeCards", b );
    return b;
}

void Settings::setBackgroundImage( QString b)
{
    QMutexLocker locker( m_mutex );

    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    config.writeEntry( "backgroundImage", b );
    config.sync();
}


QString Settings::backgroundImage() const
{
    QMutexLocker locker( m_mutex );
    QString b = KStandardDirs::locate("appdata", "data/background01.jpg");
    KConfigGroup config = KGlobal::config()->group("SchafKopf");
    b = config.readEntry( "backgroundImage", b );
    return b;
}


t_AllowedGames* Settings::allowedGames() const
{
    QMutexLocker locker( m_mutex );
    t_AllowedGames* a = new t_AllowedGames;

    KConfigGroup config = KGlobal::config()->group("Games");
    a->wenz = config.readEntry( "AllowWenz", true );
    a->farb_wenz = config.readEntry( "AllowFarbWenz", true );
    a->geier = config.readEntry( "AllowGeier", true );
    a->farb_geier = config.readEntry( "AllowFarbGeier", false );
    a->dachs = config.readEntry( "AllowDachs", false );

    return a;
}

void Settings::setAllowedGames( const t_AllowedGames* allowed )
{
    QMutexLocker locker( m_mutex );
    KConfigGroup config = KGlobal::config()->group("Games");
    config.writeEntry( "AllowWenz", allowed->wenz );
    config.writeEntry( "AllowFarbWenz", allowed->farb_wenz );
    config.writeEntry( "AllowGeier", allowed->geier );
    config.writeEntry( "AllowFarbGeier", allowed->farb_geier );
    config.writeEntry( "AllowDachs", allowed->dachs );
    config.sync();
}
