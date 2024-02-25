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
#include "canvasplayer.h"

#include "canvascard.h"
#include "card.h"
#include "cardlist.h"
#include "player.h"
#include "game.h"
#include "settings.h"

#include <QGraphicsView>
#include <QLabel>
#include <QFont>
#include <QGraphicsProxyWidget>


CanvasPlayer::CanvasPlayer(int i, QGraphicsScene* scene)
    : m_scene( scene )
{
    create();
    m_position = i;
    // TODO: get a correct id! especially important for networking!
    m_id = i;

    m_bubbleLabel = std::make_unique<QLabel>();
    m_bubbleLabel->setStyleSheet("background: lightgrey; font: 12pt; border: 1px solid black;");
    m_bubble = m_scene->addWidget(m_bubbleLabel.get());
    m_bubble->setParent(m_scene);
    m_bubble->hide();

    m_bubbleTimer.setSingleShot(true);
    m_bubbleTimer.setInterval(std::chrono::seconds(1));
    QObject::connect(&m_bubbleTimer, &QTimer::timeout, [this]() { m_bubble->hide(); });

#ifndef SIMULATION_MODE
    // TODO: This should be passed as flag depending on Player::isHuman();
    m_is_human = (m_id == 0);
#else
    m_is_human = false;
#endif // SIMULATION_MODE

}

CanvasPlayer::~CanvasPlayer()
{
    unsigned int i = 0;
    // delete m_items before m_cards
    // as m_items has references to cards in
    // m_cards.
    for(i=0;i<NUMCARDS;i++)
        delete m_items[i];

    delete m_cards;
    delete m_name;
}

void CanvasPlayer::create()
{
    unsigned int z = 0;
    for(z=0;z<NUMCARDS;z++)
    {
        m_items[z] = new CanvasCard();
        m_scene->addItem( m_items[z] );
    }

    m_cards = new CardList;
    m_cards->setAutoDelete( true );

    m_name = new QGraphicsSimpleTextItem();
    m_name->setBrush( QBrush( Qt::white ) );
    m_name->setFont( QFont( "Helvetica", 24 ) );
    m_name->hide();
    m_scene->addItem( m_name );

    m_is_last = false;
    m_has_doubled = false;
    m_player_name = QString();
}

void CanvasPlayer::position()
{
    int x = 0, y = 0;
    int num = 0;
    int w = m_scene->width();
    int h = m_scene->height();
    int offsetl = 0; //Hiermit kann man am linken Rand Platz schaffen fuer z.B. ein Bild des Spielers (Netzwerkmodus).
    int availw = m_scene->width() - 2*DIST - offsetl;
    int cardw = Card::backgroundPixmap()->width();
    int cardh = Card::backgroundPixmap()->height();
    float cardoverlap = 1./8.; //Ueberlapp der Karten im Verhaeltnis zur Kartenbreite

    for( unsigned int z = 0; z < NUMCARDS; z++ )
    {
        CanvasCard* card = m_items[z];
        if(card->isVisible())
            num++;
    }

    if(!Settings::instance()->rearrangeCards())
        num=NUMCARDS;

    if(m_position==1||m_position==3)
        qSwap( cardw, cardh );

    switch( m_position ) 
    {
        case 0:
            if(availw>num*cardw+(num-1))
                x=(w-cardw*num)/2-(num-1)/2-offsetl;
            else
                x=DIST+offsetl;
            y=h-cardh-DIST;

            m_name->setPos( (w-m_name->sceneBoundingRect().width())/2, y-m_name->sceneBoundingRect().height() );
            break;
        case 1:
            x=DIST; 
            y=(h-((cardh*cardoverlap)*(num-1)+cardh))/2; 

            m_name->setPos(x,y-m_name->sceneBoundingRect().height());
            break;
        case 2:
            x=( w-( (cardw*cardoverlap)*(num-1)+cardw ) )/2; //berechnet die Position der linken Seite des Kartenstapels
            y=DIST;

            m_name->setPos( (w-m_name->sceneBoundingRect().width())/2, y+cardh );
            break;
        case 3:
        default:
            x=w-cardw-DIST;
            y=(h-((cardh*cardoverlap)*(num-1)+cardh))/2; 

            m_name->setPos(x, y-m_name->sceneBoundingRect().height());
            break;
    }

    for( unsigned int z = 0; z < NUMCARDS; z++ )
    {
        CanvasCard* card = m_items[z];
        // only move if necessary!
        if(card->isVisible() || !Settings::instance()->rearrangeCards())
        {
            if( x != card->x() || y != card->y() )
            {
                if(num==NUMCARDS)
                    card->setPos( x, y );
                else
                {
                    card->setDestination( x, y );
                    card->animatedMove();
                }
            }
            if(m_position==0)
            {
                if(availw>num*cardw+(num-1))
                    x += cardw+1;
                else
                    x += (availw-cardw)/(num-1);
            }
            else
            {
                if(m_position==2)
                    x += (cardw*cardoverlap);
                else
                    y += (cardh*cardoverlap);
            }
        }
    }

    // swap them back
    if(m_position==1||m_position==3)
        qSwap( cardw, cardh );
}

void CanvasPlayer::init()
{
    m_name->setText( m_player_name );

    if( m_cards->count() )
    {
        for( int z = 0; z < m_cards->count(); z++ )
        {
            CanvasCard *c = m_items[z];
            c->setCard( m_cards->at( z ) );
            c->setZValue( double(-1 - z) );
            c->show();

            if(m_position==1)
                c->setRotation(270);
            else if(m_position==3)
                c->setRotation(90);
    #ifdef CHEAT
            c->setFrontVisible( true );
    #else            
            if( m_is_human )
            {
                if( m_has_doubled )
                    c->setFrontVisible( true );
                else
                {
                    if( m_is_last )
                        c->setFrontVisible( z >= NUMCARDS/2 );
                    else
                        c->setFrontVisible( z < NUMCARDS/2 );
                }
            }
            else
                c->setFrontVisible( false );
    #endif
        }
        m_name->show();
    }
    else
    {
        for( unsigned int z = 0;z<NUMCARDS;z++)
            m_items[z]->hide();

        m_name->hide();
    }
}

CanvasCard* CanvasPlayer::canvasCard( int index )
{
    if( index < 0 || index >= NUMCARDS )
        return NULL;

    CanvasCard* card = m_items[index];
    if( card->isVisible() )
        return card;
    else 
        return NULL;
}

CanvasCard* CanvasPlayer::hasCard( int cardid ) const
{
    for(unsigned int i=0;i<NUMCARDS;i++)
    {
        CanvasCard* card = m_items[i];
        if(card->card()->id() == cardid )
            return card;
    }
    return NULL;
}
        
void CanvasPlayer::cardPlayed( Card* c )
{
    CanvasCard* card = hasCard( c->id() );
    if( card )
    {
        card->hide();
    }
}

void CanvasPlayer::setCards( CardList* cards )
{
    int i;
    for( i = 0; i < NUMCARDS; i++ ) 
    {
        m_items[i]->setCard( NULL );
    }

    m_cards->clear();
    m_cards->appendList( cards );
}

void CanvasPlayer::say( const QString & message, unsigned int )
{
    m_bubbleLabel->setText(message);
    m_bubble->setPos(-10000, -10000);
    m_bubble->show();
    QPointF p;
    switch(m_position)
    {
    case 1:
        p = QPointF(m_name->x(), m_name->y() - 40);
        break;
    case 2:
        p = QPointF(m_name->x() + m_name->sceneBoundingRect().width() / 2 - m_bubble->size().width() / 2, m_name->y() + 40);
        break;
    case 3:
    default:
        p = QPointF(m_scene->width() - m_bubble->size().width() - 40, m_name->y() - 40 );
        break;
    }
    m_bubble->setPos(p);
    m_bubbleTimer.start();
}

void CanvasPlayer::hideBubble()
{
    m_bubble->hide();
}

void CanvasPlayer::setHasDoubled( bool h )
{
    m_has_doubled = h;
}

void CanvasPlayer::setLast( bool l )
{
    m_is_last = l;
}

void CanvasPlayer::setName( const QString & name )
{
    m_player_name = name;
}

bool CanvasPlayer::isHuman() const
{
    return m_is_human;
}

unsigned int CanvasPlayer::id() const
{
    return m_id;
}
