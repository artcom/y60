
//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

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
#include <asl/settings.h>
#include <dom/Nodes.h>

#include <asl/PosixThread.h>

#include "DvbTeleText.h"


DEFINE_EXCEPTION(DvbTunerException, asl::Exception);

class DvbTuner : public PosixThread
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
