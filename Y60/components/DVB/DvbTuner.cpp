
//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================


#include "DvbTuner.h"

#include <asl/string_functions.h>

using namespace asl;
using namespace std;
using namespace dom;

DvbTuner::DvbTuner(const dom::NodePtr & theChannelConfig, const string & theDeviceName)
: _myAdapter(0)
, _myFrontend(0)
, _myVideoFd(0)
, _myAudioFd(0)
, _myDvr(1)
, _myDeviceName(theDeviceName)
, _myChannelConfig(theChannelConfig)
, _myChannelName("")
, _myDvbTeleText(theDeviceName+"/demux0")
{
    setupConstants();
    openDevice();
}

DvbTuner::~DvbTuner(void){
    close(_myFrontendFd);
    close(_myVideoFd);
    close(_myAudioFd);
}

void
DvbTuner::openDevice(){
    string myFrontendDeviceName = string(_myDeviceName) + "/frontend0";
    string myDemuxDeviceName = string(_myDeviceName) + "/demux0";

    if (( _myFrontendFd = open(myFrontendDeviceName.c_str(), O_RDWR)) < 0) {
        throw(DvbTunerException(string("failed to open frontend device:")+myFrontendDeviceName , PLUS_FILE_LINE));
    }

    if ((_myVideoFd = open(myDemuxDeviceName.c_str(), O_RDWR)) < 0) {
        throw(DvbTunerException(string("failed to open demux device: ")+myDemuxDeviceName+ " for the VideoStream", PLUS_FILE_LINE));
    }

    if ((_myAudioFd = open(myDemuxDeviceName.c_str(), O_RDWR)) < 0) {
        throw(DvbTunerException(string("failed to open demux device: ")+myDemuxDeviceName+ " for the AudioStream" , PLUS_FILE_LINE));
    }
}

void
DvbTuner::setParameters(const std::string & theChannelName){
    memset(&_myFrontendParams, 0, sizeof(struct dvb_frontend_parameters));

    NodePtr myChannels = _myChannelConfig->childNode("dvb_channel_list");

    if (myChannels) {
        // *myChannels["name"].nodeValue();
        for (int i=0; i<myChannels->childNodesLength(); ++i){
            if (myChannels->childNode(i)->getAttributeString("name") == theChannelName) {
                Node myNode = *(myChannels->childNode(i));

                _myFrontendParams.inversion =
                    fe_spectral_inversion_t(_myConstants[myNode.getAttributeString("inversion")]);
            
                _myFrontendParams.u.ofdm.bandwidth =
                    fe_bandwidth_t(_myConstants[myNode.getAttributeString("bandwidth")]);
                
                _myFrontendParams.u.ofdm.code_rate_HP = 
                    fe_code_rate_t(_myConstants[myNode.getAttributeString("code_rate_HP")]);
                
                _myFrontendParams.u.ofdm.code_rate_LP = 
                    fe_code_rate_t(_myConstants[myNode.getAttributeString("code_rate_LP")]);
                
                _myFrontendParams.u.ofdm.constellation =
                    fe_modulation_t(_myConstants[myNode.getAttributeString("constellation")]);
                
                _myFrontendParams.u.ofdm.transmission_mode =
                    fe_transmit_mode_t(_myConstants[myNode.getAttributeString("transmission_mode")]);
                
                _myFrontendParams.u.ofdm.guard_interval =
                    fe_guard_interval_t(_myConstants[myNode.getAttributeString("guard_interval")]);
                
                _myFrontendParams.u.ofdm.hierarchy_information = 
                    fe_hierarchy_t(_myConstants[myNode.getAttributeString("hierarchy_information")]);
                
                _myFrontendParams.frequency = as<int>(myNode.getAttributeString("carrier_frequency"));
                
                _myVpid = as<int>(myNode.getAttributeString("video_pid"));
                _myApid = as<int>(myNode.getAttributeString("audio_pid"));
                _myVTpid = as<int>(myNode.getAttributeString("video_text_pid"));                            

                return;
            }
        }
        
        throw(DvbTunerException(string("Channel '")+theChannelName+"' not found in config.", PLUS_FILE_LINE));        
    }
}

void
DvbTuner::tuneChannel(const std::string & theChannelName) {  
    AC_DEBUG << "Trying to lock " << theChannelName;
    
    _myChannelName = theChannelName;

    setParameters(theChannelName);    
    setup_frontend();
    
    AC_DEBUG << "video pid 0x" << std::hex << _myVpid << " audio pid 0x" << _myApid;

    set_pesfilter();
        
    check_frontend();
}

void
DvbTuner::startTeleTextDecoder(){
    _myDvbTeleText.startDecoderThread(_myVTpid);
}

void
DvbTuner::stopTeleTextDecoder(){
    _myDvbTeleText.stopDecoderThread();       
}

basic_string<Unsigned16>
DvbTuner::getPage(const unsigned & thePageNumber){
    return _myDvbTeleText.getPage(thePageNumber);
}


void
DvbTuner::check_frontend(){
    fe_status_t status;
    uint16_t snr, signal;
    uint32_t ber, uncorrected_blocks;

    bool myHasLock = false;
    
    do {
        ioctl(_myFrontendFd, FE_READ_STATUS, &status);
        ioctl(_myFrontendFd, FE_READ_SIGNAL_STRENGTH, &signal);
        ioctl(_myFrontendFd, FE_READ_SNR, &snr);
        ioctl(_myFrontendFd, FE_READ_BER, &ber);
        ioctl(_myFrontendFd, FE_READ_UNCORRECTED_BLOCKS, &uncorrected_blocks);
        
        AC_DEBUG << std::hex << "status 0x" << status
                  << " signal 0x" << signal
                  << " snr 0x" << snr
                  << " ber 0x" << ber
                  << " uncBlocks 0x" << uncorrected_blocks
		  << ((status & FE_HAS_LOCK) ? " FE_HAS_LOCK" : "");
        
        if (status & FE_HAS_LOCK) {
            myHasLock = true;
        }
        
        usleep(500000);
    } while (!myHasLock);
}

void
DvbTuner::setup_frontend(void)
{
    struct dvb_frontend_info fe_info;
    
    if (ioctl(_myFrontendFd, FE_GET_INFO, &fe_info) < 0) {
        // std::cerr << "ioctl FE_GET_INFO failed: " << _myFrontendFd << std::endl;
        throw(DvbTunerException("FE_GET_INFO failed. Check Dvb Device Settings", PLUS_FILE_LINE));
        return;
    }
    
    if (fe_info.type != FE_OFDM) {
        throw(DvbTunerException("frontend device is not a OFDM (DVB-T) device", PLUS_FILE_LINE));
        return;
    }
    
    // std::cout << "tuning to " << _myFrontendParams.frequency << " Hz" << std::endl;
    
    if (ioctl(_myFrontendFd, FE_SET_FRONTEND, &_myFrontendParams) < 0) {
        throw(DvbTunerException("ioctl FE_SET_FRONTEND failed", PLUS_FILE_LINE));
        return;
    }
}

void
DvbTuner::set_pesfilter(){
    struct dmx_pes_filter_params pesfilterAudio;
    if ( _myApid <= 0 || _myApid >= 0x1fff){
        throw(DvbTunerException("Invalid AudioPID", PLUS_FILE_LINE));
    }
    
    pesfilterAudio.pid = _myApid;
    pesfilterAudio.input = DMX_IN_FRONTEND;
    pesfilterAudio.output = _myDvr ? DMX_OUT_TS_TAP : DMX_OUT_DECODER;
    pesfilterAudio.pes_type = DMX_PES_AUDIO;
    pesfilterAudio.flags = DMX_IMMEDIATE_START;
    
    if (ioctl(_myAudioFd, DMX_SET_PES_FILTER, &pesfilterAudio) < 0) {
        throw(DvbTunerException("ioctl DMX_SET_PES_FILTER failed for AudioStream", PLUS_FILE_LINE));
    } 
    

    struct dmx_pes_filter_params pesfilterVideo;
    if ( _myVpid <= 0 || _myVpid >= 0x1fff){
        throw(DvbTunerException("Invalid VideoPID", PLUS_FILE_LINE));
    }
    
    pesfilterVideo.pid = _myVpid;
    pesfilterVideo.input = DMX_IN_FRONTEND;
    pesfilterVideo.output = _myDvr ? DMX_OUT_TS_TAP : DMX_OUT_DECODER;
    pesfilterVideo.pes_type = DMX_PES_VIDEO;
    pesfilterVideo.flags = DMX_IMMEDIATE_START;
    
    if (ioctl(_myVideoFd, DMX_SET_PES_FILTER, &pesfilterVideo) < 0) {
       throw(DvbTunerException("ioctl DMX_SET_PES_FILTER failed for VideoStream", PLUS_FILE_LINE));
       return;
    }
}

void
DvbTuner::setupConstants(void){
    _myConstants["INVERSION_OFF"]  = static_cast<int>(INVERSION_OFF);
    _myConstants["INVERSION_ON"]   = static_cast<int>(INVERSION_ON);
    _myConstants["INVERSION_AUTO"] = static_cast<int>(INVERSION_AUTO);
    
    _myConstants["BANDWIDTH_6_MHZ"] = static_cast<int>(BANDWIDTH_6_MHZ);
    _myConstants["BANDWIDTH_7_MHZ"] = static_cast<int>(BANDWIDTH_7_MHZ);
    _myConstants["BANDWIDTH_8_MHZ"] = static_cast<int>(BANDWIDTH_8_MHZ);
    
    _myConstants["FEC_1_2"]  = static_cast<int>(FEC_1_2);
    _myConstants["FEC_2_3"]  = static_cast<int>(FEC_2_3);
    _myConstants["FEC_3_4"]  = static_cast<int>(FEC_3_4);
    _myConstants["FEC_4_5"]  = static_cast<int>(FEC_4_5);
    _myConstants["FEC_5_6"]  = static_cast<int>(FEC_5_6);
    _myConstants["FEC_6_7"]  = static_cast<int>(FEC_6_7);
    _myConstants["FEC_7_8"]  = static_cast<int>(FEC_7_8);
    _myConstants["FEC_8_9"]  = static_cast<int>(FEC_8_9);
    _myConstants["FEC_AUTO"] = static_cast<int>(FEC_AUTO);
    _myConstants["FEC_NONE"] = static_cast<int>(FEC_NONE);
    
    _myConstants["GUARD_INTERVAL_1_16"] = static_cast<int>(GUARD_INTERVAL_1_16);
    _myConstants["GUARD_INTERVAL_1_32"] = static_cast<int>(GUARD_INTERVAL_1_32);
    _myConstants["GUARD_INTERVAL_1_4"]  = static_cast<int>(GUARD_INTERVAL_1_4);
    _myConstants["GUARD_INTERVAL_1_8"]  = static_cast<int>(GUARD_INTERVAL_1_8);
    
    _myConstants["HIERARCHY_1"]    = static_cast<int>( HIERARCHY_1);
    _myConstants["HIERARCHY_2"]    = static_cast<int>(HIERARCHY_2);
    _myConstants["HIERARCHY_4"]    = static_cast<int>(HIERARCHY_4);
    _myConstants["HIERARCHY_NONE"] = static_cast<int>(HIERARCHY_NONE);
    
    _myConstants["QPSK"]    = static_cast<int>(QPSK);
    _myConstants["QAM_128"] = static_cast<int>(QAM_128);
    _myConstants["QAM_16"]  = static_cast<int>(QAM_16);
    _myConstants["QAM_256"] = static_cast<int>(QAM_256);
    _myConstants["QAM_32"]  = static_cast<int>(QAM_32);
    _myConstants["QAM_64"]  = static_cast<int>(QAM_64);
    
    _myConstants["TRANSMISSION_MODE_2K"] = static_cast<int>(TRANSMISSION_MODE_2K);
    _myConstants["TRANSMISSION_MODE_8K"] = static_cast<int>(TRANSMISSION_MODE_8K);
}
