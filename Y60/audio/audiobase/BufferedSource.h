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
//    $RCSfile: BufferedSource.h,v $
//     $Author: christian $
//   $Revision: 1.16 $
//       $Date: 2005/04/08 18:20:13 $
//
//  Memory-buffered audio source.
//
//=============================================================================

#ifndef INCL_BUFFEREDSOURCE
#define INCL_BUFFEREDSOURCE

#include "IFileReader.h"

#include <asl/ThreadLock.h>
#include <asl/Exception.h>
#include <string>
#include <vector>

#ifdef WIN32
#pragma warning( disable : 4244 ) // Disable ffmpeg warning
#define EMULATE_INTTYPES
#endif
#include <ffmpeg/avformat.h>
#ifdef WIN32
#pragma warning( default : 4244 ) // Reenable warning
#endif

namespace AudioBase {
    class MultiAudioBuffer;

    DEFINE_EXCEPTION(BufferedSourceException, asl::Exception);

    /**
     * @ingroup Y60audioaudiobase
     * The BufferedSource is a generic source for the audio subsystem. It is fed 
     * audio samples via the addBuffer method and forwards them into the audiograph
     * by implementing the AudioAsyncModule interface.
     */
    class BufferedSource: public IFileReader {
    public:
        BufferedSource(const std::string & theName);
        virtual ~BufferedSource();

        void setup(unsigned theNumChannels, unsigned theSampleRate);

        void addBuffer(double theTimestamp, const unsigned char * theBuffer, unsigned theBufferSize);
        double getCurrentTimestamp() const;

	    // IFileReader
	    virtual double getDuration() const { 
            return -1.0; 
        }

        // AudioAsyncModule
        virtual void stop();
        virtual void pause();
        virtual void process();

        double getNullSampleTime();

        unsigned getNumChannels() const {
            return _myNumChannels;
        }

        /**
         * Sets the number of sample bits
         * @param theBits samplebits to set (8, 16, 24, 32)
         * @warn only 16 bits are currently supported
         * @todo support other samplebits than 16
         */
        void setSampleBits(unsigned theBits) {
            _mySampleBits = theBits;
        }

        /**
         * @return the number of bits per sample
         */
        unsigned getSampleBits() const {
            return _mySampleBits;
        }

        /**
         * @return the size of a sample in bytes
         */
        unsigned getSampleSize() const {
            return _mySampleBits >> 3;
        }

        // Lockable interface
        void lock() {
            _myLock.lock();
        }
        void unlock() {
            _myLock.unlock();
        }
        
        void setRunning(bool theFlag);
        bool isRunning() const {
            return _isRunning;
        }

        void clear();

        bool empty() const {
            return (_myReadPosition == _myWritePosition);
        }

        unsigned getCacheSize() const;
        int getCacheFillLevel() const;

    private:    
        asl::ThreadLock         _myLock;

        bool                    _isRunning;
        bool                    _isStopped;

        unsigned                _myNumChannels;
        double                  _myLastTimestamp;
		ReSampleContext *       _myResampleContext;
        unsigned char *         _myResampleBuffer;

        unsigned                        _myAudioCacheSize;
        std::vector<MultiAudioBuffer*>  _myAudioCache;
        MultiAudioBuffer *              _mySilenceBuffer;
        unsigned                        _myReadPosition;
        unsigned                        _myWritePosition;
        unsigned                        _mySampleBits;

        // collector of decoded samples
        size_t          _myTmpBufSize;
        unsigned char * _myTmpBuf;
        unsigned        _myTmpBufFill;
    };
}

#endif
