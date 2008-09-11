//=============================================================================
// Copyright (C) 2004,2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "FrameConveyor.h"
#include "VideoFrame.h"
#include "FFMpegPixelEncoding.h"
#include "DecoderContext.h"

#include <asl/base/Logger.h>
#include <asl/base/Block.h>
#include <asl/base/console_functions.h>
#include <asl/audio/Pump.h>
#include <math.h>

#define DB(x) // x
#define DB2(x) // x


using namespace std;
using namespace asl;

namespace y60 {

    asl::Block FrameConveyor::_myResampledSamples(AVCODEC_MAX_AUDIO_FRAME_SIZE);

    const unsigned MAX_FRAME_CACHE_SIZE = 50;
    const double PRELOAD_CACHE_TIME = 2;    

    FrameConveyor::FrameConveyor() :
        _myVideoFrame(0),        
        _myAudioFrame(AVCODEC_MAX_AUDIO_FRAME_SIZE),
        _myCacheSizeInSecs(PRELOAD_CACHE_TIME),
        _myResampleContext(0)
    {
        AC_DEBUG << "FrameConveyor::FrameConveyor" << endl;
        _myVideoFrame = avcodec_alloc_frame();
    }

    FrameConveyor::~FrameConveyor() {
        AC_DEBUG << "FrameConveyor::~FrameConveyor" << endl;
        if (_myVideoFrame) {
            av_free(_myVideoFrame);
            _myVideoFrame = 0;
        }
        if (_myResampleContext) {
            audio_resample_close(_myResampleContext);
            _myResampleContext = 0;
        }
        if (_myAudioSink) {            
            _myAudioSink->stop();
        }        
    }
    
    void 
    FrameConveyor::setContext(DecoderContextPtr theContext) {
        _myContext = theContext;
    }

    void 
    FrameConveyor::clear() {
        _myCacheSizeInSecs = PRELOAD_CACHE_TIME;
        _myFrameCache.clear();
        if (_myAudioSink) {
            _myAudioSink->stop();
        }
    }

    void
    FrameConveyor::preload(double theInitialTimestamp) {        
        if (_myAudioSink) {
            // To re-read audio packets we have to start caching with a clear cache.
            clear();

            DB(cerr << "Fill audio/video-cache..." << endl;)
            
            while(_myFrameCache.size() < MAX_FRAME_CACHE_SIZE) 
            {
                updateCache(theInitialTimestamp);
                _myCacheSizeInSecs += (1 / _myContext->getFrameRate());
                DB(cerr << "  V-Buffersize: " << _myFrameCache.size() << endl;)
            } 
            if (_myAudioSink) {
                if (_myFrameCache.size() > MAX_FRAME_CACHE_SIZE) {
                    cerr << "   until frame cache size bigger than " << MAX_FRAME_CACHE_SIZE <<
                        " frames " << endl;
                } else if (double(_myAudioSink->getBufferedTime()) >= PRELOAD_CACHE_TIME)
                {
                    cerr << "   until audio cache size larger than " << PRELOAD_CACHE_TIME << 
                        " sec." << endl;
                }
            }
        } else {
            DB(cerr << "Fill video-cache..." << endl;)
            updateCache(theInitialTimestamp);
            _myCacheSizeInSecs = PRELOAD_CACHE_TIME;
        }
    }

    void
    FrameConveyor::printCacheInfo(double theTargetStart, double theTargetEnd) {
        double myCacheStart = -1; 
        double myCacheEnd = -1;
        if (! _myFrameCache.empty()) {
            myCacheStart = _myFrameCache.begin()->first;
            myCacheEnd = (--_myFrameCache.end())->first; 
        }

        double myMin = std::min(myCacheStart, theTargetStart);
        myMin = std::max(0.0, myMin);

        cerr << "|";
        for (unsigned i = 0; i < 40; ++i) {
            double myCharTime = myMin + double(i) * _myCacheSizeInSecs / 38;
            if ((myCharTime >= myCacheStart && myCharTime <= myCacheEnd) &&
                (myCharTime >= theTargetStart && myCharTime <= theTargetEnd)) 
            {
                cerr << TTYGREEN << "X" << ENDCOLOR;
            } else if (myCharTime >= myCacheStart && myCharTime <= myCacheEnd) {
                cerr << TTYYELLOW << "C" << ENDCOLOR;
            } else if (myCharTime >= theTargetStart && myCharTime <= theTargetEnd) {
                cerr << TTYRED << "T" << ENDCOLOR;
            } else {
                cerr << " ";
            }
        }
        cerr << "|";

        cerr <<TTYRED << " [" << theTargetStart << "; " << theTargetEnd << "]" << TTYGREEN << " [" << myCacheStart << "; " << myCacheEnd << "]" << ENDCOLOR << endl;
    }

    void
    FrameConveyor::updateCache(double theTimestamp) {
        DB(cerr << "updateCache at timestamp: " << theTimestamp << endl);
        double myCacheStart = -1; 
        double myCacheEnd = -1;
        if (! _myFrameCache.empty()) {
            myCacheStart = _myFrameCache.begin()->first;
            myCacheEnd = (--_myFrameCache.end())->first; 
        }
        
        double myTargetStart = asl::maximum(0.0, theTimestamp - 0.5 * _myCacheSizeInSecs);
        double myTargetEnd   = theTimestamp + 0.5 * _myCacheSizeInSecs;
        DB(cerr << " target " << myTargetStart << " - " <<  myTargetEnd << endl);

        //cerr << "ts: " << theTimestamp << ", start: " << myTargetStart << ", end: " << myTargetEnd << endl;
        DB(printCacheInfo(myTargetStart, myTargetEnd);)

        /*            
            |CCC| = Currently cached frames that are not needed any more
            |TTT| = Target frames that are not cached yet.
            |XXX| = Target frames that are already in the cache.
            |   | = Frames that are not in the cache and no target frames
        */

        if (myTargetStart >= myCacheEnd ||  /* |CCCCCCCCCC| |TTTTTTTTTT| */    
            myTargetEnd <= myCacheStart ||  /* |TTTTTTTTTT| |CCCCCCCCCC| */    
            (myTargetStart < myCacheStart && myTargetEnd > myCacheEnd) ) /* |TTTT|XX|TTTT| */
        {
            DB(cerr << TTYRED << "   case 3,4,5" << ENDCOLOR << endl;)
            _myFrameCache.clear();
            fillCache(myTargetStart, myTargetEnd);
            myTargetStart = _myFrameCache.begin()->first;
            myTargetEnd = _myFrameCache.rbegin()->first;
        } else if (myTargetEnd > myCacheEnd) {    
            // |CCCCCC|XX|TTTTTT| 
            DB(cerr << TTYGREEN << "   case 1" << ENDCOLOR << endl;)
            fillCache(myCacheEnd, myTargetEnd);
            myTargetEnd = _myFrameCache.rbegin()->first;
        } else if (myTargetStart < myCacheStart) { 
            // |TTTTTT|XX|CCCCCC| 
            DB(cerr << TTYRED << "   case 2" << ENDCOLOR << endl;)
            fillCache(myTargetStart, myCacheStart);
            myTargetStart = _myFrameCache.begin()->first;
        } else { 
            // |CCCCC|XX|CCCC| 
            DB(cerr << TTYRED << "   case 6" << ENDCOLOR << endl;)
        }

        //printCacheInfo(myTargetStart, myTargetEnd);
        trimCache(myTargetStart, myTargetEnd);
        //printCacheInfo(myTargetStart, myTargetEnd);
    }

    void FrameConveyor::trimCache(double theTargetStart, double theTargetEnd) {
        DB2(cerr << " trimming to " << theTargetStart << " - " << theTargetEnd << endl;);

        // Leave one frame at the beginning and end to avoid cutting just at the edge
        double myTimePerFrame = 1.0 / _myContext->getFrameRate();
        theTargetStart -= myTimePerFrame;
        theTargetEnd   += myTimePerFrame;
        FrameCacheIterator myIt = _myFrameCache.begin();
        while (myIt != _myFrameCache.end() && myIt->first < theTargetStart) {
            DB2(cerr << " erasing at " << myIt->first << endl;);
            _myFrameCache.erase(myIt++);
        }
        myIt = _myFrameCache.end();
        while (--myIt != _myFrameCache.begin() && myIt->first > theTargetEnd) {
            DB2(cerr << " erasing at " << myIt->first << endl;);
            _myFrameCache.erase(myIt);
            myIt = _myFrameCache.end();
        }
    }

    void FrameConveyor::fillCache(double theStartTime, double theEndTime) {
        DB(cerr << "fillCache [" << theStartTime << " to " << theEndTime << "]" << endl;)
        theStartTime = asl::maximum(0.0, theStartTime);
        theEndTime   = asl::maximum(theStartTime, theEndTime);
        theEndTime   = asl::minimum(_myContext->getEndOfFileTimestamp(), theEndTime);

        // seek if timestamp is outside these boundaries
        double myTimePerFrame = 1.0 / _myContext->getFrameRate();
#if LIBAVCODEC_BUILD >= 0x5100
        AVStream * myVStream = _myContext->getVideoStream();
        double myLastDecodedTime = _myVideoFrame->pts * av_q2d(myVStream->time_base);
#else
        double myLastDecodedTime = _myVideoFrame->pts / (double)AV_TIME_BASE;
#endif
        DB(cerr << "  last decoded: " << myLastDecodedTime << endl;)
        DB2(cerr << "  myTimePerFrame: " << myTimePerFrame << endl;)
        
        // Special case: prevent seek due to inaccuracies in the cache size calculation.
        if (theEndTime - theStartTime < myTimePerFrame/4) {
            DB2(cerr << "Premature end." << endl;)
            return;
        }

        if (fabs(theStartTime - myLastDecodedTime) >= myTimePerFrame * 1.5) {
            _myContext->seekToTime(theStartTime);
            if (_myAudioSink && _myAudioSink->getState() != HWSampleSink::STOPPED) {
                AC_WARNING << " Seek: " << fabs(theStartTime - myLastDecodedTime);
//                _myAudioSink->stop();
//                _myAudioSink = HWSampleSinkPtr(0);
            } else {
                AC_DEBUG << " Seek: " << fabs(theStartTime - myLastDecodedTime);
            }
        }

        double myLastDecodedVideoTime = theStartTime;
        bool   myEndOfFileFlag = false;
        while (!myEndOfFileFlag && myLastDecodedVideoTime < theEndTime ) {
            decodeFrame(myLastDecodedVideoTime, myEndOfFileFlag);
        }
        
        if (_myAudioSink) {
            myEndOfFileFlag = false;
            double myLastDecodedAudioTime = theStartTime;
            while (!myEndOfFileFlag && myLastDecodedAudioTime < theEndTime ) {
                myEndOfFileFlag = _myContext->decodeAudio(&_myAudioFrame);
                if (!myEndOfFileFlag) {
                    int numFrames = _myAudioFrame.getSizeInBytes() /
                        (getBytesPerSample(SF_S16) * _myContext->getAudioCannelCount());
                    AudioBufferPtr myBuffer;
                    if (_myResampleContext) {
                        DB2(cerr << "Resampling" << endl;)
                        numFrames = audio_resample(_myResampleContext, 
                            (int16_t*)(_myResampledSamples.begin()),
                            (int16_t*)(_myAudioFrame.getSamples()), 
                            numFrames);
                        myBuffer = Pump::get().createBuffer(numFrames);
                        myBuffer->convert(_myResampledSamples.begin(), SF_S16, 
                                _myContext->getAudioCannelCount());
                    } else {   
                        myBuffer = Pump::get().createBuffer(numFrames);
                        myBuffer->convert(_myAudioFrame.getSamples(), SF_S16, 
                            _myContext->getAudioCannelCount());
                    }
                    _myAudioSink->queueSamples(myBuffer);
                    myLastDecodedAudioTime = _myAudioFrame.getTimestamp();
                } else {
                    DB(cerr << "FrameConveyor::fillCache: eof" << endl;)
                }
            }
            
        }
        if (myEndOfFileFlag) {
            _myContext->setEndOfFileTimestamp(myLastDecodedVideoTime);  
        }
        DB2(
            cerr << "  V-Buffersize: " << _myFrameCache.size() << endl;
            cerr << "  current time: " << myLastDecodedVideoTime << endl;
            if (_myAudioSink) {
                cerr << "  A-Buffersize in secs: " << (_myAudioSink->getBufferedTime()) << endl;
            }
        )
    }

    double
    FrameConveyor::getFrame(double theTimestamp, dom::ResizeableRasterPtr theTargetRaster) {
        DB(cerr << "getFrame at ts: " << theTimestamp << endl);
        updateCache(theTimestamp);
        if (_myFrameCache.empty() ) {
            AC_ERROR << "FrameConveyor:: No frames to deliver because framecache is empty.";
            return theTimestamp; 
        }

        // Find frame in cache that is closest to the requested timestamp
        VideoFramePtr myFrame;
        FrameCache::iterator myNextFrameIt     = _myFrameCache.lower_bound(theTimestamp);
        FrameCache::iterator myPreviousFrameIt = myNextFrameIt;
        if (myNextFrameIt != _myFrameCache.begin()) {
            myPreviousFrameIt--; //normal case
        }   
        if (myNextFrameIt == _myFrameCache.end()) {
            myNextFrameIt--;
        }
        double myNextFrameDelta = fabs(myNextFrameIt->first - theTimestamp);
        double myPreviousFrameDelta = fabs(myPreviousFrameIt->first - theTimestamp);

        if (myNextFrameDelta < myPreviousFrameDelta) {
            myFrame = myNextFrameIt->second;
        } else {
            myFrame = myPreviousFrameIt->second;
        }

        DB(
            cerr << "getFrame: " << theTimestamp << "the system time " << asl::Time() << endl;
            cerr << "    previous/next:     " << myPreviousFrameIt->first << " / " << myNextFrameIt->first << endl;
            cerr << "   using frame: " << myFrame->getTimestamp() << endl;
        )

        copyFrame(myFrame->getData()->begin(), theTargetRaster);
        return myFrame->getTimestamp();
    }

    void
    FrameConveyor::decodeFrame(double & theLastDecodedVideoTime, bool & theEndOfFileFlag) {
        DB2(cerr << "decodeFrame" << endl;)
        if (!_myContext) {
            throw FrameConveyorException(std::string("No decoding context set, yet."),
                    PLUS_FILE_LINE);
        }

        theEndOfFileFlag = _myContext->decodeVideo(_myVideoFrame);
        if ( ! theEndOfFileFlag) {
            VideoFramePtr myNewFrame = VideoFramePtr(new VideoFrame(_myContext->getWidth(),
                    _myContext->getHeight(), _myContext->getBytesPerPixel()));

            _myContext->convertFrame(_myVideoFrame, myNewFrame->getData()->begin());
#if LIBAVCODEC_BUILD >= 0x5100
            AVStream * myVStream = _myContext->getVideoStream();
            myNewFrame->setTimestamp(_myVideoFrame->pts * av_q2d(myVStream->time_base));
#else
            myNewFrame->setTimestamp(_myVideoFrame->pts / (double)AV_TIME_BASE);
#endif            
            _myFrameCache[myNewFrame->getTimestamp()] = myNewFrame;
            theLastDecodedVideoTime = myNewFrame->getTimestamp();
        }
        DB2(cerr << "decodeFrame decoded " << theLastDecodedVideoTime << endl;)
    }

    void 
    FrameConveyor::copyFrame(unsigned char * theSourceBuffer, dom::ResizeableRasterPtr theTargetRaster) {
        // TODO: Pass target raster owning dom node instead of target raster and replace raster instead
        // of copy raster
        theTargetRaster->resize(_myContext->getWidth(), _myContext->getHeight());
        memcpy(theTargetRaster->pixels().begin(), theSourceBuffer, theTargetRaster->pixels().size());
    }

    double 
    FrameConveyor::getEndOfFileTimestamp() const {
        return _myContext->getEndOfFileTimestamp();
    }
    
    void 
    FrameConveyor::setupAudio(bool theUseAudioFlag) {
        AVStream * myAudioStream = _myContext->getAudioStream();
#if LIBAVCODEC_BUILD >= 0x5100
        AVCodecContext * myACodec = myAudioStream->codec;
#else
        AVCodecContext * myACodec = &myAudioStream->codec;
#endif

        if (myAudioStream && theUseAudioFlag) {
            AVCodec * myCodec = avcodec_find_decoder(myACodec->codec_id);
            if (!myCodec) {
                AC_WARNING << "Unable to find audio decoder: " << _myContext->getFilename();
                 _myAudioSink = HWSampleSinkPtr(0);
                 return;
            }
            if (avcodec_open(myACodec, myCodec) < 0 ) {
                AC_WARNING << "Unable to find audio decoder: " << _myContext->getFilename();
                _myAudioSink = HWSampleSinkPtr(0);
                return;
            }

            Pump & myAudioPump = Pump::get();
//            if (!myAudioController.isRunning()) {
//                myAudioController.init(asl::maximum
//                      ((unsigned)myACodec->sample_rate, (unsigned) 44100));
//            }

            _myAudioSink = myAudioPump.createSampleSink(_myContext->getFilename());
            //_myAudioSink->setVolume(getMovie()->get<VolumeTag>());
            AC_INFO << "Audio: channels=" << myACodec->channels << ", samplerate=" 
                    << myACodec->sample_rate;
            initResample(myACodec->channels, myACodec->sample_rate);
        } else {
            _myAudioSink = HWSampleSinkPtr(0);
            _myContext->disableAudio();
        }
        
    }
    
    void 
    FrameConveyor::playAudio() {
        if (_myAudioSink) {            
            _myAudioSink->play();
        }        
    }
    void 
    FrameConveyor::stopAudio() {
        if (_myAudioSink) {            
            _myAudioSink->stop();
        }        
    }
    void 
    FrameConveyor::pauseAudio() {
        if (_myAudioSink) {            
            _myAudioSink->pause();
        }        
    }
    void 
    FrameConveyor::setVolume(double theVolume) {
        if (_myAudioSink) {
            _myAudioSink->setVolume(float(theVolume));
        }
    }
            
    void 
    FrameConveyor::initResample(int theNumInputChannels, int theInputSampleRate) {
        // TODO: Convert num. of channels here?
        if (theInputSampleRate != Pump::get().getNativeSampleRate()) {
            _myResampleContext = audio_resample_init(theNumInputChannels, theNumInputChannels,
                    Pump::get().getNativeSampleRate(), theInputSampleRate);
        }
    }
    
}
