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

#include <audio/AudioController.h>
#include <asl/Logger.h>

#define DB(x) //x

using namespace std;
using namespace asl;

namespace y60 {

    FrameConveyor::FrameConveyor() {
    }

    FrameConveyor::~FrameConveyor() {
    }
    
    void
    FrameConveyor::setup(unsigned theWidth, unsigned theHeight, PixelEncoding thePixelEncoding) {
        _myFrameCache[0] = VideoFramePtr(new VideoFrame(theWidth, theHeight, thePixelEncoding));
    }

    /*
    void 
    FrameConveyor::initBufferedSource(unsigned theNumChannels, unsigned theSampleRate, unsigned theSampleBits) {
        AudioApp::AudioController & myAudioController = AudioApp::AudioController::get();
        std::string myURL = getMovie()->get<ImageSourceTag>();
        if (theSampleBits != 16) {
            AC_WARNING << "Movie '" << myURL << "' contains " << theSampleBits
                << " bit audio stream. Only 16 bit supported. Playing movie without sound.";
        } else {
            if (!myAudioController.isRunning()) {
                myAudioController.init(asl::maximum(theSampleRate, (unsigned) 44100));
            }

            std::string myId = myAudioController.createReader(myURL, "Mixer", theSampleRate, theNumChannels);
            _myAudioBufferedSource = dynamic_cast<AudioBase::BufferedSource *>(myAudioController.getFileReaderFromID(myId));
            _myAudioBufferedSource->setVolume(getMovie()->get<VolumeTag>());
            _myAudioBufferedSource->setSampleBits(theSampleBits);
            AC_INFO << "Audio: channels=" << theNumChannels << ", samplerate=" << theSampleRate << ", samplebits=" << theSampleBits;
        }

    }
*/

    dom::ResizeableRasterPtr
    FrameConveyor::getFrame(long long theTimestamp) {
        return _myFrameCache[0]->getRaster();
    }


}
