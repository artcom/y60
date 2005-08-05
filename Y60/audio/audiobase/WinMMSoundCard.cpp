//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: WinMMSoundCard.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.8 $
//
//
// Description:
//
//
//=============================================================================

#include "WinMMSoundCard.h"
#include "SoundException.h"
#include "AudioBuffer.h"


#include <asl/Logger.h>
#include <asl/Auto.h>
#include <asl/Time.h>
#include <asl/numeric_functions.h>
#include <asl/string_functions.h>

#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

namespace AudioBase {

using namespace std;
using namespace asl;

WinMMSoundCard::WinMMSoundCard (int mySystemID)
    : SoundCard(),
      _mySentBlocks (0),
      _myLastFrameTime(0.0),
      _myAudioOutDevice(0),
      _myAudioInDevice(0),
      _mySystemID(mySystemID)
{
    WAVEOUTCAPS caps;
    MMRESULT myRetVal = waveOutGetDevCaps(mySystemID, &caps, sizeof(caps));
    handleWavError(myRetVal, "WinMMSoundCard::WinMMSoundCard: waveOutGetDevCaps failed.",
            PLUS_FILE_LINE);
    stringstream s;
    s << mySystemID;

    // Not sure why soundcards ever return this, but at least the
    // Crystal WDM Audio does...
    if (caps.wChannels == 65535) {
        caps.wChannels = 2;
    }
    init(caps.szPname, s.str(), caps.wChannels, caps.wChannels);
}

WinMMSoundCard::~WinMMSoundCard () {
   
}

void 
WinMMSoundCard::openOutput() {
    SoundCard::openOutput();
    WAVEFORMATEX theFmt;
    theFmt.wFormatTag = WAVE_FORMAT_PCM;
    theFmt.nChannels = getNumOutChannels();
    theFmt.nSamplesPerSec = getSampleRate();
    theFmt.nAvgBytesPerSec = getSampleRate()*2;
    theFmt.nBlockAlign = 2;
    theFmt.wBitsPerSample = 16;
    theFmt.cbSize = 0;
    
    int myRetVal = waveOutOpen(&_myAudioOutDevice, _mySystemID, &theFmt,
                               reinterpret_cast<DWORD_PTR>(outputWaveProc),
                               reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
    handleWavError(myRetVal, "WinMMSoundCard::openOutput: waveOutOpen failed.",
            PLUS_FILE_LINE);
    allocateBuffers(_myWaveOutBuffers, getNumOutChannels(), true, 
            _myAudioOutDevice);
    _myOutFreeBufferSem = CreateSemaphore(0, getNumOutChannels(), getNumOutChannels(), 0);
    _myCurOutputBuffer = 0;
}

void 
WinMMSoundCard::openInput(int myNumChannels) {
    SoundCard::openInput(myNumChannels);
    WAVEFORMATEX theFmt;
    theFmt.wFormatTag = WAVE_FORMAT_PCM;
    theFmt.nChannels = getNumInChannels();
    theFmt.nSamplesPerSec = getSampleRate();
    theFmt.nAvgBytesPerSec = getSampleRate()*2;
    theFmt.nBlockAlign = 2;
    theFmt.wBitsPerSample = 16;
    theFmt.cbSize = 0;
    
    int myRetVal = waveInOpen(&_myAudioInDevice, _mySystemID, &theFmt,
                               reinterpret_cast<DWORD_PTR>(inputWaveProc),
                               reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION);
    handleWavError(myRetVal, "WinMMSoundCard::openInput: waveInOpen() failed.",
            PLUS_FILE_LINE);
    allocateBuffers(_myWaveInBuffers, getNumInChannels(), false, 
            _myAudioInDevice);
    _myInFreeBufferSem = CreateSemaphore(0, 0, getNumInChannels(), 0);
    _myCurInputBuffer = 0;
    myRetVal = waveInStart(_myAudioInDevice);
    handleWavError(myRetVal, "WinMMSoundCard::openInput: waveInStart() failed.",
            PLUS_FILE_LINE);
}

void 
WinMMSoundCard::closeOutput() {
    if (isOutputOpen()) {
        
        MMRESULT myRetVal = waveOutReset(_myAudioOutDevice);
        handleWavError(myRetVal, "WinMMSoundCard::closeOutput(): waveOutReset failed.",
                PLUS_FILE_LINE);
        deallocateBuffers(_myWaveOutBuffers, true);
        myRetVal = waveOutClose(_myAudioOutDevice);
        handleWavError(myRetVal, "WinMMSoundCard::closeOutput(): waveOutClose failed.",
                PLUS_FILE_LINE);
        CloseHandle(_myOutFreeBufferSem);
        SoundCard::closeOutput();
    }
}

void 
WinMMSoundCard::closeInput() {
    if (isInputOpen()) {
        MMRESULT myRetVal = waveInReset(_myAudioInDevice);
        handleWavError(myRetVal, "WinMMSoundCard::closeInput(): waveInReset failed.",
                PLUS_FILE_LINE);
        deallocateBuffers(_myWaveInBuffers, false);
        myRetVal = waveInClose(_myAudioInDevice);
        handleWavError(myRetVal, "WinMMSoundCard::closeInput(): waveInClose failed.",
                PLUS_FILE_LINE);
        
        CloseHandle(_myInFreeBufferSem);
        SoundCard::closeInput();
    }
}

void 
WinMMSoundCard::allocateBuffers(std::vector<WinWaveBufferPtr> & theBuffers, 
        int NumChannels, bool isOutput, HANDLE theDevice) {
    double myTimePerBuffer = AudioBuffer::getNumSamples()/double(getSampleRate());
    int myNumBuffers = (int)(getLatency()/myTimePerBuffer);
    if (myNumBuffers < 2) {
      myNumBuffers = 2;
    }

    for (int i = 0; i<myNumBuffers; i++) {
        theBuffers.push_back(WinWaveBufferPtr(
                new WinWaveBuffer(AudioBuffer::getNumSamples()*2*NumChannels, (DWORD)this)));
        MMRESULT myRetVal;
        if (isOutput) {                
            myRetVal = waveOutPrepareHeader((HWAVEOUT)theDevice, 
                    theBuffers[i]->getWavHeader(), sizeof(WAVEHDR));
        } else {
            myRetVal = waveInPrepareHeader((HWAVEIN)theDevice, 
                    theBuffers[i]->getWavHeader(), sizeof(WAVEHDR));
            if (myRetVal == MMSYSERR_NOERROR) {
                myRetVal = waveInAddBuffer((HWAVEIN)theDevice, theBuffers[i]->getWavHeader(), 
                        sizeof(WAVEHDR));
            }
        }
        if (myRetVal != MMSYSERR_NOERROR) {
            handleWavError(myRetVal, "WinMMSoundCard::allocateBuffers failed.",
                    PLUS_FILE_LINE);
        }
    }
}


void
WinMMSoundCard::deallocateBuffers(std::vector<WinWaveBufferPtr> & theBuffers, 
         bool isOutput)
{
    for (int i = 0; i<theBuffers.size(); i++) {
        MMRESULT myRetVal;
        if (isOutput) {
            myRetVal = waveOutUnprepareHeader(_myAudioOutDevice, 
                    theBuffers[i]->getWavHeader(), sizeof(WAVEHDR));
        } else {
            myRetVal = waveInUnprepareHeader(_myAudioInDevice, 
                    theBuffers[i]->getWavHeader(), sizeof(WAVEHDR));
        }            
        handleWavError(myRetVal, 
                "WinMMSoundCard::deallocateBuffers: waveXxxUnprepareHeader failed.",
                PLUS_FILE_LINE);
    }
    theBuffers.clear();
}

void
WinMMSoundCard::write(vector<AudioBuffer*>& theBuffers) {

    WaitForSingleObject(_myOutFreeBufferSem, INFINITE);
//    cerr << "Writing to " << _myCurOutputBuffer << endl;

    WinWaveBufferPtr CurDestBuffer = _myWaveOutBuffers[_myCurOutputBuffer];
    asl::Signed16 * myDestSample = (asl::Signed16*)CurDestBuffer->getBuffer();
    for (int i=0; i<AudioBuffer::getNumSamples(); i++) {
        *myDestSample = theBuffers[0]->getData16(i);
        myDestSample++;
        if (getNumOutChannels() == 2) {
            *myDestSample = theBuffers[1]->getData16(i);
            myDestSample++;
        }
    }
            
    MMRESULT myRetVal = waveOutWrite(_myAudioOutDevice, CurDestBuffer->getWavHeader(), 
            sizeof(WAVEHDR));
    handleWavError(myRetVal, "WinMMSoundCard::write: waveOutWrite failed.",
            PLUS_FILE_LINE);
    
    _myCurOutputBuffer++;
    if (_myCurOutputBuffer == _myWaveOutBuffers.size()) {
        _myCurOutputBuffer = 0;
    }
    
    {
        AutoLocker<ThreadLock> myLocker(_myTimeLock);
        _myLastFrameTime = (double)asl::Time();
        _mySentBlocks++;
    }
}

void 
WinMMSoundCard::read(std::vector<AudioBuffer*>& theBuffers) {
    WaitForSingleObject(_myInFreeBufferSem, INFINITE);
//    cerr << "Reading from " << _myCurBuffer << endl;

    WinWaveBufferPtr CurSrcBuffer = _myWaveInBuffers[_myCurInputBuffer];
            
    asl::Signed16 * mySrcSample = (asl::Signed16*)CurSrcBuffer->getBuffer();
    if (getNumInChannels() == 1) {
        theBuffers[0]->set16Bit(mySrcSample, Mono, getNumInChannels());
    } else {
        theBuffers[0]->set16Bit(mySrcSample, Left, getNumInChannels());
        theBuffers[1]->set16Bit(mySrcSample, Right, getNumInChannels());
    }
    MMRESULT myResult = waveInAddBuffer(_myAudioInDevice, CurSrcBuffer->getWavHeader(), 
            sizeof(WAVEHDR));
    handleWavError(myResult, "WinMMSoundCard::read: waveInAddBuffer() failed.", PLUS_FILE_LINE);

    _myCurInputBuffer++;
    if (_myCurInputBuffer == _myWaveInBuffers.size()) {
        _myCurInputBuffer = 0;
    }
}


void CALLBACK 
WinMMSoundCard::outputWaveProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
            DWORD dwParam1, DWORD dwParam2)
{
    WinMMSoundCard * me = reinterpret_cast<WinMMSoundCard *>(dwInstance);
    switch (uMsg) {
        case WOM_OPEN:
            break;
        case WOM_CLOSE:
//            cerr << "WOM_CLOSE" << endl;
            break;
        case WOM_DONE:
            {
                WAVEHDR * theHeader = (WAVEHDR*)dwParam1;
                long thePrevCount;
                bool ok = ReleaseSemaphore(me->_myOutFreeBufferSem, 1, &thePrevCount);
                if (!ok) {
                    cerr << "error in ReleaseSemaphore" << endl;
                }
//                cerr << "WOM_DONE, PrevCount= " << thePrevCount << endl;
            }
            break;
    }
}

void CALLBACK 
WinMMSoundCard::inputWaveProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
            DWORD dwParam1, DWORD dwParam2)
{
    WinMMSoundCard * me = reinterpret_cast<WinMMSoundCard *>(dwInstance);
    switch (uMsg) {
        case WIM_OPEN:
            break;
        case WIM_CLOSE:
//            cerr << "WIM_CLOSE" << endl;
            break;
        case WIM_DATA:
            {
                WAVEHDR * theHeader = (WAVEHDR*)dwParam1;
                long thePrevCount;
                bool ok = ReleaseSemaphore(me->_myInFreeBufferSem, 1, &thePrevCount);
                if (!ok) {
                    cerr << "error in ReleaseSemaphore" << endl;
                }
//                cerr << "WOM_DONE, PrevCount= " << thePrevCount << endl;
            }
            break;
    }
}

#define outputIfSupported(var, FMTID) \
    if (var & FMTID) { \
        str << #FMTID << " "; \
    }

std::string 
WinMMSoundCard::asString() const {
    stringstream str;
    WAVEOUTCAPS caps;
    MMRESULT err = waveOutGetDevCaps(_mySystemID, &caps, sizeof(caps));
    stringstream s;
    s << "Could not get device caps for device " << _mySystemID << ".";
    handleWavError(err, s.str(), PLUS_FILE_LINE);
  
    str << "Audio Device: " << getDeviceName() << endl;
    str << "    Manufacturer id (wMid): " << caps.wMid << endl;
    str << "    Product id (wPid): " << caps.wPid << endl;
    str << "    Driver version (vDriverVersion): " << caps.vDriverVersion << endl;
    str << "    Product name (szPname): " << caps.szPname << endl;
    str << "    Supported formats (dwFormats): " << endl;
    str << "        ";
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_1M08);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_1S08);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_1M16);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_1S16);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_2M08);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_2S08);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_2M16);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_2S16);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_44M08);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_44S08);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_44M16);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_44S16);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_48M08);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_48S08);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_48M16);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_48S16);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_96M08);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_96S08);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_96M16);
    outputIfSupported(caps.dwFormats, WAVE_FORMAT_96S16);
    str << endl;
    str << "    Number of channels (wChannels): " << caps.wChannels << endl;
    str << "    Optional capabilities supported (dwSupport): " << endl;
    str << "        ";
    outputIfSupported(caps.dwSupport, WAVECAPS_PITCH);
    outputIfSupported(caps.dwSupport, WAVECAPS_PLAYBACKRATE);
    outputIfSupported(caps.dwSupport, WAVECAPS_VOLUME);
    outputIfSupported(caps.dwSupport, WAVECAPS_LRVOLUME);
    outputIfSupported(caps.dwSupport, WAVECAPS_SYNC);
    outputIfSupported(caps.dwSupport, WAVECAPS_SAMPLEACCURATE);
    str << endl << endl;
    
    return str.str();
}

void
WinMMSoundCard::setParams(double theLatency, int theSampleRate, int theSampleBits, unsigned theNumChannels) {
    AutoLocker<ThreadLock> myLocker(_myTimeLock);
    if (theSampleRate != getSampleRate()) {
        _mySentBlocks = 0;
        _myLastFrameTime = 0.0;
    }
    SoundCard::setParams(theLatency, theSampleRate, theSampleBits, theNumChannels);
}

double
WinMMSoundCard::getCurrentTime() {
    double thisTime = 0.0;
    {
        AutoLocker<ThreadLock> myLocker(_myTimeLock);
        MMTIME myMMTime;
        MMRESULT myResult = waveOutGetPosition(_myAudioOutDevice, (LPMMTIME)&myMMTime, sizeof(myMMTime));
        if (myResult == MMSYSERR_NOERROR && myMMTime.wType == TIME_SAMPLES) {
            thisTime = myMMTime.u.sample / (double)getSampleRate();
        }
        else {
            double now = (double)asl::Time();
            if (_myLastFrameTime == 0.0) {
                _myLastFrameTime = now;
            }
            double myFrameTimeOffset = now - _myLastFrameTime;
            thisTime = double(_mySentBlocks * AudioBuffer::getNumSamples()) / getSampleRate() + myFrameTimeOffset;
        }
    }
    return thisTime;
}

int
WinMMSoundCard::getNumUnderruns() const {
    return 0;
}

void
WinMMSoundCard::handleWavError(MMRESULT theRetVal, const string& what, 
        const string& where) 
{
    if (theRetVal != MMSYSERR_NOERROR) {
        throw SoundException(what+" Reason: " + stringFromWavError(theRetVal), where);
    }
}

string 
WinMMSoundCard::stringFromWavError(MMRESULT err) {
    switch(err) {
        case MMSYSERR_NOERROR:
            return "no error";
        case MMSYSERR_BADDEVICEID:
            return "device ID out of range";
        case MMSYSERR_NOTENABLED:
            return "driver failed enable";
        case MMSYSERR_ALLOCATED:
            return "device already allocated";
        case MMSYSERR_INVALHANDLE:
            return "device handle is invalid";
        case MMSYSERR_NODRIVER:
            return "no device driver present";
        case MMSYSERR_NOMEM:
            return "memory allocation error";
        case MMSYSERR_NOTSUPPORTED: 
            return "function isn't supported";
        case MMSYSERR_BADERRNUM:
            return "error value out of range";
        case MMSYSERR_INVALFLAG:
            return "invalid flag passed";
        case MMSYSERR_INVALPARAM:
            return "invalid parameter passed";
        case MMSYSERR_HANDLEBUSY:
            return "handle being used simultaneously on another thread (eg callback)";
        case MMSYSERR_INVALIDALIAS:
            return "specified alias not found";
        case MMSYSERR_BADDB:
            return "bad registry database";
        case MMSYSERR_KEYNOTFOUND:
            return "registry key not found";
        case MMSYSERR_READERROR:
            return "registry read error";
        case MMSYSERR_WRITEERROR:
            return "registry write error";
        case MMSYSERR_DELETEERROR:
            return "registry delete error";
        case MMSYSERR_VALNOTFOUND:
            return "registry value not found";
        case MMSYSERR_NODRIVERCB:
            return "driver does not call DriverCallback";
        case MMSYSERR_MOREDATA:
            return "more data to be returned";
        case WAVERR_BADFORMAT:
            return "attempted to open with an unsupported waveform-audio format";
        case WAVERR_SYNC:
            return "device is synchronous";
        case WAVERR_STILLPLAYING:
            return "still something playing";
        case WAVERR_UNPREPARED:
            return "header not prepared";
        default:
            return "unknown error code";
    }
}

}
