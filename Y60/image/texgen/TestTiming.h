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
//    $RCSfile: TestTiming.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description: 
//
//=============================================================================

#if !defined(INCL_TESTTIMING)
#define INCL_TESTTIMING

#include <asl/UnitTest.h>

#include <paintlib/planybmp.h>

#include <vector>

class TestTiming: public UnitTest {
public:
    explicit TestTiming (): UnitTest ("TestTiming")
        {}
    virtual void run();

private:
};


#endif


