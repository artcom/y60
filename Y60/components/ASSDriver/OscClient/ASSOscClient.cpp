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

#include <asl/Assure.h>
#include <y60/SettingsParser.h>

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
    _myServerPort( 7000 )
{
    AC_DEBUG << "created osc sender";
}

void
ASSOscClient::poll() {

    

    for (int i = 0; i < _myReceivers.size(); ++i){
        _myOSCStreams.push_back(OutboundPacketStreamPtr(new osc::OutboundPacketStream( myBuffer, BUFFER_SIZE )));
        _myOSCStreams[i]->Clear();
        *_myOSCStreams[i] << osc::BeginBundle(ourPacketCounter++);//Immediate;
    }

    // Send a new "configure" to all receivers, not only the
    // newly connected one. This behaviour may change if
    // needed.
    createTransportLayerEvent("configure");
   
    processInput();
        
    for (int i = 0; i < _myReceivers.size(); ++i){
        *_myOSCStreams[i] << osc::EndBundle;
    }

    //ASSURE( _myOSCStream.IsReady() );

    for (int i = 0; i < _myReceivers.size(); ++i){
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
    _myReceivers[theIndex].udpConnection = UDPConnectionPtr( new UDPConnection( INADDR_ANY, _myClientPort ));
    _myReceivers[theIndex].udpConnection->connect( 
        getHostAddress( _myReceivers[theIndex].address.c_str() ), _myServerPort);
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

    // theIntensity argument added after MfN Project (TA, MS 2007-07-30)
    *_myOSCStreams[myTargetStream] << osc::BeginMessage( myAddress.c_str() )
        << theID
        << thePosition3D[0] << thePosition3D[1] << thePosition3D[2]
	<< theIntensity << osc::EndMessage;

}

void
ASSOscClient::createTransportLayerEvent( const std::string & theType) {
    for (int i = 0; i < _myReceivers.size(); ++i){
        if ( ! _myReceivers[i].udpConnection ) {
            connectToServer(i);
    
            std::string myAddress("/");
            myAddress += theType;
            *_myOSCStreams[i] << osc::BeginMessage( myAddress.c_str() )
                         << _myIDCounter++;
        
            if (theType == "configure") {
                *_myOSCStreams[i] << _myGridSize[0] << _myGridSize[1];
                AC_TRACE << "configure with " << _myGridSize << ".";
            }
        
            *_myOSCStreams[i] << osc::EndMessage;
        }
    }

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
        for (int i = 0; i < _myReceivers.size(); ++i){
            _myReceivers[i].udpConnection = UDPConnectionPtr( 0 );
        }
    }

    int myServerPort;
    getConfigSetting( mySettings, "ServerPort", myServerPort, 7000 );
    if ( myServerPort != _myServerPort ) {
        _myServerPort = myServerPort;
        for (int i = 0; i < _myReceivers.size(); ++i){
            _myReceivers[i].udpConnection = UDPConnectionPtr( 0 );
        }
    }


    std::string myReceiversString;
    getConfigSetting( mySettings, "ServerAddress", myReceiversString, string("127.0.0.1") );
    vector<string> myAddresses = splitString(myReceiversString, ",");
    for (int i = 0; i < myAddresses.size(); ++i){
        if ((int) _myReceivers.size() - 2 < i){
            _myReceivers.push_back(Receiver("invalid address", UDPConnectionPtr(0)));
        }
        if ( myAddresses[i].compare(_myReceivers[i].address) != 0 ) {
            AC_DEBUG << "adding '" << myAddresses[i] << "' to the osc receiver list";
            _myReceivers[i].address = myAddresses[i];
            _myReceivers[i].udpConnection = UDPConnectionPtr( 0 );
        }
    }

    std::string myOscRegionString;
    getConfigSetting( mySettings, "OscClientASSWiresFilter", myOscRegionString, string("") );
    vector<string> myOscRegions = splitString(myOscRegionString, ",");
    if (myOscRegions.size() > 0) {
        if (myOscRegions.size() == _myReceivers.size()+1) {
            int myFirstWire = as<int>(myOscRegions[0]);
            for (int i = 1; i < myOscRegions.size(); ++i){
                int myNextWire = as<int>(myOscRegions[i]);
                if (myFirstWire < myNextWire) {
                    Vector2i myRegion(myFirstWire, myNextWire);
                    _myOscRegions.push_back(myRegion);
                    myFirstWire = myNextWire;
                    AC_PRINT << "i: " << i << " region: " << myRegion;
                } else {
                    AC_ERROR << "Keypoints # " << (i -1) << " and #: " << i << " not ascending!";
                }
            }
        } else {
            // not enough keypoints
            AC_ERROR << "Not correct keypoints : " << myOscRegions.size() << " must be: " << _myReceivers.size()+1 << "!";
        }
        
        
    }
    
}

} // end of namespace y60


