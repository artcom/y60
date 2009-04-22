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

#include "ASSOscClient.h"

#include <asl/base/Assure.h>
#include <y60/base/SettingsParser.h>

using namespace asl;
using namespace y60;
using namespace std;
using namespace inet;

namespace y60 {
static long ourPacketCounter = 0;

ASSOscClient::ASSOscClient() :
    ASSDriver(),
    //_myOSCStream( myBuffer, BUFFER_SIZE ),
    _myClientPort( 7001 ),
    _myServerPort( 7000 ),
    _myStreamIndex(0)
{
    AC_DEBUG << "created osc sender";
}

void
ASSOscClient::poll() {

    
    for (std::vector<Receiver>::size_type i = 0; i < _myReceivers.size(); ++i){
        _myOSCStreams[i]->Clear();
        *_myOSCStreams[i] << osc::BeginBundle(ourPacketCounter++);//Immediate;

        if ( ! _myReceivers[i].udpConnection ) {
            connectToServer(i);
        
            // Send a new "configure" to all receivers, not only the
            // newly connected one. This behaviour may change if
            // needed.
            _myStreamIndex = i;
            createTransportLayerEvent("configure");
        }
    }
   
    processInput();
        
    for (std::vector<Receiver>::size_type i = 0; i < _myReceivers.size(); ++i){
        *_myOSCStreams[i] << osc::EndBundle;
    }

    //ASSURE( _myOSCStream.IsReady() );

    for (std::vector<Receiver>::size_type i = 0; i < _myReceivers.size(); ++i){
        ASSURE( _myOSCStreams[i]->IsReady() );
        if (_myReceivers[i].udpConnection) {
            if ( _myOSCStreams[i]->Size() > 16 ) { // empty bundles have size 16
                // determined experimentally ... ain't nice
                try {
                    _myReceivers[i].udpConnection->send( _myOSCStreams[i]->Data(), _myOSCStreams[i]->Size() );
                } catch (const inet::SocketException & ex) {
                    AC_WARNING << "Failed to connect to " << _myReceivers[i].address << " at port "
                               << _myServerPort << ": " << ex;
                    _myReceivers[i].udpConnection = UDPConnectionPtr( 0 );
                }
            }
        }
    }

    //AC_PRINT << "==== done ====";
}

void
ASSOscClient::connectToServer(int theIndex) {
    AC_DEBUG << "connecting to server " << _myReceivers[theIndex].address << ":" << _myServerPort;
    _myReceivers[theIndex].udpConnection = UDPConnectionPtr( new UDPConnection( INADDR_ANY, static_cast<asl::Unsigned16>(_myClientPort) ));
    _myReceivers[theIndex].udpConnection->connect( 
        getHostAddress( _myReceivers[theIndex].address.c_str() ), static_cast<asl::Unsigned16>(_myServerPort) );
}



void
ASSOscClient::createEvent( int theID, const std::string & theType,
        const Vector2f & theRawPosition, const Vector3f & thePosition3D,
        const asl::Box2f & theROI, float theIntensity, const ASSEvent & theEvent)
{
    // TODO send ROI?
    //AC_PRINT << "createEvent: " << theType;
    unsigned myTargetStream = 0;
    for (unsigned myOscRegionsIndex =0; myOscRegionsIndex < _myOscRegions.size(); myOscRegionsIndex++) {
        if (thePosition3D[0] >= _myOscRegions[myOscRegionsIndex][0] && 
            thePosition3D[0] <= _myOscRegions[myOscRegionsIndex][1] ) {
                myTargetStream = myOscRegionsIndex;
                AC_DEBUG << "Dispatch eventpos : " << thePosition3D << " to client # " << myTargetStream << " cause regions fitted: " << _myOscRegions[myOscRegionsIndex];
                break;    
            }
    }
    std::string myAddress("/");
    myAddress += theType;   

    for (unsigned myOscRegionsIndex =0; myOscRegionsIndex < _myOscRegions.size(); myOscRegionsIndex++) {
        if ( (_myOscRegions[myOscRegionsIndex][0] ==-1 && _myOscRegions[myOscRegionsIndex][0] ==-1 ) ||
             ( thePosition3D[0] >= _myOscRegions[myOscRegionsIndex][0] && 
            thePosition3D[0] <= _myOscRegions[myOscRegionsIndex][1]) ) {
                
            // theIntensity argument added after MfN Project (TA, MS 2007-07-30)
            *_myOSCStreams[myOscRegionsIndex] << osc::BeginMessage( myAddress.c_str() )
                << theID
                << thePosition3D[0] << thePosition3D[1] << thePosition3D[2]
        	<< theIntensity << osc::EndMessage;
                AC_DEBUG << "Dispatch eventpos : " << thePosition3D << " to client # " << myOscRegionsIndex << " cause regions fitted: " << _myOscRegions[myOscRegionsIndex];
            }
    }

}

void
ASSOscClient::createTransportLayerEvent( const std::string & theType) {

    if(_myOSCStreams.size() == 0) {
        AC_WARNING << "Ignoring event " << theType << " because there are no streams.";
        return;
    }
    
    std::string myAddress("/");
    myAddress += theType;
    *_myOSCStreams[_myStreamIndex] << osc::BeginMessage( myAddress.c_str() );
    *_myOSCStreams[_myStreamIndex] << _myIDCounter++;

    if (theType == "configure") {
        *_myOSCStreams[_myStreamIndex] << _myGridSize[0] << _myGridSize[1];
        AC_TRACE << "configure with " << _myGridSize << ".";
    }

    *_myOSCStreams[_myStreamIndex] << osc::EndMessage;

}

void 
ASSOscClient::onUpdateSettings( dom::NodePtr theSettings ) {

    AC_WARNING << "updating osc sender settings";

    ASSDriver::onUpdateSettings( theSettings );

    AC_WARNING << "ASSDriver was happy";

    dom::NodePtr mySettings = getASSSettings( theSettings );

    AC_WARNING << "at client port";

    int myClientPort;
    getConfigSetting( mySettings, "ClientPort", myClientPort, 7001 );
    if ( myClientPort != _myClientPort ) {
        _myClientPort = myClientPort;
        for (std::vector<Receiver>::size_type i = 0; i < _myReceivers.size(); ++i){
            _myReceivers[i].udpConnection = UDPConnectionPtr( 0 );
        }
    }

    AC_WARNING << "at server port";

    int myServerPort;
    getConfigSetting( mySettings, "ServerPort", myServerPort, 7000 );
    if ( myServerPort != _myServerPort ) {
        _myServerPort = myServerPort;
        for (std::vector<Receiver>::size_type i = 0; i < _myReceivers.size(); ++i){
            _myReceivers[i].udpConnection = UDPConnectionPtr( 0 );
        }
    }

    AC_WARNING << "at receivers";
    
    _myOSCStreams.clear();
    if (mySettings->childNode("OscReceiverList")) {
        for (int oscReceiverIndex = 0; 
             oscReceiverIndex < static_cast<int>(mySettings->childNode("OscReceiverList")->childNodesLength()); 
             oscReceiverIndex++) {
            AC_WARNING << "recv " << oscReceiverIndex;

            dom::NodePtr myReceiverNode = mySettings->childNode("OscReceiverList")->childNode(oscReceiverIndex);
            if (oscReceiverIndex >= _myReceivers.size()){
                AC_WARNING << "pushing one recv";
                _myReceivers.push_back(Receiver("invalid address", UDPConnectionPtr(0)));
            }
            string myAddress = myReceiverNode->getAttributeString("ip");
            AC_WARNING << "addr " << myAddress;
            AC_WARNING << "cursize: " << _myReceivers.size();
                AC_DEBUG << "adding '" << myAddress << "' to the osc receiver list";
                _myReceivers[oscReceiverIndex].address =myAddress;
                _myReceivers[oscReceiverIndex].udpConnection = UDPConnectionPtr( 0 );
                char* myBuffer = new char[ BUFFER_SIZE ];
                _myOSCStreams.push_back(OutboundPacketStreamPtr(new osc::OutboundPacketStream( myBuffer, BUFFER_SIZE )));            
            AC_WARNING << "region";
            asl::Vector2i myRegion(-1,-1);
            if (myReceiverNode->getAttribute("wire_range")) {
               myRegion = myReceiverNode->getAttributeValue<asl::Vector2i>("wire_range");
            }
            _myOscRegions.push_back(myRegion);
            AC_DEBUG << "Added Region #: " << oscReceiverIndex << " region: " << myRegion;
        }
    }
}

} // end of namespace y60


