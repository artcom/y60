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
//    $RCSfile: FrameAnalyser.cpp,v $
//     $Author: ulrich $
//   $Revision: 1.6 $
//       $Date: 2005/04/01 17:03:27 $
//
//  ffmpeg movie decoder.
//
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
        theTable.setField("frame",as_string(ourFrameCounter++));

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
        theTable.setField("dts", as_string(thePacket.dts / ourTimeBase));
        theTable.setField("pts", as_string(theFrame.pts / ourTimeBase));
        theTable.setField("streamposition", as_string(theFrame.coded_picture_number));
    }

    void 
    FrameAnalyser::run(unsigned theNumberOfFrames) {
        Table myTable;
        setupTable(myTable);

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
                int myLen = avcodec_decode_video(&_myVStream->codec, myFrame, &myFrameCompleteFlag,
                                                 myPacket.data, myPacket.size);

                if (myLen < 0) {
                    AC_ERROR << "av_decode_video error";
                } else if (myLen < myPacket.size) {
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
            } 
        }
        myTable.print(cerr);
        av_free(myFrame);
        av_free_packet(&myPacket);
        exit(0);
    }
}
