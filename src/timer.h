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
#ifndef TIMER_H
#define TIMER_H

#include <qobject.h>

/** includes for our eventloop hacks
  */
#include <kapplication.h>
#include <setjmp.h> 
#if QT_VERSION >= 0x030100
    #include <qeventloop.h>
#else
    #include <qapplication.h>
#endif

/** macro to start our eventloop hacks 
  */
#if QT_VERSION >= 0x030100 
    #define ENTER_LOOP() \
        kapp->eventLoop()->enterLoop(); 
#else 
    #define ENTER_LOOP() \
        kapp->enter_loop(); 
#endif 

/** macro to end our eventloop hacks 
  */
#if QT_VERSION >= 0x030100 
    #define EXIT_LOOP() \
        kapp->eventLoop()->exitLoop(); 
#else 
    #define EXIT_LOOP() \
        kapp->exit_loop();
#endif 

class QTimer;
/**
@author Dominik Seichter
*/
class Timer : public QObject
{
    Q_OBJECT
    public:
        Timer(QObject *parent = 0, const char *name = 0);
        ~Timer();
        
        void block( int seconds );
        
    private slots:
        void returnLoop();
        
    private:
        QTimer *m_timer;
};

#endif
