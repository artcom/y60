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

#ifndef _ac_DvbTeleText_h_
#define _ac_DvbTeleText_h_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <asl/base/Exception.h>
#include <asl/base/PosixThread.h>
#include <asl/base/ThreadLock.h>
#include <asl/base/settings.h>

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <linux/dvb/dmx.h>

DEFINE_EXCEPTION(DvbTeleTextException, asl::Exception);

class DvbTeleText: public asl::PosixThread {
public:
    DvbTeleText(const std::string & theDemuxDeviceName);
    ~DvbTeleText();

    // videotext thread loop
    virtual void run();
    
    void startDecoderThread(const int & thePid);
    void stopDecoderThread();
    
    void set_filter(void);
    std::basic_string<asl::Unsigned16> getPage(const unsigned thePageNumber);

    void processStream();
    void safe_read(void *buf, int count);
    void setChannelLock(bool theLock);    
private:
    int _myDemuxFd;
    int _myPid;
    bool _myChannelHasLock; 

    std::map<unsigned int, std::basic_string<asl::Unsigned16> > _myTeleTextBuffer;

    std::string _myDemuxDeviceName;
    std::basic_string<asl::Unsigned16> _myCurrentPageBuffer;
    unsigned int _myCurrentPageNumber;

    mutable asl::ThreadLock _myLock;
};

#endif // _ac_DvbTeleText_h_
