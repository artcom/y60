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
//    $RCSfile: TestINetMonitor.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description: 
//
//=============================================================================
#ifndef INCL_TESTINETMONITOR
#define INCL_TESTINETMONITOR

#include <string>
#include <asl/UnitTest.h>

class TestINetMonitor: public UnitTest{
    public:
        TestINetMonitor ()
            : UnitTest("TestINetMonitor") {}

        virtual void run();
    private:
        static void* startDummyServer(void *arg);
};


#endif
