
//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

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

#include <asl/Exception.h>
#include <asl/PosixThread.h>
#include <asl/ThreadLock.h>
#include <asl/settings.h>

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
