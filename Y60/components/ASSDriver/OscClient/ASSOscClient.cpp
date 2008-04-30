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

ASSOscClient::ASSOscClient() :
    ASSDriver(),
    _myOSCStream( myBuffer, BUFFER_SIZE ),
    _myClientPort( 7001 ),
    _myServerPort( 7000 )
{
    AC_DEBUG << "created osc sender";
}

void
ASSOscClient::poll() {

    _myOSCStream.Clear();
    
    _myOSCStream << osc::BeginBundleImmediate;

    for (int i = 0; i < _myReceivers.size(); ++i){
        
        if ( ! _myReceivers[i].udpConnection ) {
            connectToServer(i);
            // Send a new "configure" to all receivers, not only the
            // newly connected one. This behaviour may change if
            // needed.
            createTransportLayerEvent("configure");
        }
    }
   
    processInput();
        
    _myOSCStream << osc::EndBundle;

    ASSURE( _myOSCStream.IsReady() );

    for (int i = 0; i < _myReceivers.size(); ++i){

        if (_myReceivers[i].udpConnection) {
            if ( _myOSCStream.Size() > 16 ) { // empty bundles have size 16
                // determined experimentally ... ain't nice
                try {
                    _myReceivers[i].udpConnection->send( _myOSCStream.Data(), _myOSCStream.Size() );
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
    std::string myAddress("/");
    myAddress += theType;

    // theIntensity argument added after MfN Project (TA, MS 2007-07-30)
    _myOSCStream << osc::BeginMessage( myAddress.c_str() )
        << theID
        << thePosition3D[0] << thePosition3D[1] << thePosition3D[2]
	<< theIntensity << osc::EndMessage;

}

void
ASSOscClient::createTransportLayerEvent( const std::string & theType) {
    std::string myAddress("/");
    myAddress += theType;
    _myOSCStream << osc::BeginMessage( myAddress.c_str() )
        << _myIDCounter++;

    if (theType == "configure") {
        _myOSCStream << _myGridSize[0] << _myGridSize[1];
    }

    _myOSCStream << osc::EndMessage;

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
}

} // end of namespace y60


