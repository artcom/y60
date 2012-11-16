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
*/


#include "TestSynergyServer.h"

#include <asl/base/Time.h>
#include <asl/base/Logger.h>
#include <asl/base/begin_end.h>

const unsigned RESPONSE_TIMEOUT = 1500;
const unsigned READ_BUFFER_SIZE = 20000;
const unsigned BITMASK = 0x000000FF;


TestSynergyServer::TestSynergyServer() : UnitTest( "TestSynergyServer" ),
    _mySynergyServer( inet::getHostAddress( "localhost" ), 24800 )
{
}


void TestSynergyServer::run() {

    _myTestSocket.setRemoteAddr( inet::getHostAddress("localhost"), 24800 );
    _myTestSocket.connect();
    _myTestSocket.setBlockingMode( false );
    if (!_myTestSocket.isValid()) {
        _myTestSocket.retry(10);
    }
    ENSURE( _myTestSocket.isValid() );

    testSynergyPacket();
    testHandshake();
    testMouse();

    _myTestSocket.close();

}

void TestSynergyServer::testHandshake() {

    AC_TRACE << "TestSynergyServer::testHandshake";

    std::vector<unsigned char> myData;
    unsigned myNumBytes = 0;

    // check for correct handshake message

    ENSURE( waitUntilTimeout() );
    myNumBytes = receive( myData );
    printVectorAsHex( std::vector<unsigned char>( myData.begin(),
                                                  myData.begin() + myNumBytes) );

    ENSURE( myData[3] == 0x0b ); // ensure msg length == 11
    std::string myMsgString( myData.begin() + 4, myData.begin() + 11 );
    ENSURE( myMsgString == "Synergy" );
    ENSURE( myData[11] == 0x00 );
    ENSURE( myData[12] == 0x01 );
    ENSURE( myData[13] == 0x00 );
    ENSURE( myData[14] == 0x03 );

    char myHandshakeResponse[28] = {0x00, 0x00, 0x00, 0x18,  // length
                                    'S', 'y', 'n', 'e', 'r', 'g', 'y', // message type
                                    0x00, 0x01, 0x00, 0x03,  // version number
                                    0x00, 0x00, 0x00, 0x09,  // length
                                    'l', 'o', 'c', 'a', 'l', 'h', 'o', 's', 't'}; // client

    AC_TRACE << "Sending handshake response...";
    _myTestSocket.send( myHandshakeResponse, sizeof(myHandshakeResponse) );

    // check for query information message

    SynergyPacket myPacket;

    testResponse( 4, "QINF", myPacket );

    char myDInfoMessage[22] = { 0x00, 0x00, 0x00, 0x12, // length
                               'D','I','N','F',         // message type
                                0x00, 0x00, 0x00, 0x00,
                                0x07, '\x80', 0x04, '\xb0',
                                0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00 };

    _myTestSocket.send( myDInfoMessage, sizeof(myDInfoMessage) );

    testResponse( 4, "CIAK", myPacket );
    testResponse( 4, "CROP", myPacket );
    testResponse( 8, "DSOP", myPacket );
    testResponse( 14, "CINN", myPacket );
    testResponse( 4, "CALV", myPacket );

    // test keepalive
    //
    char myKeepAliveMsg[8] = { 0x00, 0x00, 0x00, 0x04,
                               'C', 'A', 'L', 'V' };
    for (unsigned i = 0; i < 5; i++) {
        _myTestSocket.send( myKeepAliveMsg, sizeof(myKeepAliveMsg) );
        testResponse( 4, "CALV", myPacket );
    }

    // test handshake-complete state
    ENSURE( _mySynergyServer.isConnected() );
    ENSURE( _mySynergyServer.getScreenSize() == asl::Vector2i( 1920, 1200 ) );

    char myDInfoMessage2[22] = { 0x00, 0x00, 0x00, 0x12, // length
                                'D','I','N','F',         // message type
                                 0x00, 0x00, 0x00, 0x00,
                                 0x04, 0x00, 0x03, 0x00,
                                 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00 };
    _myTestSocket.send( myDInfoMessage2, sizeof( myDInfoMessage2 ) );
    testResponse( 4, "CIAK", myPacket );
    ENSURE( _mySynergyServer.getScreenSize() == asl::Vector2i( 1024, 768 ) );

}

void TestSynergyServer::testSynergyPacket() {

    AC_TRACE << "TestSynergyServer::testSynergyPacket";

    char myTestPacket[26] = { 0x00, 0x00, 0x00, 0x01,
                              'T',
                              0x00, 0x00, 0x00, 0x02,
                              'T','e',
                              0x00, 0x00, 0x00, 0x03,
                              'T','e','s',
                              0x00, 0x00, 0x00, 0x04,
                              'T', 'e', 's', 't' };

    SynergyPacket myPacket( std::vector<unsigned char>( myTestPacket, myTestPacket +
                                                        sizeof(myTestPacket) ) );

    std::vector<unsigned char> myMsg;
    myPacket.getNextMessage( myMsg );
    ENSURE( std::string(myMsg.begin(), myMsg.end()) == "T" );
    myPacket.getNextMessage( myMsg );
    ENSURE( std::string(myMsg.begin(), myMsg.end()) == "Te" );
    myPacket.getNextMessage( myMsg );
    ENSURE( std::string(myMsg.begin(), myMsg.end()) == "Tes" );
    myPacket.getNextMessage( myMsg );
    ENSURE( std::string(myMsg.begin(), myMsg.end()) == "Test" );
    ENSURE( !myPacket.getNextMessage( myMsg ) );

}

void TestSynergyServer::testMouse() {

    AC_TRACE << "TestSynergyServer::testMouse";

    SynergyPacket myPacket;


    // test mouse motion
    unsigned theX = 1234;
    unsigned theY = 5678;

    std::vector<unsigned char> myMouseData;
    myMouseData.push_back( theX >> 8 & BITMASK );
    myMouseData.push_back( theX & BITMASK );
    myMouseData.push_back( theY >> 8 & BITMASK );
    myMouseData.push_back( theY & BITMASK );

    _mySynergyServer.onMouseMotion( theX, theY );
    testResponse( 8, "DMMV", myPacket, myMouseData );

    _mySynergyServer.onRelMouseMotion( theX, theY );
    testResponse( 8, "DMRM", myPacket, myMouseData );


    // test mouse buttons
    myMouseData.clear();
    unsigned char myButton = 1;

    myMouseData.push_back(myButton);

    _mySynergyServer.onMouseButton( myButton, true );
    testResponse( 5, "DMDN", myPacket, myMouseData );

    _mySynergyServer.onMouseButton( myButton, false );
    testResponse( 5, "DMUP", myPacket, myMouseData );

    // test mouse wheel
    myMouseData.clear();
    myMouseData.push_back(0x00);
    myMouseData.push_back(0x00);
    myMouseData.push_back(0x00);
    myMouseData.push_back(0x78);

    _mySynergyServer.onMouseWheel( 0, -1 );
    testResponse( 8, "DMWM", myPacket, myMouseData );

    myMouseData.clear();
    myMouseData.push_back(0x00);
    myMouseData.push_back(0x00);
    myMouseData.push_back(0xFF);
    myMouseData.push_back(0x88);

    _mySynergyServer.onMouseWheel( 0, 1 );
    testResponse( 8, "DMWM", myPacket, myMouseData );

}


void TestSynergyServer::testResponse( unsigned theLength, const std::string & theMessage,
                                 SynergyPacket & thePacket,
                                 const std::vector<unsigned char> & theTestData )
{
    AC_DEBUG << "TestSynergyServer::testResponse(" << theLength << "," << theMessage << ")";

    if (thePacket.empty()) {
        std::vector<unsigned char> myInputBuffer;
        unsigned myNumBytes = 0;

        ENSURE( waitUntilTimeout() );
        myNumBytes = receive( myInputBuffer );
        thePacket.assign( std::vector<unsigned char>( myInputBuffer.begin(),
                                                      myInputBuffer.begin()+myNumBytes ) );
    }

    std::vector<unsigned char> myMsg;
    ENSURE( thePacket.getNextMessage( myMsg ) );
    if (!myMsg.empty()) {
        std::string myMsgString( myMsg.begin(), myMsg.begin() + theMessage.size());
        ENSURE( myMsgString == theMessage );
        ENSURE( myMsg.size() == theLength );
        AC_TRACE << "Got Message: " << myMsgString;
        AC_TRACE << "Message len: " << myMsg.size();
    }


    if (!theTestData.empty()) {
        std::vector<unsigned char> myMsgData( myMsg.begin() + theMessage.size(),
                                              myMsg.end() );
        ENSURE( theTestData.size() == myMsgData.size() );
        ENSURE( std::equal( theTestData.begin(), theTestData.end(), myMsgData.begin() ) );
    }

}

bool TestSynergyServer::waitUntilTimeout() {
    asl::Time myCurrentTime;
    asl::Time myStartTime;
    while (!_myTestSocket.peek(1)
           && (myCurrentTime.millis() - myStartTime.millis() <= RESPONSE_TIMEOUT)) {
        myCurrentTime = myCurrentTime.setNow();
    }
    return (myCurrentTime.millis() - myStartTime.millis() < RESPONSE_TIMEOUT
            && _myTestSocket.peek(1));
}


unsigned TestSynergyServer::receive( std::vector<unsigned char> & theData ) {
    theData.resize( READ_BUFFER_SIZE, 0x00 );
    return _myTestSocket.receive( reinterpret_cast<char*>(asl::begin_ptr(theData)),
                                 theData.size() );
}
