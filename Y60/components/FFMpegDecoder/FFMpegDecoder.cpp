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

#include <audio/AudioController.h>
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
        PlugInBase(theDLHandle),
        _myVideoFrame(0),
        _myAudioFrame(AVCODEC_MAX_AUDIO_FRAME_SIZE)
    {}

    FFMpegDecoder::~FFMpegDecoder() {
        if (_myVideoFrame) {
            av_free(_myVideoFrame);
            _myVideoFrame = 0;
        }
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

    bool
    FFMpegDecoder::hasVideo() const {
        return _myContext.hasVideo(); 
    }

    bool
    FFMpegDecoder::hasAudio() const {
        return _myContext.hasAudio();
    }

    void
    FFMpegDecoder::load(const std::string & theFilename) {
        _myContext.load(theFilename);
        setupVideo(theFilename);
        setupAudio(theFilename);
        _myContext.runFrameAnalyser();
    }

    void
    FFMpegDecoder::setupVideo(const std::string & theFilename) {
        AVStream * myVideoStream = _myContext.getVideoStream();
        if (myVideoStream) {
            // open codec
            AVCodec * myCodec = avcodec_find_decoder(myVideoStream->codec.codec_id);
            if (!myCodec) {
                throw FFMpegDecoderException(std::string("Unable to find decoder: ") + theFilename, PLUS_FILE_LINE);
            }
            if (avcodec_open(&myVideoStream->codec, myCodec) < 0) {
                throw FFMpegDecoderException(std::string("Unable to open codec: ") + theFilename, PLUS_FILE_LINE);
            }

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

            // Hack to correct wrong frame rates that seem to be generated
            // by some codecs
            if (myVideoStream->codec.frame_rate > 1000 && myVideoStream->codec.frame_rate_base == 1) {
                myVideoStream->codec.frame_rate_base = 1000;
            }
            float myFPS = myVideoStream->codec.frame_rate / (float) myVideoStream->codec.frame_rate_base;
            if (myFPS > 1000.0f) {
                myFPS /= 1000.0f;
            }
            myMovie->set<FrameRateTag>(myFPS);

            if (myVideoStream->duration == AV_NOPTS_VALUE ||
                int(myFPS * (myVideoStream->duration / (double) AV_TIME_BASE)) <= 0)
            {
                AC_WARNING << "url='" << theFilename << "' contains no valid duration";
                myMovie->set<FrameCountTag>(INT_MAX);
            } else {
                myMovie->set<FrameCountTag>(int(myFPS * (myVideoStream->duration / (float) AV_TIME_BASE)));
            }

            // allocate frame for YUV data
            _myVideoFrame = avcodec_alloc_frame();
            _myFrameConveyor.setup(myMovie->get<ImageWidthTag>(), myMovie->get<ImageHeightTag>(), myMovie->getPixelEncoding());
            AC_INFO << "url='" << theFilename << "' fps=" << myFPS << " framecount=" << getFrameCount();            
        } else {
            AC_INFO << "FFMpegDecoder::load " << theFilename << " no video stream found";
        }
    }

    void 
    FFMpegDecoder::initBufferedSource(unsigned theNumChannels, unsigned theSampleRate, unsigned theSampleBits) {
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

    void
    FFMpegDecoder::setupAudio(const std::string & theFilename) {
        AVStream * myAudioStream = _myContext.getAudioStream();
        if (myAudioStream && getAudioFlag()) {
            AVCodec * myCodec = avcodec_find_decoder(myAudioStream->codec.codec_id);
            if (!myCodec) {
                throw FFMpegDecoderException(std::string("Unable to find audio decoder: ") + theFilename, PLUS_FILE_LINE);
            }
            if (avcodec_open(&myAudioStream->codec, myCodec) < 0 ) {
                throw FFMpegDecoderException(std::string("Unable to open audio codec: ") + theFilename, PLUS_FILE_LINE);
            }

            initBufferedSource(myAudioStream->codec.channels, myAudioStream->codec.sample_rate, 16);            
        } else {
            AC_INFO << "FFMpegDecoder::load " << theFilename << " no audio stream found or audio disabled";
        }
    }

    double
    FFMpegDecoder::readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster) {
        int64_t myFrameTimestamp = (int64_t)asl::round(theTime * AV_TIME_BASE);
        AC_TRACE << "readFrame: time=" << theTime << " timestamp=" << myFrameTimestamp;
        if (!decodeFrame(myFrameTimestamp, theTargetRaster)) {
            AC_PRINT << "EOF reached!";
            setEOF(true);
            _myAudioBufferedSource->setRunning(false);
        }
        return theTime;
    }

    bool
    FFMpegDecoder::decodeFrame(long long & theTimestamp, dom::ResizeableRasterPtr theTargetRaster) {
        bool myValidFilePosition = true;
 
        DecoderContext::FrameType myFrameType;
        if (getAudioFlag()) {
            myFrameType = _myContext.decode(_myVideoFrame, &_myAudioFrame);
        } else {
            myFrameType = _myContext.decode(_myVideoFrame, 0);
        }

        switch (myFrameType) {
            case DecoderContext::FrameTypeVideo:
            {
                dom::ResizeableRasterPtr myRaster = _myFrameConveyor.getFrame(theTimestamp);        
                convertFrame(_myVideoFrame, myRaster);
                copyFrame(myRaster, theTargetRaster);
                break;
            }
            case DecoderContext::FrameTypeAudio:
                addAudioPacket();
                break;
            case DecoderContext::FrameTypeEOF:
                //cleanup();
                break;
            default:
                throw FFMpegDecoderException(std::string("Unknown frame type: ") + as_string(myFrameType), PLUS_FILE_LINE);
        }

        return (myFrameType != DecoderContext::FrameTypeEOF);
    }

    void
    FFMpegDecoder::addAudioPacket() {
        _myAudioBufferedSource->addBuffer(_myAudioFrame.getTimestamp(), _myAudioFrame.getSamples(), 
            _myAudioFrame.getSampleSize());


        if (!_myAudioBufferedSource->isRunning()) {
            _myAudioBufferedSource->setRunning(true);
        }
        
        float myVolume = getMovie()->get<VolumeTag>();
        if (!asl::almostEqual(_myAudioBufferedSource->getVolume(), myVolume)) {
            _myAudioBufferedSource->setVolume(myVolume);
        } 
    }

    void FFMpegDecoder::convertFrame(AVFrame * theFrame, dom::ResizeableRasterPtr theTargetRaster) {
        if (!theFrame) {
            AC_ERROR << "FFMpegDecoder::decodeVideoFrame invalid AVFrame";
            return;
        }

        unsigned int myLineSizeBytes = getBytesRequired(getFrameWidth(), getPixelFormat());
        unsigned char * myBuffer = theTargetRaster->pixels().begin();

        AVPicture myDestPict;
        myDestPict.data[0] = myBuffer;
        myDestPict.data[1] = myBuffer + 1;
        myDestPict.data[2] = myBuffer + 2;

        myDestPict.linesize[0] = myLineSizeBytes;
        myDestPict.linesize[1] = myLineSizeBytes;
        myDestPict.linesize[2] = myLineSizeBytes;

        // convert to RGB
        int myDestFmt;
        if (getPixelFormat() == y60::RGB) {
            myDestFmt = PIX_FMT_RGB24;
        } else {
            myDestFmt = PIX_FMT_BGR24;
        }

        img_convert(&myDestPict, myDestFmt, (AVPicture*)theFrame, _myContext.getPixelFormat(),
                    getMovie()->get<ImageWidthTag>(), getMovie()->get<ImageHeightTag>());
    }

    void FFMpegDecoder::copyFrame(dom::ResizeableRasterPtr theVideoFrame, dom::ResizeableRasterPtr theTargetRaster) {
        // TODO: Pass target raster owning dom node instead of target raster and replace raster instead
        // of copy raster
        theTargetRaster->resize(getFrameWidth(), getFrameHeight());
        memcpy(theTargetRaster->pixels().begin(), theVideoFrame->pixels().begin(), theTargetRaster->pixels().size());
    }
}
