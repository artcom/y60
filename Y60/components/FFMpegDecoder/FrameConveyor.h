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

#include "AudioFrame.h"

#include <asl/Ptr.h>
#include <dom/Value.h>
#include <map>

namespace AudioBase {
    class BufferedSource;
}

struct AVFrame;

namespace y60 {

    class VideoFrame;
    class DecoderContext;

    DEFINE_EXCEPTION(FrameConveyorException, asl::Exception);

    class FrameConveyor {
        public:
            FrameConveyor();
            ~FrameConveyor();

            void load(asl::Ptr<DecoderContext> theContext, AudioBase::BufferedSource * theAudioBufferedSource); 

            bool getFrame(double theTimestamp, dom::ResizeableRasterPtr theTargetRaster);

        private:
            /// Decode frame at theTimestamp into theTargetRaster. Returns true if EOF was met.
            bool decodeFrame(double theTimestamp);

            /// Convert frame vom YUV to RGB
            void convertFrame(AVFrame * theFrame, unsigned char * theTargetBuffer);

            /// Copy raster to raster
            void copyFrame(unsigned char * theSourceBuffer, dom::ResizeableRasterPtr theTargetRaster);

            void fillCache(double theStartTimestamp);
            void updateCache(double theTimestamp);

            void setupAudio();

            asl::Ptr<DecoderContext>    _myContext;
            AVFrame *                   _myVideoFrame;
            AudioFrame                  _myAudioFrame;
            AudioBase::BufferedSource * _myAudioBufferedSource;

            typedef std::map<double, asl::Ptr<VideoFrame> > FrameCache;
            FrameCache _myFrameCache;
    };
}

#endif
