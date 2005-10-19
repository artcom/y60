//=============================================================================
// Copyright (C) 2004,2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: FrameConveyor.cpp,v $
//     $Author: ulrich $
//   $Revision: 1.6 $
//       $Date: 2005/04/01 17:03:27 $
//
//  ffmpeg movie decoder.
//
//=============================================================================

#include "FrameConveyor.h"
#include "VideoFrame.h"
#include "FFMpegPixelEncoding.h"
#include "DecoderContext.h"

#include <asl/Logger.h>
#include <asl/Block.h>
#include <asl/console_functions.h>
#include <asl/Pump.h>
#include <math.h>

#define DB(x) // x
#define DB2(x) // x


using namespace std;
using namespace asl;

namespace y60 {

    const unsigned MAX_FRAME_CACHE_SIZE = 500;
    const double PRELOAD_CACHE_TIME = 2;    

    FrameConveyor::FrameConveyor() :
        _myVideoFrame(0),        
        _myAudioPacket(AVCODEC_MAX_AUDIO_FRAME_SIZE),
        _myCacheSizeInSecs(PRELOAD_CACHE_TIME)
    {
        _myVideoFrame = avcodec_alloc_frame();
    }

    FrameConveyor::~FrameConveyor() {
        if (_myVideoFrame) {
            av_free(_myVideoFrame);
            _myVideoFrame = 0;
        }
    }
    
    void 
    FrameConveyor::load(DecoderContextPtr theContext, asl::HWSampleSinkPtr theAudioSink) {
        _myContext = theContext;
        _myAudioSink = theAudioSink;
        if (theAudioSink) {
            setupAudio();
        }        
    }

    void 
    FrameConveyor::clear() {
        _myCacheSizeInSecs = PRELOAD_CACHE_TIME;
        _myFrameCache.clear();
        // TODO: Audio::stop()?
    }

    void
    FrameConveyor::preload(double theInitialTimestamp) {        
        if (_myAudioSink) {
            // To re-read audio packets we have to start caching with a clear cache.
            clear();

            cerr << "Fill audio/video-cache..." << endl;
            while(_myFrameCache.size() < MAX_FRAME_CACHE_SIZE &&
                  double(_myAudioSink->getBufferedTime()) < PRELOAD_CACHE_TIME) 
            {
                updateCache(theInitialTimestamp);
                _myCacheSizeInSecs += (1 / _myContext->getFrameRate());
                DB(cerr << "  A/V-Buffersize: " << _myAudioSink->getBufferedTime() << "secs "
                        << "/" << _myFrameCache.size() << endl;)
            }            

            if (_myFrameCache.size() > MAX_FRAME_CACHE_SIZE) {
                cerr << "   until frame cache size bigger than " << MAX_FRAME_CACHE_SIZE <<
                        " frames " << endl;
            } else if (double(_myAudioSink->getBufferedTime()) >= PRELOAD_CACHE_TIME)
            {
                cerr << "   until audio cache size larger than " << PRELOAD_CACHE_TIME << 
                        " sec." << endl;
            }
        } else {
            cerr << "Fill video-cache..." << endl;
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
        DB(cerr << "updateCache at ts: " << theTimestamp << endl);
        double myCacheStart = -1; 
        double myCacheEnd = -1;
        if (! _myFrameCache.empty()) {
            myCacheStart = _myFrameCache.begin()->first;
            myCacheEnd = (--_myFrameCache.end())->first; 
        }
        
        double myTargetStart = asl::maximum(0.0, theTimestamp - 0.5 * _myCacheSizeInSecs);
        double myTargetEnd   = theTimestamp + 0.5 * _myCacheSizeInSecs;

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
        theStartTime = asl::maximum(0.0, theStartTime);
        theEndTime   = asl::maximum(theStartTime, theEndTime);
        theEndTime   = asl::minimum(_myContext->getEndOfFileTimestamp(), theEndTime);

        // seek if timestamp is outside these boundaries
        double myTimePerFrame = 1.0 / _myContext->getFrameRate();
        double myLastDecodedTime = _myVideoFrame->pts / (double)AV_TIME_BASE;

        DB2(cerr << "fillCache [" << theStartTime << " to " << theEndTime << "]" << endl;)
        DB2(cerr << " last decoded " << myLastDecodedTime << endl;)
        if (fabs(theStartTime - myLastDecodedTime) >= myTimePerFrame * 2) {
            _myContext->seekToTime(theStartTime);
        }

        double myCurrentTime = theStartTime;
        bool   myEndOfFileFlag = false;
        while (!myEndOfFileFlag && myCurrentTime < theEndTime) {
            decodeFrame(myCurrentTime, myEndOfFileFlag);
            if (myEndOfFileFlag) {
                _myContext->setEndOfFileTimestamp(myCurrentTime);  
            }
            DB2(
                cerr << "  V-Buffersize: " << _myFrameCache.size() << endl;
                cerr << "  current time: " << myCurrentTime << endl;
                if (_myContext->getAudioStream()) {
                    cerr << "  A-Buffersize in secs: " << (_myAudioSink->getBufferedTime())
                            <<endl;
                }
            )
        }
    }

    double
    FrameConveyor::getFrame(double theTimestamp, dom::ResizeableRasterPtr theTargetRaster) {
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
/*
        DB(
            cerr << "getFrame: " << theTimestamp << "the system time " << asl::Time() << endl;
            cerr << "    previous/next:     " << myPreviousFrameIt->first << " / " << myNextFrameIt->first << endl;
            cerr << "   using frame: " << myFrame->getTimestamp() << endl;
        )
*/
        copyFrame(myFrame->getData()->begin(), theTargetRaster);
        return myFrame->getTimestamp();
    }

    void
    FrameConveyor::setupAudio() {
        // TODO: setup sample rate conversion.
        AVStream * myAudioStream = _myContext->getAudioStream();
        if (myAudioStream) {
            AVCodec * myCodec = avcodec_find_decoder(myAudioStream->codec.codec_id);
            if (!myCodec) {
                throw FrameConveyorException(std::string("Unable to find audio decoder: ") + _myContext->getFilename(), PLUS_FILE_LINE);
            }
            if (avcodec_open(&myAudioStream->codec, myCodec) < 0 ) {
                throw FrameConveyorException(std::string("Unable to open audio codec: ") + _myContext->getFilename(), PLUS_FILE_LINE);
            }
        } else {
            AC_INFO << _myContext->getFilename() << ": No audio stream found.";
        }
    }

    void
    FrameConveyor::decodeFrame(double & theCurrentTime, bool & theEndOfFileFlag) {
        theEndOfFileFlag = false;
        if (!_myContext) {
            throw FrameConveyorException(std::string("No decoding context set, yet."),
                    PLUS_FILE_LINE);
        }

        DecoderContext::FrameType myFrameType;
        if (_myAudioSink) {
            myFrameType = _myContext->decode(_myVideoFrame, &_myAudioPacket);
        } else {
            myFrameType = _myContext->decode(_myVideoFrame, 0);
        }

        switch (myFrameType) {
            case DecoderContext::FrameTypeVideo:
            {
                unsigned myBPP = getBytesRequired(1, getPixelEncoding(_myContext->getVideoStream()));
                VideoFramePtr myNewFrame = VideoFramePtr(new VideoFrame(_myContext->getWidth(), _myContext->getHeight(), myBPP));

                convertFrame(_myVideoFrame, myNewFrame->getData()->begin());
                myNewFrame->setTimestamp(_myVideoFrame->pts / (double)AV_TIME_BASE);
                _myFrameCache[myNewFrame->getTimestamp()] = myNewFrame;
                theCurrentTime = myNewFrame->getTimestamp();
                break;
            }
            case DecoderContext::FrameTypeAudio:
            {
                AudioBufferPtr myBuffer;
/*
                if (_myResampleContext) {
                    numFrames = audio_resample(_myResampleContext, 
                            (int16_t*)(_myResampledSamples.begin()),
                            (int16_t*)(_mySamples.begin()), 
                            numFrames);
                    myBuffer = Pump::get().createBuffer(numFrames);
                    myBuffer->convert(_myResampledSamples.begin(), SF_S16, _myNumChannels);
                } else {
*/                
                myBuffer = Pump::get().createBuffer(_myAudioPacket.getSampleSize() / 
                        (2 * _myContext->getNumAudioChannels()));
                myBuffer->convert(_myAudioPacket.getSamples(), SF_S16, 
                        _myContext->getNumAudioChannels());
                _myAudioSink->queueSamples(myBuffer);
                theCurrentTime = _myAudioPacket.getTimestamp();
                break;
            }
            case DecoderContext::FrameTypeEOF:
                theEndOfFileFlag = true;
                break;
            default:
                throw FrameConveyorException(std::string("Unknown frame type: ") 
                        + as_string(myFrameType), PLUS_FILE_LINE);
        }
    }

    void 
    FrameConveyor::convertFrame(AVFrame * theFrame, unsigned char * theTargetBuffer) {
        if (!_myContext || !theFrame) {
            AC_ERROR << "FFMpegDecoder::decodeVideoFrame no context or invalid frame";
            return;
        }

        unsigned int myLineSizeBytes = getBytesRequired(_myContext->getWidth(), 
                getPixelEncoding(_myContext->getVideoStream()));
        AVPicture myDestPict;
        myDestPict.data[0] = theTargetBuffer;
        myDestPict.data[1] = theTargetBuffer + 1;
        myDestPict.data[2] = theTargetBuffer + 2;

        myDestPict.linesize[0] = myLineSizeBytes;
        myDestPict.linesize[1] = myLineSizeBytes;
        myDestPict.linesize[2] = myLineSizeBytes;

        // convert to RGB
        int myDestFmt;
        if (_myContext->getPixelFormat() == PIX_FMT_RGB24) {
            myDestFmt = PIX_FMT_RGB24;
        } else {
            myDestFmt = PIX_FMT_BGR24;
        }

        img_convert(&myDestPict, myDestFmt, (AVPicture*)theFrame, _myContext->getPixelFormat(),
                    _myContext->getWidth(), _myContext->getHeight());
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
}
