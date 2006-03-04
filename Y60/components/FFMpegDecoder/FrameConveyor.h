//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_FrameConveyor_h_
#define _ac_y60_FrameConveyor_h_

#include "AudioFrame.h"

#include <asl/HWSampleSink.h>
#include <asl/Ptr.h>
#include <dom/Value.h>

#include <map>

#ifdef WIN32
#pragma warning( disable : 4244 ) // Disable ffmpeg warning
#define EMULATE_INTTYPES
#endif
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
#ifdef WIN32
#pragma warning( default : 4244 ) // Renable ffmpeg warning
#endif

struct AVFrame;

namespace y60 {

    class VideoFrame;
    class DecoderContext;

    DEFINE_EXCEPTION(FrameConveyorException, asl::Exception);

    class FrameConveyor {
        public:
            FrameConveyor();
            ~FrameConveyor();

            void setContext(asl::Ptr<DecoderContext> theContext); 

            /// Copies the frame with theTimestamp from the cache into theTargetRaster
            double getFrame(double theTimestamp, dom::ResizeableRasterPtr theTargetRaster);

            /// Preloads enought frames into the cache to allow audio-playback without underruns
            void preload(double theInitialTimestamp);

            double getEndOfFileTimestamp() const;


            void setupAudio(bool theUseAudioFlag);
            void playAudio();
            void stopAudio();
            void pauseAudio();
            void setVolume(double theVolume);
            bool hasAudio() const {
                return _myAudioSink != 0;
            }
            double getAudioTime() const {
                return _myAudioSink ? double(_myAudioSink->getCurrentTime()) : 0;
            }

        private:
            /// Clears the frame and audio cache
            void clear();

            /** Decode frame at theTimestamp into theTargetRaster. 
             *  Returns the timestamp of the decoded frame or -1 if EOF was met.
             **/
            void decodeFrame(double & theLastDecodedVideoTime, bool & theEndOfFileFlag);

            /// Copy raster to raster
            void copyFrame(unsigned char * theSourceBuffer, 
                    dom::ResizeableRasterPtr theTargetRaster);

            /// Fills the cache from start to end time
            void fillCache(double theStartTime, double theEndTime);            

            /// Deletes all frames from cache that are before start or after end time
            void trimCache(double theTargetStart, double theTargetEnd);

            /// Rearranges the cache relative to theTimestamp
            void updateCache(double theTimestamp);

            /// Visualizes the current fill level of the video cache
            void printCacheInfo(double theTargetStart, double theTargetEnd);

            /// Resample Audio acording to theInputSampleRate vs. Pump::getNativeSampleRate
            void initResample(int theNumInputChannels, int theInputSampleRate);

            asl::Ptr<DecoderContext>    _myContext;
            AVFrame *                   _myVideoFrame;
            AudioFrame                  _myAudioFrame;
            asl::HWSampleSinkPtr        _myAudioSink;

            double                      _myCacheSizeInSecs;

            typedef std::map<double, asl::Ptr<VideoFrame> > FrameCache;
            typedef FrameCache::iterator FrameCacheIterator;
            FrameCache _myFrameCache;
            ReSampleContext * _myResampleContext;

            static asl::Block _myResampledSamples;
    };
}

#endif
