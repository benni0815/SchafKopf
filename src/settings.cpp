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

#include "moneyresults.h"
#include "pointresults.h"
#include "schafkopfdef.h"

#include <kapplication.h>
#include <kcarddialog.h>
#include <kconfig.h>
#include <kuser.h>

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
}


Settings::~Settings()
{
    delete m_instance;
}

const QString Settings::cardDeck() const
{
    KConfig* config = kapp->config();
    config->setGroup("CardDeck");
    return config->readEntry("Cards", KCardDialog::getDefaultCardDir() );    
}

const QString Settings::cardBackground() const
{
    KConfig* config = kapp->config();
    config->setGroup("CardDeck");
    return config->readEntry("Deck", KCardDialog::getDefaultDeck() );    
}

void Settings::configureCardDecks( QWidget* parent )
{
    QString dir = cardDeck();
    QString deck = cardBackground();
        
    if (KCardDialog::getCardDeck(deck, dir, parent, KCardDialog::Both ) == QDialog::Accepted)
    {
        KConfig* config = kapp->config();
        config->setGroup("CardDeck");
        config->writeEntry( "Cards", dir );
        config->writeEntry( "Deck", deck );
        kapp->config()->sync();
        
        emit cardChanged();
    }
}

const QStringList Settings::playerNames() const
{
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
    KConfig* config = kapp->config();
    config->writeEntry( "Splitter", list );
    config->sync();
}

void Settings::setResultsType( int r )
{
    KConfig* config = kapp->config();
    config->setGroup( "SchafKopf" );
    config->writeEntry("ResultMode", r );
    config->sync();
    
    emit resultsTypeChanged();
}

int Settings::resultsType() const
{
    KConfig* config = kapp->config();
    config->setGroup( "SchafKopf" );
    return config->readNumEntry("ResultMode", MONEY );
}

Results* Settings::results() const
{
    int r = resultsType();
    if( r == MONEY )
        return new MoneyResults();
    else if( r == POINTS )
        return new PointResults();
        
    return NULL;
}

void Settings::setMoneyResults( const t_ResultValues* r )
{
    KConfig* config = kapp->config();
    config->setGroup( "MoneyValues" );
    config->writeEntry( "Solo", r->solo );
    config->writeEntry( "Rufspiel", r->rufspiel );
    config->writeEntry( "Laufende", r->laufende );
    config->writeEntry( "Schneider", r->schneider );
    config->writeEntry( "Schwarz", r->schwarz );
    config->sync();
}

void Settings::setPointResults( const t_ResultValues* r )
{
    KConfig* config = kapp->config();
    config->setGroup( "PointValues" );
    config->writeEntry( "Solo", (int)r->solo );
    config->writeEntry( "Rufspiel", (int)r->rufspiel );
    config->writeEntry( "Laufende", (int)r->laufende );
    config->writeEntry( "Schneider", (int)r->schneider );
    config->writeEntry( "Schwarz", (int)r->schwarz );
    config->sync();
}

t_ResultValues* Settings::moneyResults() const
{
    t_ResultValues* r = new t_ResultValues;
    KConfig* config = kapp->config();
    config->setGroup( "MoneyValues" );
    r->solo = config->readDoubleNumEntry( "Solo", 0.2 );
    r->rufspiel = config->readDoubleNumEntry( "Rufspiel", 0.1 );
    r->laufende = config->readDoubleNumEntry( "Laufende", 0.1 );
    r->schneider = config->readDoubleNumEntry( "Schneider", 0.1 );
    r->schwarz = config->readDoubleNumEntry( "Schwarz", 0.1 );
    return r;    
}

t_ResultValues* Settings::pointResults() const
{
    t_ResultValues* r = new t_ResultValues;
    KConfig* config = kapp->config();
    config->setGroup( "PointValues" );
    r->solo = config->readNumEntry( "Solo", 2 );
    r->rufspiel = config->readNumEntry( "Rufspiel", 1 );
    r->laufende = config->readNumEntry( "Laufende", 1 );
    r->schneider = config->readNumEntry( "Schneider", 1 );
    r->schwarz = config->readNumEntry( "Schwarz", 1 );
    return r;    
}

void Settings::setNoGame( int e )
{
    KConfig* config = kapp->config();
    config->setGroup("SchafKopf");
    config->writeEntry( "KeinSpiel", e );
    config->sync();
}

int Settings::noGame() const
{
    int e = NOGAME_NEUGEBEN;
    KConfig* config = kapp->config();
    config->setGroup("SchafKopf");
    e = config->readNumEntry( "KeinSpiel", e );
    
    return e;
}

#include "settings.moc"
