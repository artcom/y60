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
//    $RCSfile: TestButterworthFilter.h,v $
//
//     $Author: david $
//
//   $Revision: 1.1 $
//
//=============================================================================

#ifndef TEST_BUTTERWORTH_FILTER
#define TEST_BUTTERWORTH_FILTER

#include <asl/UnitTest.h>

namespace AudioBase {
    class ButterworthFilter;
}

class TestButterworthFilter: public UnitTest {
public:
    TestButterworthFilter(const char * myName): UnitTest (myName) {}

protected:    
    void runWithFilter (AudioBase::ButterworthFilter * theFilter, 
                        double expectedVolume);
};

#endif

