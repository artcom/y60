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
//    $RCSfile: TestMixer.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.4 $
//
//
// Description: 
//
//=============================================================================

#if !defined(INCL_TESTMIXER)
#define INCL_TESTMIXER

#include "AudioDefs.h"

#include <asl/UnitTest.h>

namespace AudioBase {
    class SoundOut;
    class Mixer;
    class NullSource;
    class SineGenerator;
    class SoundCard;
    class AudioScheduler;
}

class TestMixer: public UnitTest {
public:
    explicit TestMixer (): UnitTest ("TestMixer") {}
    virtual void run();

private:
    void setupMixer(AudioBase::AudioScheduler& myScheduler, AudioBase::SoundOut * myWriter, bool myStereo);
    void teardownMixer();
    AudioBase::SineGenerator * 
            createAndConnectSineGenerator
                (AudioBase::AudioScheduler& myScheduler, AudioBase::ChannelType theType=AudioBase::Mono);
    void initModules();
    void simpleTest (bool myStereo);
    void complexTest (bool myStereo);
    void threadSafetyTest (bool myStereo);
    void volumeTest(bool myStereo);

    AudioBase::Mixer * _myMixer;
    AudioBase::NullSource * _myNull;
};


#endif


