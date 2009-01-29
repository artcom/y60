/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ac_y60_FrameConveyor_h_
#define _ac_y60_FrameConveyor_h_

#include "y60_ffmpegdecoder_settings.h"

#include "AudioFrame.h"

#include <asl/audio/HWSampleSink.h>
#include <asl/base/Ptr.h>
#include <asl/dom/Value.h>

#include <map>

#ifdef OSX
    extern "C" {
#       include <libavformat/avformat.h>
    }
#   undef AV_NOPTS_VALUE
#   define AV_NOPTS_VALUE 0x8000000000000000LL
#else
#   if defined(_MSC_VER)
#       pragma warning(push,1)
#   endif
    extern "C" {
#       include <libavformat/avformat.h>
    }
#   if defined(_MSC_VER)
#       pragma warning(pop)
#   endif
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
            double getAudioTime() {
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
