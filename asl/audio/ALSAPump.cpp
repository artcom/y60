//=============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

#include "ALSAPump.h"
#include "AudioException.h"

#include <asl/Logger.h>
#include <asl/string_functions.h>
#include <asl/numeric_functions.h>
#include <asl/Assure.h>
#include <asl/Auto.h>
#include <asl/Dashboard.h>

#include <exception>
#include <sstream>
#include <string.h>

using namespace std;

namespace asl {

void checkRetVal (int theRetVal, const char * theMsg, const string& theWhere);
void outputHWParams (snd_pcm_t * theDevice); 
void outputSWParams (snd_pcm_t * theDevice);

ALSAPump::~ALSAPump ()
{
    AC_INFO << "ALSAPump::~ALSAPump";
    Pump::stop();
    closeOutput();
}

void ALSAPump::dumpState () const {
    Pump::dumpState();
    if (isOutputOpen()) {
        AC_DEBUG << "  Output State: " << snd_pcm_state_name(snd_pcm_state(_myOutputDevice));
        outputHWParams(_myOutputDevice);
        outputSWParams(_myOutputDevice);
    }
}

ALSAPump::ALSAPump () 
    : Pump(SF_F32, 1),
      _myIsOutOpen(false)
{
    AC_INFO << "ALSAPump::ALSAPump";
    int myRetVal;
    static snd_output_t * myOutput = 0;

    string myDeviceName = _myDeviceName_Init;
    if (myDeviceName.empty()) {
        myDeviceName = "plughw:0,0";
    }
    AC_DEBUG << "ALSA Device name: \"" << myDeviceName << "\"";
    setDeviceName(myDeviceName);
    
    myRetVal = snd_output_stdio_attach(&myOutput, stdout, 0);
    checkRetVal (myRetVal, "failed", PLUS_FILE_LINE);

    int myIndex = 0;
    char * myName;
    myRetVal = snd_card_get_name (myIndex, &myName);
    checkRetVal (myRetVal, "snd_card_get_name error: ", PLUS_FILE_LINE);
    setCardName(myName);
    free(myName);

    openOutput();
    dumpState();
    start();
}

void
ALSAPump::openOutput() {
    AC_INFO << "ALSAPump::openOutput";
    if (isOutputOpen()) {
        AC_WARNING << "ALSAPump::openOutput: Device already open";
        return; 
    }

    _myOutputDevice = openDevice(SND_PCM_STREAM_PLAYBACK) ;
    setHWParams(_myOutputDevice, getNumOutputChannels());
    setSWParams(_myOutputDevice);

    int myRetVal = snd_pcm_prepare(_myOutputDevice);
    checkRetVal (myRetVal, "Prepare output device error: ", PLUS_FILE_LINE);

    _myOutputBuffer.init(_myFramesPerBuffer, getNumOutputChannels(), getNativeSampleRate());
    
    _myIsOutOpen = true;
}

void
ALSAPump::closeOutput() {
    AC_INFO << "ALSAPump::closeOutput";
    if (isOutputOpen()) {
        snd_pcm_close(_myOutputDevice);
        _myOutputDevice = 0;
    }
    _myIsOutOpen = false;
}


bool 
ALSAPump::isOutputOpen() const {
    return _myIsOutOpen;
}

snd_pcm_t *
ALSAPump::openDevice(snd_pcm_stream_t theStreamType) {
    snd_pcm_t * myDevice = 0;
    int myRetVal;

    // Open device in nonblocking mode first. Open in blocking mode would hang
    // if the device isn't available.
    myRetVal = snd_pcm_open(&myDevice, getDeviceName().c_str(), theStreamType,
            SND_PCM_NONBLOCK);
    checkRetVal (myRetVal, "Open error: ", PLUS_FILE_LINE);
    snd_pcm_close (myDevice);

    // Nonblocking open worked, so we can open the device in blocking mode again.
    myRetVal = snd_pcm_open(&myDevice, getDeviceName().c_str(), theStreamType, 0);
    checkRetVal (myRetVal, "Open error (Blocking! Huh?): ", PLUS_FILE_LINE);

    return myDevice;
}


void
ALSAPump::setHWParams(snd_pcm_t * theDevice, int myNumChannels) {
    int myRetVal;

    snd_pcm_hw_params_t * myHWParams;
    myRetVal = snd_pcm_hw_params_malloc(&myHWParams);
    checkRetVal (myRetVal,
            "Can't allocate hw param struct", PLUS_FILE_LINE);

    myRetVal = snd_pcm_hw_params_any(theDevice, myHWParams);
    checkRetVal (myRetVal,
            "Broken configuration: no configurations available", PLUS_FILE_LINE);

    myRetVal = snd_pcm_hw_params_set_access(theDevice, myHWParams,
            SND_PCM_ACCESS_RW_INTERLEAVED);
    checkRetVal (myRetVal, "Access type not available", PLUS_FILE_LINE);

    // This is CPU-specific endianness.
    myRetVal = snd_pcm_hw_params_set_format(theDevice, myHWParams, SND_PCM_FORMAT_FLOAT);
    checkRetVal (myRetVal, "Sample format SND_PCM_FORMAT_FLOAT not available", 
            PLUS_FILE_LINE);

    unsigned myRate = static_cast<unsigned>(getNativeSampleRate());
    myRetVal = snd_pcm_hw_params_set_rate(theDevice, myHWParams, myRate, 0);
    checkRetVal(myRetVal, (string("ALSAPump: Rate ") + as_string(myRate) + "Hz not available: "
            + snd_strerror(myRetVal) + ". ").c_str(), PLUS_FILE_LINE);

    myRetVal = snd_pcm_hw_params_set_channels(theDevice, myHWParams, myNumChannels);
    checkRetVal(myRetVal, (string("ALSAPump: Channel count ") + as_string(myNumChannels) 
                + " not available: " + snd_strerror(myRetVal) + ". ").c_str(), PLUS_FILE_LINE);

    snd_pcm_uframes_t myBufferSize = nextPowerOfTwo(int(getLatency() * getNativeSampleRate()));
    myRetVal = snd_pcm_hw_params_set_buffer_size_near
            (theDevice, myHWParams, &myBufferSize);
    _myFramesPerBuffer = myBufferSize;
    
    checkRetVal (myRetVal,(string("Unable to set buffer size to ")+
                as_string(myBufferSize)+".").c_str(), PLUS_FILE_LINE);

    myRetVal = snd_pcm_hw_params_set_periods
            (theDevice, myHWParams, 2, 0);
    checkRetVal (myRetVal, "Unable to set number of buffers.", PLUS_FILE_LINE);

    /* write the parameters to device */
    myRetVal = snd_pcm_hw_params(theDevice, myHWParams);
    checkRetVal (myRetVal, "Unable to set hw params.", PLUS_FILE_LINE);

    snd_pcm_hw_params_free(myHWParams);
}


void
ALSAPump::setSWParams (snd_pcm_t * theDevice) {
    int myRetVal;

    snd_pcm_sw_params_t * mySWParams;
    snd_pcm_sw_params_malloc(&mySWParams);

    myRetVal = snd_pcm_sw_params_current(theDevice, mySWParams);
    checkRetVal (myRetVal, "Unable to determine current swparams.", PLUS_FILE_LINE);

    myRetVal = snd_pcm_sw_params_set_start_threshold
            (theDevice, mySWParams, 0);
    checkRetVal (myRetVal, "Unable to set start threshold mode for.", PLUS_FILE_LINE);

    /* allow transfer when at least period_size samples can be processed */
    myRetVal = snd_pcm_sw_params_set_avail_min(theDevice, mySWParams, _myFramesPerBuffer/2);
    checkRetVal (myRetVal, "Unable to set avail min for playback: ", PLUS_FILE_LINE);

    /* align all transfers to 1 samples */
    myRetVal = snd_pcm_sw_params_set_xfer_align(theDevice, mySWParams, 1);
    checkRetVal (myRetVal, "Unable to set transfer align: ", PLUS_FILE_LINE);

    /* write the parameters to device */
    myRetVal = snd_pcm_sw_params(theDevice, mySWParams);
    checkRetVal (myRetVal, "Unable to set sw params ", PLUS_FILE_LINE);
    snd_pcm_sw_params_free(mySWParams);
}

void ALSAPump::pump()
{
    int myRetVal = snd_pcm_wait(_myOutputDevice, 1000);

    MAKE_SCOPE_TIMER(Pump);
    handleUnderrun(myRetVal);

    /* find out how much space is available for playback data */
    snd_pcm_sframes_t numFramesToDeliver;
    numFramesToDeliver = snd_pcm_avail_update (_myOutputDevice);
    handleUnderrun(numFramesToDeliver);

    // The ALSA example has this code. It seems to be utterly useless, but who knows.
    if (numFramesToDeliver > _myFramesPerBuffer) {
        numFramesToDeliver = _myFramesPerBuffer;
    }    

    mix(_myOutputBuffer, numFramesToDeliver);

    AC_TRACE << "ALSAPump::pump called with " << numFramesToDeliver << " frames";

    {
        MAKE_SCOPE_TIMER(Write_to_Card);
        myRetVal = snd_pcm_writei(_myOutputDevice, _myOutputBuffer.begin(), 
                numFramesToDeliver);
        handleUnderrun(myRetVal);
    }
    
    addFramesToTime(numFramesToDeliver);
}

void
ALSAPump::handleUnderrun (int err) {
    if (err == -EPIPE || err == -EBADFD) {
        int myRetVal;
        myRetVal = snd_pcm_prepare(_myOutputDevice);
        checkRetVal (myRetVal, "Can't recover from underrun, prepare failed: ", 
                PLUS_FILE_LINE);
        if (err == -EPIPE) {
            if (getNumUnderruns() == 0) {
                AC_ERROR << "ALSAPump::handleUnderrun(): Buffer underrun (EPIPE).";
            } else {
                AC_DEBUG << "ALSAPump::handleUnderrun(): Buffer underrun (EPIPE).";
            }
        } else {
            if (getNumUnderruns() == 0) {
                AC_DEBUG << "ALSAPump::handleUnderrun(): Buffer underrun (EBADFD).";
            } else {
                AC_DEBUG << "ALSAPump::handleUnderrun(): Buffer underrun (EBADFD).";
            }
        }
        addUnderrun();
    } else {
        checkRetVal (err, "handleUnderrun: ", PLUS_FILE_LINE);
    }
}

void
checkRetVal (int theRetVal, const char * theMsg, const string& theWhere) {
    if (theRetVal < 0) {
        throw AudioException(string("ALSASoundCard: ") + theMsg +
                snd_strerror(theRetVal) + ". ",theWhere);
    }
}

void outputHWParams (snd_pcm_t * theDevice) {
    AC_DEBUG << "HW Params: ";
    int myDir;
    int myRetVal;

    snd_pcm_hw_params_t * myParams;
    snd_pcm_hw_params_malloc(&myParams);
    snd_pcm_hw_params_current(theDevice, myParams);
    
    snd_pcm_uframes_t mySize;
    myRetVal = snd_pcm_hw_params_get_period_size(myParams, & mySize, & myDir);
    AC_DEBUG << "  Period size: " << mySize;
    
    checkRetVal (myRetVal, "Unable to get period size: ", PLUS_FILE_LINE);

    if (snd_pcm_hw_params_is_double (myParams)) {
        AC_DEBUG << "  Double buffering is on.";
    } else {
        AC_DEBUG << "  Double buffering is off.";
    }

    unsigned int myRateNum;
    unsigned int myRateDen;
    myRetVal = snd_pcm_hw_params_get_rate_numden
        (myParams, &myRateNum, &myRateDen);
    checkRetVal (myRetVal, "", PLUS_FILE_LINE);
    AC_DEBUG << "  Rate: " << double (myRateNum)/myRateDen;

    AC_DEBUG << "  Significant bits per sample: " << snd_pcm_hw_params_get_sbits (myParams);
    snd_pcm_subformat_t mySubFormat;
    
    myRetVal = snd_pcm_hw_params_get_subformat(myParams, & mySubFormat);
    checkRetVal (myRetVal, "Unable to get subformat: ", PLUS_FILE_LINE);

    //    AC_DB("  Subformat: " << snd_pcm_subformat_description (mySubFormat));
    if (snd_pcm_hw_params_is_block_transfer(myParams)) {
        AC_DEBUG << "  Block transfer.";
    } else {
       AC_DEBUG << "  No block transfer.";
    }
    unsigned myNumChannels;
    snd_pcm_hw_params_get_channels (myParams, & myNumChannels);
    AC_DEBUG << "  Number of channels: " << myNumChannels;
    snd_pcm_hw_params_free(myParams);
}

void
outputSWParams (snd_pcm_t * theDevice) {
    AC_DEBUG << "SW Params: ";
    int myDir;
    int myRetVal;
    snd_pcm_sw_params_t * myParams;
    snd_pcm_sw_params_malloc(&myParams);
    myRetVal = snd_pcm_sw_params_current (theDevice, myParams);
    checkRetVal (myRetVal, "snd_pcm_sw_params_current", PLUS_FILE_LINE);

    snd_pcm_uframes_t myMinAvail;
    myRetVal = snd_pcm_sw_params_get_avail_min(myParams, & myMinAvail);
    checkRetVal (myRetVal, "snd_pcm_sw_params_get_avail_min", PLUS_FILE_LINE);
    AC_DEBUG << "  avail min (frames): " << myMinAvail;
    snd_pcm_sw_params_free(myParams);
}

}

