/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef ASL_EXTERNAL_TIME_SOURCE_INCLUDED
#define ASL_EXTERNAL_TIME_SOURCE_INCLUDED

#include "asl_audio_settings.h"

#include "TimeSource.h"
#include "AudioTimeSource.h"

#include <asl/base/Auto.h>
#include <asl/base/Time.h>
#include <asl/base/Ptr.h>
#include <asl/base/settings.h>

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
