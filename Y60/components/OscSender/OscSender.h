/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: Base class for OscSender
//
*/
#ifndef INCL_Y60_OSCSENDER
#define INCL_Y60_OSCSENDER

#include <asl/net/UDPConnection.h>
#include <asl/dom/Nodes.h>
#include <oscpack/osc/OscOutboundPacketStream.h>

namespace y60 {


class OscSender {
    public:
        /// creates a new OscSender
        OscSender();
        bool connect(std::string theReceiverAddress, unsigned theReceiverPort, unsigned theSenderPort);
        virtual ~OscSender();

        void close();
        unsigned send(dom::NodePtr theOscEvent);
    private:
        std::string              _myReceiverAddress;
        unsigned                _myReceiverPort;
        inet::UDPConnectionPtr  _myReceiverUDPConnection;
    
};

}
#endif
