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
}


Settings::~Settings()
{
    delete m_mutex;
    delete m_instance;
}

const QString Settings::cardDeck() const
{
    QMutexLocker locker( m_mutex );
    
    QString cardDir = getCardDir()+"cards-bavarian-old/";
    KConfig* config = kapp->config();
    config->setGroup("CardDeck");
    if(!KStandardDirs::exists ( cardDir ))
        cardDir = KCardDialog::getDefaultCardDir();
    return config->readEntry("Cards", cardDir );    
}

const QString Settings::cardBackground() const
{
    QMutexLocker locker( m_mutex );

    QString deckCard = getCardDir()+"decks/bavaria_tux.png";
    KConfig* config = kapp->config();
    config->setGroup("CardDeck");
    if(!KStandardDirs::exists ( deckCard ))
        deckCard = KCardDialog::getDefaultDeck();
    return config->readEntry("Deck", deckCard );    
}

QString Settings::getCardDir() const
{
    // do not lock private members
    
    QString dir = KCardDialog::getDefaultCardDir();
    int k;
    dir = dir.remove( dir.length()-1, 1 );
    k = dir.findRev( '/', -1 );
    dir = dir.remove( k+1, dir.length()-k );
    return dir;
}

void Settings::configureCardDecks( QWidget* parent )
{
    // no mutex locker here as we would lock cardDeck and cardBackground
    
    QString dir = cardDeck();
    QString deck = cardBackground();
        
    if (KCardDialog::getCardDeck(deck, dir, parent, KCardDialog::Both ) == QDialog::Accepted)
    {
        //m_mutex->lock();
        KConfig* config = kapp->config();
        config->setGroup("CardDeck");
        config->writeEntry( "Cards", dir );
        config->writeEntry( "Deck", deck );
        kapp->config()->sync();
        //m_mutex->unlock();
        emit cardChanged();
    }
}

const QStringList Settings::playerNames() const
{
    QMutexLocker locker( m_mutex );
    
    QStringList list;
    KUser user;
    KConfig* config = kapp->config();
    config->setGroup("PlayerNames");
    list.append( config->readEntry( "Player1", user.loginName() ) );
    list.append( config->readEntry( "Player2", "Dom" ) );
    list.append( config->readEntry( "Player3", "Lenz" ) );
    list.append( config->readEntry( "Player4", "G.J." ) );
    return list;
}

void Settings::setPlayerNames( const QStringList & names )
{
    QMutexLocker locker( m_mutex );

    int i;
    KConfig* config = kapp->config();
    QStringList old = this->playerNames();
    config->setGroup("PlayerNames");
    for( i=0;i<PLAYERS;i++ )
        config->writeEntry( QString("Player%1").arg(i+1), names[i] );
    config->sync();
    
    if( old != names )
        emit playerNamesChanged();
}

QValueList<int> Settings::splitterSizes( int width )
{
    QMutexLocker locker( m_mutex );
    
    KConfig* config = kapp->config();
    QValueList<int> list;
    
    config->setGroup("SchafKopf");
    list = config->readIntListEntry( "Splitter" );
    if( list.isEmpty() )
    {
        list.append( int((width/4)*3) );
        list.append( int(width/4) );
    }
    
    return list;
}

void Settings::setSplitterSizes( QValueList<int> list )
{
    QMutexLocker locker( m_mutex );
    
    KConfig* config = kapp->config();
    config->writeEntry( "Splitter", list );
    config->sync();
}

void Settings::setResultsType( int r )
{
    QMutexLocker locker( m_mutex );
    
    KConfig* config = kapp->config();
    config->setGroup( "SchafKopf" );
    config->writeEntry("ResultMode", r );
    config->sync();
    
    emit resultsTypeChanged();
}

int Settings::resultsType() const
{
    QMutexLocker locker( m_mutex );
    
    KConfig* config = kapp->config();
    config->setGroup( "SchafKopf" );
    return config->readNumEntry("ResultMode", MONEY );
}

void Settings::setMoneyResults( const t_ResultValues* r )
{
    QMutexLocker locker( m_mutex );
    
    KConfig* config = kapp->config();
    config->setGroup( "MoneyValues" );
    config->writeEntry( "Solo", r->solo );
    config->writeEntry( "Rufspiel", r->rufspiel );
    config->writeEntry( "Laufende", r->laufende );
    config->writeEntry( "Schneider", r->schneider );
    config->writeEntry( "Schwarz", r->schwarz );
    config->writeEntry( "Ramsch", r->ramsch );
    config->sync();
}

void Settings::setPointResults( const t_ResultValues* r )
{
    QMutexLocker locker( m_mutex );
    
    KConfig* config = kapp->config();
    config->setGroup( "PointValues" );
    config->writeEntry( "Solo", (int)r->solo );
    config->writeEntry( "Rufspiel", (int)r->rufspiel );
    config->writeEntry( "Laufende", (int)r->laufende );
    config->writeEntry( "Schneider", (int)r->schneider );
    config->writeEntry( "Schwarz", (int)r->schwarz );
    config->writeEntry( "Ramsch", (int)r->ramsch );
    config->sync();
}

t_ResultValues* Settings::moneyResults() const
{
    QMutexLocker locker( m_mutex );
    
    t_ResultValues* r = new t_ResultValues;
    KConfig* config = kapp->config();
    config->setGroup( "MoneyValues" );
    r->solo = config->readDoubleNumEntry( "Solo", 0.2 );
    r->rufspiel = config->readDoubleNumEntry( "Rufspiel", 0.1 );
    r->laufende = config->readDoubleNumEntry( "Laufende", 0.1 );
    r->schneider = config->readDoubleNumEntry( "Schneider", 0.1 );
    r->schwarz = config->readDoubleNumEntry( "Schwarz", 0.1 );
    r->ramsch = config->readDoubleNumEntry( "Ramsch", 0.2 );
    return r;    
}

t_ResultValues* Settings::pointResults() const
{
    QMutexLocker locker( m_mutex );

    t_ResultValues* r = new t_ResultValues;
    KConfig* config = kapp->config();
    config->setGroup( "PointValues" );
    r->solo = config->readNumEntry( "Solo", 2 );
    r->rufspiel = config->readNumEntry( "Rufspiel", 1 );
    r->laufende = config->readNumEntry( "Laufende", 1 );
    r->schneider = config->readNumEntry( "Schneider", 1 );
    r->schwarz = config->readNumEntry( "Schwarz", 1 );
    r->ramsch = config->readNumEntry( "Ramsch", 1 );
    return r;    
}

void Settings::setNoGame( int e )
{
    QMutexLocker locker( m_mutex );
    
    KConfig* config = kapp->config();
    config->setGroup("SchafKopf");
    config->writeEntry( "KeinSpiel", e );
    config->sync();
}

int Settings::noGame() const
{
    QMutexLocker locker( m_mutex );
    
    int e = NOGAME_NEUGEBEN;
    KConfig* config = kapp->config();
    config->setGroup("SchafKopf");
    e = config->readNumEntry( "KeinSpiel", e );
    
    return e;
}

void Settings::setDoublerHasToPlay( bool b )
{
    QMutexLocker locker( m_mutex );
    
    KConfig* config = kapp->config();
    config->setGroup("SchafKopf");
    config->writeEntry( "DoublerHasToPlay", b );
    config->sync();
}

bool Settings::doublerHasToPlay() const
{
    QMutexLocker locker( m_mutex );
    
    bool b = true;
    KConfig* config = kapp->config();
    config->setGroup("SchafKopf");
    b = config->readBoolEntry( "DoublerHasToPlay", b );
    return b;
}

void Settings::setDoubleNextGame( bool b )
{
    QMutexLocker locker( m_mutex );
    
    KConfig* config = kapp->config();
    config->setGroup("SchafKopf");
    config->writeEntry( "DoubleNextGame", b );
    config->sync();
}

bool Settings::doubleNextGame() const
{
    QMutexLocker locker( m_mutex );
    
    bool b = true;
    KConfig* config = kapp->config();
    config->setGroup("SchafKopf");
    b = config->readBoolEntry( "DoubleNextGame", b );
    return b;
}

void Settings::setRearrangeCards( bool b)
{
    QMutexLocker locker( m_mutex );
    
    KConfig* config = kapp->config();
    config->setGroup("SchafKopf");
    config->writeEntry( "RearrangeCards", b );
    config->sync();
}

bool Settings::rearrangeCards() const
{
    QMutexLocker locker( m_mutex );
    
    bool b = true;
    KConfig* config = kapp->config();
    config->setGroup("SchafKopf");
    b = config->readBoolEntry( "RearrangeCards", b );
    return b;
}

void Settings::setBackgroundImage( QString b)
{
    QMutexLocker locker( m_mutex );
    
    KConfig* config = kapp->config();
    config->setGroup("SchafKopf");
    config->writeEntry( "backgroundImage", b );
    config->sync();
}


QString Settings::backgroundImage() const
{
    QMutexLocker locker( m_mutex );
    QString b = locate("appdata", "data/background01.jpg");
    KConfig* config = kapp->config();
    config->setGroup("SchafKopf");
    b = config->readEntry( "backgroundImage", b );
    return b;
}


t_AllowedGames* Settings::allowedGames() const
{
    QMutexLocker locker( m_mutex );
    t_AllowedGames* a = new t_AllowedGames;
    
    KConfig* config = kapp->config();
    config->setGroup( "Games" );
    a->wenz = config->readBoolEntry( "AllowWenz", true );
    a->farb_wenz = config->readBoolEntry( "AllowFarbWenz", true );
    a->geier = config->readBoolEntry( "AllowGeier", true );
    a->farb_geier = config->readBoolEntry( "AllowFarbGeier", false );
    a->dachs = config->readBoolEntry( "AllowDachs", false );
    
    return a;
}

void Settings::setAllowedGames( const t_AllowedGames* allowed )
{
    QMutexLocker locker( m_mutex );
    KConfig* config = kapp->config();
    config->setGroup( "Games" );
    config->writeEntry( "AllowWenz", allowed->wenz );
    config->writeEntry( "AllowFarbWenz", allowed->farb_wenz );
    config->writeEntry( "AllowGeier", allowed->geier );
    config->writeEntry( "AllowFarbGeier", allowed->farb_geier );
    config->writeEntry( "AllowDachs", allowed->dachs );
    config->sync();
}

#include "settings.moc"
