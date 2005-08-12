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
//   $RCSfile: VideoFrame.h,v $
//   $Author: ulrich $
//   $Revision: 1.4 $
//   $Date: 2005/04/01 10:12:21 $
//
//  ffmpeg movie decoder.
//
//=============================================================================

#ifndef _ac_y60_VideoFrame_h_
#define _ac_y60_VideoFrame_h_

#include <y60/PixelEncoding.h>
#include <asl/Ptr.h>
#include <dom/Value.h>

namespace y60 {

    class VideoFrame {
        public:
            VideoFrame::VideoFrame(unsigned theWidth, unsigned theHeight, PixelEncoding thePixelEncoding) {
                _myBuffer = dynamic_cast_Ptr<dom::ResizeableRaster>(createRasterValue(thePixelEncoding, theWidth, theHeight));
            }

            VideoFrame::~VideoFrame() {
            }

            void setTimestamp(long long theTimestamp) {
                _myTimestamp = theTimestamp;
            }

            long long getTimestamp() const {
                return _myTimestamp;
            }
            
            dom::ResizeableRasterPtr getRaster() {
                return _myBuffer;
            }

        private:
            long long _myTimestamp;
            dom::ResizeableRasterPtr _myBuffer;
    };
    typedef asl::Ptr<VideoFrame> VideoFramePtr;
}

#endif
