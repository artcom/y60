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
        _myReceiverAddress(""), _myReceiverPort(0), _myReceiverUDPConnection()
    {
    }

    OscSender::~OscSender()
    {
        if(_myReceiverUDPConnection) close();
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
                try {
                    if (myChildNode->nodeName() == "float") {
                        myOSCStream << as<float>(string(myChildNode->childNode("#text")->nodeValue()).c_str());
                    } else if (myChildNode->nodeName() == "int") {
                        myOSCStream << as<osc::int32>(string(myChildNode->childNode("#text")->nodeValue()).c_str());
                    } else if (myChildNode->nodeName() == "double") {
                        myOSCStream << as<double>(string(myChildNode->childNode("#text")->nodeValue()).c_str());
                    } else if (myChildNode->nodeName() == "bool") {
                        myOSCStream << as<bool>(string(myChildNode->childNode("#text")->nodeValue()).c_str());
                    } else{
                        myOSCStream << string(myChildNode->childNode("#text")->nodeValue()).c_str();
                    }
                } catch(ParseException theEx) {
                    // conversion failed, send it as a string
                    myOSCStream << string(myChildNode->childNode("#text")->nodeValue()).c_str();
                }
            }


            if (myValidPacket) {
                myOSCStream << osc::EndMessage;
                myOSCStream << osc::EndBundle;
                ASSURE( myOSCStream.IsReady() );
                if ( myOSCStream.Size() > 16 ) { // empty bundles have size 16
                    _myReceiverUDPConnection->send( myOSCStream.Data(), myOSCStream.Size() );
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
