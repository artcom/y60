//=============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_Demux_h_
#define _ac_y60_Demux_h_

#include <asl/Ptr.h>

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4244)
#define EMULATE_INTTYPES
#endif

#include <ffmpeg/avformat.h>

#ifdef WIN32
#pragma warning(pop)
#endif

#include <list>
#include <map>

namespace y60 {

    class Demux {
        public:
            Demux(AVFormatContext * myFormatContext);
            virtual ~Demux();
           
            void enableStream(int theStreamIndex);
            AVPacket * getPacket(int theStreamIndex);
            void clearPacketCache();
            
        private:
            typedef std::list<AVPacket *> PacketList;
            std::map<int, PacketList> _myPacketLists;
           
            AVFormatContext * _myFormatContext;
            int _myNumStreams;
    };
    typedef asl::Ptr<Demux> DemuxPtr;
}

#endif
