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


#include "GrainSource.h"
#include <asl/AudioBuffer.h>
#include <asl/VolumeFader.h>

using namespace asl;
using namespace y60;

GrainSource::GrainSource(const std::string& theName, SampleFormat theSampleFormat, unsigned theSampleRate, unsigned theNumChannels) :
    SampleSource(theName, theSampleFormat, theSampleRate, theNumChannels),
    _myGrainSize(50),
    _myGrainSizeJitter(0),
    _myGrainRate(20),
    _myGrainRateJitter(0),
    _myGrainPosition(0),
    _myGrainPositionJitter(0),
    _myTransposition(0),
    _myTranspositionJitter(0),
    _myGrainOffset(0),
    _myLastBuffersize(0)
{
    _myAudioData = AudioBufferPtr(createAudioBuffer(_mySampleFormat, 0u, _numChannels, _mySampleRate));
    _myAudioData->clear();
    _myVolumeFader = VolumeFaderPtr(new VolumeFader(_mySampleFormat));
    _myVolumeFader->setVolume(1, 0); 
    createOverlapBuffer(_myGrainSize);
    createWindowBuffer(1024);
}


GrainSource::~GrainSource() {
    // ...
}

//
// inherited from SampleSource
//
void GrainSource::deliverData(AudioBufferBase& theBuffer) {

    //AC_INFO << "\n";
    //AC_INFO << "GrainSource::deliverData();";
    ASSURE(getNumChannels() == theBuffer.getNumChannels());
    ASSURE(_mySampleRate == theBuffer.getSampleRate());
    theBuffer.clear();

    //AC_INFO << "OldOffset: " << _myGrainOffset << " , Last Buffersize: " << _myLastBuffersize;
    unsigned myNumFrames = theBuffer.getNumFrames();
    float mySamplesPerMS = _mySampleRate * 0.001f;
    _myGrainOffset -= _myLastBuffersize;
    _myLastBuffersize = myNumFrames;
    
    /////////////////////////////////////////////////////////////////////////////
    // copy overlap contents to frame buffer
    theBuffer.copyFrames(0u, *_myOverlapBuffer, 0u, myNumFrames);
    // save remaining sample data to tmp buffer
    AudioBufferPtr myTmpBuffer = AudioBufferPtr(0);
    if (myNumFrames < _myOverlapBuffer->getNumFrames()) {
        myTmpBuffer = AudioBufferPtr(_myOverlapBuffer->partialClone(myNumFrames, _myOverlapBuffer->getNumFrames()-1));
    }
    _myOverlapBuffer->clear();
    // copy tmp to beginning of overlap buffer
    if (myTmpBuffer) {
        _myOverlapBuffer->copyFrames(0u, *myTmpBuffer, 0u, myTmpBuffer->getNumFrames());
        //AC_INFO << "Temp       " << *myTmpBuffer;
        //AC_INFO << "Overlap    " << *_myOverlapBuffer;
        //AC_INFO << "Output     " << theBuffer;
    }

    /////////////////////////////////////////////////////////////////////////////
    // repeat for all grains until end of frame buffer is reached
    //AC_INFO << "Offset: " << _myGrainOffset << " , Buffersize: " << myNumFrames;
    while (_myGrainOffset < myNumFrames) {

        unsigned myGrainFrames =(unsigned)( (float)(jitterValue(_myGrainSize, _myGrainSizeJitter)) * mySamplesPerMS);
        unsigned myRateFrames = (unsigned)( (float)(jitterValue(_myGrainRate, _myGrainRateJitter)) * mySamplesPerMS);
        unsigned myPositionFrame = (unsigned)(jitterValue(_myGrainPosition,_myGrainPositionJitter) * 
                                              _myAudioData->getNumFrames());
        ASSURE(myPositionFrame>=0);
        if (myPositionFrame>_myAudioData->getNumFrames()-myGrainFrames) {
            myPositionFrame = _myAudioData->getNumFrames()-myGrainFrames;
        }
    
        // get grain from audio data
        AudioBufferPtr myGrain = AudioBufferPtr(_myAudioData->partialClone(myPositionFrame, myPositionFrame + myGrainFrames));
        //AC_INFO << "Grain: " << *myGrain;
        // apply hann window
        myGrain->applyWindow(_myWindowBuffer);

        unsigned myRemainingFrames = myNumFrames - _myGrainOffset;
        unsigned myToBufferFrames = (myGrainFrames > myRemainingFrames) ? myRemainingFrames : myGrainFrames;
        unsigned myToOverlapFrames = myGrainFrames - myToBufferFrames;

        //AC_INFO << "Remaining: " << myRemainingFrames << " , To Buffer: " << myToBufferFrames << " , To Olap: " << myToOverlapFrames;
        // add contents to the frame buffer
        theBuffer.partialAdd(_myGrainOffset, *myGrain, 0u, myToBufferFrames);
        // add rest (if any) to the overlap buffer
        _myOverlapBuffer->partialAdd(0u, *myGrain, myToBufferFrames, myToOverlapFrames);
        _myGrainOffset += myRateFrames; // proceed to next grain

        //AC_INFO << "Overlap: " << *_myOverlapBuffer;
        
    }

    _myVolumeFader->apply(theBuffer, 0);
    // XXXXXX resize overlap buffer
    //AC_INFO << "OutBuffer: " << theBuffer;

}


//
// inherited from ISampleSink
//
bool GrainSource::queueSamples(AudioBufferPtr& theBuffer) {
    unsigned myNumFrames = _myAudioData->getNumFrames();
    unsigned mySrcFrames = theBuffer->getNumFrames();
    _myAudioData->resize(myNumFrames + mySrcFrames);
    _myAudioData->copyFrames(myNumFrames, *theBuffer, 0u, mySrcFrames);
    //AC_INFO << "Resized _myAudioData to : " << _myAudioData->getNumFrames() << " Frames";
    return true;
}


void GrainSource::clearAudioData() {
    _myAudioData->clear();
}


inline void GrainSource::setAudioData(const AudioBufferPtr& theAudioData) {
    _myAudioData = theAudioData;
}


void GrainSource::createOverlapBuffer(unsigned theGrainSize) {
    _myOverlapBuffer = AudioBufferPtr(createAudioBuffer(_mySampleFormat, (unsigned)(theGrainSize*_mySampleRate*0.001), _numChannels, _mySampleRate));
    _myOverlapBuffer->clear();
}


void GrainSource::createWindowBuffer(unsigned theWindowSize) {
    //    _myWindowBuffer.reserve(theWindowSize*2);
    _myWindowBuffer.reserve(theWindowSize);
    for (unsigned i = 0; i < theWindowSize; i++) {
        float value = 0.5 * (1.0 -cos(2*3.14159265f*i/(theWindowSize-1))); // Hann Window
        //float value = 1.0f;
        //float value = 0.5f * 2.0f/theWindowSize * (theWindowSize*0.5f - std::abs((float)i - (theWindowSize - 1)*0.5f));
        AC_INFO << "value: " << value;
        _myWindowBuffer.push_back(value); 
        //_myWindowBuffer
    }
//     std::vector<float>::iterator myPos = _myWindowBuffer.begin() + theWindowSize/2;
//     _myWindowBuffer.insert(myPos, theWindowSize, 1.0f);
}


void GrainSource::setGrainSize(unsigned theSize) {
    _myGrainSize = theSize;
    createOverlapBuffer(theSize);
}


unsigned GrainSource::getGrainSize() const {
    return _myGrainSize;
}


void GrainSource::setGrainSizeJitter(unsigned theJitter) {
    _myGrainSizeJitter = theJitter;
}


unsigned GrainSource::getGrainSizeJitter() const {
    return _myGrainSizeJitter;
}


void GrainSource::setGrainRate(unsigned theRate) {
    _myGrainRate = theRate;
}


unsigned GrainSource::getGrainRate() const {
    return _myGrainRate;
}


void GrainSource::setGrainRateJitter(unsigned theJitter) {
    _myGrainRateJitter = theJitter;
}


unsigned GrainSource::getGrainRateJitter() const {
    return _myGrainRateJitter;
}


void GrainSource::setGrainPosition(float thePosition) {
    _myGrainPosition = thePosition;
}


float GrainSource::getGrainPosition() const {
    return _myGrainPosition;
}


void GrainSource::setGrainPositionJitter(float theJitter) {
    _myGrainPositionJitter = theJitter;
}


float GrainSource::getGrainPositionJitter() const {
    return _myGrainPositionJitter;
}


// void GrainSource::setTransposition(unsigned theTransposition) {
//     _myTransposition = theTransposition;
// }


// unsigned GrainSource::getTransposition() const {
//     return _myTransposition;
// }


// void GrainSource::setTranspositionJitter(unsigned theJitter) {
//     _myTranspositionJitter = theJitter;
// }


// unsigned GrainSource::getTranspositionJitter() const {
//     return _myTranspositionJitter;
// }

void GrainSource::setVolume(float theVolume) {
    _myVolumeFader->setVolume(theVolume,0);
}

float GrainSource::getVolume() const {
    return _myVolumeFader->getVolume();
}


inline unsigned GrainSource::jitterValue(unsigned theValue, unsigned theJitter) const {
    AC_DEBUG << "jitterValue(<" << theValue << ">,<" << theJitter << ">)";
    if (theJitter > 0) {
        theValue = theValue + (2 * (std::rand() % theJitter) - theJitter);
        if (theValue < 0) theValue = 0;
    }
    AC_DEBUG << "result: " << theValue;
    return theValue;
}

inline float GrainSource::jitterValue(float theValue, float theJitter) const {
    AC_DEBUG << "jitterValue(<" << theValue << ">,<" << theJitter << ">)";
    float myRandomFloat = (float)(std::rand() % 1000) / 1000.f; 
    float myResult = theValue + (2 * myRandomFloat * theJitter - theJitter);
    AC_DEBUG << "randomFloat " << myRandomFloat;
    AC_DEBUG << "result: " << myResult;
    return myResult > 0 ? myResult : 0;
}





