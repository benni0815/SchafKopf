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
#include "gamecanvas.h"

#include "card.h"
#include "canvascard.h"
#include "player.h"
#include "settings.h"

/** Uncomment this line to make all cards
  * user visible, this is useful for debuggin
  * and improving the AI.
  * But it might cause problems in network mode for cheaters!!
  */
#define CHEAT

#ifdef CHEAT
    #warning "CHEATING ENABLED!!!"
#endif

#define NUMCARDS 8
// DISTANCE from the border
#define DIST 20

GameCanvas::GameCanvas(QCanvas* c, QWidget *parent, const char *name)
 : QCanvasView(c,parent, name)
{
    m_game = NULL;
    
    canvas()->setBackgroundColor( Qt::darkGreen );
    update();
    
    connect( this, SLOT(redrawAll()), Settings::instance(), SLOT(cardChanged()));
}


GameCanvas::~GameCanvas()
{
    // TODO: delete m_items, crashes for some reeson MEMORY LEAK!!!
    // for( unsigned int i = 0; i < PLAYERS; i++ )
    //    delete m_items[i];
}

void GameCanvas::setGame( Game* game )
{
    // no need to delete m_game, because it is a QObject
    m_game = game;
    createCards();
}

void GameCanvas::createCards()
{
    for( unsigned int i = 0; i < PLAYERS; i++ )
        m_items[i] = new QCanvasItemList();
    
    for( unsigned int i = 0; i < PLAYERS; i++ ) {
        Player* player = m_game->findIndex( i );
        for( unsigned int z = 0; z < player->cards()->count(); z++ ) {
            CanvasCard *c = new CanvasCard( player->cards()->at(z), canvas() );
            
            if(i==1)
                c->setRotation(90);
            else if(i==3)
                c->setRotation(270);
#ifdef CHEAT
            c->setFrontVisible( true );
#else            
            c->setFrontVisible( player->rtti() == Player::HUMAN );
#endif
            m_items[i]->append( c );
            
        }
    }
    
    lineupCards();
}

void GameCanvas::lineupCards()
{
    int w = canvas()->width()-DIST;
    int h = canvas()->height()-DIST;
    int cardw = Card::backgroundPixmap()->width();
    int cardh = Card::backgroundPixmap()->height();
    
    for( unsigned int i = 0; i < PLAYERS; i++ ) {
        int x = 0, y = 0;
        QCanvasItemList* list = m_items[i];
    
        if(i==1||i==3)
            qSwap( cardw, cardh );
        
        switch( i ) {
            case 0:
                x=(w-cardw*NUMCARDS)/2;
                y=h-cardh; 
                break;
            case 1:
                x=w-cardw;
                y=(h-((cardh/2)*(NUMCARDS-1)+cardh))/2; 
                break;
            case 2: 
                x=(w-((cardw/2)*(NUMCARDS-1)+cardw))/2;
                y=DIST;
                break;
            case 3:
            default:
                x=DIST; 
                y=(h-((cardh/2)*(NUMCARDS-1)+cardh))/2; 
                break;
        }

        for( unsigned int z = 0; z < list->count(); z++ ) {
            CanvasCard* card = static_cast<CanvasCard*>((*list)[z]); 
            card->move( x, y );
            if(i==0)
                x += cardw;
            else if(i==2)
                x += (cardw/2);
            else
                y += (cardh/2);            
        }
        
        // swap them back
        if(i==1||i==3)
            qSwap( cardw, cardh );
    }
    
    canvas()->update();
}

void GameCanvas::resizeEvent( QResizeEvent * r )
{
// TODO: remove this constants...
    canvas()->resize( this->width() -20, this->height()-20 );
    QCanvasView::resizeEvent( r );
    
    lineupCards();
}

void GameCanvas::redrawAll()
{
    canvas()->setAllChanged();
    canvas()->update();
}

#include "gamecanvas.moc"
