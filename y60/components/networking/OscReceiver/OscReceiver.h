/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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

            OscReceiver(int thePort, const char * theEndpointName = 0);
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

    protected:

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
