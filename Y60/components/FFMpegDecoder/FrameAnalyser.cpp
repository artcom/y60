//=============================================================================
// Copyright (C) 2004,2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "FrameAnalyser.h"

#ifdef WIN32
#pragma warning( disable : 4244 ) // Disable ffmpeg warning
#define EMULATE_INTTYPES
#endif
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
#ifdef WIN32
#pragma warning( default : 4244 ) // Renable ffmpeg warning
#endif

#include <asl/string_functions.h>
#include <asl/numeric_functions.h>
#include <asl/os_functions.h>
#include <asl/Dashboard.h>

#define DB(x) //x

using namespace std;
using namespace asl;

namespace y60 {

    static const double ourTimeBase  = double(AV_TIME_BASE) / 1000;        
    static unsigned ourFrameCounter  = 0;
    static unsigned ourPacketCounter = 0;       
    
    FrameAnalyser::FrameAnalyser(AVFormatContext * theFormatContext, AVStream * theVStream, int theVStreamIndex) :
        _myFormatContext(theFormatContext),
        _myVStream(theVStream),
        _myVStreamIndex(theVStreamIndex)        
    {}

    void
    FrameAnalyser::printPacketInfo(AVPacket & thePacket) {         
        cerr << "--- Packet " << ourPacketCounter++ << " ---" << endl;
        cerr << "    duration:        " << thePacket.duration / ourTimeBase << endl;
        cerr << "    size:            " << thePacket.size << endl;
        cerr << "    pts:             ";
        if (thePacket.pts != AV_NOPTS_VALUE) {            
             cerr << thePacket.pts / ourTimeBase << endl;
        } else {
            cerr << "N/V" << endl;
        }

        cerr << "    dts:             ";
        if (thePacket.dts != AV_NOPTS_VALUE) {            
            cerr << thePacket.dts / ourTimeBase << endl;
        } else {
            cerr << "N/V" << endl;
        }
    }

    void
    FrameAnalyser::printFrameInfo(AVFrame & theFrame) {
        cerr << "-+- Frame " << ourFrameCounter << " ---" << endl;
        cerr << "    keyframe:        " << theFrame.key_frame << endl;
        cerr << "    picturetype:     " << theFrame.pict_type;
        switch (theFrame.pict_type) {
            case FF_I_TYPE:
                cerr << " (I-Frame)" << endl;
                break;
            case FF_P_TYPE:
                cerr << " (P-Frame)" << endl;
                break;
            case FF_B_TYPE:
                cerr << " (B-Frame)" << endl;
                break;
            default:
                cerr << " (unknown)" << endl;
                break;

        }
        cerr << "    stream position: " << theFrame.coded_picture_number << endl;
        cerr << "    display number:  " << theFrame.display_picture_number << endl;
        cerr << "    age:             " << theFrame.age << endl;
        cerr << "    pts:             ";
        if (theFrame.pts != AV_NOPTS_VALUE) {            
             cerr << theFrame.pts / ourTimeBase << endl;
        } else {
            cerr << "N/V" << endl;
        }
    }

    void 
    FrameAnalyser::setupTable(Table & theTable) {
        theTable.addColumn("frame", "Frame", Table::JUSTIFIED_MIDDLE);
        theTable.addColumn("type", "Type", Table::JUSTIFIED_MIDDLE);
        theTable.addColumn("dts", "DTS", Table::JUSTIFIED_MIDDLE);
        theTable.addColumn("pts", "PTS", Table::JUSTIFIED_MIDDLE);
        theTable.addColumn("streamposition", "Stream Position", Table::JUSTIFIED_MIDDLE);        
    }

    void
    FrameAnalyser::printTableRow(Table & theTable, AVPacket & thePacket, AVFrame & theFrame) {
        theTable.addRow();

        if (thePacket.stream_index == _myVStreamIndex) {
            ourFrameCounter++;
#if LIBAVCODEC_BUILD >= 0x5100
            int myFrameIndex = asl::round(double(thePacket.dts - _myVStream->start_time) 
                    / av_q2d(_myVStream->codec->time_base));
#else
            int myFrameIndex = asl::round(double(thePacket.dts - _myVStream->start_time) 
                    / AV_TIME_BASE * _myVStream->r_frame_rate / _myVStream->r_frame_rate_base);
#endif
            theTable.setField("frame", as_string(myFrameIndex));        
            switch (theFrame.pict_type) {
                case FF_I_TYPE:
                    theTable.setField("type","*I*");
                    break;
                case FF_P_TYPE:
                    theTable.setField("type","P");
                    break;
                case FF_B_TYPE:
                    theTable.setField("type","B");
                    break;
                default:
                    theTable.setField("type","Unknown");
                    break;
            }
        } else {
            theTable.setField("frame", "--");
            theTable.setField("type", "-A-");
        }
        
        theTable.setField("dts", as_string(thePacket.dts / ourTimeBase));
        theTable.setField("pts", as_string(theFrame.pts / ourTimeBase));
        theTable.setField("streamposition", as_string(theFrame.coded_picture_number));
    }

    long long
    FrameAnalyser::advance() const {
        int64_t   myLastDTS;
        AVPacket  myPacket;
        AVFrame * myFrame = avcodec_alloc_frame();
        int myFoundPictureFlag = 0;

#if LIBAVCODEC_BUILD >= 0x5100
        AVCodecContext * myVCodec = _myVStream->codec;
#else
        AVCodecContext * myVCodec = &_myVStream->codec;
#endif

        while (myFoundPictureFlag == 0) {
            if (av_read_frame(_myFormatContext, &myPacket) < 0) {
                break;
            }
            myLastDTS = myPacket.dts;
    
            // Make sure that the packet is from the actual video stream.
            if (myPacket.stream_index == _myVStreamIndex) {
                avcodec_decode_video(myVCodec,
                    myFrame, &myFoundPictureFlag,
                    myPacket.data, myPacket.size);
            }
            av_free_packet(&myPacket);
        }

        // From ffmpeg apiexample.c: some codecs, such as MPEG, transmit the
        // I and P frame with a latency of one frame. You must do the
        // following to have a chance to get the last frame of the video.
        if (!myFoundPictureFlag) {
            avcodec_decode_video(myVCodec,
                                 myFrame, &myFoundPictureFlag,
                                 0, 0);
#if LIBAVCODEC_BUILD >= 0x5100
            myLastDTS += (int64_t) av_q2d(_myVStream->time_base);
#else
            myLastDTS += AV_TIME_BASE * _myVStream->r_frame_rate_base / _myVStream->r_frame_rate;
#endif
        }
        if (!myFoundPictureFlag) {
            myLastDTS = UINT_MAX;
        }

        av_free(myFrame);
        return myLastDTS;
    }

    void
    FrameAnalyser::seekToFrame(unsigned theFrame) const {
        if (theFrame != 0) {
            theFrame--;
        } else {
#if LIBAVFORMAT_BUILD <= 4616
            av_seek_frame(_myFormatContext, _myVStreamIndex, 0);
#else
            av_seek_frame(_myFormatContext, _myVStreamIndex, 0, 0);
#endif              
            return;
        }

#if LIBAVCODEC_BUILD >= 0x5100
        int64_t myHalfFrame = av_q2d(_myVStream->time_base) / 2;
        int64_t myTimestamp = theFrame * (int64_t)av_q2d(_myVStream->time_base) + _myVStream->start_time;
#else
        int64_t myHalfFrame = int64_t(AV_TIME_BASE) * _myVStream->r_frame_rate_base / 
                _myVStream->r_frame_rate / 2;        
        int64_t myTimestamp = int64_t(theFrame) * AV_TIME_BASE * 
                _myVStream->r_frame_rate_base / _myVStream->r_frame_rate + 
                _myVStream->start_time;
#endif

        if (myTimestamp > myHalfFrame) {
            myTimestamp -= myHalfFrame;
        } else {
            myTimestamp = 0;
        }
        
        // newer releases of ffmpeg require a 4th argument to av_seek_frame
#if LIBAVFORMAT_BUILD <= 4616
        int myResult = av_seek_frame(_myFormatContext, _myVStreamIndex, theFrame);
#else
        int myResult = av_seek_frame(_myFormatContext, _myVStreamIndex, theFrame, 0);
#endif

        if (myResult < 0) {
            cerr << "### ERROR: seek error!!!" << endl;
            return;
        }

        // We got to a key frame. Forward until we get to the frame we want.
        while (true) {
            long long myLastDTS = advance();

            if (myLastDTS == UINT_MAX) {
                cerr << "### WARNING: seek reached end of file" << endl;
                return;
            }

            if (myLastDTS >= myTimestamp) {
                if (myLastDTS >= myTimestamp + 2 * myHalfFrame) {
                    cerr << "### WARNING: seek went into the future!" << endl;
                    return;
                }
                return;
            }
        }
    }

    void 
    FrameAnalyser::run(unsigned theNumberOfFrames) {
        Table myTable;
        setupTable(myTable);

        cerr << "Video Stream: " << endl;
        cerr << "    starttime: " << _myVStream->start_time << endl;

        int64_t mySeekTimestamp = _myVStream->start_time;

        string mySeekToFrameString;
        if (asl::get_environment_var("Y60_SEEK_TO_FRAME", mySeekToFrameString)) {                       
            mySeekTimestamp = (int64_t)(ourTimeBase * asl::as<unsigned>(mySeekToFrameString));
            cerr << "seek to frame: " << mySeekToFrameString << endl;
            seekToFrame(10);
            seekToFrame(20);
            seekToFrame(5);
            seekToFrame(asl::as<unsigned>(mySeekToFrameString));            
        }

        AVPacket myPacket;
        AVFrame * myFrame = avcodec_alloc_frame();

        if (theNumberOfFrames <= 0) {
            theNumberOfFrames = 100;
        }
        while (true) {
            bool myEndOfFileFlag = (av_read_frame(_myFormatContext, &myPacket) < 0);
            if (myEndOfFileFlag) {
                cerr << "END OF FILE!" << endl;
                break;
            } 

            if (myPacket.stream_index == _myVStreamIndex) {                
                printPacketInfo(myPacket);
                int myFrameCompleteFlag = 0;
#if LIBAVCODEC_BUILD >= 0x5100
                int myLength = avcodec_decode_video(_myVStream->codec, myFrame, &myFrameCompleteFlag,
                                                 myPacket.data, myPacket.size);
#else
                int myLength = avcodec_decode_video(&_myVStream->codec, myFrame, &myFrameCompleteFlag,
                                                 myPacket.data, myPacket.size);
#endif

                if (myLength < 0) {
                    AC_ERROR << "av_decode_video error";
                } else if (myLength < myPacket.size) {
                    AC_ERROR << "av_decode_video: Could not decode video in one step";
                }

                if (myFrameCompleteFlag) {
                    printFrameInfo(*myFrame);
                    printTableRow(myTable, myPacket, *myFrame);
                } else {
                    cerr << "-> Packet contains unfinished frame." << endl;
                }
                if (ourFrameCounter >= theNumberOfFrames) {
                    break;
                }
            } else {
                cerr << "*** Audio Packet *** " << endl;
                printPacketInfo(myPacket);
                printTableRow(myTable, myPacket, *myFrame);
            }
        }
        myTable.print(cerr);
        av_free(myFrame);
        av_free_packet(&myPacket);
        exit(0);
    }
}
