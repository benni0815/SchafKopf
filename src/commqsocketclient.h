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

#ifndef COMMQSOCKETCLIENT_H
#define COMMQSOCKETCLIENT_H 

#include <qobject.h>
#include <qsocket.h>

#include "commbase.h"

using namespace std;

class CommQSocketClient 
    : public CommBase, public QObject
{
    public:
        CommQSocketClient(const char *hostname, unsigned int port, CommSocketServer::ClientType type);
        ~CommQSocketServer();
        bool waitMsg(int timeout=-1, int *rec=NULL); 
        bool receiveMsg(Message *msg);
        bool sendMsg(Message *msg, bool waitAck=false);
        
    protected:
        QSocket clientSocket;
        
        virtual void msgReceived() = 0;

    private slots:
        void dataWaiting();
};

#endif

