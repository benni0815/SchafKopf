
#include "commqsocketclient.h"
#include "commsocketserver.h"

CommQSocketClient::CommQSocketClient(const char *hostname, unsigned int port, CommSocketServer::ClientType type)
    : CommBase(hostname), QObject()
{
    CommSocketServer::RegisterMessage msg;
    
    clientSocket.connectToHost(hostname, port);
    if(!clientSocket.connected())
        throw EConnectError();
    msg.data.msg=CommBase::msg_msg;
    msg.data.type=type;
    sendMsg(&msg, false);
    connect(this, SIGNAL(readyRead()), this, SLOT(dataWaiting()));
}

CommQSocketClient::~CommQSocketClient()
{
}

bool CommQSocketClient::waitMsg(int timeout, int *rec)
{
    if(clientSocket.waitForMore(timeout))
        return true;
    return false;
}

bool CommQSocketClient::receiveMsg(Message *msg)
{
    int n;
    
    if(!msg)
        return false;
    if(!clientSocket.connected())
        throw EConnectionClosed();
    n=clientSocket.readBlock(msg->data, MESSAGE_LENGTH);
    if(n!=MESSAGE_LENGTH)
        return false;
    return true;
}

bool CommQSocketClient::sendMsg(const Message *msg, bool waitAck)
{
    int n;
    Message ack;
    Message m;
    
    if(!msg)
        return false;
    memcpy(&m, msg, sizeof(CommBase::Message));
    if(waitAck)
        m.msg |= CommBase::msg_ack; 
    if(!clientSocket.connected())
        throw EConnectionClosed();
    n=writeBlock(m.data, MESSAGE_LENGTH);
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

