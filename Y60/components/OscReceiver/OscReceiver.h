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

#include <asl/base/PosixThread.h>
#include <asl/base/Exception.h>

#include <y60/input/IEventSource.h>
#include <y60/jsbase/IScriptablePlugin.h>

namespace y60 {

    DEFINE_EXCEPTION( OscException, asl::Exception );

    class OscReceiver : public osc::OscPacketListener,
        public y60::IEventSource,
        public asl::PosixThread {

    public: 
            struct OscMessageInfo {
                OscMessageInfo(const std::string& theMessage, 
                               const IpEndpointName& theSender) : 
                    _myMessage(theMessage), _mySender(theSender) {}
                std::string          _myMessage;
                IpEndpointName       _mySender;    
            };
            
            OscReceiver(int thePort);
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
            virtual void ProcessBundle( const osc::ReceivedBundle& theBundle, 
                                         const IpEndpointName& remoteEndpoint );

    private:

            std::string createMessageString(const osc::ReceivedMessage& m, 
                                            const IpEndpointName& remoteEndpoint);

            y60::EventPtr createY60Event(const std::string& theMessage);

            asl::ThreadLock           _myThreadLock;
            y60::EventPtrList         _myCurrentY60Events;
            std::list<std::string>    _myNewMessages;
            asl::Ptr<UdpListeningReceiveSocket> _myOscReceiverSocket;
            dom::NodePtr                 _myEventSchema;
            asl::Ptr<dom::ValueFactory>  _myValueFactory;
            long                         _myCurrentBundleTimeTag;

        };

    typedef asl::Ptr<OscReceiver> OscReceiverPtr;

}

#endif // OSC_RECEIVER_INCLUDED
