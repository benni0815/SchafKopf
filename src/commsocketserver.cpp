
#include <iostream>
#include <sys/poll.h>
#include "commsocketserver.h"

CommSocketServer::CommSocketServer(const char *name, int port, bool global)
    : CommBase(name)
{
    pthread_attr_t threadattr;

    serverParams.serverSocket=create_socket(port, global);
    serverParams.port=port;
    serverParams.self=this;
    sem_init(&serverParams.syncSem, 0, 1); 
    sem_init(&serverParams.waitSem, 0, 0); 
    serverParams.parent=pthread_self();
    pthread_create(&threadHandle, NULL, (void *(*)(void *))serverThread, &serverParams);
    pthread_detach(threadHandle);
}

CommSocketServer::~CommSocketServer()
{
    int i;
    
    pthread_kill(threadHandle, SIGTERM);
    sem_wait(&serverParams.syncSem);
    for(i=0;i<serverParams.clients.size();i++)
        close(serverParams.clients[i].socket);
    sem_post(&serverParams.syncSem);
    sem_destroy(&serverParams.syncSem);
    close(serverParams.serverSocket);
}

bool CommSocketServer::waitMsg(int timeout, int *rec)
{
    sem_wait(&serverParams.waitSem);
    sem_post(&serverParams.waitSem);
    return waitIncoming(timeout, rec);
}

bool CommSocketServer::receiveMsg(Message *msg)
{
    int n;
    
    if(!msg)
        return false;
    sem_wait(&serverParams.syncSem);
    n=read(serverParams.clients[msg->sender].socket, msg->data, MESSAGE_LENGTH);
    sem_post(&serverParams.syncSem);
    if(n==0)
    {
        sem_wait(&serverParams.syncSem);
        close(serverParams.clients[msg->sender].socket);
        serverParams.clients.erase(serverParams.clients.begin()+msg->sender);
        sem_post(&serverParams.syncSem);
        sem_wait(&serverParams.waitSem);
        throw EConnectionClosed();
    }
    else if(n!=MESSAGE_LENGTH)
        return false;
    return true;
}

bool CommSocketServer::sendMsg(const Message *msg, bool waitAck)
{
    int n;
    Message m;
    Message ack;
    
    if(!msg)
        return false;
    memcpy(&m, msg, sizeof(CommBase::Message));
    if(waitAck)
        m.msg = (CommBase::msgtype)((int)CommBase::msg_ack | (int)m.msg); 
    sem_wait(&serverParams.syncSem);
    n=write(serverParams.clients[m.receiver].socket, m.data, MESSAGE_LENGTH);
    sem_post(&serverParams.syncSem);
    if(n==0)
    {
        sem_wait(&serverParams.syncSem);
        close(serverParams.clients[m.sender].socket);
        serverParams.clients.erase(serverParams.clients.begin()+m.sender);
        sem_post(&serverParams.syncSem);
        throw EConnectionClosed();
    }
    else if(n!=MESSAGE_LENGTH)
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

int CommSocketServer::clients(void)
{
    return serverParams.clients.size();
}

CommSocketServer::ClientType CommSocketServer::getClientType(int index)
{
    return serverParams.clients[index].clientType;
}

bool CommSocketServer::waitIncoming(int timeout, int *rec)
{
    int i, n;
    pollfd *fds;

    sem_wait(&serverParams.syncSem);
    fds=new pollfd[serverParams.clients.size()];
    for(i=0;i<serverParams.clients.size();i++)
    {
        fds[i].fd=serverParams.clients[i].socket;
        fds[i].events=POLLIN | POLLHUP | POLLNVAL | POLLERR;
        fds[i].revents=0;
    }
    n=poll(fds, serverParams.clients.size(), timeout);
    sem_post(&serverParams.syncSem);
    if(!rec)
    {
        delete fds;
        return n ? true : false;
    }
    if(!n)
    {
        delete fds;
        return false;
    }
    sem_wait(&serverParams.syncSem);
    for(i=0;i<serverParams.clients.size();i++)
        if(fds[i].revents)
            break;
    sem_post(&serverParams.syncSem);
    *rec=i;
    delete fds;
    return true;
}

bool CommSocketServer::init_sockaddr(sockaddr_in *name, const char *hostname, unsigned int port)
{
    hostent *hostinfo;

    name->sin_family = AF_INET;
    name->sin_port = htons (port);
    hostinfo = gethostbyname (hostname);
    if (hostinfo == NULL)
        return false;
    name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
    return true;
}

int CommSocketServer::create_socket(unsigned int port, bool global)
{
    int sock;
    sockaddr_in addr;
    int set=1;
    
    sock=socket (global ? PF_INET : PF_LOCAL, SOCK_STREAM, 0);
    if(sock<0)
        return -1;
    if(!init_sockaddr(&addr, "localhost", port))
    {
        close(sock);
        return -1;
    }
    if(bind(sock, (sockaddr *)&addr, sizeof(sockaddr_in))<0)
    {
        close(sock);
        return -1;
    }
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int));
    return sock;
}

void CommSocketServer::serverThread(ServerParams *params)
{
    fd_set fds;
    ClientSocket client;
    RegisterMessage msg;
    int i;
    char ch;
    
    for(;;)
    {
        FD_SET(params->serverSocket , &fds);
        select(params->serverSocket+1, &fds, &fds, &fds, NULL);
        if(listen(params->serverSocket, MAX_CONNECTIONS)<0)
            continue;
        client.socket=accept(params->serverSocket, NULL, NULL);
        sem_wait(&params->syncSem);
        params->clients.push_back(client);
        msg.client=params->clients.size()-1;
        sem_post(&params->syncSem);
        params->self->waitIncoming();
        if(!params->self->receiveMsg((CommBase::Message *)&msg))
        {
            sem_wait(&params->syncSem);
            close(params->clients[msg.client].socket);
            params->clients.pop_back();
            sem_post(&params->syncSem);
            continue;
        }
        sem_wait(&params->syncSem);
        params->clients[msg.client].clientType=msg.data.type;
        sem_post(&params->syncSem);
        sem_post(&params->waitSem);
    }
}

