//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.  
//=============================================================================

#ifndef _TestSynergyServer_h_
#define _TestSynergyServer_h_

#include "SynergyServer.h"
#include <asl/base/UnitTest.h>
#include <asl/net/TCPClientSocket.h>

class TestSynergyServer : public UnitTest {

    public:

        TestSynergyServer();
        virtual void run();

    private: 

        void testSynergyPacket();
        void testHandshake();
        void testMouse();
//        void testKeyboard();

        unsigned receive( std::vector<unsigned char> & theData );
        void testResponse( unsigned theLength, const std::string & theMessage,
                           SynergyPacket & thePacket,
                           const std::vector<unsigned char> & theData = 
                           std::vector<unsigned char>() );
        bool waitUntilTimeout();

        SynergyServer _mySynergyServer;
        inet::TCPClientSocket _myTestSocket;

};

#endif //_TestSynergyServer_h_


