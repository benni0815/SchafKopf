#include <iostream>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netdb.h>
#include "commsocketclient.h"
#include "commsocketserver.h"

CommSocketClient::CommSocketClient(const char *hostname, unsigned int port, bool local, CommSocketServer::ClientType type)
    : CommBase(hostname)
{
    CommSocketServer::RegisterMessage msg;
    
    clientSocket=open_socket(hostname, port, local);
    if(clientSocket<0)
        throw EConnectError();
    msg.data.msg=CommBase::msg_msg;
    msg.data.type=type;
    sendMsg((CommBase::Message *)&msg, false);
}

CommSocketClient::~CommSocketClient()
{
    close(clientSocket);
}

bool CommSocketClient::waitMsg(int timeout, int *rec)
{
    int i, n;
    pollfd fds;

    fds.fd=clientSocket;
    fds.events=POLLIN | POLLHUP | POLLNVAL | POLLERR;
    fds.revents=0;
    n=poll(&fds, 1, timeout);
    if(!n)
        return false;
    if(fds.revents & (POLLHUP | POLLNVAL | POLLERR))
        throw ESocketError();
    return true;
}

bool CommSocketClient::receiveMsg(Message *msg)
{
    int n;
    
    if(!msg)
        return false;
    n=read(clientSocket, msg->data, MESSAGE_LENGTH);
    if(n==0)
    {
        close(clientSocket);
        throw EConnectionClosed();
    }
    if(n!=MESSAGE_LENGTH)
        return false;
    return true;
}

bool CommSocketClient::sendMsg(const Message *msg, bool waitAck)
{
    int n;
    Message ack;
    Message m;
    
    if(!msg)
        return false;
    memcpy(&m, msg, sizeof(Message));
    if(waitAck)
        m.msg = (CommBase::msgtype) ((int)CommBase::msg_ack | (int)m.msg); 
    n=write(clientSocket, m.data, MESSAGE_LENGTH);
    if(n==0)
    {
        close(clientSocket);
        throw EConnectionClosed();
    }
    if(n!=MESSAGE_LENGTH)
        return false;
    if(waitAck)
    {
        for(;;)
        {
            ack.sender=m.receiver;
            if(!receiveMsg(&ack))
                return false;
            if(!(ack.msg & CommBase::msg_ack))
                continue;
            break;
        }
    }
    return true;
}

int CommSocketClient::init_sockaddr (sockaddr_in *name, const char *hostname, unsigned int port)
{
    struct hostent *hostinfo;

    name->sin_family = AF_INET;
    name->sin_port = htons (port);
    hostinfo = gethostbyname (hostname);
    if (hostinfo == NULL)
    {
        printf("Unbekanter host %s.\n", hostname);
        return 0;
    }
    name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
    return 1;
}

int CommSocketClient::open_socket(const char *hostname, unsigned int port, bool local)
{
    int sock;
    sockaddr_in addr;
    
    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock<0)
    {
        return -1;
    }
    if(!init_sockaddr(&addr, hostname, port))
    {
        close(sock);
        return -1;
    }
    if(connect(sock, (sockaddr *)&addr, sizeof(sockaddr_in))<0)
    {
        close(sock);
        return -1;
    }
    return sock;
}

