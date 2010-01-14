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

#ifndef _ac_DvbTuner_h_
#define _ac_DvbTuner_h_

#include <iostream>
#include <fstream>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <linux/dvb/dmx.h>
#include <linux/dvb/frontend.h>
#include <asl/base/settings.h>
#include <asl/dom/Nodes.h>

#include <asl/base/PosixThread.h>

#include "DvbTeleText.h"


DEFINE_EXCEPTION(DvbTunerException, asl::Exception);

class DvbTuner : public asl::PosixThread
{
public:
    DvbTuner(const dom::NodePtr & theChannelConfig, const std::string & theDeviceName = "/dev/dvb/adapter0");
    ~DvbTuner(void);

    void tuneChannel(const std::string & theChannelName);

    // property getters
    const std::string& getDeviceName(void) const {
        return _myDeviceName;
    }
    const std::string& getChannelName(void) const {
        return _myChannelName;
    }
    int getVideoPID() const {
        return _myVpid;
    }
    int getAudioPID() const {
        return _myApid;
    }
    int getVideoTextPID() const{
        return _myVTpid;
    }

    // property setters
    void setDeviceName(const std::string & theDeviceName){
        _myDeviceName = theDeviceName;
    }
    void setVideoPID(const int & theVideoPID){
        _myVpid = theVideoPID;
    }
    void setAudioPID(const int & theAudioPID){
        _myApid = theAudioPID;
    }
    void setVideoTextPID(const int & theVideoTextPID){
        _myVTpid = theVideoTextPID;
    }

    void startTeleTextDecoder();
    void stopTeleTextDecoder();
    std::basic_string<asl::Unsigned16> getPage(const unsigned & thePageNumber);

    static bool deviceAvailable(const std::string & theDeviceName);

private:
    void openDevice();

    void setParameters(const std::string & theChannelName);
    void setupConstants(void);

    void find_channel(int theFd, const char *theChannelName);
    void setup_frontend(void);
    void set_pesfilter(void);
    void run();

    void set_filter(void);
    void process_pes(void);

    std::string _myDeviceName;
    dom::NodePtr _myChannelConfig;

    struct dvb_frontend_parameters _myFrontendParams;

    // member variables
    std::map<std::string, int> _myConstants;

    int _myDeviceDescriptor;
    int _myAdapter;
    int _myFrontend;
    int _myDvr;

    // parameters
    std::string _myChannelName;
    int _myCarrierFrequency;
    int _myVpid;
    int _myApid;
    int _myVTpid;
    int _myPmtPid;

    int _myFrontendFd;
    int _myAudioFd;
    int _myVideoFd;
    int _myPmtFd;
    int _myNullFd;
    DvbTeleText _myDvbTeleText;
};

#endif // _DVB_DEVICE_
