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
//=============================================================================

#ifndef _ac_y60_AudioFrame_h_
#define _ac_y60_AudioFrame_h_

#include <asl/Ptr.h>

namespace y60 {

    class AudioFrame {
        public:
            AudioFrame::AudioFrame(unsigned theFrameSize) : _myTimestamp(0), _mySizeInBytes(0) {
                _myBuffer = new unsigned char[theFrameSize];
            }

            AudioFrame::~AudioFrame() {
                if (_myBuffer) {
                    delete[] _myBuffer;
                    _myBuffer = 0;
                }
            }

            void setTimestamp(double theTimestamp) {
                _myTimestamp = theTimestamp;
            }
            
            double getTimestamp() const {
                return _myTimestamp;
            }
            
            unsigned char * getSamples() {
                return _myBuffer;
            }

            void setSizeInBytes(unsigned theSizeInBytes) {
                _mySizeInBytes = theSizeInBytes;
            }

            unsigned getSizeInBytes() const {
                return _mySizeInBytes;
            }

        private:
            double          _myTimestamp;
            unsigned char * _myBuffer;
            unsigned        _mySizeInBytes;
    };
    typedef asl::Ptr<AudioFrame> AudioFramePtr;
}

#endif
