//=============================================================================
//
// Copyright (C) 1993-2006, ART+COM AG
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

#ifndef INCL_SAMPLESOURCE
#define INCL_SAMPLESOURCE

#include "AudioBufferBase.h"
#include "SampleFormat.h"
#include "AudioBufferBase.h"

#include <asl/Ptr.h>
#include <asl/settings.h>

#include <string>

namespace asl {

    class SampleSource;

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
