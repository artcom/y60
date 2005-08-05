// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioBuffer.cpp,v $
//     $Author: pavel $
//   $Revision: 1.15 $
//
//=============================================================================

#include "AudioBuffer.h"
#include "AudioScheduler.h"

#include <asl/Logger.h>

#include <cmath>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <limits.h>

using namespace std;

namespace AudioBase {

//int AudioBuffer::_mySize = 0;

AudioBuffer::AudioBuffer () : 
    _myLastSample(0.0),
    _mySize(AudioScheduler::get().getBufferSize())
{
    //AC_DEBUG << "AudioBuffer " << (void*)this << endl;
    // The soundcard needs to be initialized first so setSize() gets called.
    if (!_mySize) {
        AC_ERROR << "Buffer size not set, using default";
        _mySize = 2048;
    }

    _myBuffer = new SAMPLE[_mySize];
    clear();
}


AudioBuffer::AudioBuffer(const AudioBuffer & otherBuffer) {
    if (this != &otherBuffer) {
        //_myChannelType = otherBuffer._myChannelType;
        _mySize = otherBuffer._mySize;
        _myBuffer = new SAMPLE[_mySize];
        memcpy(_myBuffer, otherBuffer._myBuffer, _mySize);
        _myLastSample = otherBuffer._myLastSample;
    }
}

AudioBuffer::~AudioBuffer() {
    //AC_DEBUG << "~AudioBuffer " << (void*)this << endl;
    delete[] _myBuffer;
    _myBuffer = 0;
}

AudioBuffer &
AudioBuffer::operator=(const AudioBuffer & otherBuffer) {
    if (this != &otherBuffer) {
        //_myChannelType = otherBuffer._myChannelType;
        _mySize = otherBuffer._mySize;
        delete[] _myBuffer;
        _myBuffer = new SAMPLE[_mySize];
        memcpy(_myBuffer, otherBuffer._myBuffer, _mySize);
        _myLastSample = otherBuffer._myLastSample;
    }
    return *this;
}

void
AudioBuffer::setData (const SAMPLE * myData) {
    _myLastSample = _myBuffer[_mySize-1];
    memcpy (_myBuffer, myData, sizeof(SAMPLE)*_mySize);
}

void
AudioBuffer::set16Bit (const asl::Signed16 * myData, ChannelType inputChannelType, unsigned numChannels) {
    _myLastSample = _myBuffer[_mySize-1];
    int myMultiplexIndex = getChannelIndex(inputChannelType, numChannels);
    if (myMultiplexIndex >= 0) {
        for (int i=0; i<_mySize; i++) {
            _myBuffer[i] = double(myData[i*numChannels+myMultiplexIndex])/32768;
        }
    } else {
        AC_ERROR << "Illegal channel type " << inputChannelType << " for " << numChannels << " Channels.";
    }
}

double
doublefrom24bit (const char * myData) {
    asl::Signed32 longVersion;
    char * longVerPtr = (char*)&longVersion;
    longVerPtr[3] = myData[2];
    longVerPtr[2] = myData[1];
    longVerPtr[1] = myData[0];
    longVerPtr[0] = 0;
    return double(longVersion)/(32768.0*256.0*256.0);
}

void
AudioBuffer::set24Bit (const char * myData, ChannelType inputChannelType, unsigned numChannels) {
    _myLastSample = _myBuffer[_mySize-1];
    int myMultiplexIndex = getChannelIndex(inputChannelType, numChannels);
    if (myMultiplexIndex >= 0) {
        for (int i=0; i<_mySize; i++) {
            _myBuffer[i] = doublefrom24bit(myData+(i*numChannels+myMultiplexIndex)*3)/32768;
        }
    } else {
        AC_ERROR << "Illegal channel type " << inputChannelType << " for " << numChannels << " Channels.";
    }
}

void
AudioBuffer::set32Bit (const asl::Signed32 * myData, ChannelType inputChannelType, unsigned numChannels) {
    int myMultiplexIndex = getChannelIndex(inputChannelType, numChannels);
    if (myMultiplexIndex >= 0) {
        for (int i=0; i<_mySize; i++) {
            _myBuffer[i] = double(myData[i*numChannels+myMultiplexIndex])/(32768.0*256.0*256.0);
        }
    } else {
        AC_ERROR << "Illegal channel type " << inputChannelType << " for " << numChannels << " Channels.";
    }
}


void
AudioBuffer::clear() {
    memset (_myBuffer, 0, sizeof(SAMPLE)*_mySize);
}

void
AudioBuffer::multiply(double d) {
    for (int i=0; i<_mySize; i++) {
        _myBuffer[i] *= d;
    }
}


SAMPLE *
AudioBuffer::getData () {
    return _myBuffer;
}

asl::Signed32 *
AudioBuffer::createBuffer32 () {
    asl::Signed32 * myNIData = new asl::Signed32[_mySize];
    // Convert to S32 format
    for (int i=0; i<_mySize; i++) {
        myNIData[i] = int (_myBuffer[i]*65536*32768);
    }
    return myNIData;
}

asl::Signed16
AudioBuffer::getData16(int theSample) {
    asl::Signed16 myData = asl::Signed16(_myBuffer[theSample]*32768);
    return myData;
}

asl::Signed16 *
AudioBuffer::createBuffer16 () {
    asl::Signed16 * myNIData = new asl::Signed16[_mySize];
    copyToBuffer16(myNIData);
    return myNIData;
}

void
AudioBuffer::copyToBuffer16(asl::Signed16* myData) {
    int i;
    // Convert to S16 format
    for (i=0; i<_mySize; i++) {
        myData[i] = asl::Signed16 (_myBuffer[i]*32768);
    }
}

AudioBuffer *
AudioBuffer::getSilenceBuffer() {
    static AudioBuffer * mySilenceBuffer = 0;
    if (mySilenceBuffer == 0) {
        mySilenceBuffer = new AudioBuffer;
        mySilenceBuffer->clear();
    }
    return mySilenceBuffer;
}

unsigned
AudioBuffer::getSize() {
    return sizeof(SAMPLE) * _mySize;
}

void
AudioBuffer::dumpSamples (int startSample, int endSample) {
    int i;
    for (i=startSample; i<endSample; i++) {
        if ((i-startSample)%4 == 0) {
            cerr << endl << "  " << std::setw(4) << i << ": ";
        }
        cerr << std::setprecision(8) << std::setw(8) << _myBuffer[i] << " ";
    }
}

void
AudioBuffer::dump (int importantSample) {
    cerr << "---- ";
    dumpSamples (0, 32);
    cerr << endl << "  ...";
    if (importantSample > 31 && importantSample < _mySize-31) {
        dumpSamples (importantSample-16, importantSample+15);
        cerr << endl << "  ...";
    }
    dumpSamples (_mySize-32, _mySize);
    cerr << endl << "Maximum: " << getMax() << endl;
}

void
AudioBuffer::setSize(unsigned theSize) {
    AudioScheduler::get().setBufferSize(theSize);
}

unsigned
AudioBuffer::getNumSamples() {
    return AudioScheduler::get().getBufferSize();
}

bool
AudioBuffer::hasDistortion() {
    return getMax() > 1.0;
}

double
AudioBuffer::getMax() {
    double myMax = 0;
    for (unsigned i=0; i<_mySize; ++i) {
        if (_myBuffer[i] > myMax) {
            myMax = _myBuffer[i];
        }
    }
    return myMax;
}

void
AudioBuffer::normalize(double theMaximum) {
    for (int i = 0; i < _mySize; i++) {
        _myBuffer[i] = _myBuffer[i] / theMaximum;
    }
}

bool
AudioBuffer::britzelTest (double theSample1, double theSample2, int theBufIndex) {
    const double MaxDiff = 0.1;
    double britzelIntensity = fabs(theSample1-theSample2);
    if (britzelIntensity > MaxDiff) {
//        AC_DB("AudioBuffer: Britzeln, intensity " << britzelIntensity
//             << ", index: " << theBufIndex);
//        dump(theBufIndex);
        return true;
    }
    return false;
}

bool
AudioBuffer::hasClicks() {
    bool hasClicks = false;
    for (int i=0; i<_mySize-1; i++) {
        if (britzelTest (_myBuffer[i], _myBuffer[i+1], i)) {
            hasClicks = true;
            break;
        }
    }
    if (_myLastSample != 0.0 && britzelTest (_myBuffer[0], _myLastSample, -1)) {
        hasClicks = true;
    }
    return hasClicks;
}
/*
ChannelType
AudioBuffer::getType() {
    return _myChannelType;
}
*/
}
