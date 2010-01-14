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

#ifndef INCL_SAMPLESOURCE
#define INCL_SAMPLESOURCE

#include "asl_audio_settings.h"

#include "AudioBufferBase.h"
#include "SampleFormat.h"
#include "AudioBufferBase.h"

#include <asl/base/Ptr.h>
#include <asl/base/settings.h>

#include <string>

namespace asl {

    class ASL_AUDIO_DECL SampleSource;

    // Use this pointer as smart pointer to SampleSource. If you use the default Ptr,
    // you'll get thread-specific free lists. Since lots of pointers are allocated
    // in one thread and deleted in another, that will cause memory leaks. The
    // PtrHeapAllocator used here is slower but works.
    typedef Ptr<SampleSource, MultiProcessor, PtrHeapAllocator<MultiProcessor> > SampleSourcePtr;
    typedef WeakPtr<SampleSource, MultiProcessor, PtrHeapAllocator<MultiProcessor> > SampleSourceWeakPtr;


    class SampleSource
    {
    public:

        SampleSource(const std::string & myName, SampleFormat mySampleFormat,
                     unsigned mySampleRate, unsigned numChannels);
        virtual ~SampleSource();
        std::string getName() const;
        unsigned getNumChannels() const;
        unsigned getSampleRate() const;
        unsigned getBytesPerSecond() const;
        SampleFormat getSampleFormat() const;
        unsigned getBytesPerFrame() const;
        virtual void deliverData(AudioBufferBase& theBuffer) = 0;

        void enable(void);
        void disable(void);
        bool isEnabled();

        void lock();
        void unlock();

    protected:

        std::string      _myName;
        unsigned         _mySampleRate;
        SampleFormat     _mySampleFormat;
        unsigned         _numChannels;
        bool             _myEnableState;

        mutable asl::ThreadLock _mySampleSourceLock;
    };
}

#endif // INCL_SAMPLESOURCE
