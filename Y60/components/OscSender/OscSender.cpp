/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
*/

#include "OscSender.h"

#include <asl/base/Logger.h>
#include <asl/base/Assure.h>

#include <string>
#include <assert.h>

using namespace std;
using namespace asl;
using namespace inet;

namespace y60 {
    static long ourPacketCounter = 0;
    OscSender::OscSender() : 
        _myReceiverAddress(""), _myReceiverPort(0), _myReceiverUDPConnection(0)
    {
    }

    OscSender::~OscSender()
    {
        close();
    }
    
    bool OscSender::connect(string theReceiverAddress, unsigned theReceiverPort, unsigned theSenderPort) {      
        _myReceiverAddress = theReceiverAddress;
        _myReceiverPort    = theReceiverPort;
        
        _myReceiverUDPConnection = UDPConnectionPtr( new UDPConnection( INADDR_ANY, static_cast<asl::Unsigned16>(theSenderPort) ));
        _myReceiverUDPConnection->connect( getHostAddress( theReceiverAddress.c_str() ), static_cast<asl::Unsigned16>(theReceiverPort));
        AC_DEBUG << "Connected to : " << theReceiverAddress << " with port : " << theReceiverPort << " and sender port: " << theSenderPort;
        return true; 
    }


    void OscSender::close() {
        _myReceiverUDPConnection->close();
        AC_DEBUG << "OscSender::Connection closed.";
    }

    unsigned OscSender::send(dom::NodePtr theOscEvent)
    {
        unsigned myBytesWitten = 0;
        if (_myReceiverUDPConnection) {
            string myValidationMessage = "";
            bool myValidPacket = true;
            char myBuffer[ MAX_UDP_PACKET_SIZE ];        
            osc::OutboundPacketStream myOSCStream( myBuffer, MAX_UDP_PACKET_SIZE);
            myOSCStream << osc::BeginBundle(ourPacketCounter++);
            
            std::string myAddress("/");
            if (theOscEvent->getAttribute("type") ) {                
                myAddress += theOscEvent->getAttributeString("type");
                myOSCStream << osc::BeginMessage( myAddress.c_str() );
            } else {
                myValidationMessage = "No type attribute found!";
                myValidPacket = false;  
            }
            for (unsigned myChildIndex = 0 ; myChildIndex < theOscEvent->childNodesLength(); myChildIndex++) {
                dom::NodePtr myChildNode = theOscEvent->childNode(myChildIndex);
                //AC_DEBUG << "Child : " << myChildIndex << " -> " << *myChildNode;
                myOSCStream << string(myChildNode->childNode("#text")->nodeValue()).c_str();
            }
            
            
            if (myValidPacket) {
                myOSCStream << osc::EndMessage;
                myOSCStream << osc::EndBundle;
                ASSURE( myOSCStream.IsReady() );
                if ( myOSCStream.Size() > 16 ) { // empty bundles have size 16                
                    try {
                        // determined experimentally ... ain't nice                    
                        _myReceiverUDPConnection->send( myOSCStream.Data(), myOSCStream.Size() );                            
                    } catch (const inet::SocketException & ex) {
                        AC_WARNING << "Failed to send to " << _myReceiverAddress << " at port "
                                   << _myReceiverPort << ": " << ex;
                        _myReceiverUDPConnection = UDPConnectionPtr( 0 );
                    }
                    myBytesWitten = myOSCStream.Size();
                    AC_DEBUG << "Send event: " << *theOscEvent;
                }
            } else {
                AC_WARNING << "Sorry, invalid oscpacket: " << *theOscEvent << " -> " << myValidationMessage;
            }
        }
        return myBytesWitten;
    }
}
