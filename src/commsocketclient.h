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

#ifndef COMMSOCKETCLIENT_H
#define COMMSOCKETCLIENT_H 

#include "commbase.h"
#include "commsocketserver.h"

class CommSocketClient 
    : public CommBase
{
    public:
        CommSocketClient(const char *hostname, unsigned int port, bool global, CommSocketServer::ClientType type); 
        ~CommSocketClient(); 
        bool waitMsg(int timeout=-1, int *rec=NULL); 
        bool receiveMsg(Message *msg);
        bool sendMsg(const Message *msg, bool waitAck=false);
        
    private:
        int clientSocket;
        
        static int init_sockaddr (sockaddr_in *name, const char *hostname, unsigned int port);
        static int open_socket(const char *hostname, unsigned int port, bool global);
};

#endif

