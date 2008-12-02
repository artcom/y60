//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

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
    
    std::string myAddress("/");
    myAddress += theType;
    *_myOSCStreams[_myStreamIndex] << osc::BeginMessage( myAddress.c_str() )
                 << _myIDCounter++;

    if (theType == "configure") {
        *_myOSCStreams[_myStreamIndex] << _myGridSize[0] << _myGridSize[1];
        AC_TRACE << "configure with " << _myGridSize << ".";
    }

    *_myOSCStreams[_myStreamIndex] << osc::EndMessage;

}

void 
ASSOscClient::onUpdateSettings( dom::NodePtr theSettings ) {

    AC_DEBUG << "updating osc sender settings";

    ASSDriver::onUpdateSettings( theSettings );

    dom::NodePtr mySettings = getASSSettings( theSettings );

    int myClientPort;
    getConfigSetting( mySettings, "ClientPort", myClientPort, 7001 );
    if ( myClientPort != _myClientPort ) {
        _myClientPort = myClientPort;
        for (std::vector<Receiver>::size_type i = 0; i < _myReceivers.size(); ++i){
            _myReceivers[i].udpConnection = UDPConnectionPtr( 0 );
        }
    }

    int myServerPort;
    getConfigSetting( mySettings, "ServerPort", myServerPort, 7000 );
    if ( myServerPort != _myServerPort ) {
        _myServerPort = myServerPort;
        for (std::vector<Receiver>::size_type i = 0; i < _myReceivers.size(); ++i){
            _myReceivers[i].udpConnection = UDPConnectionPtr( 0 );
        }
    }

    _myOSCStreams.clear();
    if (mySettings->childNode("OscReceiverList")) {
        for (int oscReceiverIndex = 0; 
             oscReceiverIndex < static_cast<int>(mySettings->childNode("OscReceiverList")->childNodesLength()); 
             oscReceiverIndex++) {
            dom::NodePtr myReceiverNode = mySettings->childNode("OscReceiverList")->childNode(oscReceiverIndex);
            if (static_cast<int>(_myReceivers.size()> - 2) < static_cast<int>(oscReceiverIndex) ){
                _myReceivers.push_back(Receiver("invalid address", UDPConnectionPtr(0)));
            }
            string myAddress = myReceiverNode->getAttributeString("ip");
            if ( myAddress.compare(_myReceivers[oscReceiverIndex].address) != 0 ) {
                AC_DEBUG << "adding '" << myAddress << "' to the osc receiver list";
                _myReceivers[oscReceiverIndex].address =myAddress;
                _myReceivers[oscReceiverIndex].udpConnection = UDPConnectionPtr( 0 );
                char* myBuffer = new char[ BUFFER_SIZE ];
                _myOSCStreams.push_back(OutboundPacketStreamPtr(new osc::OutboundPacketStream( myBuffer, BUFFER_SIZE )));            
            } else {
                AC_PRINT << "sollte hier nicht vorbei kommen";
            }
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


