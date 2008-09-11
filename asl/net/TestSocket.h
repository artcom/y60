/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: 
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      ok
//    documentation           :      fair
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      ok
//    interfaces              :      ok
//    confidence              :      ok
//    integration             :      ok
//    dependencies            :      ok
//    error handling          :      ok
//    logging                 :      notapp
//    cheesyness              :      ok
//
//    overall review status   :      ok
//
//    recommendations: move to inner class of *.tst.cpp file 
*/
#ifndef INCL_TESTSOCKETS
#define INCL_TESTSOCKETS

#include <string>
#include <asl/base/UnitTest.h>

#include "TCPServer.h"

class TestSocket: public UnitTest{
    public:
        TestSocket ()
            : UnitTest("TestSocket") {}

        virtual void run();
    private:

        /// creates a new tcp server. you 'll need to delete it
        inet::TCPServer * createTCPServer();
        
        void testHostAddressMethods ();
        void BaseTest();
        void TCPTest();
        void UDPTest();
        void testConnectionTimeout ();
        static void * TCPServerThread(void *arg);
        static void * UDPServerThread(void *arg); 

};

#endif
