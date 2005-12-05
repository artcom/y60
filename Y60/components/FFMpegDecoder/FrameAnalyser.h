//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: FrameAnalyser.h,v $
//   $Author: ulrich $
//   $Revision: 1.4 $
//   $Date: 2005/04/01 10:12:21 $
//
//  ffmpeg movie decoder.
//
//=============================================================================

#ifndef _ac_y60_FrameAnalyser_h_
#define _ac_y60_FrameAnalyser_h_

struct AVStream;
struct AVPacket;
struct AVFrame;
struct AVFormatContext;

namespace asl {
    class Table;
}

namespace y60 {

    class FrameAnalyser {
        public:
            FrameAnalyser::FrameAnalyser(AVFormatContext * theFormatContext, AVStream * theVStream, int theVStreamIndex);

            void printPacketInfo(AVPacket & thePacket);
            void printFrameInfo(AVFrame & theFrame);
            void run(unsigned theNumberOfFrames);

        private:
            long long advance() const;
            void seekToFrame(unsigned theFrame) const;
            void setupTable(asl::Table & theTable);
            void printTableRow(asl::Table & theTable, AVPacket & thePacket, AVFrame & theFrame);

            AVFormatContext * _myFormatContext;
            AVStream *        _myVStream;
            int               _myVStreamIndex;
    };
}

#endif
