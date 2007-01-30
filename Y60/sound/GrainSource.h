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

#ifndef INCL_GRAINSOURCE
#define INCL_GRAINSOURCE

#include <asl/ISampleSink.h>
#include <asl/SampleSource.h>
#include <asl/AudioBuffer.h>
#include <asl/VolumeFader.h>
#include <asl/ThreadLock.h>
#include <asl/settings.h>

#include "Resampler.h"
#include "WindowFunction.h"

#include <vector>

namespace y60 {

    class GrainSource;

    typedef asl::Ptr<GrainSource, asl::MultiProcessor, asl::PtrHeapAllocator<asl::MultiProcessor> > GrainSourcePtr;
    typedef asl::WeakPtr<GrainSource, asl::MultiProcessor, asl::PtrHeapAllocator<asl::MultiProcessor> > GrainSourceWeakPtr;
    
    class GrainSource : public asl::SampleSource, public asl::ISampleSink {

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
