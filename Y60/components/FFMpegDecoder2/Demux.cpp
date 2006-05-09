//=============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Demux.h"

#include <asl/Logger.h>

using namespace std;

namespace y60 {

Demux::Demux(AVFormatContext * myFormatContext)
    : _myFormatContext(myFormatContext)
{
}

Demux::~Demux()
{
    clearPacketCache();
}

void Demux::enableStream(int theStreamIndex)
{
    _myPacketLists[theStreamIndex] = PacketList();
}

AVPacket * Demux::getPacket(int theStreamIndex)
{
    AC_TRACE << "Demux::getPacket";
    if (_myPacketLists.find(theStreamIndex) == _myPacketLists.end()) {
        AC_ERROR << "Demux::getPacket called with nonexistent stream index " 
            << theStreamIndex << ".";
    }
    PacketList & myCurPacketList = _myPacketLists.find(theStreamIndex)->second;
    if (!myCurPacketList.empty()) {
        AC_TRACE << "Demux::getPacket: packet already there.";
        AVPacket * myPacket = myCurPacketList.front();
        myCurPacketList.pop_front();
        return myPacket;
    } else {
        AC_TRACE << "Demux::getPacket: packet needs to be read.";
        AVPacket * myPacket;
        bool myEndOfFileFlag;
        do {
            AC_TRACE << "Demux::getPacket: read.";
            myPacket = new AVPacket;
            myEndOfFileFlag = (av_read_frame(_myFormatContext, myPacket) < 0);
            if (myEndOfFileFlag) {
                AC_DEBUG << "Demux::getPacket: end of file.";
//                av_free_packet(myPacket);
                delete myPacket;
                myPacket = 0;
                return 0;
            }
            if (myPacket->stream_index != theStreamIndex) {
                if (_myPacketLists.find(myPacket->stream_index) != _myPacketLists.end()) {
                    AC_TRACE << "Demux::getPacket: caching packet.";
                    // Without av_dup_packet, ffmpeg reuses myPacket->data at first 
                    // opportunity and trashes our memory.
                    av_dup_packet(myPacket);
                    PacketList& myOtherPacketList = 
                            _myPacketLists.find(myPacket->stream_index)->second;
                    myOtherPacketList.push_back(myPacket);
                } else {
                    AC_TRACE << "Demux::getPacket: rejecting packet.";
                    av_free_packet(myPacket);
                    delete myPacket;
                    myPacket = 0;
                }
            }
        } while (!myPacket || myPacket->stream_index != theStreamIndex);
        AC_TRACE << "Demux::getPacket: end.";
        av_dup_packet(myPacket);
        return myPacket; 
    }
}

void Demux::clearPacketCache()
{
    map<int, PacketList>::iterator it;
    for (it=_myPacketLists.begin(); it != _myPacketLists.end(); ++it) {
        PacketList::iterator it2;
        PacketList* thePacketList = &(it->second);
        for (it2=thePacketList->begin(); it2 != thePacketList->end(); ++it2) {
            av_free_packet(*it2);
            delete *it2;
        }
        thePacketList->clear();
    }
}

void Demux::dump()
{
    AC_DEBUG << "Demux list sizes: ";
    map<int, PacketList>::iterator it;
    for (it=_myPacketLists.begin(); it != _myPacketLists.end(); ++it) {
        AC_DEBUG << "  " << it->second.size();
    }
}

}
