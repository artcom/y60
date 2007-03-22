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

using namespace asl;
using namespace y60;
using namespace std;
using namespace inet;

namespace y60 {

ASSOscClient::ASSOscClient() :
    ASSDriver(),
    _myOSCStream( myBuffer, BUFFER_SIZE ),
    _myClientPort( 7001 ),
    _myServerPort( 7000 ),
    _myServerAddress("127.0.0.1"),
    _myReconnectCounter( 0 )
{
}


void
ASSOscClient::poll() {
    _myOSCStream.Clear();

    _myOSCStream << osc::BeginBundleImmediate;

    processInput();

    _myOSCStream << osc::EndBundle;

    ASSURE( _myOSCStream.IsReady() );

    if (_myConnection) {
        if ( _myOSCStream.Size() > 16 ) { // empty bundles have size 16
                                          // determined experimentally ... ain't nice
            try {
                _myConnection->send( _myOSCStream.Data(), _myOSCStream.Size() );
            } catch (const inet::SocketException & ex) {
                AC_WARNING << "Connection lost: " << ex;
                _myConnection = UDPConnectionPtr( 0 );
            }
        }
    } else {
        connectToServer();
    }
}

void
ASSOscClient::connectToServer() {
    if ( _myReconnectCounter == 0) {
        _myConnection = UDPConnectionPtr( new UDPConnection( INADDR_ANY, _myClientPort ));
        _myConnection->connect( getHostAddress( _myServerAddress.c_str() ), _myServerPort);
        _myReconnectCounter = 30;
    } else {
        _myReconnectCounter -= 1;
    }
}



void
ASSOscClient::createEvent( int theID, const std::string & theType,
        const Vector2f & theRawPosition, const Vector3f & thePosition3D)
{
    std::string myAddress("/");
    myAddress += theType;
    _myOSCStream << osc::BeginMessage( myAddress.c_str() )
        << theID
        << thePosition3D[0] << thePosition3D[1] << thePosition3D[2]
        << osc::EndMessage;

}

void
ASSOscClient::createTransportLayerEvent( int theID, const std::string & theType) {
    std::string myAddress("/");
    myAddress += theType;
    _myOSCStream << osc::BeginMessage( myAddress.c_str() )
        << theID;

    if (theType == "configure") {
        _myOSCStream << _myGridSize[0] << _myGridSize[1];
    }

    _myOSCStream << osc::EndMessage;

}

void 
ASSOscClient::onUpdateSettings( dom::NodePtr theSettings ) {

    ASSDriver::onUpdateSettings( theSettings );

    dom::NodePtr mySettings = getASSSettings( theSettings );

    int myClientPort;
    getConfigSetting( mySettings, "ClientPort", myClientPort );
    if ( myClientPort != _myClientPort ) {
        _myClientPort = myClientPort;
        _myConnection = UDPConnectionPtr( 0 );
        _myReconnectCounter = 0;
    }

    int myServerPort;
    getConfigSetting( mySettings, "ServerPort", myServerPort );
    if ( myServerPort != _myServerPort ) {
        _myServerPort = myServerPort;
        _myConnection = UDPConnectionPtr( 0 );
        _myReconnectCounter = 0;
    }

    std::string myServerAddress;
    getConfigSetting( mySettings, "ServerAddress", myServerAddress );
    if ( myServerAddress != _myServerAddress ) {
        _myServerAddress = myServerAddress;
        _myConnection = UDPConnectionPtr( 0 );
        _myReconnectCounter = 0;
    }
}

} // end of namespace y60


