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
//    $RCSfile: TestAudioController.h,v $
//
//     $Author: pavel $
//
//   $Revision: 1.3 $
//
//
// Description: 
//
//=============================================================================

#ifndef INCL_TESTAUDIOCONTROLLER
#define INCL_TESTAUDIOCONTROLLER

#include <asl/UnitTest.h>
#include <asl/Point234.h>

#include "AudioController.h"

#include <string>

namespace asl {
    class Coord2dD;
}

class TestAudioController: public UnitTest {
public:
    explicit TestAudioController (const std::string & _myDataDir);

    virtual void run();

private:
    void runMultiChannel();
    void runLatencyTests(const dom::Node& myConfig);
    void runSimple();
    void playFileOnMixer(const std::string & theMixerID);
    void testAllChannels();
    void runWithMixer(const std::string& myMixer);
    void testLotsOfSounds();
    void playAtPos(const asl::Point2d & thePos);
    void testSpatialMixer();
    void testThreading();
    void runBase(bool hasDefaultConfig);
    
    const std::string _myDataDir;
};


#endif


