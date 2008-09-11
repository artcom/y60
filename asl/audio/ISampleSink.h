//=============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

#ifndef INCL_ISAMPLESINK
#define INCL_ISAMPLESINK

#include "AudioBufferBase.h"

#include <asl/base/Ptr.h>

namespace asl {

class ISampleSink
{
    public:
        virtual ~ISampleSink() {};
        virtual bool queueSamples(AudioBufferPtr& theBuffer) = 0; // Return false on error.
};

typedef Ptr<ISampleSink, MultiProcessor, PtrHeapAllocator<MultiProcessor> > ISampleSinkPtr;

}

#endif 
