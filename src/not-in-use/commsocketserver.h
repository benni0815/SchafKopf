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

#ifndef COMMSOCKETSERVER_H
#define COMMSOCKETSERVER_H

#include <vector>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <exception>

#include "commbase.h"

using namespace std;

#define MAX_CONNECTIONS 10

class ESocketError
    : public exception
{
    public:
        ESocketError()
            : exception() {}
        virtual const char *what()
        {
            return "Socket error";
        }
};

class EConnectError
    : public ESocketError
{
    public:
        EConnectError()
            : ESocketError() {}
        const char *what()
        {
            return "Error while connecting";
        }
};

class EConnectionClosed
    : public ESocketError
{
    public:
        EConnectionClosed()
            : ESocketError() {}
        const char *what()
        {
            return "The connection was closed.";
        }
};

class CommSocketServer 
    : public CommBase
{
    public:
        typedef enum ClientType { player=1, gui, debugger, unregister };
        struct msgdata
        {
            msgtype msg;
            ClientType type;
        };
        struct RegisterMessage
        {
            int client;
            union
            {
                msgdata data;
                char foo[MESSAGE_LENGTH];
            };
        };

        CommSocketServer(const char *name, int port, bool global=false);
        ~CommSocketServer();
        bool waitMsg(int timeout=-1, int *rec=NULL);
        bool receiveMsg(Message *msg);
        bool sendMsg(const Message *msg, bool waitAck=false);
        int clients(void);
        ClientType getClientType(int index);
    
    private:
        struct ClientSocket
        {
            int socket;
            ClientType clientType;
        };
        struct ServerParams
        {
            vector<ClientSocket> clients;
            sem_t syncSem;
            sem_t waitSem;
            pthread_t parent;
            int serverSocket;
            unsigned int port;
            CommSocketServer *self;
        };
        
        ServerParams serverParams;
        pthread_t threadHandle;
        
        bool waitIncoming(int timeout=-1, int *rec=NULL);
        static bool init_sockaddr(sockaddr_in *name, const char *hostname, unsigned int port);
        static int create_socket(unsigned int port, bool global);
        static void serverThread(ServerParams *params);
};

#endif
