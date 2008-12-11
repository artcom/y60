/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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

#include <y60/video/Movie.h>
#include <asl/base/Logger.h>
#include <asl/base/file_functions.h>
#include <asl/audio/Pump.h>

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
    FFMpegDecoder::canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream) {
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
            //float myXResize = float(myWidth) / asl::nextPowerOfTwo(myWidth);
            //float myYResize = float(myHeight) / asl::nextPowerOfTwo(myHeight);

            //asl::Matrix4f myMatrix;
            //myMatrix.makeScaling(asl::Vector3f(myXResize, myYResize, 1.0f));
            //myMovie->set<ImageMatrixTag>(myMatrix);

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

