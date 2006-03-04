//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_VideoFrame_h_
#define _ac_y60_VideoFrame_h_

#include <asl/Ptr.h>
#include <asl/Block.h>

namespace y60 {

    class VideoFrame {
        public:
            VideoFrame::VideoFrame(unsigned theWidth, unsigned theHeight, unsigned theBPP) {
                _myBuffer = asl::Ptr<asl::Block>(new asl::Block( theWidth * theHeight * theBPP));
            }

            void setTimestamp(double theTimestamp) {
                _myTimestamp = theTimestamp;
            }

            double getTimestamp() const {
                return _myTimestamp;
            }

            asl::Ptr<asl::Block> getData() {
                return _myBuffer;
            }

        private:
            double _myTimestamp;
            asl::Ptr<asl::Block> _myBuffer;
    };
    typedef asl::Ptr<VideoFrame> VideoFramePtr;
}

#endif
