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
#include "gameinfo.h"
#include "card.h"
#include "cardlist.h"
#include "player.h"

#include <klocale.h>
#include <qstring.h>

GameInfo::GameInfo()
{
    m_color=Card::NOCOLOR;
    m_mode=GameInfo::STICHT;
    m_spieler=0;
    m_mitspieler=0;
}

GameInfo::~GameInfo()
{
}


const QString GameInfo::toString() const
{
    QString s, c, g;
    switch( m_color )
    {
        case Card::NOCOLOR:
            c = i18n("colorless"); break;
        case Card::EICHEL:
            c = i18n("Eichel"); break;
        case Card::GRAS:
            c = i18n("Gras"); break;
        case Card::HERZ:
            c = i18n("Heart"); break;
        case Card::SCHELLEN:
            c = i18n("Schellen"); break;
        default:
            c = QString::null;
            break;
    }
    
    switch( m_mode )
    {
        case GameInfo::STICHT:
            g = i18n("Sticht"); break;
        case GameInfo::GEIER:
            g = i18n("Geier"); break;
        case GameInfo::WENZ:
            g = i18n("Wenz"); break;
        case GameInfo::RAMSCH:
            g = i18n("Ramsch"); break;
        case GameInfo::RUFSPIEL:
        default:            
            g = QString::null;
    }
    
    if( m_mode == GameInfo::RUFSPIEL )
        s = i18n("%1 plays on the %2 Ace.").arg( m_spieler ? m_spieler->name() : QString::null ).arg(c);
    else if( m_mode == GameInfo::RAMSCH )
        s = i18n("Ramsch is played.");
    else
        s = i18n("%1 plays %2 %3.").arg( m_spieler ? m_spieler->name() : QString::null ).arg(c).arg(g);

    return s; 
}

int GameInfo::weight( Card* card ) const
{
    if( istTrumpf( card ) )
    {
        if( card->card()==Card::OBER && mode() != GameInfo::WENZ )
            return 3;
        else if( card->card()==Card::UNTER && mode() != GameInfo::WENZ )
            return 2;
        else if( card->card()==Card::UNTER && mode() == GameInfo::WENZ )
            return 3;
        else
            return 1;
    }
    else 
    {
        if( card->card() == Card::SAU )   
            return 1;
        else
            // TODO: ausser man hat auch die SAU
            return -1;
    }
}

bool GameInfo::istTrumpf(Card *card) const
{
    switch(mode())
    {
        case GameInfo::RUFSPIEL:
        case GameInfo::RAMSCH:
                if(card->card()==Card::OBER || card->card()==Card::UNTER || card->color()==Card::HERZ)
                    return true;
                break;
        case GameInfo::STICHT:
                if(card->card()==Card::OBER || card->card()==Card::UNTER || card->color()==color())
                    return true;
                break;
        case GameInfo::GEIER:
                if(card->card()==Card::OBER || card->color()==color())
                    return true;
                break;
        case GameInfo::WENZ:
                if(card->card()==Card::UNTER || card->color()==color())
                    return true;
        default:
                break;
    };
    return false;
}

bool GameInfo::isAllowed( CardList* cards, int mode, int color )
{
    if( mode==GameInfo::RUFSPIEL ) 
    {
        CardList* sau = cards->FindCards(color, Card::SAU);
        if( color==Card::NOCOLOR || color==Card::HERZ || !sau->isEmpty() ) {
            delete sau;
            return false;
        } else
            delete sau;
    } else if( mode==GameInfo::STICHT && color==Card::NOCOLOR )
        return false;

    return true;
}

bool GameInfo::operator>( GameInfo info )
{
/*
  2.2.3. Hat ein Teilnehmer einen Wenz oder ein Solo und vorher schon ein Spiel angesagt worden, so muß der Einzelspieler erklären "Ich spiele auch". Wenn der Erstmeldende "Gut" sagt, kann der Einzelspieler sein Spiel bekanntgeben. Will der Voransitzende aber selber alleine spielen, so nennt er sein Einzelspiel. Der nachfolgende Teilnehmer kann nun nur noch mit einem höherrangigen Einzelspiel ans Spiel kommen. Denn: Bei gleichrangigen Spielen entscheidet die Sitzreihenfolge, wer zum Zuge kommt. Farbsoli sind untereinander alle gleichberechtigt. Herzsoli geht nicht vor.
*/  
    if( this->mode() == info.mode() )
    {
        // according to the above paragraph,
        // we have only to care about NOCOLOR
        // e.g. Farblos Wenz > Herz Wenz 
        // but Eichel Wenz == Gras Wenz
        return ( this->color() == Card::NOCOLOR );
    } else
        return this->mode() < info.mode();
}
