//=============================================================================
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
//    $RCSfile: TestSoundOut.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.3 $
//
//
// Description: 
//
//=============================================================================

#if !defined(INCL_TESTSOUNDOUT)
#define INCL_TESTSOUNDOUT

#include <asl/UnitTest.h>

class TestSoundOut: public UnitTest {
public:
    explicit TestSoundOut (): UnitTest ("TestSoundOut") {}
    virtual void run();

private:
    void infoTest();
    void sineTest ();
    void timeTest();
    void volumeTest();
};


#endif


