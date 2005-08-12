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

#include <y60/PixelEncoding.h>
#include <asl/Ptr.h>
#include <dom/Value.h>
#include <map>

namespace y60 {

    class VideoFrame;

    DEFINE_EXCEPTION(FrameConveyorException, asl::Exception);

    class FrameConveyor {
        public:
            FrameConveyor();
            ~FrameConveyor();

            void setup(unsigned theWidth, unsigned theHeight, PixelEncoding thePixelEncoding);

            dom::ResizeableRasterPtr getFrame(long long theTimestamp);

        private:
            std::map<long long, asl::Ptr<VideoFrame> > _myFrameCache;
    };
}

#endif
