//============================================================================
//
// Copyright (C) 2000-2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef ASL_AUDIO_TIME_SOURCE_INCLUDED
#define ASL_AUDIO_TIME_SOURCE_INCLUDED

#include "TimeSource.h"

#include <asl/Time.h>
#include <asl/ThreadLock.h>
#include <asl/settings.h>

namespace asl {
 
class AudioTimeSource {
    public:
        AudioTimeSource(unsigned myInitialDelay, unsigned mySampleRate);
        virtual ~AudioTimeSource();
        
        virtual Time getCurrentTime();

    protected:
        void stop();
        void pause();
        void run();
        void addFramesToTime(unsigned numFrames);
    
    private:
        asl::ThreadLock _myTimeLock;
        Unsigned64 _mySentFrames;
        asl::Time _mySysTimeAtLastBuffer;
        unsigned _myInitialDelay;
        unsigned _mySampleRate;

        bool _myRunning;
};


}
#endif
