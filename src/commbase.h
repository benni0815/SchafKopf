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

#ifndef COMMBASE_H
#define COMMBASE_H

#include <string>

using namespace std;

#define MESSAGE_LENGTH 128

class CommBase
{
    public:
        typedef enum msgtype   { msg_broadcast = 0, 
                                 msg_ack = 1 << 1,
                                 msg_ackbroadcast= CommBase::msg_broadcast | CommBase::msg_ack,
                                 msg_msg = 1, 
                                 msg_ackmsg = CommBase::msg_msg | CommBase::msg_ack,
                                 msg_exit = 1 << 2 };
        struct Message
        {
            union
            {
                int receiver;
                int sender;
            };
            union
            {
                CommBase::msgtype msg;
                char data[MESSAGE_LENGTH];
            };
        };
        
        CommBase(const char *name) { this->name=name; }
        virtual ~CommBase() {}
        /// Waits for an incoming message. 
        /**
         * \param timeout Time to wait for an incoming message. -1 for no timeout. 0 only checks if a message is available. 
         * \param *rec Contains a pointer to an int for the client who sent
         * a message
         * \return true if a message is waiting or false for no message.
         */
        virtual bool waitMsg(int timeout=-1, int *rec=NULL) = 0;
        
        /// Receives a message
        /**
         * \param *msg Pointer to CommBase::Message-struct for the received Message.
         * \return true if a message was received. 
         */
        virtual bool receiveMsg(Message *msg) = 0;

        /// Sends a message. 
        /**
         * \param *msg Pointer to the message to be sent. 
         * \return true if the message was sent. 
         */
        virtual bool sendMsg(const Message *msg, bool waitAck=false) = 0;
        
    protected:
        string name;
};

#endif

