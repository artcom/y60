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

#include <asl/base/Time.h>
#include <asl/base/ThreadLock.h>
#include <asl/base/settings.h>

namespace asl {
 
class AudioTimeSource {
    public:
        AudioTimeSource(unsigned myInitialDelay, unsigned mySampleRate);
        virtual ~AudioTimeSource();
      
        void setCurrentTime(asl::Time theTime);
        virtual Time getCurrentTime();

    protected:
        void stop();
        void pause();
        void run();
        void addFramesToTime(unsigned numFrames);
    
    private:
        mutable asl::ThreadLock _myTimeLock;
        Unsigned64 _mySentFrames;
        asl::Time _mySysTimeAtLastBuffer;
        unsigned _myInitialDelay;
        unsigned _mySampleRate;
    
        asl::Time _myGlobalTimeOffset;
        
        bool _myRunning;
};


}
#endif
