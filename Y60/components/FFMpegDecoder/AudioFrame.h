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
//   $RCSfile: AudioFrame.h,v $
//   $Author: ulrich $
//   $Revision: 1.4 $
//   $Date: 2005/04/01 10:12:21 $
//
//  ffmpeg movie decoder.
//
//=============================================================================

#ifndef _ac_y60_AudioFrame_h_
#define _ac_y60_AudioFrame_h_

#include <asl/Ptr.h>

namespace y60 {

    class AudioFrame {
        friend class DecoderContext;

        public:
            AudioFrame::AudioFrame(unsigned theFrameSize) : _myTimestamp(0), _mySampleSize(0) {
                _myBuffer = new unsigned char[theFrameSize];
            }

            AudioFrame::~AudioFrame() {
                if (_myBuffer) {
                    delete[] _myBuffer;
                    _myBuffer = 0;
                }
            }

            double getTimestamp() const {
                return _myTimestamp;
            }
            
            unsigned char * getSamples() {
                return _myBuffer;
            }

            unsigned getSampleSize() const {
                return _mySampleSize;
            }

        private:
            double          _myTimestamp;
            unsigned char * _myBuffer;
            unsigned        _mySampleSize;
    };
    typedef asl::Ptr<AudioFrame> AudioFramePtr;
}

#endif
