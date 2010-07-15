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

#include "DvbTeleText.h"
#include <sstream>
#include <iostream>
#include <asl/base/Auto.h>
#include <asl/base/settings.h>
#include <asl/base/Logger.h>
#include <asl/base/string_functions.h>

#include <y60/jsbase/ArgumentHolder.impl>

#define MAX_PES_SIZE (4*1024)

using namespace std;
using namespace asl;

namespace jslib {
    template struct ResultConverter<basic_string<Unsigned16> >;
}

Unsigned16 reverse[] = {
    0x20/*_?*/, 0x20, 0x20, 0x20, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x20/*P?*/, 0x50/*P*/, 0x30/*6*/, 0x30/*6*/, 0x70/*p*/, 0xF0,
    0x08, 0x88, 0x48, 0x48/*H*/, 0x28, 0x28/*(*/, 0x68, 0xE8, 0x18, 0x98, 0x58, 0x58/*X*/, 0x38, 0xB8, 0x78, 0x78/*x*/,
    0x20, 0x84, 0x44, 0x44/*D*/, 0x24, 0xA4, 0x64, 0x71/*p*/, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0x74/*t*/,
    0x0C, 0x8C, 0x4C, 0x4C/*L*/, 0x20/*,?*/, 0xAC, 0x6C, 0x6C/*l*/, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0x3C/*<*/, 0xF6/*ö*/, 0x20/*ü?*/,
    0x20, 0x82, 0x20, 0x42/*B*/, 0x22, 0x27/*`*/, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0x72/*r*/,
    0x42/*B*/, 0x20/* */, 0x4A, 0xCA, 0x2A, 0x6A/*j*/, 0x6A, 0x4A/*j*/, 0x1A, 0x9A, 0x5A, 0x5A/*Z*/, 0x3A, 0x3A/*:*/, 0x7A, 0xFA,
    0x06, 0x20/* */, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0x66/*f*/, 0x16, 0x96, 0x56, 0x56/*V*/, 0x36/*0*/, 0x36/*0*/, 0x76, 0x20/*ö?*/,
    0x0E, 0x8E, 0x4E, 0x4E/*N*/, 0x2E, 0x2E/*.*/, 0x6E/*n*/, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0x00DF/*ß*/,
    0x20, 0x20, 0x41, 0x41/*A*/, 0x21, 0x21/*!*/, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0x63/*c*/,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0x69/*i*/, 0x19, 0x99, 0x59, 0x59/*Y*/, 0x39, 0x39/*9*/, 0x79, 0xF9,
    0x05, 0x70/*85*/, 0x45, 0xC5, 0x25, 0x70/*A5*/, 0xE9/**/, 0x65/*e*/, 0x15, 0x95, 0x55, 0x55/*U*/, 0x35, 0x35/*5*/, 0x75, 0x20,
    0x20/*0D*/, 0x8D, 0x4D, 0x4D/*M*/, 0x2D, 0x2D/*-*/, 0x6D/*m*/, 0xED, 0x1D, 0x20/* */, 0xDC/*Ü*/, 0xDD, 0x3D, 0x63/*c*/, 0x7D, 0xFC/*ü*/,
    0x03, 0x20, 0x43, 0x43/*C*/, 0x23, 0xA3, 0x71, 0x63/*c*/, 0x13, 0x93, 0x53, 0x53/*S*/, 0x33, 0x33/**/, 0x73, 0xF3,
    0x20/*0B*/, 0x8B, 0x4B, 0x4B/*K*/, 0x2B, 0x2B/*+*/, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xE4/*ä*/,
    0x20, 0x87, 0x47, 0x47/*G*/, 0x27, 0x27/*'*/, 0x67, 0xE7, 0x17, 0x20, 0x57, 0x57/*W*/, 0x37, 0xB7, 0x77, 0x77/*w*/,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F/* / */, 0xAF, 0xF3/*´o*/, 0x6F/*o*/, 0x1F, 0x9F, 0x20/*_?*/, 0xDF, 0x3F, 0x3F/*?*/, 0x20, 0x20/*P?*/
};

DvbTeleText::DvbTeleText(const string & theDemuxDeviceName)
    : PosixThread()
    , _myDemuxFd(0)
    , _myPid(0)
    , _myChannelHasLock(false)
    , _myDemuxDeviceName(theDemuxDeviceName)
{

}

DvbTeleText::~DvbTeleText()
{

}

void
DvbTeleText::setChannelLock(bool theLock) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    _myChannelHasLock = theLock;
}

void
DvbTeleText::run(){
    try {
        while (!_myChannelHasLock) {
            if (shouldTerminate()) {
                return;
            } else {
                usleep(500000);
            }
        }

        if ((_myDemuxFd = open(_myDemuxDeviceName.c_str(), O_RDWR)) < 0){
            throw(DvbTeleTextException(string("Cannot open: "+_myDemuxDeviceName), PLUS_FILE_LINE));
        }

        set_filter();

        while (!shouldTerminate()){
            processStream();
        }
    } catch (asl::Exception & ex) {
        AC_ERROR << "Exception in DvbTeleText::run() " << ex;
    }
}

void
DvbTeleText::startDecoderThread(const int & thePid){
    _myPid = thePid;

    if(!isActive()) {
        fork();
    }
}

void
DvbTeleText::stopDecoderThread(){
    if (isActive()) {
       join();
    }

    _myTeleTextBuffer.clear();
}

basic_string<asl::Unsigned16>
DvbTeleText::getPage(const unsigned thePageNumber) {
    if (!isActive()) {
        basic_string<asl::Unsigned16> myEmptyString;
        return myEmptyString;
    }

    AutoLocker<ThreadLock> myLocker(_myLock);

    basic_string<asl::Unsigned16> myReturnString =  _myTeleTextBuffer[thePageNumber];
    _myTeleTextBuffer[thePageNumber].clear();

    return myReturnString;
}

void
DvbTeleText::set_filter(){
    struct dmx_pes_filter_params f;

    f.pid = _myPid;
    f.input = DMX_IN_FRONTEND;
    f.output = DMX_OUT_TAP;
    f.pes_type = DMX_PES_OTHER; /* DMX_PES_TELETEXT if you want vbi insertion */
    f.flags = DMX_IMMEDIATE_START;

    if (ioctl(_myDemuxFd, DMX_SET_PES_FILTER, &f) == -1) {
        throw(DvbTeleTextException("Error doing ioctl DMX_SET_PES_FILTER" , PLUS_FILE_LINE));
    }
}

void
DvbTeleText::safe_read(void *buf, int count){
    int bytes;

    do {
        bytes = read(_myDemuxFd, buf, count);
        if (bytes < 0) {
            if (errno == EOVERFLOW){
                throw(DvbTeleTextException("Read error: Buffer Overflow" , PLUS_FILE_LINE));
            } else {
                throw(DvbTeleTextException(string("Cannor Read from: ")+_myDemuxDeviceName, PLUS_FILE_LINE));
            }
        }
    } while (bytes < count);
}

void
DvbTeleText::processStream(){
    // main loop for through the videotext data
    unsigned char buf[MAX_PES_SIZE];
    int i, plen, hlen, sid, lines, l;

    // search for start of next PES data block 0x000001bd
    for (;;) {
        safe_read(buf, 1);
        if (buf[0] != 0)
            continue;
        safe_read(buf, 1);
        if (buf[0] != 0)
            continue;
        safe_read(buf, 1);
        if (buf[0] != 1)
            continue;
        safe_read(buf, 1);
        if (buf[0] == 0xbd)
            break;
    }

    safe_read(buf, 5);

    // PES packet length
    plen = ((buf[0] << 8) | buf[1]) & 0xffff;

    // PES header data length
    hlen = buf[4];
    if (hlen != 0x24) {
        throw(DvbTeleTextException("PES header data length != 0x24", PLUS_FILE_LINE));
    }

    // skip rest of PES header
    safe_read(buf, hlen);

    // read stream ID
    safe_read(buf, 1);
    sid = buf[0];
    if (sid < 0x10 || sid > 0x1f) {
        throw(DvbTeleTextException("non-EBU stream ID", PLUS_FILE_LINE));
    }

    // number of VBI lines
    lines = (plen + 6) / 46 - 1;

    // read VBI data
    for (l = 0; l < lines-2; l++) {

        safe_read(buf, 46);
        if (buf[1] != 44) {
            throw(DvbTeleTextException("VBI line has invalid length", PLUS_FILE_LINE));
        }

        // framing code, should be 11100100b
        // if (buf[3] != 0x27) {
        //     fprintf(stderr, "error: wrong framing code\n");
        //     return -1;
        // }

        // buf[4] buf[5] magazine number and row address
        unsigned int myMagazineNumber = 0;
        unsigned int myRowNumber = 0;

        for (int bc = 0; bc < 3; bc++) {
            bool isSet = buf[4] & (1 << (bc*2+2));
            myMagazineNumber |= (isSet << (2-bc));
        }

        if (myMagazineNumber==0){
            myMagazineNumber = 8;
        }

        // row address MSB
        bool isSet = buf[4] & (1 << 0);
        myRowNumber |= (isSet << (0));

        for (int bc = 0; bc < 4; bc++) {
            bool isSet = buf[5] & (1 << (bc*2));
            myRowNumber |= (isSet << (3-bc));
        }

        // parse header
        if (myRowNumber == 0) {
            if (_myCurrentPageBuffer.size()){
                AutoLocker<ThreadLock> myLocker(_myLock);
                _myTeleTextBuffer[_myCurrentPageNumber] = _myCurrentPageBuffer;
                _myCurrentPageBuffer.clear();
            }

            int myPageUnits = 0;
            int myPageTens  = 0;

            // buf[6] page number
            for (int bc = 0; bc < 4; bc++) {
                bool isSet = buf[6] & (1 << (bc*2));
                myPageUnits |= (isSet << (3-bc));
            }

            for (int bc = 0; bc < 4; bc++) {
                bool isSet = buf[7] & (1 << (bc*2));
                myPageTens |= (isSet << (3-bc));
            }

            _myCurrentPageNumber = 100*myMagazineNumber + 10*myPageTens + myPageUnits;
        } else {
            if (myMagazineNumber == (_myCurrentPageNumber/100)) {
                // bit twiddling
                for (i = 7; i < 46; i++) {
                    _myCurrentPageBuffer.append(1, Unsigned16(::reverse[buf[i]]));
                }

                _myCurrentPageBuffer.append(1, Unsigned16(0x0A)); // line feed
            }
        }
    }
}


