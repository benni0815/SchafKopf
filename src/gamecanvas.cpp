// SPDX-FileCopyrightText: Copyright 2004 by Dominik Seichter <domseichter@web.de>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "gamecanvas.h"

#include "card.h"
#include "canvascard.h"
#include "canvasplayer.h"
#include "cardlist.h"
#include "settings.h"

#include <KLocalizedString>

#include <QTimer>
#include <QImage>
#include <QMouseEvent>


class CanvasText : public QGraphicsSimpleTextItem
{
    public:
        CanvasText( const QString & t, const QFont & f, QGraphicsItem *parent = 0 )
            : QGraphicsSimpleTextItem( t, parent )
        {
            setFlag( QGraphicsItem::ItemIsSelectable );
            setFont( f );
            setBrush( QBrush( Qt::yellow ) );
        }

        int type() const
        {
            return UserType + 1;
        }
};

GameCanvas::GameCanvas(QGraphicsScene* gs, QWidget *parent )
 : QGraphicsView( gs, parent )
{
    unsigned int i = 0;
    for(;i<PLAYERS;i++)
    {
        m_stich[i] = new CanvasCard();
        scene()->addItem( m_stich[i] );
        m_players[i]=new CanvasPlayer( i, scene() );
    }
    
    m_item = NULL;
        
    QFont f( "Helvetica", 24 );
    
    m_message = new QGraphicsSimpleTextItem();
    m_message->setFont( f );
    m_message->setBrush( QBrush( Qt::yellow ) );

    m_yes = new CanvasText(i18n("Yes"), f, m_message);
    m_no = new CanvasText(i18n("No"), f, m_message);
    m_ok = new CanvasText(i18n("OK"), f, m_message);

    scene()->addItem( m_message );

    m_message->hide();
    m_yes->hide();
    m_no->hide();
    m_ok->hide();

    // Does not work :-(
    /*
    m_message->setZ( 100 );
    m_yes->setZ( 100 );
    m_no->setZ( 100 );
    m_ok->setZ( 100 );
    */
    
    m_result = 0;
    setFocusPolicy( Qt::StrongFocus );
    
    setBackgroundBrush( QBrush( Qt::darkGreen ) );
    
    loadOK = ImgBack.load( Settings::instance()->backgroundImage() );

    QTimer *timer = new QTimer( this) ;
    connect(timer, &QTimer::timeout, scene(), &QGraphicsScene::advance);
    timer->start( 30 );
    update();
    
    connect(Settings::instance(), &Settings::cardChanged, this, &GameCanvas::positionObjects);
    connect(this, &GameCanvas::clicked, this, &GameCanvas::yesNoClicked);
    
    m_focus_list.append( m_yes );
    m_focus_list.append( m_no );
    m_focus_list.append( m_ok );
    for( i=0;i<NUMCARDS;i++ )
        m_focus_list.append( m_players[0]->canvasCard( i ) );
}    

GameCanvas::~GameCanvas()
{
    for( unsigned int i = 0; i < PLAYERS; i++ )
    {
        if( m_stich[i]->card() )
            delete m_stich[i]->card();
            
        delete m_stich[i];
        delete m_players[i];
    }
}

void GameCanvas::cardForbidden( int cardid )
{
    for(unsigned int z=0;z<PLAYERS;z++)
    {
        CanvasCard* c = m_players[z]->hasCard( cardid );
        if( c )            
        {
            c->forbidden();
            break;
        }
    }
}

void GameCanvas::positionObjects()
{
    for( unsigned int i = 0; i < PLAYERS; i++ )
        m_players[i]->position();
    
    for( unsigned int i = 0; i < PLAYERS; i++ ) 
    {
        QPoint p = getStichPosition(i);
        m_stich[i]->setPos( (int)p.x(), (int)p.y() );
    }

    scene()->update();
}

QPoint GameCanvas::getStichPosition( int player )
{
    QPoint p;
    int w = scene()->width();//-DIST;
    int h = scene()->height();//-DIST;
    int cardw = Card::backgroundPixmap()->width();
    int cardh = Card::backgroundPixmap()->height();
    int stichcanvasw = scene()->width() - (2*DIST) - (2*cardh) - 50;
    int stichcanvash = scene()->height() - (2*DIST) - (2*cardh) - 50;
    int cx = w/2;
    int cy = h/2;
    
    switch( player ) 
    {
    case 0:
        if(stichcanvash>(2*cardh-20))
            p = QPoint(cx-(cardw/2)-5, cy-10 );
        else
            p = QPoint(cx-(cardw/2)-5, cy+(stichcanvash/2)-cardh);
        break;
    case 1:
        if(stichcanvasw>(2*cardh-20))
            p = QPoint(cx-cardh+10, cy-(cardw/2)-5);
        else
            p = QPoint(cx-(stichcanvasw/2), cy-(cardw/2)-5);
        break;
    case 2:
        if(stichcanvash>(2*cardh-20))
            p = QPoint(cx-(cardw/2)+5, cy-cardh+10 );
        else
            p = QPoint(cx-(cardw/2)+5, cy-(stichcanvash/2));
        break;
    case 3:
    default:
        if(stichcanvasw>(2*cardh-20))
            p = QPoint(cx-10, cy-(cardw/2)+5);
        else
            p = QPoint(cx+(stichcanvasw/2)-cardh, cy-(cardw/2)+5);
        break;
    };

    return p;
}

int GameCanvas::getStichRotation( int player )
{
    int r;
    switch( player ) 
    {
    case 0:
        r=0; break;
    case 1:
        r=90; break;
    case 2:
        r=180; break;
    case 3:
    default:
        r=270; break;
    };
    return r;
}

int GameCanvas::getCard()
{
    CanvasPlayer* human = humanPlayer();
    CanvasCard* c = NULL;
    unsigned int i;

    connect(this, &GameCanvas::clicked, this, &GameCanvas::cardClicked);
    m_result = -1;

    if( hasFocus() && human )
        for( i=0;i<NUMCARDS;i++)
        {
            c = human->canvasCard( i );
            if( c )
            {
                c->setSelected( true );
                break;
            }
        }

    m_loop.exec();

    return m_result;
}

void GameCanvas::cardClicked( QGraphicsItem* item )
{
    if( item )
        item->setSelected( false );

    if( item->type() == CANVASCARD )
    {
        CanvasCard* card = static_cast<CanvasCard*>(item);

        for( unsigned int i = 0; i < PLAYERS; i++ ) 
        {
            if( m_players[i]->isHuman() && m_players[i]->hasCard( card->card()->id() ) )
            {
                m_result = card->card()->id();
                disconnect(this, &GameCanvas::clicked, this, &GameCanvas::cardClicked);

                // be sure that focusOutEvent does not use its parameter
                focusOutEvent( NULL );
                m_loop.exit();
            }
        }
    }
}

void GameCanvas::slotPlayerPlayedCard( unsigned int player, int cardid )
{
    unsigned int i=0;
    CanvasCard* card = 0;
    for(i=0;i<PLAYERS;i++) {
        if( m_players[i]->id() == player )
        {
            card = m_players[i]->hasCard( cardid );
            break;
        }
    }

    if( card )
    {
        m_players[player]->cardPlayed( card->card() );
        if(Settings::instance()->rearrangeCards())
            m_players[player]->position();

        CanvasCard* stich = m_stich[player];
        if( stich->card() )
            delete stich->card();
        stich->setCard( new Card( card->card()->id() ) );

        int r = getStichRotation(player);
        stich->setRotation( r );

        // find out the correct z value of this card so that
        // a new card is always placed on top of the earlier ones
        int z = 0;
        for(unsigned int i=0;i<PLAYERS;i++)
        {
            if( m_stich[i]->isVisible() )        
                z++;
        }

        stich->setZValue( z );
        stich->setFrontVisible( true );
        stich->show();
    }
}

void GameCanvas::slotPlayerMadeStich(unsigned int)
{
    unsigned int i=0;
    for(;i<PLAYERS;i++)
        m_stich[i]->hide();
}

void GameCanvas::resizeEvent( QResizeEvent * r )
{
    scene()->setSceneRect( 0, 0, this->width() -2, this->height()-2 );
    QGraphicsView::resizeEvent( r );
    
    positionObjects();
    resizeBackground();
}

void GameCanvas::resizeBackground()
{
    QImage ImgBack2;
    if(loadOK)
    {
        ImgBack2 = ImgBack.scaled( scene()->width(), scene()->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
        setBackgroundBrush( QBrush( ImgBack2 ) );
    }

}

void GameCanvas::updateBackground()
{
    loadOK = ImgBack.load( Settings::instance()->backgroundImage() );
    resizeBackground();
    if(!loadOK)
        setBackgroundBrush( QBrush( Qt::darkGreen ) );
}

void GameCanvas::redrawPlayers()
{
    unsigned int i = 0;

    for(i=0;i<PLAYERS;i++)
    {
        m_players[i]->init();
        m_stich[i]->hide();
    }
}

void GameCanvas::mousePressEvent(QMouseEvent* e)
{
    m_item = NULL;
    double z = 0;
    bool first=true;
    if( e->button() == Qt::LeftButton )
    {
        QList<QGraphicsItem*> list = scene()->items();
        for( int i = 0; i < list.count(); i++ )
            if( list[i]->sceneBoundingRect().contains( e->pos() ) && list[i]->isVisible() )
            {
                if(first)
                {
                    first=false;
                    z=list[i]->zValue();
                    m_item = list[i];
                }
                //qDebug("z-wert:"+QString::number(list[i]->zValue()));
                if(list[i]->zValue()>z)
                {
                    z=list[i]->zValue();
                    m_item = list[i];
                }
            }
    }
}

void GameCanvas::mouseReleaseEvent(QMouseEvent* e)
{
    if( e->button() == Qt::LeftButton && m_item ) {
        emit clicked( m_item );
        m_item = NULL;
    }
}

void GameCanvas::keyPressEvent(QKeyEvent* e)
{
    int z = 0;

    if( e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return )
    {
        for( int i=0;i<m_focus_list.count();i++ )
            if( m_focus_list[i]->isSelected() )
            {
                if( m_focus_list[i]->type() == QGraphicsItem::UserType + 1 )
                    yesNoClicked( m_focus_list[i] );
                else if( m_focus_list[i]->type() == CANVASCARD )
                    cardClicked( m_focus_list[i] );
            }
    }
    else if( e->key() == Qt::Key_Right )
    {
        for( int i=0;i<m_focus_list.count();i++ )
            if( m_focus_list[i]->isSelected() )
            {
                for( z=i+1;z<m_focus_list.count();z++ )
                    if( m_focus_list[z]->isVisible() )
                    {
                        if( m_message->isVisible() && m_focus_list[z]->type() == CANVASCARD )
                            continue;

                        m_focus_list[i]->setSelected( false );
                        m_focus_list[z]->setSelected( true );
                        break;
                    }
                break;
            }
    }
    else if( e->key() == Qt::Key_Left )
    {
        for( int i=0;i<m_focus_list.count();i++ )
            if( m_focus_list[i]->isSelected() )
            {
                for( z=i-1;z>=0;z-- )
                    if( m_focus_list[z]->isVisible() )
                    {
                        if( m_message->isVisible() && m_focus_list[z]->type() == CANVASCARD )
                            continue;

                        m_focus_list[i]->setSelected( false );
                        m_focus_list[z]->setSelected( true );
                        break;
                    }
                break;
            }
    }
}

void GameCanvas::focusInEvent(QFocusEvent*)
{
    int i;
    for(i=0;i<m_focus_list.count();i++)
        if( m_focus_list[i]->isVisible() )
        {
            m_focus_list[i]->setSelected( true );
            break;
        }
}

void GameCanvas::focusOutEvent(QFocusEvent*)
{
    int i;
    for(i=0;i<m_focus_list.count();i++)
        if( m_focus_list[i]->isSelected() )
        {
            m_focus_list[i]->setSelected( false );
            break;
        }
}

bool GameCanvas::questionYesNo( const QString & message )
{
    m_result = NO;

    m_message->setText( message );
    m_message->setPos( scene()->sceneRect().center() - m_message->boundingRect().center() );

    m_yes->setPos( m_message->boundingRect().center() + QPoint( -100, m_message->boundingRect().height()/2. + 50 ) - m_yes->boundingRect().center());
    m_no->setPos( m_message->boundingRect().center() + QPoint( 100, m_message->boundingRect().height()/2. + 50 ) - m_no->boundingRect().center() );

    m_message->show();
    m_yes->show();
    m_no->show();

    positionObjects();
    
    scene()->update();
    if( hasFocus() )
        m_yes->setSelected( true );
    m_loop.exec();

    m_message->hide();
    m_yes->hide();
    m_no->hide();
    
    scene()->update();
        
    return ( m_result == YES );
}

void GameCanvas::yesNoClicked( QGraphicsItem* item )
{
    if( item )
        item->setSelected( false );

    if( m_message && ((m_yes && m_no) || m_ok ))
    {
        if( item == m_yes )
        {
            m_result = YES;
            m_loop.exit();
        }
        else if( item == m_no )
        {
            m_result = NO;
            m_loop.exit();
        }
        else if( item == m_ok )
        {
            m_result = NO;
            m_loop.exit();
            for(int i=0;i<PLAYERS;i++)
            {
                m_players[i]->hideBubble();
            }
        }
    }
}

void GameCanvas::information( const QString & message )
{
    m_message->setText( message );

    m_message->setPos( scene()->sceneRect().center() - m_message->boundingRect().center() );
    m_ok->setPos( m_message->boundingRect().center() - m_ok->boundingRect().center() + QPoint( 0, m_message->boundingRect().height()/2. + 50 ) );

    m_message->show();
    m_ok->show();

    positionObjects();
    
    scene()->update();
    if( hasFocus() )
        m_ok->setSelected( true );
    m_loop.exec();

    m_message->hide();
    m_ok->hide();
        
    scene()->update();
        
    return;
}

void GameCanvas::Bubble( const QString & message, unsigned int playerid )
{
    m_players[playerid]->say(message, playerid);
}

/*void GameCanvas::drawContents ( QPainter * p, int cx, int cy, int cw, int ch )
{

}*/

void GameCanvas::playerHasDoubled( unsigned int id, bool value )
{
    int i;
    for(i=0;i<PLAYERS;i++)
        if( m_players[i]->id() == id )
        {
            m_players[i]->setHasDoubled( value );
        }
}

void GameCanvas::playerIsLast( unsigned int id )
{
    int i;
    for(i=0;i<PLAYERS;i++)
        if( m_players[i]->id() == id )
        {
            m_players[i]->setLast( true );
            // TODO: init is not a good name for this function...
            m_players[i]->init();
        }
}

void GameCanvas::setPlayerName( unsigned int id, const QString & name )
{
    int i;
    for(i=0;i<PLAYERS;i++)
        if( m_players[i]->id() == id )
            m_players[i]->setName( name );

}

void GameCanvas::resetPlayers()
{
    int i;
    
    for(i=0;i<PLAYERS;i++)
    {
        m_players[i]->setHasDoubled( false );
        m_players[i]->setLast( false );
    }
}

void GameCanvas::resetPlayerCards()
{
    int i;
    CardList list;
        
    for(i=0;i<PLAYERS;i++)    
        m_players[i]->setCards( &list );
}

void GameCanvas::setPlayerCards( unsigned int id, int* cards )
{
    int i;
    CardList list( cards );
    list.setAutoDelete( false );
    
    for(i=0;i<PLAYERS;i++)
        if( m_players[i]->id() == id )
            m_players[i]->setCards( &list );
}

CanvasPlayer* GameCanvas::humanPlayer() const
{
    int i;
    for( i = 0; i < PLAYERS; i++ ) 
        if( m_players[i]->isHuman() )
            return m_players[i];
    return NULL;
}

void GameCanvas::exitLoop()
{
    m_loop.exit();
}
