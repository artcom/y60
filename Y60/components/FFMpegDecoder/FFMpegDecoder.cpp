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

#include "FFMpegDecoder.h"
#include "FFMpegPixelEncoding.h"
#include "DecoderContext.h"

#include <audio/AudioController.h>
#include <audio/BufferedSource.h>
#include <asl/Logger.h>
#include <asl/file_functions.h>
#include <iostream>

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

    void 
    FFMpegDecoder::startMovie(double theStartTime) {
        MovieDecoderBase::startMovie(theStartTime);
        if (_myAudioBufferedSource) {
            // if start from pause:
            _myAudioBufferedSource->clear();
        }

        _myFrameConveyor.preload(theStartTime);
        if (_myAudioBufferedSource) {
            _myAudioBufferedSource->setRunning(true);
        }
    }

    void 
    FFMpegDecoder::stopMovie() {
        MovieDecoderBase::stopMovie();
        if (_myAudioBufferedSource) {
            _myAudioBufferedSource->setRunning(false);
        }        
    }

    void 
    FFMpegDecoder::pauseMovie() {
        MovieDecoderBase::pauseMovie();
        if (_myAudioBufferedSource) {
            _myAudioBufferedSource->setRunning(false);            
        }        
    }

    void
    FFMpegDecoder::load(const std::string & theFilename) {        
        DecoderContextPtr myContext = DecoderContextPtr(new DecoderContext(theFilename));
        setupMovie(myContext);
        setupAudio(myContext);                    
        _myFrameConveyor.load(myContext, _myAudioBufferedSource);
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
            float myXResize = float(myVideoStream->codec.width) / asl::nextPowerOfTwo(myVideoStream->codec.width);
            float myYResize = float(myVideoStream->codec.height) / asl::nextPowerOfTwo(myVideoStream->codec.height);

            asl::Matrix4f myMatrix;
            myMatrix.makeScaling(asl::Vector3f(myXResize, myYResize, 1.0f));
            myMovie->set<ImageMatrixTag>(myMatrix); 
            double myFPS = theContext->getFrameRate();
            myMovie->set<FrameRateTag>(myFPS);

            if (myVideoStream->duration == AV_NOPTS_VALUE ||
                int(myFPS * (myVideoStream->duration / (double) AV_TIME_BASE)) <= 0)
            {
                AC_WARNING << "url='" << getMovie()->get<ImageSourceTag>() << "' contains no valid duration";
                myMovie->set<FrameCountTag>(INT_MAX);
            } else {
                myMovie->set<FrameCountTag>(int(myFPS * (myVideoStream->duration / (float) AV_TIME_BASE)));
            }

            AC_INFO << "url='" << getMovie()->get<ImageSourceTag>() << "' fps=" << myFPS << " framecount=" << getFrameCount();            
        } else {
            AC_INFO << "FFMpegDecoder::load " << getMovie()->get<ImageSourceTag>() << " no video stream found";
        }
    }

    void 
    FFMpegDecoder::setupAudio(DecoderContextPtr theContext) {
        AVStream * myAudioStream = theContext->getAudioStream();
        if (getMovie()->get<AudioTag>() && myAudioStream) {
            AudioApp::AudioController & myAudioController = AudioApp::AudioController::get();
            std::string myURL = theContext->getFilename();
            if (!myAudioController.isRunning()) {
                myAudioController.init(asl::maximum((unsigned)myAudioStream->codec.sample_rate, (unsigned) 44100));
            }

            std::string myId = myAudioController.createReader(myURL, "Mixer", myAudioStream->codec.sample_rate, myAudioStream->codec.channels);
            _myAudioBufferedSource = dynamic_cast<AudioBase::BufferedSource *>(myAudioController.getFileReaderFromID(myId));
            _myAudioBufferedSource->setVolume(getMovie()->get<VolumeTag>());
            _myAudioBufferedSource->setSampleBits(16);
            AC_INFO << "Audio: channels=" << myAudioStream->codec.channels << ", samplerate=" 
                    << myAudioStream->codec.sample_rate << ", samplebits=" << 16;
        } else {
            _myAudioBufferedSource = 0;
        }
    }

    double
    FFMpegDecoder::readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster) {       
    //cerr << "getFrame ast : " << theTime << "the system time " << asl::Time() << endl;
        double myDecodedFrameTime = _myFrameConveyor.getFrame(theTime, theTargetRaster);
        if (myDecodedFrameTime < 0) {
            AC_PRINT << "EOF reached!";
            setEOF(true);
        } else {
            if (_myAudioBufferedSource) {
                float myVolume = getMovie()->get<VolumeTag>();
                if (!asl::almostEqual(_myAudioBufferedSource->getVolume(), myVolume)) {
                    _myAudioBufferedSource->setVolume(myVolume);
                } 
            }
        }
        return myDecodedFrameTime;
    }
}
