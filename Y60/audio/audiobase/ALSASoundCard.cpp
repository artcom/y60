//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: ALSASoundCard.cpp,v $
//
//     $Author: build $
//
//   $Revision: 1.18 $
//
//
// Description:
//
//
//=============================================================================

#include "ALSASoundCard.h"
#include "SoundException.h"
#include "AudioBuffer.h"


#include <asl/Auto.h>
#include <asl/numeric_functions.h>
#include <asl/string_functions.h>
#include <asl/Logger.h>

#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

namespace AudioBase {

using namespace std;
using namespace asl;

snd_output_t *output = NULL;

void outputHWParams (snd_pcm_hw_params_t * theHWParams);
void checkRetVal (int theRetVal, const char * theMsg, const string& theWhere);

ALSASoundCard::ALSASoundCard (int myIndex)
    : SoundCard(),
      _myNumUnderruns (0),
      _myNumOverruns (0),
      _mySentBlocks (0),
      _myLastFrameTime(),
      _myLastTime(-10000),
      _myMaxFrameTime(0),
      _myInputDevice(0),
      _myOutputDevice(0),
      _myIndex(myIndex)
{
    int myRetVal;

    myRetVal = snd_output_stdio_attach(&output, stdout, 0);
    checkRetVal (myRetVal, "failed", PLUS_FILE_LINE);

    char * myName;
    myRetVal = snd_card_get_name (_myIndex, &myName);
    checkRetVal (myRetVal, "snd_card_get_name error: ", PLUS_FILE_LINE);

    // TODO: Get real hardware values.
    init("plughw:0,0", myName, 24, 24);
}

void
ALSASoundCard::openOutput() {
    if (isOutputOpen()) {
        AC_WARNING << "ALSASoundCard::openOutput: Device already open";
        return; //closeOutput();
    }
    SoundCard::openOutput();

    _myOutputDevice = openDevice(SND_PCM_STREAM_PLAYBACK) ;
    setHWParams(_myOutputDevice, getNumOutChannels());
    setSWParams(_myOutputDevice);
    int myRetVal = snd_pcm_prepare(_myOutputDevice);
    checkRetVal (myRetVal, "Prepare outputdevice error: ", PLUS_FILE_LINE);
}

void
ALSASoundCard::openInput(int myNumChannels) {
    if (isInputOpen()) {
        AC_WARNING << "ALSASoundCard::openInput: Device already open";
        return; //closeInput();
    }
    SoundCard::openInput(myNumChannels);

    _myInputDevice = openDevice(SND_PCM_STREAM_CAPTURE);
    setHWParams(_myInputDevice, getNumInChannels());
    setSWParams(_myInputDevice);

    int myRetVal = snd_pcm_prepare(_myInputDevice);
    checkRetVal (myRetVal, "Prepare inputdevice error: ", PLUS_FILE_LINE);
    myRetVal = snd_pcm_start(_myInputDevice);
    checkRetVal (myRetVal, "Start inputdevice error: ", PLUS_FILE_LINE);

    allocateInputBuffers();
}

void
ALSASoundCard::closeOutput() {
    if (isOutputOpen()) {
        snd_pcm_close(_myOutputDevice);
        _myOutputDevice = 0;
        if (_myNumUnderruns > 0) {
            AC_DEBUG << "~ALSASoundCard: Number of buffer underruns: " << _myNumUnderruns << endl;
        }
        SoundCard::closeOutput();
    }
}

void
ALSASoundCard::closeInput() {
    if (isInputOpen()) {
        snd_pcm_close(_myInputDevice);
        _myInputDevice = 0;
        if (_myNumOverruns > 0) {
            AC_DEBUG << "~ALSASoundCard: Number of buffer overruns: " << _myNumOverruns << endl;
        }
        deallocateInputBuffers();
        SoundCard::closeInput();
    }
}

snd_pcm_t *
ALSASoundCard::openDevice(snd_pcm_stream_t theDeviceType) {
    snd_pcm_t * theAnswer = 0;
    int myRetVal;

    // Open device in nonblocking mode first. Open in blocking mode would hang
    // if the device isn't available.
    myRetVal = snd_pcm_open(&theAnswer, getDeviceName().c_str(), theDeviceType,
            SND_PCM_NONBLOCK);
    checkRetVal (myRetVal, "Open error: ", PLUS_FILE_LINE);

    // Nonblocking open worked, so we can open the device in blocking mode again.
    snd_pcm_close (theAnswer);
    myRetVal = snd_pcm_open(&theAnswer, getDeviceName().c_str(), theDeviceType, 0);
    checkRetVal (myRetVal, "Open error (Blocking! Huh?): ", PLUS_FILE_LINE);

    return theAnswer;
}

void
ALSASoundCard::setHWParams(snd_pcm_t * theDevice, int myNumChannels) {
    int myRetVal;

    snd_pcm_hw_params_t * myHWParams;
    myRetVal = snd_pcm_hw_params_malloc(&myHWParams);
    checkRetVal (myRetVal,
            "Can't allocate hw param struct", PLUS_FILE_LINE);

    myRetVal = snd_pcm_hw_params_any(theDevice, myHWParams);
    checkRetVal (myRetVal,
            "Broken configuration: no configurations available", PLUS_FILE_LINE);

    myRetVal = snd_pcm_hw_params_set_access(theDevice, myHWParams,
            SND_PCM_ACCESS_RW_NONINTERLEAVED);
    checkRetVal (myRetVal, "Access type not available", PLUS_FILE_LINE);

    myRetVal = snd_pcm_hw_params_set_format(theDevice, myHWParams, SND_PCM_FORMAT_S32_LE);
    checkRetVal (myRetVal, "Sample format not available", PLUS_FILE_LINE);


#if (SND_LIB_MAJOR >= 1)
    unsigned myRate = static_cast<unsigned>(getSampleRate());
    myRetVal = snd_pcm_hw_params_set_rate(theDevice, myHWParams, myRate, 0);
#else

    myRetVal = snd_pcm_hw_params_set_rate_near(theDevice, myHWParams, getSampleRate(), 0);
    if (myRetVal != getSampleRate()) {
        throw SoundException(string("Rate doesn't match (requested ") +
                as_string(getSampleRate()) +"Hz, got " + as_string(myRetVal) + " Hz)",
                PLUS_FILE_LINE);
    }
#endif
    if (myRetVal < 0) {
        throw SoundException(string("Rate ") + as_string(getSampleRate()) +
                "Hz not available: " + snd_strerror(myRetVal),PLUS_FILE_LINE);
    }

    myRetVal = snd_pcm_hw_params_set_channels(theDevice, myHWParams, myNumChannels);
    if (myRetVal < 0) {
        throw SoundException(string("Channels count (") + as_string(myNumChannels)
             + ") not available: " + snd_strerror(myRetVal),PLUS_FILE_LINE);
    }


    snd_pcm_uframes_t myBufferSize = nextPowerOfTwo(int(getLatency() * getSampleRate()));
#if (SND_LIB_MAJOR >= 1)
    myRetVal = snd_pcm_hw_params_set_buffer_size_near
            (theDevice, myHWParams, &myBufferSize);
#else
    myRetVal = snd_pcm_hw_params_set_buffer_size_near
            (theDevice, myHWParams, myBufferSize);
#endif

    checkRetVal (myRetVal,(string("Unable to set buffer size to ")+as_string(myBufferSize)+".").c_str(), PLUS_FILE_LINE);

#if (SND_LIB_MAJOR >= 1)
    myRetVal = snd_pcm_hw_params_set_periods
            (theDevice, myHWParams, 2, 0);
    checkRetVal (myRetVal, "Unable to set number of buffers.", PLUS_FILE_LINE);
    _myPeriodSize = myBufferSize;
#else
    myRetVal = snd_pcm_hw_params_set_periods
            (theDevice, myHWParams, 2, 0);
    checkRetVal (myRetVal, "Unable to set number of buffers.", PLUS_FILE_LINE);
    int dir;
    _myPeriodSize = snd_pcm_hw_params_get_period_size(myHWParams, &dir);
#endif

    AudioBuffer::setSize(_myPeriodSize);

    /* write the parameters to device */
    myRetVal = snd_pcm_hw_params(theDevice, myHWParams);
    checkRetVal (myRetVal, "Unable to set hw params.", PLUS_FILE_LINE);

    snd_pcm_hw_params_free(myHWParams);
}


void
ALSASoundCard::setSWParams (snd_pcm_t * theDevice) {
    int myRetVal;

    snd_pcm_sw_params_t * mySWParams;
    snd_pcm_sw_params_alloca(&mySWParams);

    myRetVal = snd_pcm_sw_params_current(theDevice, mySWParams);
    checkRetVal (myRetVal, "Unable to determine current swparams.", PLUS_FILE_LINE);


//    myRetVal = snd_pcm_sw_params_set_start_threshold
//            (theDevice, mySWParams, AudioBuffer::getSize());
//    checkRetVal (myRetVal, "Unable to set start threshold mode for.", PLUS_FILE_LINE);

    /* allow transfer when at least period_size samples can be processed */
//    myRetVal = snd_pcm_sw_params_set_avail_min(theDevice, mySWParams, _myPeriodSize);
//    checkRetVal (myRetVal, "Unable to set avail min for playback: ", PLUS_FILE_LINE);

    /* align all transfers to 1 samples */
    myRetVal = snd_pcm_sw_params_set_xfer_align(theDevice, mySWParams, 1);
    checkRetVal (myRetVal, "Unable to set transfer align: ", PLUS_FILE_LINE);

    /* write the parameters to device */
    myRetVal = snd_pcm_sw_params(theDevice, mySWParams);
    checkRetVal (myRetVal, "Unable to set sw params ", PLUS_FILE_LINE);


}


ALSASoundCard::~ALSASoundCard () {
}

void
ALSASoundCard::read(vector<AudioBuffer*>& theBuffers) {
    int myRetVal;
    myRetVal = snd_pcm_readn (_myInputDevice, (void**)_myInputChannels,
                              AudioBuffer::getNumSamples());

    for (int i=0; i<theBuffers.size(); i++) {
        theBuffers[i]->set32Bit(_myInputChannels[i], getChannelType(i, getNumInChannels()), getNumInChannels());
    }

    _myReadBlocks++;

    handleOverrun (myRetVal);
}

void
ALSASoundCard::write(vector<AudioBuffer*>& theBuffers) {
    asl::Signed32 ** myChannelPtrs = new asl::Signed32* [getNumOutChannels()];

    // TODO: Recycle zeros
    AudioBuffer zeroBuffer;
    zeroBuffer.clear();

    for (int i=0; i<theBuffers.size(); i++) {
        myChannelPtrs[i] = theBuffers[i]->createBuffer32();
    }

    for (int i=theBuffers.size(); i<getNumOutChannels(); i++) {
        myChannelPtrs[i] = zeroBuffer.createBuffer32();
    }

    int myRetVal;
    myRetVal = snd_pcm_writen (_myOutputDevice, (void**)myChannelPtrs,
                               AudioBuffer::getNumSamples());
    handleUnderrun (myRetVal);

    for (int i=0; i<getNumOutChannels(); i++) {
        delete[] myChannelPtrs[i];
    }
    delete[] myChannelPtrs;
    {
        AutoLocker<ThreadLock> myLocker(_myTimeLock);
        asl::Time myCurFrameTime;
        double myCurTimeDiff = myCurFrameTime - _myLastFrameTime;
        if (myCurTimeDiff > _myMaxFrameTime) {
            _myMaxFrameTime = myCurTimeDiff;
        }
        _myLastFrameTime = myCurFrameTime;
        _mySentBlocks++;
    }
}

double
ALSASoundCard::getCurrentTime() {
    double thisTime;
    double myTimeOffset;
    {
        AutoLocker<ThreadLock> myLocker(_myTimeLock);
        asl::Time now;
        myTimeOffset = double(now)-double(_myLastFrameTime);

        // We subtract 2 from the number of blocks sent because the first two blocks
        // get pushed into the soundcard immediately.
        thisTime = (double(_mySentBlocks-2.0)*_myPeriodSize)/getSampleRate()+myTimeOffset;
    }
    if (_myLastTime > thisTime) {
/*        AC_TRACE << "********SoundOut::getCurTime: Time went backwards!";
        AC_TRACE << "  _myLastTime: " << _myLastTime << ", thisTime: " << thisTime
             << ", difference: " << _myLastTime - thisTime);
        AC_TRACE << "  _mySentBlocks: " << _mySentBlocks
             << ", myTimeOffset (yrTime difference): " << myTimeOffset << endl;
  */      thisTime = _myLastTime;
    }
    _myLastTime = thisTime;
    return thisTime;
}

std::string
ALSASoundCard::asString() const {
    stringstream str;
    str << "Audio Device: " << getDeviceName() << endl;
    str << "  Card name: " << getCardName() << endl;
    if (isOutputOpen()) {
        str << "  Output State: " << snd_pcm_state_name(snd_pcm_state(_myOutputDevice)) << endl;
    }
    if (isInputOpen()) {
        str << "  Input State: " << snd_pcm_state_name(snd_pcm_state(_myInputDevice)) << endl;
    }
    return str.str();
}

void outputHWParams (snd_pcm_hw_params_t * theHWParams) {
    AC_DEBUG << "HW Params: ";
    int myDir;
    int myRetVal;

#if (SND_LIB_MAJOR >= 1)
    snd_pcm_uframes_t mySize;
    myRetVal = snd_pcm_hw_params_get_period_size(theHWParams, & mySize, & myDir);
    AC_DEBUG << "  Period size: " << mySize;
#else
    myRetVal = snd_pcm_hw_params_get_period_size(theHWParams, & myDir);
    AC_DEBUG << "  Period size: " << myRetVal;
#endif
    checkRetVal (myRetVal, "Unable to get period size: ", PLUS_FILE_LINE);

    if (snd_pcm_hw_params_is_double (theHWParams)) {
        AC_DEBUG << "  Double buffering is on.";
    } else {
        AC_DEBUG << "  Double buffering is off.";
    }

    unsigned int myRateNum;
    unsigned int myRateDen;
    myRetVal = snd_pcm_hw_params_get_rate_numden
        (theHWParams, &myRateNum, &myRateDen);
    checkRetVal (myRetVal, "", PLUS_FILE_LINE);
    AC_DEBUG << "  Rate: " << double (myRateNum)/myRateDen;

    AC_DEBUG << "  Significant bits per sample: " << snd_pcm_hw_params_get_sbits (theHWParams);
    snd_pcm_subformat_t mySubFormat;
#if (SND_LIB_MAJOR >= 1)
    myRetVal = snd_pcm_hw_params_get_subformat( theHWParams, & mySubFormat);
    checkRetVal (myRetVal, "Unable to get subformat: ", PLUS_FILE_LINE);
#else
    mySubFormat = (snd_pcm_subformat_t)
        snd_pcm_hw_params_get_subformat (theHWParams);
#endif
//    AC_DB("  Subformat: " << snd_pcm_subformat_description (mySubFormat));
    if (snd_pcm_hw_params_is_block_transfer(theHWParams)) {
        AC_DEBUG << "  Block transfer.";
    } else {
       AC_DEBUG << "  No block transfer.";
    }
    unsigned myNumChannels;
#if (SND_LIB_MAJOR >= 1)
    snd_pcm_hw_params_get_channels (theHWParams, & myNumChannels);
#else
    myNumChannels = snd_pcm_hw_params_get_channels (theHWParams);
#endif
    AC_DEBUG << "  Number of channels: " << myNumChannels;
}

void
ALSASoundCard::handleUnderrun (int err) {
    if (err == -EPIPE || err == -EBADFD) {
        int myRetVal;
        myRetVal = snd_pcm_prepare(_myOutputDevice);
        checkRetVal (myRetVal, "Can't recover from underrun, prepare failed: ", PLUS_FILE_LINE);
        AC_DEBUG << "ALSASoundCard::handleUnderrun() warning: Buffer underrun ";
        if (err == -EPIPE) {
            AC_DEBUG << "  (EPIPE).";
        } else {
            AC_DEBUG << "  (EBADFD).";
        }
        _myNumUnderruns++;
    } else {
        checkRetVal (err, "snd_pcm_writen: ", PLUS_FILE_LINE);
    }
}

void
ALSASoundCard::handleOverrun (int err) {
    if (err == -EPIPE || err == -EBADFD) {
        int myRetVal;
        myRetVal = snd_pcm_prepare(_myInputDevice);
        checkRetVal (myRetVal, "Can't recover from overrun, prepare failed: ", PLUS_FILE_LINE);
        myRetVal = snd_pcm_start(_myInputDevice);
        checkRetVal (myRetVal, "Can't recover from overrun, start failed: ", PLUS_FILE_LINE);

        AC_DEBUG << "ALSASoundCard::handleOverrun() warning: Buffer overrun ";
        if (err == -EPIPE) {
            AC_DEBUG << "  (EPIPE).";
        } else {
            AC_DEBUG << "  (EBADFD).";
        }
        _myNumOverruns++;
    } else {
        checkRetVal (err, "snd_pcm_readn: ", PLUS_FILE_LINE);
    }
}

int
ALSASoundCard::getNumUnderruns() const {
    return _myNumUnderruns;
}


void
ALSASoundCard::outputSWParams (snd_pcm_t * theDevice) {
    AC_DEBUG << "SW Params: ";
    int myDir;
    int myRetVal;
    snd_pcm_sw_params_t * myParams;
    snd_pcm_sw_params_alloca(&myParams);
    myRetVal = snd_pcm_sw_params_current (theDevice, myParams);
    checkRetVal (myRetVal, "snd_pcm_sw_params_current", PLUS_FILE_LINE);

    snd_pcm_uframes_t myMinAvail;
#if (SND_LIB_MAJOR >= 1)
    myRetVal = snd_pcm_sw_params_get_avail_min(myParams, & myMinAvail);
    checkRetVal (myRetVal, "snd_pcm_sw_params_get_avail_min", PLUS_FILE_LINE);
#else
    myMinAvail = snd_pcm_sw_params_get_avail_min (myParams);
#endif
    AC_DEBUG << "  avail min (frames): " << myMinAvail;
    //more ...
    // is this nessesary?
    snd_pcm_sw_params_free (myParams);
}

void ALSASoundCard::allocateInputBuffers() {
    _myInputChannels = new asl::Signed32* [getNumInChannels()];
    for (int i=0; i<getNumInChannels(); i++) {
        _myInputChannels[i] = new asl::Signed32[AudioBuffer::getNumSamples()];
    }
}

void ALSASoundCard::deallocateInputBuffers() {
    for (int i=0; i<getNumInChannels(); i++) {
        delete[] _myInputChannels[i];
    }
    delete[] _myInputChannels;
}

void
checkRetVal (int theRetVal, const char * theMsg, const string& theWhere) {
    if (theRetVal < 0) {
        throw SoundException(string("ALSASoundCard: ") + theMsg +
                snd_strerror(theRetVal) + ". ",theWhere);
    }
}

}
