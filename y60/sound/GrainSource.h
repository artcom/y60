/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef INCL_GRAINSOURCE
#define INCL_GRAINSOURCE

#include "y60_sound_settings.h"

#include <asl/audio/ISampleSink.h>
#include <asl/audio/SampleSource.h>
#include <asl/audio/AudioBuffer.h>
#include <asl/audio/VolumeFader.h>
#include <asl/base/ThreadLock.h>
#include <asl/base/settings.h>

#include "Resampler.h"
#include "WindowFunction.h"

#include <vector>

namespace y60 {

    class GrainSource;

    typedef asl::Ptr<GrainSource, asl::MultiProcessor, asl::PtrHeapAllocator<asl::MultiProcessor> > GrainSourcePtr;
    typedef asl::WeakPtr<GrainSource, asl::MultiProcessor, asl::PtrHeapAllocator<asl::MultiProcessor> > GrainSourceWeakPtr;

    class Y60_SOUND_DECL GrainSource :
        public asl::SampleSource, public asl::ISampleSink
    {

    public:

        GrainSource(const std::string& theName,
                    asl::SampleFormat theSampleFormat,
                    unsigned theSampleRate,
                    unsigned theNumChannels,
                    unsigned theGrainSize=50,
                    unsigned theGrainRate=25);
        virtual ~GrainSource();

        // inherited from SampleSource
        virtual void deliverData(asl::AudioBufferBase& theBuffer);

        // inherited from ISampleSink
        virtual bool queueSamples(asl::AudioBufferPtr& theBuffer); // use it to fill the internal audio data buffer

        void clearAudioData();

        void setAudioData(const asl::AudioBufferPtr& theAudioData);

        void setGrainSize(unsigned theSize);
        unsigned getGrainSize() const;

        void setGrainSizeJitter(unsigned theJitter);
        unsigned getGrainSizeJitter() const;

        void setGrainRate(unsigned theRate);
        unsigned getGrainRate() const;

        void setGrainRateJitter(unsigned theJitter);
        unsigned getGrainRateJitter() const;

        void setGrainPosition(float thePosition);
        float getGrainPosition() const;

        void setGrainPositionJitter(float theJitter);
        float getGrainPositionJitter() const;

        void setTransposition(float theTransposition);
        float getTransposition() const;

        void setRatio(float theRatio);
        float getRatio() const;

        void setRatioJitter(float theJitter);
        float getRatioJitter() const;

        void setVolume(float theVolume);
        void fadeToVolume(float theVolume, float theTime);
        float getVolume() const;

    private:

        asl::ThreadLock _myLock;

        asl::AudioBufferPtr _myAudioData;        // the buffer containing the audio data to granuralize

        unsigned _myGrainSize;              // length of a grain (in ms)
        unsigned _myGrainSizeJitter;
        unsigned _myGrainRate;              // time interval when the next grain should be played (in ms)
        unsigned _myGrainRateJitter;
        float    _myGrainPosition;          // relative position in the audiobuffer where the grain is taken from
        float    _myGrainPositionJitter;
        float    _myRatio;                  // transposition ratio
        float    _myRatioJitter;
        // XXXX panning stuff

        asl::Unsigned64 _myAbsoluteFrames;

        // some effect(-functors) to operate on the audio buffers
        asl::VolumeFaderPtr    _myVolumeFader;
        y60::ResamplerPtr      _myResampler;
        y60::WindowFunctionPtr _myWindowFunction;

        // some helper members
        asl::AudioBufferPtr _myOverlapBuffer;    // a little space to save overlapping samples between two deliverData calls
        unsigned _myAudioDataFrames;             // the size of the contained audio data
        unsigned _myGrainOffset;                 // offset of the first grain in our frame buffer
        unsigned _myLastBuffersize;              // because subsequent framebuffers have different sizes, we need to store the last buffer size

        void createOverlapBuffer(unsigned theGrainSize);
        unsigned jitterValue(unsigned theValue, unsigned theJitter) const;
        float jitterValue(float theValue, float theJitter) const;

    };
}


#endif // INCL_GRAINSOURCE
