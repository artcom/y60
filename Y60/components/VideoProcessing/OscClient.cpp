//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "OscClient.h"

#include <asl/Assure.h>
#include <y60/SettingsParser.h>


using namespace asl;
using namespace y60;
using namespace std;
using namespace inet;

namespace y60 {

OscClient::OscClient() :
    VideoProcessingExtension(),
    _myOSCStream( myBuffer, BUFFER_SIZE ),
    _myClientPort( 7001 ),
    _myServerPort( 7000 ),
    _myServerAddress("127.0.0.1")
{
}


void
OscClient::poll() {

    //AC_PRINT << "==== poll ====";

    _myOSCStream.Clear();

    _myOSCStream << osc::BeginBundleImmediate;

    if ( ! _myConnection ) {
        connectToServer();
    }

    
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
OscClient::createData() {
    std::string myAddress("/");
    _myOSCStream << osc::BeginMessage( myAddress.c_str() )
                 << "test" << "test2";
    _myOSCStream << osc::EndMessage;

}


void
OscClient::connectToServer() {
    _myConnection = UDPConnectionPtr( new UDPConnection( INADDR_ANY, _myClientPort ));
    _myConnection->connect( getHostAddress( _myServerAddress.c_str() ), _myServerPort);
}


void 
OscClient::onUpdateSettings( dom::NodePtr theSettings ) {

    dom::NodePtr mySettings(0);
    if ( theSettings->nodeType() == dom::Node::DOCUMENT_NODE) {
        if (theSettings->childNode(0)->nodeName() == "settings") {
            mySettings = theSettings->childNode(0)->childNode("OscClient", 0);
        }
    }
    if ( ! mySettings ) {
        throw Exception(std::string("Could not find settings: ") +
            as_string( * theSettings), PLUS_FILE_LINE );
    }

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


