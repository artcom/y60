//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef __OSC_CLIENT_INCLUDED
#define __OSC_CLIENT_INCLUDED

#include "VideoProcessing.h"

#include <asl/UDPConnection.h>
#include <asl/Enum.h>
#include <dom/Nodes.h>

#include <oscpack/osc/OscOutboundPacketStream.h>

namespace y60 {

typedef asl::Ptr<inet::UDPConnection> UDPConnectionPtr;

 class OscClient : public VideoProcessingExtension {
    public:
        OscClient();
        void poll();


        const char * ClassName() {
            static const char * myClassName = "OscClient";
            return myClassName;
        }

        enum {
            BUFFER_SIZE = 1024
        };

        virtual void onUpdateSettings( dom::NodePtr theSettings );
        
        void createData();

    private:
        
        void connectToServer();

        char myBuffer[ BUFFER_SIZE ];
        osc::OutboundPacketStream _myOSCStream;

        UDPConnectionPtr _myConnection;
        int              _myClientPort;
        int              _myServerPort;
        std::string      _myServerAddress;
};

} // end of namespace y60

#endif // OSC_CLIENT_INCLUDED
