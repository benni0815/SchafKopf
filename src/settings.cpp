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

Results* Settings::results() const
{
    KConfig* config = kapp->config();
    int r = config->readNumEntry("ResultMode", MONEY );
    if( r == MONEY )
        return new MoneyResults();
    else if( r == POINTS )
        return new PointResults();
        
    return NULL;
}

#include "settings.moc"
