//=============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: FFMpegAudioReader.h,v $
//     $Author: christian $
//   $Revision: 1.18 $
//       $Date: 2005/04/08 18:20:13 $
//
//  FFMpeg audio reader.
//
//=============================================================================

#ifndef INCL_FFMPEGAUDIOREADER
#define INCL_FFMPEGAUDIOREADER

#include "BufferedSource.h"

#include <asl/Exception.h>
#include <string>

#define EMULATE_INTTYPES
#include <ffmpeg/avformat.h>
#include <ffmpeg/avcodec.h>

namespace AudioBase {

    DEFINE_EXCEPTION(FFMpegAudioReaderException, asl::Exception);

    class FFMpegAudioReader: public BufferedSource {
    public:
        FFMpegAudioReader(const std::string & theFilename, double theVolume = 1.0, unsigned theLoopCount = 1);
        virtual ~FFMpegAudioReader ();

        void setSeekOffset(double theSeekOffset);

        virtual void pause();
        virtual void process();

    protected:
        void load(const std::string & theFilename);

    private:    

        unsigned          _myLoopCount;

        AVFormatContext * _myFormatContext;
        AVCodecContext *  _myCodecContext;
        int               _myAStreamIndex;
        AVStream *        _myAStream;

        unsigned char *   _mySamples;
        int64_t           _myLastTimestamp;

        double            _mySeekOffset;
        double            _mySeekFadeEndTime;
        double            _myLastVolume;
    };
}
#endif

