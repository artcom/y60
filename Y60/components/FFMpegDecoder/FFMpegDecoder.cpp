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
//    $RCSfile: FFMpegDecoder.cpp,v $
//     $Author: ulrich $
//   $Revision: 1.6 $
//       $Date: 2005/04/01 17:03:27 $
//
//  ffmpeg movie decoder.
//
//=============================================================================

/*
    This decoder tries to combine the advantages of FFMpegDecoder1 and FFMpegDecoder2.    
    Basically that is support for audio combined with seeking support.

    Seeking includes the following sub-problems:
        - Pause, resume and stop                    (ok)
        - Jumping to frames                         (ok)
        - Different playspeeds                      (ok)
        - Playing backwards                         (ok)        
        - Precise looping                           (ok)

    Audiosupport includes the following sub-problems:
        - Lipsync between audio and video           (ok)
        - Compatible to all the seeking features above (partial)
        - Support for videos without audio          (ok)
    
    Other things to be accomplished are:
        - Jitterless playback                       (ok without audio)
        - Try to avoid multi-threading              (ok)
        - Intelligent caching                       (ok)
        - Robustness against strange mpeg-files     (partial)
             - Invalid or wrong duration            (ok)
             - First/Lastframe with wrong timestamp (no)

    The main problems to solve are:
        - Integrate the new audio library.
        - Sometimes the AV-Sync is off after seeking. This should be solved, when the new audio-
          library is integrated.
        - With audio there is some jitter in the frame-rate, which probably only can be solved with a 
          dedicated decoding task.
*/

#include "FFMpegDecoder.h"
#include "FFMpegPixelEncoding.h"
#include "DecoderContext.h"

#include <asl/Logger.h>
#include <asl/file_functions.h>
#include <asl/Pump.h>

#include <iostream>
#include <float.h>

#define DB(x) //x

using namespace std;
using namespace asl;

extern "C"
EXPORT asl::PlugInBase * y60FFMpegDecoder_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::FFMpegDecoder(myDLHandle);
}

namespace y60 {

    asl::Ptr<MovieDecoderBase> FFMpegDecoder::instance() const {
        return asl::Ptr<MovieDecoderBase>(new FFMpegDecoder(getDLHandle()));
    }

    FFMpegDecoder::FFMpegDecoder(asl::DLHandle theDLHandle) :
        PlugInBase(theDLHandle)
    {}

    FFMpegDecoder::~FFMpegDecoder() {
    }

    std::string
    FFMpegDecoder::canDecode(const std::string & theUrl, asl::ReadableStream * theStream) {
        string myExtension = asl::toLowerCase(asl::getExtension(theUrl));
        if (myExtension == "mpg" || myExtension == "m2v" || myExtension == "mov" || 
            myExtension == "avi" || myExtension == "mpeg") 
        {
            return MIME_TYPE_MPG;
        } else {
            return "";
        }
    }

    double 
    FFMpegDecoder::getMovieTime(double theSystemTime) {
        if (_myAudioSink) {
            return _myAudioSink->getCurrentTime() +
                getMovie()->get<AVDelayTag>();
        } else {
            return MovieDecoderBase::getMovieTime(theSystemTime);
        }
    }

    void 
    FFMpegDecoder::resumeMovie(double theStartTime) {
        MovieDecoderBase::resumeMovie(theStartTime);        
        _myFrameConveyor.preload(theStartTime);
              
        if (_myAudioSink) {            
            _myAudioSink->play();
        }        
    }

    void 
    FFMpegDecoder::startMovie(double theStartTime) {
        MovieDecoderBase::startMovie(theStartTime);
        _myFrameConveyor.preload(theStartTime);

        if (_myAudioSink) {
            _myAudioSink->play();
        }
    }

    void 
    FFMpegDecoder::stopMovie() {
        MovieDecoderBase::stopMovie();        
        if (_myAudioSink) {            
            _myAudioSink->stop();
        }        
    }

    void 
    FFMpegDecoder::pauseMovie() {
        MovieDecoderBase::pauseMovie();
        if (_myAudioSink) {
            _myAudioSink->pause(); 
        }        
    }

    void
    FFMpegDecoder::load(const std::string & theFilename) {        
        DecoderContextPtr myContext = DecoderContextPtr(new DecoderContext(theFilename));
        setupMovie(myContext);
        setupAudio(myContext);                    
        _myFrameConveyor.load(myContext, _myAudioSink);
    }

    void
    FFMpegDecoder::setupMovie(DecoderContextPtr theContext) {
        AVStream * myVideoStream = theContext->getVideoStream();
        if (myVideoStream) {
            Movie * myMovie = getMovie();
            myMovie->setPixelEncoding(getPixelEncoding(myVideoStream));            
            myMovie->set<ImageWidthTag>(myVideoStream->codec.width);
            myMovie->set<ImageHeightTag>(myVideoStream->codec.height);

            // Setup size and image matrix
            float myXResize = float(myVideoStream->codec.width) 
                    / asl::nextPowerOfTwo(myVideoStream->codec.width);
            float myYResize = float(myVideoStream->codec.height) 
                    / asl::nextPowerOfTwo(myVideoStream->codec.height);

            asl::Matrix4f myMatrix;
            myMatrix.makeScaling(asl::Vector3f(myXResize, myYResize, 1.0f));
            myMovie->set<ImageMatrixTag>(myMatrix); 
            double myFPS = theContext->getFrameRate();
            myMovie->set<FrameRateTag>(myFPS);

            // Durations are mostly wrong in mpeg files, so we just do not use them to avoid looping errors
            myMovie->set<FrameCountTag>(INT_MAX);

            AC_INFO << "url='" << getMovie()->get<ImageSourceTag>() << "' fps=" << myFPS << " framecount=" << getFrameCount();            
        } else {
            AC_INFO << "FFMpegDecoder::load " << getMovie()->get<ImageSourceTag>() << " no video stream found";
        }
    }

    void 
    FFMpegDecoder::setupAudio(DecoderContextPtr theContext) {
        AVStream * myAudioStream = theContext->getAudioStream();
        if (getMovie()->get<AudioTag>() && myAudioStream) {
            AVCodec * myCodec = avcodec_find_decoder(myAudioStream->codec.codec_id);
            if (!myCodec) {
                AC_WARNING << "Unable to find audio decoder: " << theContext->getFilename();
                 _myAudioSink = HWSampleSinkPtr(0);
                 return;
            }
            if (avcodec_open(&myAudioStream->codec, myCodec) < 0 ) {
                AC_WARNING << "Unable to find audio decoder: " << theContext->getFilename();
                _myAudioSink = HWSampleSinkPtr(0);
                return;
            }

            Pump & myAudioPump = Pump::get();
//            if (!myAudioController.isRunning()) {
//                myAudioController.init(asl::maximum
//                      ((unsigned)myAudioStream->codec.sample_rate, (unsigned) 44100));
//            }

            _myAudioSink = myAudioPump.createSampleSink(theContext->getFilename());
            // TODO: Sample rate conversion
            _myAudioSink->setVolume(getMovie()->get<VolumeTag>());
            AC_INFO << "Audio: channels=" << myAudioStream->codec.channels << ", samplerate=" 
                    << myAudioStream->codec.sample_rate;
        } else {
            _myAudioSink = HWSampleSinkPtr(0);
        }
    }

    double
    FFMpegDecoder::readFrame(double theTime, unsigned theFrame,
            dom::ResizeableRasterPtr theTargetRaster) {
        //cerr << "readFrame at : " << theTime << " system time " << asl::Time() << endl;
        if (theTime >= _myFrameConveyor.getEndOfFileTimestamp()) {
            setEOF(true);
            return theTime;
        }

        double myDecodedFrameTime = _myFrameConveyor.getFrame(theTime, theTargetRaster);
        if (_myAudioSink) {
            float myVolume = getMovie()->get<VolumeTag>();
            if (!asl::almostEqual(_myAudioSink->getVolume(), myVolume)) {
                _myAudioSink->setVolume(myVolume);
            } 
        }

        // In case we reached the end of file while decoding we can now correctly set the 
        // frame rate
        if (_myFrameConveyor.getEndOfFileTimestamp() != DBL_MAX) {
            getMovie()->set<FrameCountTag>((unsigned)(_myFrameConveyor.getEndOfFileTimestamp() 
                        * getMovie()->get<FrameRateTag>()));
        }
        return myDecodedFrameTime;
    }
}

