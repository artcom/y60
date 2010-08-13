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

#include "DvbTuner.h"

#include <asl/base/string_functions.h>

using namespace asl;
using namespace std;
using namespace dom;

bool
DvbTuner::deviceAvailable(const string & theDeviceName) {
    int myDeviceFd = 0;
    string myFrontendDeviceName = string(theDeviceName) + "/frontend0";
    if (( myDeviceFd = open(myFrontendDeviceName.c_str(), O_RDWR)) < 0) {
        return false;
    }

    close(myDeviceFd);
    return true;
}

DvbTuner::DvbTuner(const dom::NodePtr & theChannelConfig, const string & theDeviceName):
    _myDeviceName(theDeviceName),
    _myChannelConfig(theChannelConfig),
    _myAdapter(0),
    _myFrontend(0),
    _myDvr(1),
    _myChannelName(""),
    _myAudioFd(0),
    _myVideoFd(0), 
    _myDvbTeleText(theDeviceName+"/demux0")
{
    setupConstants();
    openDevice();
}

DvbTuner::~DvbTuner(void) {
    if (isActive()) {
        join();
    }

    close(_myFrontendFd);
    close(_myVideoFd);
    close(_myAudioFd);
    close(_myPmtFd);
    close(_myNullFd);
}

void
DvbTuner::openDevice() {
    string myFrontendDeviceName = string(_myDeviceName) + "/frontend0";
    string myDemuxDeviceName = string(_myDeviceName) + "/demux0";

    if (( _myFrontendFd = open(myFrontendDeviceName.c_str(), O_RDWR)) < 0) {
        throw(DvbTunerException(string("failed to open frontend device:")+myFrontendDeviceName , PLUS_FILE_LINE));
    }

    if ((_myPmtFd = open(myDemuxDeviceName.c_str(), O_RDWR)) < 0) {
        throw(DvbTunerException(string("failed to open demux device: ")+myDemuxDeviceName+ " for the pmt_pid setup" , PLUS_FILE_LINE));
    }

    if ((_myNullFd = open(myDemuxDeviceName.c_str(), O_RDWR)) < 0) {
        throw(DvbTunerException(string("failed to open demux device: ")+myDemuxDeviceName+ " for the null_pid setup" , PLUS_FILE_LINE));
    }

    if ((_myVideoFd = open(myDemuxDeviceName.c_str(), O_RDWR)) < 0) {
        throw(DvbTunerException(string("failed to open demux device: ")+myDemuxDeviceName+ " for the VideoStream", PLUS_FILE_LINE));
    }

    if ((_myAudioFd = open(myDemuxDeviceName.c_str(), O_RDWR)) < 0) {
        throw(DvbTunerException(string("failed to open demux device: ")+myDemuxDeviceName+ " for the AudioStream" , PLUS_FILE_LINE));
    }
}

void
DvbTuner::setParameters(const std::string & theChannelName) {
    memset(&_myFrontendParams, 0, sizeof(struct dvb_frontend_parameters));

    NodePtr myChannels = _myChannelConfig->childNode("dvb_channel_list");

    if (myChannels) {
        for (unsigned int i = 0; i < myChannels->childNodesLength(); ++i){
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
                _myPmtPid = as<int>(myNode.getAttributeString("pmt_pid"));

                return;
            }
        }

        throw(DvbTunerException(string("Channel '")+theChannelName+"' not found in config.", PLUS_FILE_LINE));
    }
}

void
DvbTuner::tuneChannel(const std::string & theChannelName) {
    _myDvbTeleText.setChannelLock(false);
    _myDvbTeleText.stopDecoderThread();

    // Stop current tuning thread
    if (isActive()) {
        join();
    }

    AC_DEBUG << "Trying to lock " << theChannelName;

    _myChannelName = theChannelName;

    setParameters(theChannelName);
    setup_frontend();

    AC_DEBUG << "video pid 0x" << std::hex << _myVpid << " audio pid 0x" << _myApid;

    set_pesfilter();

    fork();
}

void
DvbTuner::run() {
    fe_status_t myStatus;
    Unsigned16 mySnr, mySignalStrength;
    Unsigned32 myBer, myUncorrectedBlocks;

    bool myHasLock = false;

    do {
        ioctl(_myFrontendFd, FE_READ_STATUS, &myStatus);
        ioctl(_myFrontendFd, FE_READ_SIGNAL_STRENGTH, &mySignalStrength);
        ioctl(_myFrontendFd, FE_READ_SNR, &mySnr);
        ioctl(_myFrontendFd, FE_READ_BER, &myBer);
        ioctl(_myFrontendFd, FE_READ_UNCORRECTED_BLOCKS, &myUncorrectedBlocks);

        AC_DEBUG << std::hex << "status 0x" << myStatus
                 << " signal 0x" << mySignalStrength
                 << " snr 0x" << mySnr
                 << " ber 0x" << myBer
                 << " uncBlocks 0x" << myUncorrectedBlocks
                 << ((myStatus & FE_HAS_LOCK) ? " FE_HAS_LOCK" : "");

        if (myStatus & FE_HAS_LOCK) {
            myHasLock = true;
            _myDvbTeleText.setChannelLock(true);
        }

        usleep(500000);
    } while (!myHasLock && !shouldTerminate());

    //pthread_exit();
}

void
DvbTuner::startTeleTextDecoder() {
    _myDvbTeleText.startDecoderThread(_myVTpid);
}

void
DvbTuner::stopTeleTextDecoder() {
    _myDvbTeleText.stopDecoderThread();
}

basic_string<Unsigned16>
DvbTuner::getPage(const unsigned & thePageNumber) {
    return _myDvbTeleText.getPage(thePageNumber);
}

void
DvbTuner::setup_frontend(void) {
    struct dvb_frontend_info fe_info;

    if (ioctl(_myFrontendFd, FE_GET_INFO, &fe_info) < 0) {
        throw(DvbTunerException("FE_GET_INFO failed. Check Dvb Device Settings", PLUS_FILE_LINE));
        return;
    }

    if (fe_info.type != FE_OFDM) {
        throw(DvbTunerException("frontend device is not a OFDM (DVB-T) device", PLUS_FILE_LINE));
        return;
    }

    if (ioctl(_myFrontendFd, FE_SET_FRONTEND, &_myFrontendParams) < 0) {
        throw(DvbTunerException("ioctl FE_SET_FRONTEND failed", PLUS_FILE_LINE));
        return;
    }
}

void
DvbTuner::set_pesfilter() {
    struct dmx_sct_filter_params flt;
    ioctl(_myPmtFd, DMX_STOP);
    memset(&flt, 0, sizeof(struct dmx_sct_filter_params));
    flt.flags = DMX_IMMEDIATE_START;
    flt.pid = 0;
    if (ioctl(_myNullFd, DMX_SET_FILTER, &flt) < 0) {
        throw(DvbTunerException("ioctl DMX_SET_FILTER failed for null_pid", PLUS_FILE_LINE));
    }

    ioctl(_myPmtFd, DMX_STOP);
    memset(&flt, 0, sizeof(struct dmx_sct_filter_params));
    flt.flags = DMX_IMMEDIATE_START;
    flt.pid = _myPmtPid;
    if (ioctl(_myPmtFd, DMX_SET_FILTER, &flt) < 0) {
        throw(DvbTunerException("ioctl DMX_SET_FILTER failed for pmt_pid", PLUS_FILE_LINE));
    }

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
DvbTuner::setupConstants(void) {
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
