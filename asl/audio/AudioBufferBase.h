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

#ifndef INCL_AUDIOBUFFERBASE
#define INCL_AUDIOBUFFERBASE

#include "asl_audio_settings.h"

#include "SampleFormat.h"

#include <asl/base/Ptr.h>
#include <asl/base/Logger.h>
#include <asl/base/ThreadLock.h>

#include <iostream>

//using namespace std;

namespace asl {

class ASL_AUDIO_DECL AudioBufferBase {
    public:
        AudioBufferBase();
        virtual ~AudioBufferBase();
        virtual void init(unsigned numFrames, unsigned numChannels, unsigned mySampleRate) = 0;
        virtual void resize(unsigned numFrames) = 0;
        virtual void clear() = 0;
        virtual AudioBufferBase* clone() const = 0;
        virtual AudioBufferBase* partialClone(unsigned startFrame, unsigned endFrame)
                const = 0;

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
                unsigned theSrcNumChannels) = 0;
        virtual void copyToRawMem(void * theWritePtr, unsigned theStartByte,
                unsigned theLength) = 0;

        virtual void partialAdd(unsigned theDestStartFrame,
                                const AudioBufferBase& theSrcBuffer,
                                unsigned theSrcStartFrame, unsigned numFrames) = 0;

        virtual std::ostream & print(std::ostream & s) const = 0;
        virtual void dumpSamples (std::ostream & s, int startSample, int endSample) const = 0;

        virtual bool operator ==(const AudioBufferBase& theBuffer) const = 0;
        virtual bool almostEqual(const AudioBufferBase& theBuffer, double theEpsilon) const = 0;
        virtual float getSampleAsFloat(unsigned theFrame, unsigned theChannel) const = 0;

        virtual void setMarker(float theValue) = 0;
        virtual bool hasClicks(float theFirstSample) = 0;

        // For timed buffers.
        void setStartFrame(int myStartFrame);
        int getStartFrame() const;
        int getEndFrame() const;

        static int getNumBuffersAllocated();

    private:
        static asl::ThreadLock _myBuffersAllocatedLock;
        static int _myNumBuffersAllocated;
        int _myStartFrame;
};

std::ostream & operator<<(std::ostream & s, const AudioBufferBase& theBuffer);

typedef Ptr<AudioBufferBase, MultiProcessor, PtrHeapAllocator<MultiProcessor> > AudioBufferPtr;

}
#endif

