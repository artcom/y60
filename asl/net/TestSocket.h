//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: TestSocket.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.2 $
//
//
// Description: 
//
//=============================================================================
#ifndef INCL_TESTSOCKETS
#define INCL_TESTSOCKETS

#include <string>
#include <asl/UnitTest.h>

class TestSocket: public UnitTest{
    public:
        TestSocket ()
            : UnitTest("TestSocket") {}

        virtual void run();
    private:
        void testHostAddressMethods ();
        void BaseTest();
        void TCPTest();
        void UDPTest();
        static void * TCPServerThread(void *arg);
        static void * UDPServerThread(void *arg); 

};

#endif
