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
//    $RCSfile: TestSineGenerator.h,v $
//
//     $Author: david $
//
//   $Revision: 1.1 $
//
//
// Description: 
//
//=============================================================================

#if !defined(INCL_TESTSINEGENERATOR)
#define INCL_TESTSINEGENERATOR

#include <asl/UnitTest.h>

class TestSineGenerator: public UnitTest {
public:
    explicit TestSineGenerator (): UnitTest ("TestSineGenerator") {}
    virtual ~TestSineGenerator () { }
    virtual void run();

};


#endif


