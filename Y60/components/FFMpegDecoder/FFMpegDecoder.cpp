//=============================================================================
// Copyright (C) 2004,2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
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

#include <y60/Movie.h>
#include <asl/Logger.h>
#include <asl/file_functions.h>
#include <asl/Pump.h>

#include <iostream>
#include <float.h>

#define DB(x) // x

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

    FFMpegDecoder::~FFMpegDecoder() {}

    std::string
    FFMpegDecoder::canDecode(const std::string & theUrl, asl::ReadableStream * theStream) {
        string myExtension = asl::toLowerCase(asl::getExtension(theUrl));
        if (myExtension == "mpg" || myExtension == "m2v" || myExtension == "mov" ||
            myExtension == "avi" || myExtension == "mpeg")
        {
            AC_DEBUG << "FFMpegDecoder::canDecode " << myExtension;
            return MIME_TYPE_MPG;
        } else {
            return "";
        }
    }

    double
    FFMpegDecoder::getMovieTime(double theSystemTime) {
        if (_myFrameConveyor.hasAudio()) {
            return _myFrameConveyor.getAudioTime() + getMovie()->get<AVDelayTag>();
        } else {
            return MovieDecoderBase::getMovieTime(theSystemTime);
        }
    }

    void
    FFMpegDecoder::resumeMovie(double theStartTime) {
        AC_DEBUG << "FFMpegDecoder::resumeMovie() at time: " << theStartTime;
        MovieDecoderBase::resumeMovie(theStartTime);
        _myFrameConveyor.playAudio();
    }

    void
    FFMpegDecoder::startMovie(double theStartTime) {
        AC_DEBUG << "FFMpegDecoder::startMovie() at time " << theStartTime;
        MovieDecoderBase::startMovie(theStartTime);

        //XXX should preload do nothing when no audio ?
        _myFrameConveyor.preload(theStartTime);
        _myFrameConveyor.playAudio();
    }

    void
    FFMpegDecoder::stopMovie() {
        AC_DEBUG << "FFMpegDecoder::stopMovie()";
        MovieDecoderBase::stopMovie();
        _myFrameConveyor.stopAudio();
    }

    void
    FFMpegDecoder::pauseMovie() {
        AC_DEBUG << "FFMpegDecoder::pauseMovie()";
        MovieDecoderBase::pauseMovie();
        _myFrameConveyor.pauseAudio();
    }

    void
    FFMpegDecoder::load(const std::string & theFilename) {
        AC_DEBUG << "FFMpegDecoder::load " << theFilename;
        DecoderContextPtr myContext = DecoderContextPtr(new DecoderContext(theFilename));
        setupMovie(myContext);
        _myFrameConveyor.setContext(myContext);
        _myFrameConveyor.setupAudio(getMovie()->get<AudioTag>());
    }

    void
    FFMpegDecoder::setupMovie(DecoderContextPtr theContext) {
        AVStream * myVideoStream = theContext->getVideoStream();
        if (myVideoStream) {
            Movie * myMovie = getMovie();

#if LIBAVCODEC_BUILD >= 0x5100
            int myWidth = myVideoStream->codec->width;
            int myHeight = myVideoStream->codec->height;
#else
            int myWidth = myVideoStream->codec.width;
            int myHeight = myVideoStream->codec.height;
#endif
            // Setup image matrix
            float myXResize = float(myWidth) / asl::nextPowerOfTwo(myWidth);
            float myYResize = float(myHeight) / asl::nextPowerOfTwo(myHeight);

            /*asl::Matrix4f myMatrix;
            myMatrix.makeScaling(asl::Vector3f(myXResize, myYResize, 1.0f));
            myMovie->set<ImageMatrixTag>(myMatrix);*/

            myMovie->set<FrameRateTag>(theContext->getFrameRate());

            // Durations are mostly wrong in mpeg files, so we just do not use them to avoid looping errors
            myMovie->set<FrameCountTag>(INT_MAX);

            myMovie->createRaster(myWidth, myHeight, 1, y60::BGR);

            // Tell the context which encoding to use
            theContext->setTargetPixelEncoding(getPixelEncoding(myVideoStream));

            AC_INFO << "url: " << getMovie()->get<ImageSourceTag>()
                    << " fps: " << theContext->getFrameRate()
                    << " framecount: " << getFrameCount()
                    << " stream encoding: " << getPixelEncodingInfo(myVideoStream)
                    << " destination encoding: " << getMovie()->get<RasterPixelFormatTag>();
        } else {
            AC_WARNING << "FFMpegDecoder::load " << getMovie()->get<ImageSourceTag>() << " no video stream found";
        }
    }

    double
    FFMpegDecoder::readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster) {
        AC_TRACE << "FFMpegDecoder::readFrame() at: " << theTime << " system time: " << asl::Time();
        if (theTime >= _myFrameConveyor.getEndOfFileTimestamp()) {
            setEOF(true);
            return theTime;
        }

        double myDecodedFrameTime = _myFrameConveyor.getFrame(theTime, theTargetRaster);

        // The user can change the movie volue any time, so we need to poll it.
        _myFrameConveyor.setVolume(getMovie()->get<VolumeTag>());

        // In case we reached the end of file while decoding we can now correctly set the
        // frame count
        if (_myFrameConveyor.getEndOfFileTimestamp() != DBL_MAX) {
            getMovie()->set<FrameCountTag>((unsigned)(_myFrameConveyor.getEndOfFileTimestamp()
                        * getMovie()->get<FrameRateTag>()));
        }
        AC_TRACE << "readFrame returns decoded frame time: " << myDecodedFrameTime;
        return myDecodedFrameTime;
    }
}

