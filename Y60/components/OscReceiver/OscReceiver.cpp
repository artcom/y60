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

using namespace std;
using namespace asl;

extern std::string oureventxsd;

namespace y60 {

    OscReceiver::OscReceiver(DLHandle theHandle):
        _myEventSchema( new dom::Document( oureventxsd )  ),
        _myValueFactory( new dom::ValueFactory() ),
        asl::PosixThread(),
        asl::PlugInBase( theHandle ),
        _mySocket(IpEndpointName(IpEndpointName::ANY_ADDRESS, 12000), this )
    {

   
        registerStandardTypes( * _myValueFactory );
    }

    OscReceiver::~OscReceiver(){
        stop();
    }


    y60::EventPtrList OscReceiver::poll() {
        _myCurrentY60Events.clear();
        _myEventListLock.lock();
        
        _myCurrentY60Events = _myNewY60Events;
        _myNewY60Events.clear();

        _myEventListLock.unlock();

        return _myCurrentY60Events;
    }

    void OscReceiver::start(){
        AC_DEBUG << "start listening for osc events";
        fork();
    }

    void OscReceiver::stop(){
        _mySocket.AsynchronousBreak();
        join();
        AC_DEBUG << "stopped listening for osc events";
    }

    void OscReceiver::run( ) {
        try {
            AC_DEBUG << "launched the osc receiver thread";
    
            _mySocket.Run();

            AC_DEBUG << "receiver thread finished";
        } catch (const asl::Exception & ex) {
            AC_ERROR << "asl::Exception in OscReceiver thread: " << ex;
            throw;
        }
    }

    void OscReceiver::ProcessMessage( const osc::ReceivedMessage& m, 
                                      const IpEndpointName& remoteEndpoint ){

        try{
            _myEventListLock.lock();

            y60::GenericEventPtr myY60Event( new GenericEvent("onOscEvent",_myEventSchema, _myValueFactory));

            dom::NodePtr myNode = myY60Event->getNode();
    
            myNode->appendAttribute<string>("type", m.AddressPattern());

            _myNewY60Events.push_back(myY60Event);

            AC_TRACE << "c++: adding osc event " << *myNode;
            _myEventListLock.unlock();
            
        }catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                      << m.AddressPattern() << ": " << e.what() << "\n";
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

};


extern "C"
EXPORT asl::PlugInBase* OscReceiver_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::OscReceiver(myDLHandle);
}
