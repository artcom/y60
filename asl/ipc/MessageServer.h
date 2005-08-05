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
//   $RCSfile: MessageServer.h,v $
//   $Author: pavel $
//   $Revision: 1.7 $
//   $Date: 2005/04/24 00:30:18 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#ifndef _asl_message_server_included
#define _asl_message_server_included

#include "ConduitServer.h"

#include <asl/Ptr.h>
#include <asl/ThreadFifo.h>    
#include <asl/Block.h>    
#include <string.h>

#define DB(x) // x

namespace asl {

/*! \addtogroup aslipc */
/* @{ */

//! Thread-safe Message-based Server.  
/** 
    Used Threadsafe FIFO queues for incoming and outgoing messages. Internally, the server thread sends and
    receives data in the background. The application can use thread-safe methods to pick up and send queued messages
    without worrying about threading issues.
**/
template <class POLICY>
class MessageServer : public ConduitServer<POLICY> {
    public:
        struct Message {
            Message(typename asl::WeakPtr< ConduitServer<POLICY> > theServer, const ReadableBlock & thePayload) :
                server(theServer), payload(thePayload)
            {}
            Message(typename asl::WeakPtr< ConduitServer<POLICY> > theServer, const std::string & thePayload) :
                server(theServer), payload(reinterpret_cast<const unsigned char*>(thePayload.c_str()),
                    reinterpret_cast<const unsigned char*>(thePayload.c_str()+thePayload.length()))
            {}
            std::string as_string() { return std::string(reinterpret_cast<const char*>(payload.begin()),
                                        payload.size()); }
            WeakPtr< ConduitServer<POLICY> > server;
            Block payload; //TODO: change to CowBlock to avoid copying
        };
        typedef asl::Ptr<Message> MessagePtr;

        // static factory method
        static typename ConduitServer<POLICY>::Ptr create(typename POLICY::Handle theHandle) {
            return typename ConduitServer<POLICY>::Ptr(new MessageServer(theHandle));
        }

        MessageServer(typename POLICY::Handle theHandle) :
            ConduitServer<POLICY>(theHandle) {};

        virtual ~MessageServer() {
            this->stop();
        }

        void pushOutgoingMessage(asl::Ptr<Message> theMessage) {
            _myOutputQueue.push(theMessage);
        }

        MessagePtr popIncomingMessage() {
            if (_myInputQueue.empty()) {
                DB(AC_TRACE << "MessageServer nothing to pop" << endl);
                return MessagePtr(0);
            }

            MessagePtr myNextMessage = _myInputQueue.pop();
            DB(AC_TRACE << "MessageServer pop ok" << endl);
            return myNextMessage;
        }

        bool hasPendingInput() const {
            return !_myInputQueue.empty();
        }

        virtual bool isValid() const {
            return ConduitServer<POLICY>::isValid() || hasPendingInput();
        }
       
    private:
        virtual bool processData() {
            // get incoming messages;
            std::string myInputString;
            while (this->receive(myInputString)) {
                DB(AC_TRACE << "MessageServer - added to input queue '" << myInputString << "'" << endl);
                asl::Ptr<Message> myIncomingMessage = asl::Ptr<Message>(new Message(this->getSelf(), myInputString));
                _myInputQueue.push(myIncomingMessage);
            }
            // send outgoing data
            while (!_myOutputQueue.empty()) {
                asl::Ptr<Message> myMessage = _myOutputQueue.pop();
                this->send(myMessage->payload);
            }
            return true;
        }
        asl::ThreadFifo<MessagePtr> _myOutputQueue;
        asl::ThreadFifo<MessagePtr> _myInputQueue;
};

}
#undef DB

#endif
