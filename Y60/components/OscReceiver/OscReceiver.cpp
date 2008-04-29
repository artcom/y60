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

using namespace std;
using namespace asl;

namespace y60 {

    OscReceiver::OscReceiver(DLHandle theHandle):
        asl::PosixThread(),
        asl::PlugInBase( theHandle )
    {
   
/*        registerStandardTypes( * _myValueFactory );
          registerSomTypes( * _myValueFactory );
*/

        fork();
    }


    y60::EventPtrList 
    OscReceiver::poll() {
        _myEvents.clear();

        //processMessage();

        return _myEvents;
    }


/*

void
OscReceiver::onGetProperty(const std::string & thePropertyName,
PropertyValue & theReturnValue) const {

        
}

void 
OscReceiver::onSetProperty(const std::string & thePropertyName,
const PropertyValue & thePropertyValue)
{
}
*/

void OscReceiver::run( ) {
    try {
        AC_PRINT << "launched the osc receiver thread";
        UdpListeningReceiveSocket s(
            IpEndpointName( IpEndpointName::ANY_ADDRESS, 12000),
            this );
 	
        s.Run();

        AC_PRINT << "receiver thread finished";
    } catch (const asl::Exception & ex) {
        AC_ERROR << "asl::Exception in OscReceiver thread: " << ex;
        throw;
    }
}

    void OscReceiver::ProcessMessage( const osc::ReceivedMessage& m, 
                                      const IpEndpointName& remoteEndpoint ){

        try{
            std::cout << "received message: " << m.AddressPattern() <<  std::endl;
        }catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                      << m.AddressPattern() << ": " << e.what() << "\n";
        }
    }


    static JSBool
    Start(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { 
        DOC_BEGIN("");
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
            {0}
        };
        return myFunctions;
    }

}


    extern "C"
    EXPORT asl::PlugInBase* OscReceiver_instantiatePlugIn(asl::DLHandle myDLHandle) {
        return new y60::OscReceiver(myDLHandle);
    }



/*
  OscPacketReceiver receiver;

  UdpListeningReceiveSocket s(
  IpEndpointName( IpEndpointName::ANY_ADDRESS, 12000),
  &receiver );
 	
  std::cout << "press ctrl-c to end\n";
 	
  s.RunUntilSigInt();
 	
  return 0;
*/
