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
        case GameInfo::DACHS:
            g = i18n("Badger"); break;
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
        case GameInfo::DACHS:
                if(card->card()==Card::SAU || card->card()==Card::ZEHN)
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
  2.2.3. Hat ein Teilnehmer einen Wenz oder ein Solo und vorher schon ein Spiel angesagt worden, so mu�der Einzelspieler erkl�en "Ich spiele auch". Wenn der Erstmeldende "Gut" sagt, kann der Einzelspieler sein Spiel bekanntgeben. Will der Voransitzende aber selber alleine spielen, so nennt er sein Einzelspiel. Der nachfolgende Teilnehmer kann nun nur noch mit einem h�errangigen Einzelspiel ans Spiel kommen. Denn: Bei gleichrangigen Spielen entscheidet die Sitzreihenfolge, wer zum Zuge kommt. Farbsoli sind untereinander alle gleichberechtigt. Herzsoli geht nicht vor.
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

int GameInfo::evalCard(Card *card, GameInfo *gameinfo)
{
	/* Not very nice. Try to find a better way */
	
	int *l_trumpf;
	int *l_cards;
	int l_trumpf_std[]={ Card::OBER, Card::UNTER };
	int l_trumpf_geier=Card::OBER;
	int l_trumpf_wenz=Card::UNTER;
    int l_trumpf_dachs[]={ Card::SAU, Card::ZEHN };
	int l_cards_std[]={ Card::SAU, Card::ZEHN, Card::KOENIG, Card::NEUN, Card::ACHT, Card::SIEBEN, Card::NOSTICH };
	int l_cards_geier[]={ Card::SAU, Card::ZEHN, Card::KOENIG, Card::UNTER, Card::NEUN, Card::ACHT, Card::SIEBEN };
	int l_cards_wenz[]={ Card::SAU, Card::ZEHN, Card::KOENIG, Card::OBER, Card::NEUN, Card::ACHT, Card::SIEBEN };
    int l_cards_dachs[]={ Card::KOENIG, Card::OBER, Card::UNTER, Card::NEUN, Card::ACHT, Card::SIEBEN, Card::NOSTICH };
	int l_colors[4];
	int trumpf_index=-1;
	int cards_index=-1;
	int colors_index=-1;
	int trumpf_cnt;
	int col;
	int i, a;
	
	switch( gameinfo->mode() )
    {
		case GameInfo::RAMSCH:
        case GameInfo::RUFSPIEL:
			trumpf_cnt=2;
			l_trumpf=l_trumpf_std;
			l_cards=l_cards_std;
			col=Card::HERZ;
			
			break;
        case GameInfo::STICHT:
			trumpf_cnt=2;
			l_trumpf=l_trumpf_std;
			l_cards=l_cards_std;
			col=gameinfo->color();
			break;
		case GameInfo::GEIER:
			trumpf_cnt=1;
        	l_trumpf=&l_trumpf_geier;
			l_cards=l_cards_geier;
			col=gameinfo->color();
			break;
		case GameInfo::WENZ:
			trumpf_cnt=1;
        	l_trumpf=&l_trumpf_wenz;
			l_cards=l_cards_wenz;
			col=gameinfo->color();
            break;
		case GameInfo::DACHS:
            trumpf_cnt=2;
            l_trumpf=l_trumpf_dachs;
            l_cards=l_cards_dachs;
            col=Card::NOCOLOR;
            break;
        default:
        	break;
	}
	l_colors[0]=col;
	for(i=0, a=1;i<4;i++)
	{
		if(col==i)
			continue;
		else
			l_colors[a++]=i;
	}
	for(i=0;i<trumpf_cnt;i++)
	{
		if(card->card()==l_trumpf[i])
		{
			trumpf_index=i;
			break;
		}
	}
	for(i=0;i<7;i++)
	{
		if(card->card()==l_cards[i])
		{
			cards_index=i;
			break;
		}
	}
	for(i=0;i<4;i++)
	{
		if(card->color()==l_colors[i])
		{
			colors_index=i;
			break;
		}
	}
	if(trumpf_index!=-1)
		return 32-(trumpf_index*4+card->color());
	return 32-(trumpf_cnt*4+colors_index*(8-trumpf_cnt)+cards_index);
}

int GameInfo::laufende()
{
    unsigned int i = 0;
    int l = 0;
    CardList* all = new CardList();
    CardList laufend = *(spieler()->cards());
    if( mitspieler() )
        laufend.appendList( mitspieler()->cards() );

    all->init();
    all->setAutoDelete( false );
    all->sort( (eval_func)evalCard, (void *)this);
    all->setAutoDelete( true );
        
    for( i=all->count()-1;i>=0;--i)
    {
        Card* c = all->at( i );
        CardList* list = laufend.FindCards( c->color(), c->card() );
        if( list->count() && ( l>0 || i == all->count()-1 ) )
            l++;
        else if( !list->count() && l<0 || i == all->count() -1 )
            l--;
        else
        {
            delete list;
            break;
        }
        
        delete list;
    }

    delete all;   
    qDebug("LAUFENDE=%i", l);
    return l;
}       
