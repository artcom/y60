//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: MessageAcceptor.h,v $
//   $Author: pavel $
//   $Revision: 1.7 $
//   $Date: 2005/04/24 00:30:18 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#ifndef _asl_message_acceptor_included
#define _asl_message_acceptor_included

#include "ConduitServer.h"
#include "ConduitAcceptor.h"

#include "MessageServer.h"
#include <asl/ThreadFifo.h>    

#include <queue>

#define DB(x) // x;

namespace asl {

/*! \addtogroup aslipc */
/* @{ */

template <class POLICY>
class MessageAcceptor : public ConduitAcceptor<POLICY> {
    public:
        typedef typename MessageServer<POLICY>::Message Message;
        MessageAcceptor(typename POLICY::Endpoint theLocalEndpoint, 
                typename ConduitAcceptor<POLICY>::FACTORYPROC theFactoryMethod=MessageServer<POLICY>::create, 
                unsigned theMaxConnectionCount=32) :
            ConduitAcceptor<POLICY>(theLocalEndpoint, theFactoryMethod, theMaxConnectionCount)
            {
            };

        void pushIncomingMessage(Ptr<Message> theMessage) {
            _myInputQueue.push(theMessage);  
        }

        Ptr<Message> popIncomingMessage() {
            typename ConduitAcceptor<POLICY>::ServerList::iterator myServer;
            Ptr<Message> myMessage(0);
            for (myServer = this->_myServers.begin(); myServer != this->_myServers.end(); ++myServer) {
                Ptr<ConduitServer<POLICY> > myConduitServer = *myServer;
                Ptr<MessageServer<POLICY> > myMessageServer = 
                        dynamic_cast_Ptr<MessageServer<POLICY> >(myConduitServer);
                if (!myMessageServer) {
                    throw ConduitException("MessageAcceptor has a non-MessageServer as a child!", PLUS_FILE_LINE);
                }
                while ( myMessage = myMessageServer->popIncomingMessage() ) {
                    DB(AC_TRACE << "MsgAcceptor recv new message" << endl);
                    _myInputQueue.push(myMessage);
                }
            }
            if (_myInputQueue.empty()) {
                return Ptr<Message>(0);
            }
            myMessage = _myInputQueue.front();
            _myInputQueue.pop();
            return myMessage;
        }

        bool pushOutgoingMessage(WeakPtr< ConduitServer<POLICY> > theServer, 
                const std::string & thePayload) 
        {
            return pushOutgoingMessage(Ptr<Message>(new Message(theServer, thePayload)));
        }

        bool pushOutgoingMessage(WeakPtr< ConduitServer<POLICY> > theServer, 
                const ReadableBlock & thePayload) 
        {
            return pushOutgoingMessage(Ptr<Message>(new Message(theServer, thePayload)));
        }

        bool pushOutgoingMessage(Ptr<Message> theMessage) {
            Ptr< ConduitServer<POLICY> > myServer = theMessage->server.lock();
            if (myServer) {
                dynamic_cast_Ptr< MessageServer<POLICY> >(myServer)->pushOutgoingMessage(theMessage);
                return true;
            }

            return false;
        }

    private:
        std::queue<Ptr<Message> > _myInputQueue;

};
/* @} */
}
#undef DB

#endif
