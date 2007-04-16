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
    _myServerAddress("127.0.0.1")
{
}


void
ASSOscClient::poll() {

    //AC_PRINT << "==== poll ====";

    _myOSCStream.Clear();

    _myOSCStream << osc::BeginBundleImmediate;

    if ( ! _myConnection ) {
        connectToServer();
        // if(running)
        createTransportLayerEvent( 0, "configure");
    }

    processInput();

    _myOSCStream << osc::EndBundle;

    ASSURE( _myOSCStream.IsReady() );

    if (_myConnection) {
        if ( _myOSCStream.Size() > 16 ) { // empty bundles have size 16
                                          // determined experimentally ... ain't nice
            try {
                _myConnection->send( _myOSCStream.Data(), _myOSCStream.Size() );
            } catch (const inet::SocketException & ex) {
                AC_WARNING << "Failed to connect to " << _myServerAddress << " at port "
                           << _myServerPort << ": " << ex;
                _myConnection = UDPConnectionPtr( 0 );
            }
        }
    }
    //AC_PRINT << "==== done ====";
}

void
ASSOscClient::connectToServer() {
    _myConnection = UDPConnectionPtr( new UDPConnection( INADDR_ANY, _myClientPort ));
    _myConnection->connect( getHostAddress( _myServerAddress.c_str() ), _myServerPort);
}



void
ASSOscClient::createEvent( int theID, const std::string & theType,
        const Vector2f & theRawPosition, const Vector3f & thePosition3D,
        const asl::Box2f & theROI)
{
    // TODO send ROI?
    //AC_PRINT << "createEvent: " << theType;
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
    getConfigSetting( mySettings, "ClientPort", myClientPort, 7001 );
    if ( myClientPort != _myClientPort ) {
        _myClientPort = myClientPort;
        _myConnection = UDPConnectionPtr( 0 );
    }

    int myServerPort;
    getConfigSetting( mySettings, "ServerPort", myServerPort, 7000 );
    if ( myServerPort != _myServerPort ) {
        _myServerPort = myServerPort;
        _myConnection = UDPConnectionPtr( 0 );
    }

    std::string myServerAddress;
    getConfigSetting( mySettings, "ServerAddress", myServerAddress, string("127.0.0.1") );
    if ( myServerAddress != _myServerAddress ) {
        _myServerAddress = myServerAddress;
        _myConnection = UDPConnectionPtr( 0 );
    }
}

} // end of namespace y60


