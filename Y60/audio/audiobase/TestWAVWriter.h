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
//    $RCSfile: TestWAVWriter.h,v $
//
//     $Author: thomas $
//
//   $Revision: 1.2 $
//
//
// Description: 
//
//=============================================================================

#if !defined(INCL_TESTWAVWRITER)
#define INCL_TESTWAVWRITER

#include <asl/UnitTest.h>
#include <string>


class TestWAVWriter: public UnitTest {
public:
    explicit TestWAVWriter (): UnitTest ("TestWAVWriter") {}
    virtual void run();

private:
    void testSoundIn (const std::string & myFName, bool isStereo);
    void testSineGenerator(const std::string & myFName, bool isStereo);
    void testMultiChannel(const std::string & myFName, unsigned numChannels);

};


#endif


