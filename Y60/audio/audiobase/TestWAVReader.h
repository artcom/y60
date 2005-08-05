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
//    $RCSfile: TestWAVReader.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.2 $
//
//
// Description: 
//
//=============================================================================

#if !defined(INCL_TESTWAVREADER)
#define INCL_TESTWAVREADER

#include <asl/UnitTest.h>
#include <string>

namespace AudioBase {
    class WAVReader;
    class SoundOut;
    class AudioScheduler;
}

class TestWAVReader: public UnitTest {
public:
    explicit TestWAVReader (): UnitTest ("TestWAVReader") {}
    virtual void run();

private:
    void testException (const std::string & myFName, const std::string & myTestName);
    void testFile (const std::string & myFName, bool isStereo, double expectedDuration);
    void testLoop ();
    void testReader (AudioBase::AudioScheduler & theScheduler, AudioBase::SoundOut& theWriter, 
            AudioBase::WAVReader & theReader, bool isStereo, double expectedDuration);
    void testStartStop();
    void testVolume ();
};


#endif


