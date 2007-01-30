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
#include <asl/Auto.h>

using namespace asl;
using namespace y60;

GrainSource::GrainSource(const std::string& theName, 
                         SampleFormat theSampleFormat, 
                         unsigned theSampleRate, 
                         unsigned theNumChannels, 
                         unsigned theGrainSize,
                         unsigned theGrainRate) :
    SampleSource(theName, theSampleFormat, theSampleRate, theNumChannels),
    _myGrainSize(theGrainSize),
    _myGrainSizeJitter(0),
    _myGrainRate(theGrainRate),
    _myGrainRateJitter(0),
    _myGrainPosition(0),
    _myGrainPositionJitter(0),
    _myRatio(1.),
    _myRatioJitter(0.),
    _myGrainOffset(0),
    _myLastBuffersize(0),
    _myAudioDataFrames(0)
{
    _myAudioData = AudioBufferPtr(createAudioBuffer(_mySampleFormat, 0u, _numChannels, _mySampleRate));
    _myAudioData->clear();
    _myVolumeFader = VolumeFaderPtr(new VolumeFader(_mySampleFormat));
    _myVolumeFader->setVolume(1, 0); 
    _myResampler = ResamplerPtr(new Resampler(_mySampleFormat));
    _myWindowFunction = WindowFunctionPtr(new WindowFunction(_mySampleFormat));
    _myWindowFunction->createHannWindow(1024);
    createOverlapBuffer(_myGrainSize);
}


GrainSource::~GrainSource() {
    //    AC_DEBUG << "~GrainSource end (" << _myName << ")";
}

//
// inherited from SampleSource
//
void GrainSource::deliverData(AudioBufferBase& theBuffer) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    //    AC_DEBUG << "GrainSource::deliverData();";
    ASSURE(getNumChannels() == theBuffer.getNumChannels());
    ASSURE(_mySampleRate == theBuffer.getSampleRate());
    ASSURE(_myGrainSize >= 1);
    theBuffer.clear();

    unsigned myNumFrames = theBuffer.getNumFrames();
    float mySamplesPerMS = _mySampleRate * 0.001f;
    _myGrainOffset -= _myLastBuffersize;
    _myLastBuffersize = myNumFrames;
    
    /////////////////////////////////////////////////////////////////////////////
    // copy samples overlapping from the last buffer to current frame buffer
    theBuffer.copyFrames(0u, *_myOverlapBuffer, 0u, myNumFrames);
    // save remaining sample data to tmp buffer
    AudioBufferPtr myTmpBuffer = AudioBufferPtr(0);
    if (myNumFrames < _myOverlapBuffer->getNumFrames()) {
        myTmpBuffer = AudioBufferPtr(_myOverlapBuffer->partialClone(myNumFrames, _myOverlapBuffer->getNumFrames()-1));
    }

    // resize overlap buffer to fit to new grainsize if needed
    unsigned myMaxGrainFrames = (unsigned)((_myGrainSize + _myGrainSizeJitter) * mySamplesPerMS);
    if (myMaxGrainFrames > _myOverlapBuffer->getNumFrames() - myNumFrames) {
        _myOverlapBuffer->resize(myMaxGrainFrames);
    }
    _myOverlapBuffer->clear();

    // copy tmp to beginning of overlap buffer
    if (myTmpBuffer) {
        _myOverlapBuffer->copyFrames(0u, *myTmpBuffer, 0u, myTmpBuffer->getNumFrames());
    }

    /////////////////////////////////////////////////////////////////////////////
    // repeat for all grains until end of frame buffer is reached
    while (_myGrainOffset < myNumFrames) {

        float myJitteredRatio = jitterValue(_myRatio, _myRatioJitter);

        unsigned myGrainFrames =(unsigned)( (float)(jitterValue(_myGrainSize, _myGrainSizeJitter)) * mySamplesPerMS);
        unsigned myRateFrames = (unsigned)( (float)(jitterValue(_myGrainRate, _myGrainRateJitter)) * mySamplesPerMS);
        unsigned myPositionFrame = (unsigned)(jitterValue(_myGrainPosition,_myGrainPositionJitter) * 
                                              _myAudioDataFrames);
        ASSURE(myPositionFrame>=0);
        if (myPositionFrame>_myAudioDataFrames-myGrainFrames) {
            myPositionFrame = _myAudioDataFrames-myGrainFrames;
        }
 
        // get grain from audio data
        //        AC_TRACE << "GrainSource::deliverData: creating grain audiobuffer";
        AudioBufferPtr myGrain = AudioBufferPtr(_myAudioData->partialClone(myPositionFrame, myPositionFrame + (unsigned)(ceilf(myGrainFrames * myJitteredRatio))));
        //        AC_TRACE << "GrainSource::deliverData: applying resampler";
        _myResampler->setTargetFrames(myGrainFrames);
        _myResampler->apply(*myGrain,0);
        _myWindowFunction->setOverlapFactor((float)_myGrainRate/(float)_myGrainSize);
        //        AC_TRACE << "GrainSource::deliverData: applying window function";
        _myWindowFunction->apply(*myGrain,0);
        //        AC_TRACE << "GrainSource::deliverData: calculating remaining frames and overlap frames";
        unsigned myRemainingFrames = myNumFrames - _myGrainOffset;
        unsigned myToBufferFrames = (myGrainFrames > myRemainingFrames) ? myRemainingFrames : myGrainFrames;
        unsigned myToOverlapFrames = myGrainFrames - myToBufferFrames;

        // add contents to the frame buffer
        //        AC_TRACE << "GrainSource::deliverData: adding grains";
        theBuffer.partialAdd(_myGrainOffset, *myGrain, 0u, myToBufferFrames);
        // add rest (if any) to the overlap buffer
        //        AC_TRACE << "GrainSource::deliverData: adding rest to overlap buffer";
        _myOverlapBuffer->partialAdd(0u, *myGrain, myToBufferFrames, myToOverlapFrames);
        _myGrainOffset += myRateFrames; // proceed to next grain
    }
    //    AC_TRACE << "GrainSource::deliverData: applying volume fader";
    _myVolumeFader->apply(theBuffer, 0);
}


//
// inherited from ISampleSink
//
bool GrainSource::queueSamples(AudioBufferPtr& theBuffer) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    unsigned myNumFrames = _myAudioData->getNumFrames();
    unsigned mySrcFrames = theBuffer->getNumFrames();
    if (_myAudioDataFrames + mySrcFrames > myNumFrames) { // doesn't fit into the current audio data buffer
        _myAudioData->resize(_myAudioDataFrames + mySrcFrames);
    }
    _myAudioData->copyFrames(_myAudioDataFrames, *theBuffer, 0u, mySrcFrames);
    _myAudioDataFrames += mySrcFrames; 
    return true;
}


void GrainSource::clearAudioData() {
    AutoLocker<ThreadLock> myLocker(_myLock);
    _myAudioData->clear();
    _myAudioDataFrames = 0;
}


inline void GrainSource::setAudioData(const AudioBufferPtr& theAudioData) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    _myAudioData = theAudioData;
}


void GrainSource::createOverlapBuffer(unsigned theGrainSize) {
    _myOverlapBuffer = AudioBufferPtr(createAudioBuffer(_mySampleFormat, (unsigned)(theGrainSize*_mySampleRate*0.001), _numChannels, _mySampleRate));
    _myOverlapBuffer->clear();
}


void GrainSource::setGrainSize(unsigned theSize) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    ASSURE(theSize >= 1);
    _myGrainSize = theSize;
}


unsigned GrainSource::getGrainSize() const {
    return _myGrainSize;
}


void GrainSource::setGrainSizeJitter(unsigned theJitter) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    _myGrainSizeJitter = theJitter;
}


unsigned GrainSource::getGrainSizeJitter() const {
    return _myGrainSizeJitter;
}


void GrainSource::setGrainRate(unsigned theRate) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    ASSURE(theRate >= 1);
    _myGrainRate = theRate;
}


unsigned GrainSource::getGrainRate() const {
    return _myGrainRate;
}


void GrainSource::setGrainRateJitter(unsigned theJitter) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    _myGrainRateJitter = theJitter;
}


unsigned GrainSource::getGrainRateJitter() const {
    return _myGrainRateJitter;
}


void GrainSource::setGrainPosition(float thePosition) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    _myGrainPosition = thePosition;
}


float GrainSource::getGrainPosition() const {
    return _myGrainPosition;
}


void GrainSource::setGrainPositionJitter(float theJitter) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    _myGrainPositionJitter = theJitter;
}


float GrainSource::getGrainPositionJitter() const {
    return _myGrainPositionJitter;
}

float GrainSource::getRatio() const {
    return _myRatio;
}

void GrainSource::setRatio(float theRatio) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    //    AC_DEBUG << "GrainSource::setRatio("<<theRatio<<");";
    ASSURE(_myRatio > 0);
    if (theRatio > 0) {
        _myRatio = theRatio;
    }
}

void GrainSource::setRatioJitter(float theJitter) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    //    AC_DEBUG << "GrainSource::setRatioJitter("<<theJitter<<");";
    ASSURE(theJitter < _myRatio);
    if (theJitter < _myRatio) {
        _myRatioJitter = theJitter;
    }
}


float GrainSource::getRatioJitter() const {
    return _myRatioJitter;
}

void GrainSource::setTransposition(float theTransposition) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    //    AC_DEBUG << "GrainSource::setTransposition("<<theTransposition<<");";
    _myRatio = pow(2.f, theTransposition/12.f);
    ASSURE(_myRatio > 0);
}


float GrainSource::getTransposition() const {
    float myTranspose = 12.f * log(_myRatio)/log(2.f);
    //    AC_DEBUG << myTranspose << " = GrainSource::getTransposition()";
    return myTranspose;
}

void GrainSource::setVolume(float theVolume) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    _myVolumeFader->setVolume(theVolume,0);
}

float GrainSource::getVolume() const {
    return _myVolumeFader->getVolume();
}


inline unsigned GrainSource::jitterValue(unsigned theValue, unsigned theJitter) const {
    //    AC_DEBUG << "jitterValue(<" << theValue << ">,<" << theJitter << ">)";
    if (theJitter > 0) {
        theValue = theValue + (2 * (std::rand() % theJitter) - theJitter);
        if (theValue < 0) theValue = 1;
    }
    //    AC_DEBUG << "result: " << theValue;
    return theValue;
}

inline float GrainSource::jitterValue(float theValue, float theJitter) const {
    //    AC_DEBUG << "jitterValue(<" << theValue << ">,<" << theJitter << ">)";
    float myRandomFloat = (float)(std::rand() % 1000) / 1000.f; 
    float myResult = theValue + (2 * myRandomFloat * theJitter - theJitter);
    //    AC_DEBUG << "randomFloat " << myRandomFloat;
    //    AC_DEBUG << "result: " << myResult;
    return myResult > 0 ? myResult : 0;
}





