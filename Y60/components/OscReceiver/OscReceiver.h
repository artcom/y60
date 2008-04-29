//============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef OSC_RECEIVER_INCLUDED
#define OSC_RECEIVER_INCLUDED

#include <iostream>

#include <oscpack/osc/OscReceivedElements.h>
#include <oscpack/osc/OscPacketListener.h>
#include <oscpack/ip/UdpSocket.h>

#include <asl/PosixThread.h>
#include <asl/PlugInBase.h>
#include <y60/IEventSource.h>
#include <y60/IScriptablePlugin.h>

namespace y60 {

    class OscReceiver : public osc::OscPacketListener,
        public asl::PlugInBase,
        public jslib::IScriptablePlugin,
        public y60::IEventSource,
        public asl::PosixThread {

    public: 

            OscReceiver(asl::DLHandle theHandle);
            virtual ~OscReceiver();

            const char * ClassName() {
                static const char * myClassName = "OscReceiver";
                return myClassName;
            }

            EventPtrList poll();

            void start();
            void stop();

            // threading
            void run();

            JSFunctionSpec * Functions();

    protected:

            static void threadMain( asl::PosixThread & theThread );

            virtual void ProcessMessage( const osc::ReceivedMessage& m, 
                                         const IpEndpointName& remoteEndpoint );

    private:

            asl::ThreadLock           _myEventListLock;
            y60::EventPtrList         _myCurrentY60Events;
            y60::EventPtrList         _myNewY60Events;
            UdpListeningReceiveSocket _mySocket;
 	

        };

}

#endif // OSC_RECEIVER_INCLUDED
