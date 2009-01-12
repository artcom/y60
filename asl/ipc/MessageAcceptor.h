/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: 
//     Classes for networked or local communication between processes
//
// Last Review:  ms 2007-08-15
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      fair
//    documentation           :      ok
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      fair
//    interfaces              :      ok
//    confidence              :      ok
//    integration             :      ok
//    dependencies            :      ok
//    error handling          :      ok
//    logging                 :      notapp
//    cheesyness              :      ok
//
//    overall review status   :      ok
//
//    recommendations: add high-level documentation, improve doxygen documentation 
*/
#ifndef _asl_message_acceptor_included
#define _asl_message_acceptor_included

#include "asl_ipc_settings.h"

#include "ConduitServer.h"
#include "ConduitAcceptor.h"

#include "MessageServer.h"
#include <asl/base/ThreadFifo.h>

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
