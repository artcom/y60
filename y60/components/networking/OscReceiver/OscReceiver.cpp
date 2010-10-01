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

#include <iostream>

#include "OscReceiver.h"
#include "osceventxsd.h"
#include <asl/base/Auto.h>
#include <asl/base/ThreadLock.h>
#include "y60/jsbase/Documentation.h"
#include <y60/input/GenericEvent.h>
#include <y60/base/DataTypes.h>
#include <asl/base/string_functions.h>
#include <y60/base/SettingsParser.h>

#include <y60/input/EventDispatcher.h>

using namespace std;
using namespace asl;

namespace y60 {

    OscReceiver::OscReceiver(int thePort, const char * theEndpointName):
        asl::PosixThread(),
        _myEventSchema( new dom::Document( ourosceventxsd )  ),
        _myValueFactory( new dom::ValueFactory() ),
        _myCurrentBundleTimeTag(0)
    {
        registerStandardTypes( * _myValueFactory );

        AC_DEBUG << "Initiated osc receiver on port: " << thePort;
        if (theEndpointName) {
            _myOscReceiverSocket = asl::Ptr<UdpListeningReceiveSocket>(
                new UdpListeningReceiveSocket(IpEndpointName(theEndpointName,
                                                             thePort), this));
        } else {
            _myOscReceiverSocket = asl::Ptr<UdpListeningReceiveSocket>(
                new UdpListeningReceiveSocket(IpEndpointName(IpEndpointName::ANY_ADDRESS,
                                                             thePort), this));
        }

        y60::EventDispatcher::get().addSource(this);
    }

    OscReceiver::~OscReceiver(){
        stop();
    }


    y60::EventPtrList OscReceiver::poll() {
        AutoLocker<ThreadLock> scopeLock(_myThreadLock);

        _myCurrentY60Events.clear();

        while (!_myNewMessages.empty()){
            _myCurrentY60Events.push_back(createY60Event(_myNewMessages.front()));
            _myNewMessages.pop_front();
        }

        return _myCurrentY60Events;
    }

    void OscReceiver::start(){
        if (!_myOscReceiverSocket) {
            AC_WARNING << "Sorry, no osc receiver port settet, use the 'port'-propperty to do so.";
        }
        AC_DEBUG << "start listening for osc events";
        fork();
    }

    void OscReceiver::stop(){
        _myOscReceiverSocket->AsynchronousBreak();
        join();
        AC_DEBUG << "stopped listening for osc events";
    }

    void OscReceiver::run( ) {
        try {
            AC_DEBUG << "launched the osc receiver thread";

            _myOscReceiverSocket->Run();

            AC_DEBUG << "receiver thread finished";
        } catch (const asl::Exception & ex) {
            AC_ERROR << "asl::Exception in OscReceiver thread: " << ex;
            throw;
        }
    }



    EventPtr OscReceiver::createY60Event(const string& theMessage){
        y60::GenericEventPtr myY60Event( new GenericEvent("onOscEvent", _myEventSchema, _myValueFactory));

        dom::NodePtr myNode = myY60Event->getDocument();
        dom::NodePtr myOldEvent = myNode->firstChild();
        try {
            myNode->parseAll(theMessage);
            myNode->removeChild(myOldEvent);
            //myNode->firstChild()->
            //    appendAttribute<string>("when", myOldEvent->getAttributeString("when"));
            myNode->firstChild()->
                appendAttribute<string>("callback", myOldEvent->getAttributeString("callback"));
            myY60Event->when = as<double>(myNode->firstChild()->getAttributeString("when"));
            AC_TRACE << "new osc event: " << *myY60Event->getNode();
        } catch (...){
            AC_ERROR << "bad osc message: "<< endl << theMessage;
        }
        return myY60Event;

    }

    string OscReceiver::createMessageString(const osc::ReceivedMessage& theMessage,
                                            const IpEndpointName& theRemoteEndpoint){


        char myBuffer[IpEndpointName::ADDRESS_STRING_LENGTH];
        theRemoteEndpoint.AddressAsString(myBuffer);

        osc::ReceivedMessage::const_iterator myArgItr =
            theMessage.ArgumentsBegin();
        string myArguments = "";
        while ( myArgItr != theMessage.ArgumentsEnd() ){

            if (myArgItr->IsString()){
                myArguments += "<string>" +
                    asl::as_string(myArgItr->AsString()) +
                    "</string>";
            } else if (myArgItr->IsFloat()){
                myArguments += "<float>" +
                    asl::as_string(myArgItr->AsFloat()) +
                    "</float>";
            } else if (myArgItr->IsBool()){
                myArguments += "<bool>" +
                    asl::as_string(myArgItr->AsBool()) +
                    "</bool>";
            } else if (myArgItr->IsInt32()){
                myArguments += "<int>" +
                    asl::as_string(myArgItr->AsInt32()) +
                    "</int>";
            } else if (myArgItr->IsInt64()){
                myArguments += "<int>" +
                    asl::as_string(myArgItr->AsInt64()) +
                    "</int>";
            } else if (myArgItr->IsDouble()){
                myArguments += "<double>" +
                    asl::as_string(myArgItr->AsDouble()) +
                    "</double>";
            } else {
                AC_ERROR << "unknown osc message received";
                break;
            }

            myArgItr++;

        }
        string myMessageString = "<generic type='" +
            string(theMessage.AddressPattern()).substr(1) +
            "' when='" + asl::as_string(_myCurrentBundleTimeTag) +
            "' sender='" + string(myBuffer) + "'>";
        myMessageString += myArguments;
        myMessageString += "</generic>";
        return myMessageString;
    }
    void OscReceiver::ProcessBundle( const osc::ReceivedBundle& theBundle,
                                     const IpEndpointName& remoteEndpoint ) {
        _myCurrentBundleTimeTag = static_cast<long>(theBundle.TimeTag());
        OscPacketListener::ProcessBundle(theBundle, remoteEndpoint);
    }

    void OscReceiver::ProcessMessage( const osc::ReceivedMessage& theMessage,
                                      const IpEndpointName& theRemoteEndpoint )
    {

        AutoLocker<ThreadLock> scopeLock(_myThreadLock);
        try{

            string myMessageString =
                createMessageString(theMessage, theRemoteEndpoint);
            _myNewMessages.push_front(myMessageString);


            //AC_TRACE << "c++: adding osc event " << *myNode;

        }catch( OscException& e ){
            // any parsing errors such as unexpected argument types, or
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                      << theMessage.AddressPattern() << ": " << e.what() << "\n";
        }
    }
}
