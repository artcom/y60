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

#ifndef ASL_TIME_SOURCE_INCLUDED
#define ASL_TIME_SOURCE_INCLUDED

#include <asl/base/Time.h>
#include <asl/base/ThreadLock.h>
#include <asl/base/settings.h>

namespace asl {
 
class TimeSource {
    public:
        virtual Time getCurrentTime() = 0;
};


}
#endif
