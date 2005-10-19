//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: FrameConveyor.h,v $
//   $Author: ulrich $
//   $Revision: 1.4 $
//   $Date: 2005/04/01 10:12:21 $
//
//  ffmpeg movie decoder.
//
//=============================================================================

#ifndef _ac_y60_FrameConveyor_h_
#define _ac_y60_FrameConveyor_h_

#include "AudioPacket.h"

#include <asl/HWSampleSink.h>

#include <asl/Ptr.h>
#include <dom/Value.h>
#include <map>

struct AVFrame;

namespace y60 {

    class VideoFrame;
    class DecoderContext;

    DEFINE_EXCEPTION(FrameConveyorException, asl::Exception);

    class FrameConveyor {
        public:
            FrameConveyor();
            ~FrameConveyor();

            /// Sets up video and audio for decoding
            void load(asl::Ptr<DecoderContext> theContext, asl::HWSampleSinkPtr theAudioSink); 

            /// Copies the frame with theTimestamp from the cache into theTargetRaster
            double getFrame(double theTimestamp, dom::ResizeableRasterPtr theTargetRaster);

            /// Preloads enought frames into the cache to allow audio-playback without underruns
            void preload(double theInitialTimestamp);

            double getEndOfFileTimestamp() const;

        private:
            /// Clears the frame and audio cache
            void clear();

            /** Decode frame at theTimestamp into theTargetRaster. 
             *  Returns the timestamp of the decoded frame or -1 if EOF was met.
             **/
            void decodeFrame(double & theCurrentTime, bool & theEndOfFileFlag);

            /// Convert frame vom YUV to RGB
            void convertFrame(AVFrame * theFrame, unsigned char * theTargetBuffer);

            /// Copy raster to raster
            void copyFrame(unsigned char * theSourceBuffer, 
                    dom::ResizeableRasterPtr theTargetRaster);

            /// Fills the cache from start to end time
            void fillCache(double theStartTime, double theEndTime);            

            /// Deletes all frames from cache that are before start or after end time
            void trimCache(double theTargetStart, double theTargetEnd);

            /// Rearranges the cache relative to theTimestamp
            void updateCache(double theTimestamp);

            /// Initial setup of audio buffered source
            void setupAudio();

            /// Visualizes the current fill level of the video cache
            void printCacheInfo(double theTargetStart, double theTargetEnd);

            asl::Ptr<DecoderContext>    _myContext;
            AVFrame *                   _myVideoFrame;
            AudioPacket                 _myAudioPacket;
            asl::HWSampleSinkPtr        _myAudioSink;

            double                      _myCacheSizeInSecs;

            typedef std::map<double, asl::Ptr<VideoFrame> > FrameCache;
            typedef FrameCache::iterator FrameCacheIterator;
            FrameCache _myFrameCache;
    };
}

#endif
