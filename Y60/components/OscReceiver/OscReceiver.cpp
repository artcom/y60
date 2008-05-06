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
#include "y60/Documentation.h"
#include <y60/JSScriptablePlugin.h>
#include <y60/GenericEvent.h>
#include <y60/DataTypes.h>
#include <asl/string_functions.h>
#include <y60/SettingsParser.h>

using namespace std;
using namespace asl;

extern std::string ourosceventxsd;
extern std::string ourasseventxsd;


namespace y60 {

    OscReceiver::OscReceiver(DLHandle theHandle):
        _myEventSchema( new dom::Document( ourosceventxsd )  ),
        _myASSEventSchema( new dom::Document( ourasseventxsd )  ),
        _myValueFactory( new dom::ValueFactory() ),
        asl::PosixThread(),
        asl::PlugInBase( theHandle )
    {
        registerStandardTypes( * _myValueFactory );
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
        if (_myOscReceiverSockets.size() == 0 ) {
            AC_WARNING << "Sorry, no osc receiver ports configured, use settings file with node <OscReceiver>.";
        }
        AC_DEBUG << "start listening for osc events";
        fork();
    }

    void OscReceiver::stop(){
        for (unsigned int mySocketIndex = 0; mySocketIndex < _myOscReceiverSockets.size(); mySocketIndex++) {
            if (mySocketIndex >0) {
                msleep(100);
            }
            _myOscReceiverSockets[mySocketIndex]->AsynchronousBreak();
        }
        join();
        AC_DEBUG << "stopped listening for osc events";
    }

    void OscReceiver::run( ) {
        try {
            AC_DEBUG << "launched the osc receiver thread";
    
            for (unsigned int mySocketIndex = 0; mySocketIndex < _myOscReceiverSockets.size(); mySocketIndex++) {
                _myOscReceiverSockets[mySocketIndex]->Run();
            }

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
        myNode->parseAll(theMessage);
        myNode->removeChild(myOldEvent);
        myNode->firstChild()->
            appendAttribute<string>("when", myOldEvent->getAttributeString("when"));
        myNode->firstChild()->
            appendAttribute<string>("callback", myOldEvent->getAttributeString("callback"));

        AC_TRACE << "new osc event: " << *myY60Event->getNode();
        return myY60Event;

    }

    string OscReceiver::createMessageString(const osc::ReceivedMessage& theMessage,
                                            const IpEndpointName& theRemoteEndpoint){


        char myBuffer[IpEndpointName::ADDRESS_STRING_LENGTH];
        theRemoteEndpoint.AddressAsString(myBuffer);

        string myMessageString = "<generic type='" + 
            string(theMessage.AddressPattern()).substr(1) +
            "' sender='" + string(myBuffer) + "'>";

        osc::ReceivedMessage::const_iterator myArgItr = 
            theMessage.ArgumentsBegin();

        while ( myArgItr != theMessage.ArgumentsEnd() ){
       
            if (myArgItr->IsString()){
                myMessageString += "<string>" + 
                                              asl::as_string(myArgItr->AsString()) + 
                                              "</string>";
            } else if (myArgItr->IsFloat()){
                myMessageString += "<float>" + 
                                              asl::as_string(myArgItr->AsFloat()) + 
                                              "</float>";
            } else if (myArgItr->IsBool()){
                myMessageString += "<bool>" + 
                                              asl::as_string(myArgItr->AsBool()) + 
                                              "</bool>";
            } else if (myArgItr->IsInt32()){
                myMessageString += "<int>" + 
                                              asl::as_string(myArgItr->AsInt32()) + 
                                              "</int>";
            } else if (myArgItr->IsInt64()){
                myMessageString += "<int>" + 
                                              asl::as_string(myArgItr->AsInt64()) + 
                                              "</int>";
            } else if (myArgItr->IsDouble()){
                myMessageString += "<double>" + 
                                              asl::as_string(myArgItr->AsDouble()) + 
                                              "</double>";
            } else {
                AC_ERROR << "unknown osc message received";
                break;
            }
            
            myArgItr++;

        }

        myMessageString += "</generic>";

        return myMessageString;
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
    
    void
    OscReceiver::onUpdateSettings(dom::NodePtr theSettings) {
    
    
        AC_DEBUG << "updating OscReceiver settings";

        VectorOfUnsignedInt myDefaultPorts;
        myDefaultPorts.push_back(12000);
        dom::NodePtr mySettings = getOscReceiverSettings( theSettings );                
        VectorOfUnsignedInt myPorts;
        getConfigSetting( theSettings, "ReceiverPorts", myPorts, myDefaultPorts );        
        for (unsigned i =0; i < myPorts.size();i++) {
            AC_DEBUG << "Initiated osc receiver on port: " << myPorts[i];
            _myOscReceiverSockets.push_back(asl::Ptr<UdpListeningReceiveSocket>(new UdpListeningReceiveSocket(IpEndpointName(IpEndpointName::ANY_ADDRESS, myPorts[i]), this )));
        }
    }

    void
    OscReceiver::onGetProperty(const std::string & thePropertyName,
                               PropertyValue & theReturnValue) const
    {
        if (thePropertyName == "eventSchema") {
            theReturnValue.set( _myASSEventSchema );
            return;
        }
    }

    static JSBool
    Stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
        DOC_BEGIN("Start listening for osc events in a seperate thread (fork)");
        DOC_END;
   
        asl::Ptr<y60::OscReceiver> myNative = jslib::getNativeAs<y60::OscReceiver>(cx, obj);
        if (myNative) {
            myNative->stop();
        } else {
            assert(myNative);
        }
        return JS_TRUE;
    }

    static JSBool
    Start(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
        DOC_BEGIN("Stop listening for osc events in a seperate thread (join).");
        DOC_END;
       AC_DEBUG << "JSOscReceiver Start";

        asl::Ptr<y60::OscReceiver> myNative = jslib::getNativeAs<y60::OscReceiver>(cx, obj);
        if (myNative) {
            myNative->start();
        } else {
            assert(myNative);
        }
        return JS_TRUE;
    }


    JSFunctionSpec * 
    OscReceiver::Functions() {
        static JSFunctionSpec myFunctions[] = {
            {"start", Start, 0},
            {"stop", Stop, 0},
            {0}
        };
        return myFunctions;
    }
    
    dom::NodePtr
    getOscReceiverSettings(dom::NodePtr theSettings) {
        dom::NodePtr mySettings(0);
        if ( theSettings->nodeType() == dom::Node::DOCUMENT_NODE) {
            if (theSettings->childNode(0)->nodeName() == "settings") {
                mySettings = theSettings->childNode(0)->childNode("OscReceiver", 0);
            }
        } else if ( theSettings->nodeName() == "settings") {
            mySettings = theSettings->childNode("OscReceiver", 0);
        } else if ( theSettings->nodeName() == "OscReceiver" ) {
            mySettings = theSettings;
        }
        
        if ( ! mySettings ) {
            throw y60::OscException(
                std::string("Could not find OscReceiver node in settings: ") +
                as_string( * theSettings), PLUS_FILE_LINE );
        }
        return mySettings;
    }

};


extern "C"
EXPORT asl::PlugInBase* OscReceiver_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::OscReceiver(myDLHandle);
}
