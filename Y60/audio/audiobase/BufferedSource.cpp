//=============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: BufferedSource.cpp,v $
//     $Author: ulrich $
//   $Revision: 1.26 $
//       $Date: 2005/04/29 14:08:26 $
//
//  Memory-buffered audio source.
//
//=============================================================================

#include "BufferedSource.h"
#include "SoundCardManager.h"
#include "MultiAudioBuffer.h"
#include "AudioOutput.h"

#include <asl/string_functions.h>
#include <asl/Auto.h>
#include <asl/Time.h>
#include <asl/Logger.h>

#define DB(x) // x
#define DB2(x) //x

using namespace std;

namespace AudioBase {

    const unsigned AUDIO_CACHE_SIZE = 128;

    BufferedSource::BufferedSource(const string & theName) : 
        IFileReader(theName, -1, 1.0, false),
        _isRunning(false), 
        _isStopped(false), 
        _myNumChannels(0), 
        _mySampleBits(16),
        _myTmpBufSize(0), 
        _myTmpBuf(0), 
        _myTmpBufFill(0), 
        _myLastTimestamp(0),
        _myAudioCacheSize(AUDIO_CACHE_SIZE),
        _myReadPosition(0), 
        _myWritePosition(0), 
        _myResampleContext(0), 
        _myResampleBuffer(0),
        _mySilenceBuffer(0)
    {
        AC_DEBUG << "BufferedSource::BufferedSource '" << theName << "' " << (void*)this;
    }

    BufferedSource::~BufferedSource() {
        asl::AutoLocker<BufferedSource> myLocker(*this);
        AC_DEBUG << "BufferedSource::~BufferedSource " << (void*)this;

        // stop outputs
        for (unsigned i = 0; i < getNumOutputs(); ++i) {
            getOutput(i)->stop();
        }

        // free sample buffer
        if (_myTmpBuf) {
            delete[] _myTmpBuf;
            _myTmpBuf = 0;
            _myTmpBufSize = 0;
        }

        // free audio buffers
        for (unsigned i = 0; i < _myAudioCache.size(); ++i) {
            delete _myAudioCache[i];
        }

        if (_mySilenceBuffer) {
            delete _mySilenceBuffer;
            _mySilenceBuffer = 0;
        }

        if (_myResampleContext) {
            audio_resample_close(_myResampleContext);
        }

        if (_myResampleBuffer) {
            delete [] _myResampleBuffer;
        }
    }

    void BufferedSource::setRunning(bool theFlag) {
        asl::AutoLocker<BufferedSource> myLocker(*this);
        _isRunning = theFlag;
        AC_DEBUG << "BufferedSource::setRunning " << (void*)this << " " << _isRunning;
    }

    void BufferedSource::setup(unsigned theNumChannels, unsigned theSampleRate) {
        _myNumChannels = theNumChannels;
        setSampleRate(theSampleRate);

        if (_myTmpBuf || _myResampleContext) {
            throw BufferedSourceException("BufferedSource already setup", PLUS_FILE_LINE);
        }

        // temporary assembly buffer
        _myTmpBufSize = AudioBuffer::getNumSamples() * getNumChannels() * sizeof(int16_t);
        _myTmpBuf = new unsigned char[_myTmpBufSize];
        _myTmpBufFill = 0;
      
        // create outputs
        if (getNumChannels() == 1) {
            newOutput(Mono);
        } else if (getNumChannels() == 2) {
            newOutput(Left);
            newOutput(Right);
        } else if (getNumChannels() == 4) {
            newOutput(Left);
            newOutput(Right);
            newOutput(RearLeft);
            newOutput(RearRight);
        } else if (getNumChannels() == 6) {
            newOutput(Left);
            newOutput(Right);
            newOutput(Mono);
            newOutput(Sub);
            newOutput(RearLeft);
            newOutput(RearRight);
        } else {
            throw BufferedSourceException("BufferedSource only works with 1,2,4, or 6 outputs", PLUS_FILE_LINE);
        }

        _myAudioCache.resize(_myAudioCacheSize);
        for (unsigned i = 0; i < _myAudioCache.size(); ++i) {
            _myAudioCache[i] = new MultiAudioBuffer(getNumChannels());
        }

        _mySilenceBuffer = new MultiAudioBuffer(getNumChannels());
        _mySilenceBuffer->clear();
        
        unsigned mySoundCardSampleRate = SoundCardManager::get().getCard(0)->getSampleRate();
        if (theSampleRate != mySoundCardSampleRate) {
            if (getSampleBits() != 16) {
                throw BufferedSourceException("BufferedSource does only support resampling for 16 bit samples", PLUS_FILE_LINE);
            }
            // Initialize resampling
            _myResampleContext = audio_resample_init(getNumChannels(), getNumChannels(),
                mySoundCardSampleRate, theSampleRate);
            AC_DEBUG << "Resampling from " << theSampleRate << " to " << mySoundCardSampleRate;
            if (!_myResampleBuffer) {
                _myResampleBuffer = new unsigned char[AVCODEC_MAX_AUDIO_FRAME_SIZE];
            }
        } 

        AC_INFO << "Using " << _myAudioCache.size() << " buffers, " << _myAudioCache.size() * (AudioBuffer::getNumSamples() / (float)mySoundCardSampleRate) << "s";

        AudioAsyncModule::init();
    }

    void
    BufferedSource::addBuffer(double theTimestamp, const unsigned char * theBuffer, unsigned theBufferSize) {

        asl::AutoLocker<BufferedSource> myLocker(*this);
        if (_myTmpBuf == 0) {
            throw BufferedSourceException("BufferedSource not setup", PLUS_FILE_LINE);
        }

        if (theBufferSize % (getNumChannels() * (getSampleSize())) != 0) {
            // UH: this shouldn't happen but it does...
            AC_ERROR << "theBufferSize=" + asl::as_string(theBufferSize) + " must be multiple of getNumChannels()*getSampleSize()=" + asl::as_string(getNumChannels() * getSampleSize());
            while (theBufferSize % (getNumChannels() * getSampleSize()) != 0) {
                theBufferSize--;
            }
        }

        // temp. assembly myBuffer
        unsigned char * myTmpBufP = _myTmpBuf + _myTmpBufFill;
        unsigned myTmpLen = _myTmpBufSize - _myTmpBufFill;

        // delta-t per AudioBuffer
        const double myDeltaT = AudioBuffer::getNumSamples() / (double)getSampleRate();

        // resample if necessary and 16 bit
        if (_myResampleContext && _myResampleBuffer && getSampleBits() == 16) {
            // resampling, buffersize is in number of samples
            int numSamples = audio_resample(_myResampleContext, (int16_t*)_myResampleBuffer, (int16_t*)theBuffer, 
                theBufferSize / (getNumChannels() * sizeof(int16_t)));
            // mySamplesize is numSamples * numChannels * samplesize (16 bit)
            //DB(AC_TRACE << "resampling buffer " << theBufferSize << " -> " << numSamples * getNumChannels() * 2);
            theBuffer = _myResampleBuffer;
            theBufferSize = numSamples * getNumChannels() * sizeof(int16_t);
        } 

        while (theBufferSize > 0) {
            // copy samples to assembly buffer
            unsigned myLen = (theBufferSize < myTmpLen ? theBufferSize : myTmpLen); 
            memcpy(myTmpBufP, theBuffer, myLen);

            // advance sample pointer
            theBuffer += myLen;
            theBufferSize -= myLen;

            // advance assembly buffer
            myTmpBufP += myLen;
            myTmpLen -= myLen;

            // fill audio buffer and push to decoded list
            if (myTmpLen == 0) {
                _myAudioCache[_myWritePosition % _myAudioCache.size()]->setData((asl::Signed16*)_myTmpBuf, theTimestamp);
                _myWritePosition++;                
                int myDiff = _myWritePosition - _myReadPosition;
                if (myDiff >= int(_myAudioCache.size())) {
                    AC_ERROR << "AudioCache overrun. Write: " << _myWritePosition << " / Read: " << _myReadPosition << " / Diff: " << myDiff << " / Size: " << _myAudioCache.size();
                    _myReadPosition = _myWritePosition - 1;
                }
                        
                myTmpBufP = _myTmpBuf;
                myTmpLen = _myTmpBufSize;
                theTimestamp += myDeltaT;
            }
        }
        _myTmpBufFill = _myTmpBufSize - myTmpLen;
    }

    double
    BufferedSource::getCurrentTimestamp() const {
        return _myLastTimestamp;
    }

    void
    BufferedSource::stop() {
        asl::AutoLocker<BufferedSource> myLocker(*this);
        AC_DEBUG << "BufferedSource::stop " << (void*)this;
        setRunning(false);
        _isStopped = true;       
    }

    void
    BufferedSource::pause() {
        AC_DEBUG << "BufferedSource::pause " << (void*)this;
        setRunning(!_isRunning);
    }

    void 
    BufferedSource::process() {
        asl::AutoLocker<BufferedSource> myLocker(*this);
        if (_myTmpBuf == 0) {
            throw BufferedSourceException("BufferedSource not setup", PLUS_FILE_LINE);
        }
        DB(AC_TRACE << "BufferedSource::process " << (void*)this);

        MultiAudioBuffer * myBuffer = 0;
        if (!_isRunning) {
            DB(AC_TRACE << "Silence");
            myBuffer = _mySilenceBuffer;
            asl::msleep(1);
        } else if (_myReadPosition < _myWritePosition) {
            myBuffer = _myAudioCache[_myReadPosition % _myAudioCache.size()];
            _myReadPosition++;
            _myLastTimestamp = myBuffer->getTimeStamp();
        } else {
            AC_WARNING << "AudioCache underrun";
            myBuffer = _mySilenceBuffer;
            //_myReadPosition++;
        }

        switch (getNumChannels()) {
        case 1:
            if (getOutput(0)->getInputModule()) {
                getOutput(0)->put(*myBuffer->getMono());
            }
            break;
        case 2:
            if (getOutput(0)->getInputModule() && getOutput(1)->getInputModule()) {
                getOutput(0)->put(*myBuffer->getChannel(Left));
                getOutput(1)->put(*myBuffer->getChannel(Right));
            }
            break;
        case 4:
            if (getOutput(0)->getInputModule() && getOutput(1)->getInputModule() &&
                getOutput(2)->getInputModule() && getOutput(3)->getInputModule()) 
            {
                // Multichannel
                const ChannelType myTypes[] = {Left, Right, RearLeft, RearRight};
                for (int i = 0; i < getNumChannels(); ++i) {
                    getOutput(i)->put(*myBuffer->getChannel(myTypes[i]));
                }
            }
            break;
        case 6: 
            if (getOutput(0)->getInputModule() && getOutput(1)->getInputModule() &&
                getOutput(2)->getInputModule() && getOutput(3)->getInputModule() &&
                getOutput(4)->getInputModule() && getOutput(5)->getInputModule()) 
            {
                // Multichannel
                const ChannelType myTypes[] = {Left, Right, Mono, Sub, RearLeft, RearRight};
                for (int i = 0; i < getNumChannels(); ++i) {
                    getOutput(i)->put(*myBuffer->getChannel(myTypes[i]));
                }
            } else {
                AC_ERROR << "BufferedSource I/O error";
            }
            break;
        case 8:
        default:
            throw BufferedSourceException("Unhandled number of channels", PLUS_FILE_LINE);
        }

        if (_isStopped) {
            DB(AC_TRACE << "BufferedSource::process stop");
            AudioAsyncModule::stop();
        }
    }

    void 
    BufferedSource::clear() {
        asl::AutoLocker<BufferedSource> myLocker(*this);
        _myLastTimestamp = 0.0;
        _myReadPosition  = _myWritePosition;
        _myTmpBufFill    = 0;
    }

    double 
    BufferedSource::getCacheFillLevelInSecs() const {
        return double(getCacheFillLevel() * AudioBuffer::getNumSamples()) / SoundCardManager::get().getCard(0)->getSampleRate();
    }

    unsigned
    BufferedSource::getCacheSize() const {
        return _myAudioCache.size();
    }

    int
    BufferedSource::getCacheFillLevel() const {
        return (_myWritePosition - _myReadPosition);
    }

} 
