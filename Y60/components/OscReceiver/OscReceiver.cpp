//============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include <iostream>

#include "OscReceiver.h"
#include "y60/jsbase/Documentation.h"
#include <y60/input/GenericEvent.h>
#include <y60/base/DataTypes.h>
#include <asl/base/string_functions.h>
#include <y60/base/SettingsParser.h>

#include <y60/input/EventDispatcher.h>

using namespace std;
using namespace asl;

extern std::string ourosceventxsd;


namespace y60 {

    OscReceiver::OscReceiver(int thePort):
        _myEventSchema( new dom::Document( ourosceventxsd )  ),
        _myValueFactory( new dom::ValueFactory() ),
        asl::PosixThread(),
        _myCurrentBundleTimeTag(0)
    {
        registerStandardTypes( * _myValueFactory );

        AC_DEBUG << "Initiated osc receiver on port: " << thePort;
        _myOscReceiverSocket = asl::Ptr<UdpListeningReceiveSocket>(
            new UdpListeningReceiveSocket(IpEndpointName(IpEndpointName::ANY_ADDRESS, 
                                                         thePort), this));

        y60::EventDispatcher::get().addSource(this);
    }

    OscReceiver::~OscReceiver(){
        stop();
    }


    y60::EventPtrList OscReceiver::poll() {
        _myThreadLock.lock();

        _myCurrentY60Events.clear();

        while (!_myNewMessages.empty()){            
            _myCurrentY60Events.push_back(createY60Event(_myNewMessages.front()));
            _myNewMessages.pop_front();
        }

        _myThreadLock.unlock();

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
                                      const IpEndpointName& theRemoteEndpoint ){

        try{
            _myThreadLock.lock();


            string myMessageString = 
                createMessageString(theMessage, theRemoteEndpoint);
            _myNewMessages.push_front(myMessageString);

            
            //AC_TRACE << "c++: adding osc event " << *myNode;
            _myThreadLock.unlock();
            
        }catch( OscException& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                      << theMessage.AddressPattern() << ": " << e.what() << "\n";
            _myThreadLock.unlock();
        }
    }    
}
