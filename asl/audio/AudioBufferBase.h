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

#ifndef INCL_AUDIOBUFFERBASE
#define INCL_AUDIOBUFFERBASE

#include "SampleFormat.h"

#include <asl/Ptr.h>
#include <asl/Logger.h>
#include <asl/ThreadLock.h>

#include <iostream>

using namespace std;

namespace asl {

class AudioBufferBase {
    public:
        AudioBufferBase();
        virtual ~AudioBufferBase();
        virtual void init(unsigned numFrames, unsigned numChannels, unsigned mySampleRate) = 0;
        virtual void resize(unsigned numFrames) = 0;
        virtual void clear() = 0;
        virtual AudioBufferBase* clone() const = 0;

        virtual unsigned getNumFrames() const = 0;
        virtual unsigned getNumChannels() const = 0;
        virtual SampleFormat getSampleFormat() const = 0;
        virtual unsigned getSampleRate() const = 0;
        virtual unsigned getBytesPerFrame() const = 0;
        virtual unsigned getSizeInBytes() const = 0;

        virtual AudioBufferBase& operator +=(const AudioBufferBase& theOtherBuffer) = 0;
        virtual void copyFrames(unsigned theDestStartFrame, 
                const AudioBufferBase& theSrcBuffer,
                unsigned theSrcStartFrame, unsigned numFrames) = 0;
        virtual void convert(void * theReadPtr, SampleFormat theSrcSampleFormat, 
                unsigned theSrcSampleRate, unsigned theSrcNumChannels) = 0;
        virtual void copyToRawMem(void * theWritePtr, unsigned theStartByte,
                unsigned theLength) = 0;

        virtual std::ostream & print(std::ostream & s) const = 0;
        virtual void dumpSamples (std::ostream & s, int startSample, int endSample) const = 0;

        virtual bool operator ==(const AudioBufferBase& theBuffer) const = 0;
        virtual bool almostEqual(const AudioBufferBase& theBuffer, double theEpsilon) const = 0;
        virtual float getSampleAsFloat(unsigned theFrame, unsigned theChannel) const = 0;

        static int getNumBuffersAllocated();
        virtual void setMarker(float theValue) = 0;
        virtual bool hasClicks(float theFirstSample) = 0;

    private:
        static asl::ThreadLock _myBuffersAllocatedLock;
        static int _myNumBuffersAllocated;
};

std::ostream & operator<<(std::ostream & s, const AudioBufferBase& theBuffer);

typedef Ptr<AudioBufferBase, MultiProcessor, PtrHeapAllocator<MultiProcessor> > AudioBufferPtr;

}
#endif

