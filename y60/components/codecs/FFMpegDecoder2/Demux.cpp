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

#include "Demux.h"

#include <cstring>
#include <asl/base/Logger.h>

#define DB(x) //x

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

void Demux::enableStream(const int theStreamIndex)
{
    _myPacketLists[theStreamIndex] = PacketList();
}

AVPacket * Demux::getPacket(const int theStreamIndex)
{
    DB(AC_TRACE << "Demux::getPacket for stream: " << theStreamIndex);
    if (_myPacketLists.find(theStreamIndex) == _myPacketLists.end()) {
        AC_ERROR << "Demux::getPacket called with nonexistent stream index "
            << theStreamIndex << ".";
    }
    PacketList & myCurPacketList = _myPacketLists.find(theStreamIndex)->second;
    if (!myCurPacketList.empty()) {
        DB(AC_TRACE << "Demux::getPacket: packet already there.");
        AVPacket * myPacket = myCurPacketList.front();
        myCurPacketList.pop_front();
        return myPacket;
    } else {
        DB(AC_TRACE << "Demux::getPacket: packet needs to be read.");
        AVPacket * myPacket;
        do {
            DB(AC_TRACE << "Demux::getPacket: read.");
            myPacket = new AVPacket;
            memset(myPacket, 0, sizeof(AVPacket));

            bool myEndOfFileFlag = (av_read_frame(_myFormatContext, myPacket) == AVERROR_EOF);
            if (myEndOfFileFlag) {
                AC_DEBUG << "Demux::getPacket: end of file.";
                av_free_packet(myPacket);
                delete myPacket;
                myPacket = 0;
                return 0;
            }
            if (myPacket->stream_index != theStreamIndex) {
                if (_myPacketLists.find(myPacket->stream_index) != _myPacketLists.end()) {
                    DB(AC_TRACE << "Demux::getPacket: caching packet.");
                    // Without av_dup_packet, ffmpeg reuses myPacket->data at first
                    // opportunity and trashes our memory.
                    av_dup_packet(myPacket);
                    PacketList& myOtherPacketList =
                            _myPacketLists.find(myPacket->stream_index)->second;
                    myOtherPacketList.push_back(myPacket);
                } else {
                    DB(AC_DEBUG << "Demux::getPacket: rejecting packet.");
                    av_free_packet(myPacket);
                    delete myPacket;
                    myPacket = 0;
                }
            }
        } while (!myPacket || myPacket->stream_index != theStreamIndex);
        DB(AC_TRACE << "Demux::getPacket: end.");
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

void Demux::clearPacketCache(const int theStreamIndex)
{
    if (_myPacketLists.find(theStreamIndex) == _myPacketLists.end()) {
        AC_ERROR << "Demux::clearPacketCache called with nonexistent stream index "
            << theStreamIndex << ".";
    }
    PacketList * myCurPacketList = &(_myPacketLists.find(theStreamIndex)->second);
    PacketList::iterator it;
    for (it=myCurPacketList->begin(); it != myCurPacketList->end(); ++it) {
        av_free_packet(*it);
        delete *it;
    }
    myCurPacketList->clear();
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
