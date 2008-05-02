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
#include <asl/string_functions.h>

using namespace std;
using namespace asl;

extern std::string ourosceventxsd;
extern std::string ourasseventxsd;


namespace y60 {

    OscReceiver::OscReceiver(DLHandle theHandle):
        _myEventSchema( new dom::Document( ourosceventxsd )  ),
        _myASSEventSchema( new dom::Document( ourasseventxsd )  ),
        _myValueFactory( new dom::ValueFactory() ),
        asl::PlugInBase( theHandle ),
        _mySocket(IpEndpointName(IpEndpointName::ANY_ADDRESS, 12000), this )
    {

   
        registerStandardTypes( * _myValueFactory );
    }

    OscReceiver::~OscReceiver(){
        
    }


    y60::EventPtrList OscReceiver::poll() {
       
        // start the socket listening (sockets "Break()"-Method is called at the
        // end of PrecessMessage to get nice single threading)
        _mySocket.Run();
    
        _myCurrentY60Events.clear();
        
        _myCurrentY60Events = _myNewY60Events;
        _myNewY60Events.clear();


        return _myCurrentY60Events;
    }


    void OscReceiver::ProcessMessage( const osc::ReceivedMessage& theMessage, 
                                      const IpEndpointName& theRemoteEndpoint ){

        try{

            y60::GenericEventPtr myY60Event( new GenericEvent("onOscEvent",_myEventSchema, _myValueFactory));

            dom::NodePtr myNode = myY60Event->getNode();
    
            myNode->appendAttribute<string>("type", string(theMessage.AddressPattern()).substr(1));
            char mySenderAddress[IpEndpointName::ADDRESS_STRING_LENGTH];
            theRemoteEndpoint.AddressAsString(mySenderAddress);
            myNode->appendAttribute<string>("sender", string(mySenderAddress));

            osc::ReceivedMessage::const_iterator myArgItr = 
                 theMessage.ArgumentsBegin();

            while ( myArgItr != theMessage.ArgumentsEnd() ){
       
                if (myArgItr->IsString()){
                    myNode->appendChild(dom::Node("<string>" + 
                                                  asl::as_string(myArgItr->AsString()) + 
                                                  "</string>").firstChild());
                } else if (myArgItr->IsFloat()){
                    myNode->appendChild(dom::Node("<float>" + 
                                                  asl::as_string(myArgItr->AsFloat()) + 
                                                  "</float>").firstChild());
                } else if (myArgItr->IsBool()){
                    myNode->appendChild(dom::Node("<bool>" + 
                                                  asl::as_string(myArgItr->AsBool()) + 
                                                  "</bool>").firstChild());
                } else if (myArgItr->IsInt32()){
                    myNode->appendChild(dom::Node("<int>" + 
                                                  asl::as_string(myArgItr->AsInt32()) + 
                                                  "</int>").firstChild());
                } else if (myArgItr->IsInt64()){
                    myNode->appendChild(dom::Node("<int>" + 
                                                  asl::as_string(myArgItr->AsInt64()) + 
                                                  "</int>").firstChild());
                } else if (myArgItr->IsDouble()){
                    myNode->appendChild(dom::Node("<double>" + 
                                                  asl::as_string(myArgItr->AsDouble()) + 
                                                  "</double>").firstChild());
                } else {
                    AC_ERROR << "unknown osc message received";
                }
            
                myArgItr++;

            }

            _myNewY60Events.push_back(myY60Event);

            cout << "c++: adding osc event " << *myNode;
    
            // exit from osc sockets "Run()" method
            _mySocket.Break();
            
        }catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                      << theMessage.AddressPattern() << ": " << e.what() << "\n";
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

    JSFunctionSpec * 
    OscReceiver::Functions() {
        static JSFunctionSpec myFunctions[] = {
            {0}
        };
        return myFunctions;
    }

};


extern "C"
EXPORT asl::PlugInBase* OscReceiver_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::OscReceiver(myDLHandle);
}
