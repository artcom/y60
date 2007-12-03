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

#ifndef ASL_EXTERNAL_TIME_SOURCE_INCLUDED
#define ASL_EXTERNAL_TIME_SOURCE_INCLUDED

#include "TimeSource.h"
#include "AudioTimeSource.h"

#include <asl/Auto.h>
#include <asl/Time.h>
#include <asl/Ptr.h>
#include <asl/settings.h>

namespace asl {
    
    class ExternalTimeSource {
        public:
            ExternalTimeSource();
            virtual ~ExternalTimeSource();
          
            Time getCurrentTime() const;    
            void stop();
            void pause();
            void run();    
        private:
            asl::Time _myStartTime;        
            asl::Time _myTimeOffset;
            asl::Time _myPausedTime;
            bool _myPauseFlag;
    };
    typedef Ptr<ExternalTimeSource, MultiProcessor, PtrHeapAllocator<MultiProcessor> > ExternalTimeSourcePtr;


}
#endif
