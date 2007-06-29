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

#ifndef INCL_AUDIOBUFFER
#define INCL_AUDIOBUFFER

#include "AudioBufferBase.h"
#include "SampleFormat.h"

//#define ASSURE_POLICY AssurePolicy::Exit
#include <asl/Block.h>
#include <asl/Logger.h>
#include <asl/Assure.h>
#include <asl/numeric_functions.h>
#include <asl/settings.h>


#include <iostream>
#include <limits.h>
#include <vector>

namespace asl {

template <class SAMPLE>
class AudioBuffer: public AudioBufferBase {
    public:
        static float sampleToFloat(const SAMPLE s);
        static SAMPLE floatToSample(float f);

        AudioBuffer() {
//            AC_TRACE << "AudioBuffer::AudioBuffer(" << this << ")";
            _numFrames = 0;
            _numChannels = 0;
            _mySampleRate = 0;
        }

        AudioBuffer(unsigned numFrames, unsigned numChannels, unsigned mySampleRate) 
        {
//            AC_TRACE << "AudioBuffer::AudioBuffer(" << this << ")";
            init (numFrames, numChannels, mySampleRate);
        }

        virtual ~AudioBuffer() {
//            AC_TRACE << "AudioBuffer::~AudioBuffer(" << this << ")";
        }

        void init(unsigned numFrames, unsigned numChannels, unsigned mySampleRate) 
        {
            if (numFrames == 0) {
                AC_DEBUG << "AudioBuffer::init, numFrames=0";
            }
            _numFrames = numFrames;
            _numChannels = numChannels;
            _mySampleRate = mySampleRate;
            _myBlock.resize(getSizeInBytes());
        }

        void resize(unsigned numFrames) {
            if (numFrames == 0) {
                AC_WARNING << "AudioBuffer::resize, numFrames=0";
            }
            _numFrames = numFrames;
            ASSURE_MSG(_numChannels != 0,
                    "AudioBuffer::resize called but _numChannels=0 (forgot init?)");
            _myBlock.resize(getSizeInBytes());
        }

        void clear() {
            ASSURE_MSG(_numChannels != 0,
                    "AudioBuffer::clear called but _numChannels=0 (forgot init?)");
            memset(begin(), 0, getSizeInBytes());
        }

        AudioBufferBase* clone() const {
            AudioBuffer<SAMPLE>* newBuffer;
            newBuffer = new AudioBuffer<SAMPLE>(_numFrames, _numChannels, _mySampleRate);
            newBuffer->_myBlock = _myBlock;
            return newBuffer;
        }

        AudioBufferBase* partialClone(unsigned startFrame, unsigned endFrame) 
                const 
        {
            AudioBuffer<SAMPLE>* newBuffer;
            unsigned numFrames = endFrame-startFrame;
            newBuffer = new AudioBuffer<SAMPLE>(numFrames, _numChannels, _mySampleRate);
            SAMPLE * _mySrcStartFrame = ((SAMPLE*)_myBlock.begin())+(startFrame*_numChannels);
            memcpy(newBuffer->begin(), _mySrcStartFrame, numFrames*getBytesPerFrame());
            return newBuffer;

        }
        
        const SAMPLE * begin() const {
            return (const SAMPLE*)_myBlock.begin();
        }
        const SAMPLE * end() const {
            return (const SAMPLE*)_myBlock.end();
        }
        SAMPLE * begin() {
            return (SAMPLE*)_myBlock.begin();
        }
        SAMPLE * end() {
            return (SAMPLE*)_myBlock.end();
        }

        unsigned getNumFrames() const {
            ASSURE_MSG(_numChannels != 0,
                    "AudioBuffer::getNumFrames called but _numChannels=0 (forgot init?)");
            return _numFrames;
        }

        unsigned getNumChannels() const {
            ASSURE_MSG(_numChannels != 0,
                    "AudioBuffer::getNumChannels called but _numChannels=0 (forgot init?)");
            return _numChannels;
        }

        unsigned getSampleRate() const {
            ASSURE_MSG(_numChannels != 0,
                    "AudioBuffer::getSampleRate called but _numChannels=0 (forgot init?)");
            return _mySampleRate;
        }

        SampleFormat getSampleFormat() const;

        unsigned getBytesPerFrame() const {
            ASSURE_MSG(_numChannels != 0,
                    "AudioBuffer::getBytesPerFrame called but _numChannels=0 (forgot init?)");
            return _numChannels*sizeof(SAMPLE);
        }

        unsigned getSizeInBytes() const {
            ASSURE_MSG(_numChannels != 0,
                    "AudioBuffer::getSizeInBytes called but _numChannels=0 (forgot init?)");
            return _numFrames*_numChannels*sizeof(SAMPLE);
        }

        AudioBufferBase& operator+=(const AudioBufferBase& theOtherBuffer) {
            ASSURE_MSG(_numChannels != 0,
                    "AudioBuffer::operator += called but _numChannels=0 (forgot init?)");
            const AudioBuffer<SAMPLE>* myOtherBuffer = getBufferFromInterface(theOtherBuffer);
            ASSURE(myOtherBuffer->getNumChannels() == getNumChannels());
            SAMPLE * curOutSample = begin();
            const SAMPLE * curInSample = myOtherBuffer->begin();
            while (curOutSample != end()) {
                *curOutSample += *curInSample;
                curInSample++;
                curOutSample++;
            }
            return *this;
        }

        void partialAdd(unsigned theDestStartFrame,
                        const AudioBufferBase& theSrcBuffer,
                        unsigned theSrcStartFrame, unsigned numFrames) 
        {
            const AudioBuffer<SAMPLE>* mySrcBuffer = getBufferFromInterface(theSrcBuffer);
            unsigned theSrcNumFrames = theSrcBuffer.getNumFrames();
            unsigned theDestNumFrames = getNumFrames();
//             AC_PRINT << "AudioBuffer::partialAdd:\n"
//                      << "theDestStartFrame=" << theDestStartFrame << ", "
//                      << "theSrcStartFrame=" << theSrcStartFrame << ", "
//                      << "numFrames=" << numFrames << ", "
//                      << "theSrcNumFrames=" << theSrcNumFrames << ", "
//                      << "theDestNumFrames=" << theDestNumFrames;
            ASSURE(theSrcBuffer.getNumChannels() == getNumChannels());
            ASSURE(theSrcNumFrames-theSrcStartFrame >= numFrames);
            ASSURE(theDestStartFrame + numFrames <= theDestNumFrames);
            
            SAMPLE * curOutSample = begin() + theDestStartFrame*getNumChannels();
            const SAMPLE * curInSample = mySrcBuffer->begin() + theSrcStartFrame*getNumChannels();
            unsigned numSamples = numFrames * getNumChannels();
            while (numSamples--) {
                (*curOutSample++) += (*curInSample++);
            }
        }

        void copyFrames(unsigned theDestStartFrame, 
			const AudioBufferBase& theSrcBuffer,
                        unsigned theSrcStartFrame, 
			unsigned numFrames)
        {
            ASSURE_MSG(_numChannels != 0,
                       "AudioBuffer::copyFrames called but _numChannels=0 "
		       "(forgot init?)");

            const AudioBuffer<SAMPLE>* mySrcBuffer = 	
	        getBufferFromInterface(theSrcBuffer);

	    ASSURE(getNumFrames()-theDestStartFrame >= numFrames);
	    ASSURE(mySrcBuffer->getNumFrames()-theSrcStartFrame >= numFrames);

            unsigned myDestStartSample = theDestStartFrame*getNumChannels();
            unsigned mySrcStartSample = 
		theSrcStartFrame*mySrcBuffer->getNumChannels();
            unsigned numBytes = numFrames*getBytesPerFrame();
            
	    SAMPLE * curOutSample = begin()+myDestStartSample;
            const SAMPLE * curInSample = mySrcBuffer->begin()+mySrcStartSample;
            ASSURE (mySrcBuffer->getNumChannels() == getNumChannels());
            
	    memcpy(begin()+myDestStartSample, 
	           mySrcBuffer->begin()+mySrcStartSample, numBytes);
        }

        void convert(void * theReadPtr, SampleFormat theSrcSampleFormat, 
                unsigned theSrcNumChannels)
        {
            if (theSrcSampleFormat == getSampleFormat() && 
                    theSrcNumChannels == getNumChannels()) 
            {
                memcpy(((char*)begin()), theReadPtr, getNumFrames()*getBytesPerFrame());
            } else {
                ASSURE(getNumChannels() == 2);
                ASSURE(theSrcNumChannels <= 2);
                if (getSampleFormat() == SF_F32 && theSrcSampleFormat == SF_S16)
                {
                    // Conversion from S16 to float.
                    for (unsigned i = 0; i<getNumFrames(); ++i) {
                        if (theSrcNumChannels == 1) {
                            begin()[i*2] = SAMPLE(*((short*)theReadPtr+i)/float(SHRT_MAX));
                            begin()[i*2+1] = begin()[i*2];
                        } else {
                            begin()[i*2] = SAMPLE(*((short*)theReadPtr+i*2)/
                                    float(SHRT_MAX));
                            begin()[i*2+1] = SAMPLE(*((short*)theReadPtr+i*2+1)/
                                    float(SHRT_MAX));
                            
                        }
                    }
                } else {
                    // Conversion mono to stereo.
                    for (unsigned i = 0; i<getNumFrames(); ++i) {
                        begin()[i*2] = *((SAMPLE*)theReadPtr+i);
                        begin()[i*2+1] = *((SAMPLE*)theReadPtr+i);
                    }
                }
            }
        }
            
        // TODO: Change interface to use frames, not bytes.
        void copyToRawMem(void * theWritePtr, unsigned theStartByte,
                          unsigned theLength)
        {
            memcpy(theWritePtr, ((char*)begin())+theStartByte, theLength);
        }

        std::ostream & print (std::ostream & s) const {
            s << "(num frames: " << getNumFrames()
                << ", num channels: " << getNumChannels()
                << ", sample rate: " << getSampleRate()
                << ", sample format:" << getSampleFormat() << ")";
            if (_numChannels != 0) {
                dumpSamples(s, 0, 8);
                unsigned numSamples = getNumFrames()*getNumChannels();
                dumpSamples(s, numSamples-8, numSamples);
            }
            s << std::endl;
            return s;
        }

        void dumpSamples (std::ostream & s, int startSample, int endSample) const {
            int i;
            for (i=startSample; i<endSample; i++) {
                if ((i-startSample)%8 == 0) {
                    s << std::endl << "  " << std::setw(4) << i << ": ";
                }
                s << std::setw(6) << *(begin()+i) << " ";
            }
        }

        virtual bool operator ==(const AudioBufferBase& otherBuffer) const {
            const AudioBuffer<SAMPLE>* myOtherBuffer = 
                    dynamic_cast<const AudioBuffer<SAMPLE>*>(&otherBuffer);
            if (!myOtherBuffer) {
                AC_WARNING << "Comparing buffers of different types.";
                return false;
            } else {
                return (_myBlock == myOtherBuffer->_myBlock &&
                        _numFrames == myOtherBuffer->_numFrames &&
                        _numChannels == myOtherBuffer->_numChannels &&
                        _mySampleRate == myOtherBuffer->_mySampleRate);
            }
        }

        virtual bool almostEqual(const AudioBufferBase& otherBuffer, double theEpsilon) const {
            const AudioBuffer<SAMPLE>* myOtherBuffer = 
                    dynamic_cast<const AudioBuffer<SAMPLE>*>(&otherBuffer);
            if (!myOtherBuffer) {
                AC_WARNING << "Comparing buffers of different types.";
                return false;
            } else {
                if (_numFrames != myOtherBuffer->_numFrames ||
                    _numChannels != myOtherBuffer->_numChannels ||
                    _mySampleRate != myOtherBuffer->_mySampleRate)
                {
                    return false;
                } else {
                    const SAMPLE * otherSample = myOtherBuffer->begin();
                    for (const SAMPLE * mySample = begin(); mySample != end(); ++mySample) {
                        if (!asl::almostEqual(sampleToFloat(*mySample), 
                                    sampleToFloat(*otherSample), theEpsilon)) 
                        {
                            return false;
                        }
                        otherSample++;
                    }
                    return true;
                }
            }            
        }

        virtual float getSampleAsFloat(unsigned theFrame, unsigned theChannel) const {
            return sampleToFloat(*(begin()+theFrame*_numChannels+theChannel));
        }

        // Adds a marker into the buffer that can be seen in a waveform editor :-). 
        void setMarker(float theValue) {
            SAMPLE * myPtr = begin();
            int numSamples = 20;
            if (_numFrames*_numChannels < numSamples) {
                numSamples = _numFrames*_numChannels;
            }
            for (int i=0; i<numSamples; i++) {
                *myPtr = floatToSample(theValue);
                ++myPtr;
            }
        }

        virtual bool hasClicks(float theFirstSample) {
            // TODO: This only tests the first channel.
            const float MaxDiff = 0.1f;
            if (fabs(sampleToFloat(*begin())-theFirstSample)>MaxDiff) {
                AC_TRACE << "Click at buffer start: " << sampleToFloat(*begin()) << " -- "
                    << theFirstSample;
                return true;
            }
            for (const SAMPLE * mySample = begin()+_numChannels; mySample != end(); 
                    mySample += _numChannels)
            {
                if (fabs(sampleToFloat(*mySample)-sampleToFloat(*(mySample-_numChannels))) 
                        > MaxDiff) 
                {
                    AC_TRACE << "Click at sample " << mySample-begin() << ": " <<  sampleToFloat(*mySample) << " -- " << 
                            sampleToFloat(*(mySample-_numChannels));
                    return true;
                }
            }
            return false;
        }

    private:
        const AudioBuffer<SAMPLE>* getBufferFromInterface(const AudioBufferBase& theBuffer)
            const
        {
            const AudioBuffer<SAMPLE>* myBuffer =
                    &dynamic_cast<const AudioBuffer<SAMPLE>&>(theBuffer);
            ASSURE_MSG(myBuffer, "Audio buffers of different types used in one operation.");
            ASSURE_MSG(myBuffer->getSampleRate() == getSampleRate(),
                    "Audio buffers that have a different sample rate used in one operation.");
            return myBuffer;
        }

        Block _myBlock;
        unsigned _numFrames;
        unsigned _numChannels;
        unsigned _mySampleRate;
};

AudioBufferBase * createAudioBuffer(SampleFormat mySampleFormat, unsigned numFrames,
        unsigned numChannels, unsigned mySampleRate);

template<>
inline float AudioBuffer<Signed16>::sampleToFloat(Signed16 s) {
    return s/float(SHRT_MAX);
}

template<>
inline float AudioBuffer<float>::sampleToFloat(float s) {
    return s;
}

template<>
inline Signed16 AudioBuffer<Signed16>::floatToSample (float f) {
    return Signed16(f*SHRT_MAX);
}

template<>
inline float AudioBuffer<float>::floatToSample (float f) {
    return f;
}

}
#endif
