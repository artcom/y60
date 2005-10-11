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

#include "DirectSoundPump.h"

#include "DSMessages.h"

#include <asl/Logger.h>
#include <asl/string_functions.h>
#include <asl/numeric_functions.h>
#include <asl/Assure.h>
#include <asl/Auto.h>
//#include <asl/Dashboard.h>

#include <exception>
#include <sstream>
#include <string.h>

#include <mmsystem.h>
#include <Mmreg.h>
#include <Ks.h>
#include <Ksmedia.h>

using namespace std;

namespace asl {

void dumpDSCaps(const DSCAPS& theDSCaps);

DirectSoundPump::~DirectSoundPump () {
    AC_INFO << "DirectSoundPump::~DirectSoundPump";
    Pump::stop();
    closeOutput();
    if (_myNumUnderruns > 0) {
        AC_WARNING << "DirectSoundPump had " << _myNumUnderruns << " underruns.";
    }
}

void DirectSoundPump::dumpState () const {
    Pump::dumpState();
    if (isOutputOpen()) {
        dumpDSCaps(_myDSCaps);
    }
}

Time DirectSoundPump::getCurrentTime() {
    updateFramesPlayed();
    while (_myFramesPlayed < getNativeSampleRate()) {
        msleep(10);
        updateFramesPlayed();
    }
    return double(_myFramesPlayed)/getNativeSampleRate();
}

DirectSoundPump::DirectSoundPump () 
    : Pump(SF_F32, 1),
      _myIsOutOpen(false),
      _myDS(NULL),
      _myPrimaryBuffer(NULL),
      _myWriteCursor(0),
      _myNumUnderruns(0),
      _myFramesPlayed(0)
{
    AC_INFO << "DirectSoundPump::DirectSoundPump";

    _myFramesPerBuffer = unsigned(getLatency() * getNativeSampleRate())*2;

    setDeviceName("DirectSound default device");
    
    setCardName("");

    openOutput();
//    dumpState();
    start();
}

void
DirectSoundPump::openOutput() {
    AC_INFO << "DirectSoundPump::openOutput";
    if (isOutputOpen()) {
        AC_WARNING << "DirectSoundPump::openOutput: Device already open";
        return; 
    }
    HRESULT theRetVal;

    theRetVal = DirectSoundCreate8(NULL, &_myDS, NULL);
    checkDSRetVal(theRetVal, PLUS_FILE_LINE);

    // Determine window. This stuff is mostly bullshit because SetCooperativeLevel 
    // really doesn't do anything for WDM sound cards.
    HWND myWindow = NULL;
    myWindow = ::GetForegroundWindow();
    if (!myWindow) {
        myWindow = ::GetDesktopWindow();
    }
    if (myWindow) {
        theRetVal = _myDS->SetCooperativeLevel(myWindow, DSSCL_PRIORITY);
        checkDSRetVal(theRetVal, PLUS_FILE_LINE);
    } 
    ASSURE_MSG(myWindow, "Could not retrieve a window and hence could not set Cooperative level");
    ZeroMemory(&_myDSCaps, sizeof(DSCAPS));
    _myDSCaps.dwSize = sizeof(DSCAPS);
    theRetVal = _myDS->GetCaps(&_myDSCaps);
    checkDSRetVal(theRetVal, PLUS_FILE_LINE);

//    initPrimaryBuffer();
    initSecondaryBuffer();
    initNotification();

    _myOutputBuffer.init(_myFramesPerBuffer, getNumOutputChannels(), getNativeSampleRate());

    HRESULT hr;
    DWORD myPlayCursor;
    hr = _myDSBuffer->GetCurrentPosition(&myPlayCursor, &_myWriteCursor);
    checkDSRetVal (hr, PLUS_FILE_LINE);
    hr = _myDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
    checkDSRetVal (hr, PLUS_FILE_LINE);

    _myIsOutOpen = true;
}

void
DirectSoundPump::closeOutput() {
    AC_INFO << "DirectSoundPump::closeOutput";
    if (isOutputOpen()) {
        if (_myDSBuffer) {
            _myDSBuffer->Stop();
        }
        if (_myPrimaryBuffer) {
            _myPrimaryBuffer->Release();
            _myPrimaryBuffer = NULL;
        }
        if (_myDS) {
            _myDS->Release();
            _myDS = NULL;
        }
    }
    _myIsOutOpen = false;
}


bool 
DirectSoundPump::isOutputOpen() const {
    return _myIsOutOpen;
}

void DirectSoundPump::initPrimaryBuffer() {
    // This whole function is probably completely unnessesary for WDM driver model cards.
    // We're not calling it anymore anyway...
    HRESULT theRetVal;
    WAVEFORMATEX wfx;
    DSBUFFERDESC dsbdesc;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = getNumOutputChannels();
    wfx.nSamplesPerSec = getNativeSampleRate();
    wfx.wBitsPerSample = getBytesPerSample(getNativeSampleFormat())*8;
    wfx.nBlockAlign = 4;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize        = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags       = DSBCAPS_PRIMARYBUFFER; // all panning, mixing, etc done by synth
    dsbdesc.dwBufferBytes = 0;
    dsbdesc.lpwfxFormat   = NULL;
    theRetVal = _myDS->CreateSoundBuffer( &dsbdesc, &_myPrimaryBuffer, NULL);
    checkDSRetVal(theRetVal, PLUS_FILE_LINE);
    theRetVal = _myPrimaryBuffer->SetFormat(&wfx);
    checkDSRetVal(theRetVal, PLUS_FILE_LINE);
}

void DirectSoundPump::initSecondaryBuffer() {
  	WAVEFORMATEX* myWF = 0; 
    DSBUFFERDESC dsbdesc; 
    HRESULT hr; 
 
    switch (getNativeSampleFormat()) {
        case SF_S16:
        case SF_S8:
            {
                myWF = new WAVEFORMATEX;
                // Set up wave format structure. 
                memset(myWF, 0, sizeof(WAVEFORMATEX));
                myWF->wFormatTag = WAVE_FORMAT_PCM; 
                myWF->nChannels = getNumOutputChannels(); 
	            // This is really frames (samples per channel) per second...
                myWF->nSamplesPerSec = getNativeSampleRate(); 
                myWF->wBitsPerSample = getBytesPerSample(getNativeSampleFormat())*8; 
                myWF->nBlockAlign = getOutputBytesPerFrame(); 
                myWF->nAvgBytesPerSec = myWF->nSamplesPerSec * myWF->nBlockAlign;
                myWF->cbSize = sizeof(WAVEFORMATEX);
            }
            break;
        case SF_F32:
            {
                WAVEFORMATEXTENSIBLE * myFloatWF = new WAVEFORMATEXTENSIBLE;
//                WAVEFORMATIEEEFLOATEX * myFloatWF = new WAVEFORMATIEEEFLOATEX;
                // Set up wave format structure. 
                memset(myFloatWF, 0, sizeof(WAVEFORMATEXTENSIBLE));
                myFloatWF->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE; 
                myFloatWF->Format.nChannels = getNumOutputChannels(); 
	            // This is really frames (samples per channel) per second...
                myFloatWF->Format.nSamplesPerSec = getNativeSampleRate(); 
                myFloatWF->Format.wBitsPerSample = getBytesPerSample(getNativeSampleFormat())*8; 
                myFloatWF->Format.nBlockAlign = getOutputBytesPerFrame(); 
                myFloatWF->Format.nAvgBytesPerSec = myFloatWF->Format.nSamplesPerSec * 
                        myFloatWF->Format.nBlockAlign;
                myFloatWF->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE);
                myFloatWF->Samples.wValidBitsPerSample = getBytesPerSample(getNativeSampleFormat())*8;
                myFloatWF->dwChannelMask = 0; // SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT; ?!
                myFloatWF->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
                myWF = (WAVEFORMATEX *)myFloatWF;
            }
            break;
        default:
            ASSURE_MSG(false, "Illegal sample format in DSSampleSink::createDSBuffer");
    }
 
    // Set up DSBUFFERDESC structure. 
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 

    dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLPOSITIONNOTIFY | 
            DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STICKYFOCUS;
    dsbdesc.dwBufferBytes = _myFramesPerBuffer*getOutputBytesPerFrame();
    dsbdesc.lpwfxFormat = (LPWAVEFORMATEX)myWF; 
 
    // Create buffer. 
    IDirectSoundBuffer * myDSBuffer;
    hr = _myDS->CreateSoundBuffer(&dsbdesc, &myDSBuffer, NULL);
    checkDSRetVal(hr, PLUS_FILE_LINE);

    hr = myDSBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&_myDSBuffer);
    checkDSRetVal(hr, PLUS_FILE_LINE);
    myDSBuffer->Release();
    delete myWF;

    // Clear the buffer
    zeroDSBuffer();
}

void
DirectSoundPump::initNotification()
{
    LPDIRECTSOUNDNOTIFY8 myDsNotify; 
    HRESULT hr = _myDSBuffer->QueryInterface(IID_IDirectSoundNotify8, 
            (LPVOID *)&myDsNotify); 
    checkDSRetVal (hr, PLUS_FILE_LINE);

    _myWakeup0Handle = CreateEvent(NULL, FALSE, TRUE, NULL);
    _myWakeup1Handle = CreateEvent(NULL, FALSE, TRUE, NULL);
    _myWakeupOnStopHandle = CreateEvent(NULL, FALSE, TRUE, NULL);
    DSBPOSITIONNOTIFY PositionNotify[3];

    PositionNotify[0].dwOffset = 0;
    PositionNotify[0].hEventNotify = _myWakeup0Handle;
    PositionNotify[1].dwOffset = _myFramesPerBuffer*getOutputBytesPerFrame()/2;
    PositionNotify[1].hEventNotify = _myWakeup1Handle;
    PositionNotify[2].dwOffset = DSBPN_OFFSETSTOP;
    PositionNotify[2].hEventNotify = _myWakeupOnStopHandle;
    hr = myDsNotify->SetNotificationPositions(3, PositionNotify);
    checkDSRetVal (hr, PLUS_FILE_LINE);

    myDsNotify->Release();
}

void DirectSoundPump::zeroDSBuffer()
{
    HRESULT hr; 
    BYTE * myBufferData = NULL;
    DWORD myDataLen;
    unsigned myBytesToWrite = _myFramesPerBuffer*getOutputBytesPerFrame();
    
    hr = _myDSBuffer->Lock(0, myBytesToWrite, (LPVOID*) &myBufferData, &myDataLen, NULL, 0, 0);
    checkDSRetVal (hr, PLUS_FILE_LINE);
    ZeroMemory(myBufferData, myDataLen);
    hr = _myDSBuffer->Unlock(myBufferData, myDataLen, NULL, 0);
    checkDSRetVal (hr, PLUS_FILE_LINE);
    
}

void DirectSoundPump::pump()
{
    HANDLE myHandles[3];
    myHandles[0] = _myWakeup0Handle;
    myHandles[1] = _myWakeup1Handle;
    myHandles[2] = _myWakeupOnStopHandle;
    DWORD mySignaledIndex = WaitForMultipleObjects(3, myHandles, false, 1000);
        if (mySignaledIndex != 2) {
            // Beginning of buffer - increase time offset.
            addFramesToTime(_myFramesPerBuffer/2);
        }
    AutoLocker<Pump> myLocker(*this);
    if (mySignaledIndex == WAIT_TIMEOUT) {
        AC_WARNING << "Timeout while waiting for event from buffer";
    } else if (mySignaledIndex == WAIT_FAILED) {
        AC_WARNING << "Error in DirectSoundPump::pump():" 
                << DSoundMessages::WindowsError(GetLastError());
   } else if (mySignaledIndex == 2) {
       AC_DEBUG << "DirectSoundPump::pump: Stop signaled." << endl;
    } else {
        writeToDS();
    }
}

void DirectSoundPump::writeToDS() {
    HRESULT hr; 

    DWORD myPlayCursor;
    DWORD myWriteCursor;
    hr = _myDSBuffer->GetCurrentPosition(&myPlayCursor, &myWriteCursor);
    checkDSRetVal(hr, PLUS_FILE_LINE);

    unsigned numBytesToDeliver ;
    if (myPlayCursor > _myWriteCursor) {
        numBytesToDeliver = myPlayCursor-_myWriteCursor;
    } else {
        numBytesToDeliver = myPlayCursor+(_myFramesPerBuffer*getOutputBytesPerFrame())
                -_myWriteCursor;
    }
    AC_TRACE << "_myWriteCursor: " << _myWriteCursor << ", numBytesToDeliver: " 
            << numBytesToDeliver << endl;
    AC_TRACE << "Before mix: DS PlayCursor: " << myPlayCursor 
            << ", DS Write Cursor: " << myWriteCursor << endl;
    unsigned oldWriteCursor = _myWriteCursor;

    unsigned numFramesToDeliver = numBytesToDeliver/getOutputBytesPerFrame();
    // Obtain memory address of write block. This will be in two parts
    // if the block wraps around.
    void * myWritePtr1; 
    unsigned long myWriteBytes1; 
    void * myWritePtr2; 
    unsigned long myWriteBytes2; 

    mix(_myOutputBuffer, numFramesToDeliver);

    hr = _myDSBuffer->GetCurrentPosition(&myPlayCursor, &myWriteCursor);
    checkDSRetVal(hr, PLUS_FILE_LINE);
    AC_TRACE << "After mix: DS PlayCursor: " << myPlayCursor 
            << ", DS Write Cursor: " << myWriteCursor << endl;

//    MAKE_SCOPE_TIMER(DSBufferLock);
    hr = _myDSBuffer->Lock(_myWriteCursor, numBytesToDeliver, &myWritePtr1, 
            &myWriteBytes1, &myWritePtr2, &myWriteBytes2, 0);     
    // If the buffer was lost, restore and retry lock. 
    // (I don't think this ever happens with current drivers/os'es)
    if (DSERR_BUFFERLOST == hr) 
    {
        AC_DEBUG << "Lost buffer!";
        _myDSBuffer->Restore(); 
        hr = _myDSBuffer->Lock(_myWriteCursor, numBytesToDeliver, &myWritePtr1, 
                &myWriteBytes1, &myWritePtr2, &myWriteBytes2, 0); 
    }
    checkDSRetVal(hr, PLUS_FILE_LINE);
    
    // Copy data to DirectSound memory. 
    _myOutputBuffer.copyToRawMem(myWritePtr1, 0, myWriteBytes1);
    _myWriteCursor += myWriteBytes1;
    if (myWritePtr2 != 0) {
        _myOutputBuffer.copyToRawMem(myWritePtr2, myWriteBytes1, myWriteBytes2);
        _myWriteCursor += myWriteBytes2;
    }
    _myWriteCursor %= _myFramesPerBuffer*getOutputBytesPerFrame();

    // Release the data back to DirectSound. 
    hr = _myDSBuffer->Unlock(myWritePtr1, myWriteBytes1, myWritePtr2, myWriteBytes2);
    checkDSRetVal (hr, PLUS_FILE_LINE);     
    
    // Test for buffer underrun
    hr = _myDSBuffer->GetCurrentPosition(&myPlayCursor, &myWriteCursor);
    checkDSRetVal (hr, PLUS_FILE_LINE);     
    if (myPlayCursor < myWriteCursor) {
        // Nonusable part of buffer isn't split.
        if (oldWriteCursor < myWriteCursor && oldWriteCursor > myPlayCursor) {
            handleUnderrun();
        }
    } else {
        // Nonusable part of buffer is split.
        if (oldWriteCursor > myPlayCursor || oldWriteCursor < myWriteCursor) {
            handleUnderrun();
        }
    }
    updateFramesPlayed();
}

void DirectSoundPump::handleUnderrun() {
    _myNumUnderruns++;
    if (_myNumUnderruns == 1) {
        AC_WARNING << "DirectSound buffer underrun.";
    } else {
        AC_TRACE << "DirectSound buffer underrun.";
    }
}

void DirectSoundPump::updateFramesPlayed() {
    DWORD myPlayCursor;
    AutoLocker<ThreadLock> myLocker(_myTimeLock);
    HRESULT hr = _myDSBuffer->GetCurrentPosition(&myPlayCursor, 0);
    checkDSRetVal (hr, PLUS_FILE_LINE);
    int myDelta = (int)myPlayCursor/getOutputBytesPerFrame() 
            - int(_myFramesPlayed % _myFramesPerBuffer);
    if (myDelta < 0) {
        myDelta += _myFramesPerBuffer;
        AC_TRACE << "### Wrap around.";
    }
    _myFramesPlayed+=myDelta;
} 

void dumpDSCaps(const DSCAPS& theDSCaps) {
    AC_DEBUG << "DirectSound driver caps:";
    AC_DEBUG << "  Certified or WDM driver: " << 
        ((theDSCaps.dwFlags & DSCAPS_CERTIFIED) ? "true" : "false");
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_CONTINUOUSRATE ) ? "Supports" : "Does not support")
         << " variable sample rate playback.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_EMULDRIVER) ? "Is" : "Is not")
        << " being emulated with waveform audio functions.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_PRIMARY16BIT) ? "Supports" : "Does not support")
        << " 16-bit primary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_PRIMARY8BIT) ? "Supports" : "Does not support")
        << " 8-bit primary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_PRIMARYMONO) ? "Supports" : "Does not support")
        << " mono primary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_PRIMARYSTEREO) ? "Supports" : "Does not support")
        << " stereo primary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_SECONDARY16BIT ) ? "Supports" : "Does not support")
        << " 16-bit secondary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_SECONDARY8BIT ) ? "Supports" : "Does not support")
        << " 8-bit secondary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_SECONDARYMONO) ? "Supports" : "Does not support")
        << " mono secondary buffers.";
    AC_DEBUG << "  " << ((theDSCaps.dwFlags & DSCAPS_SECONDARYSTEREO) ? "Supports" : "Does not support")
        << " stereo secondary buffers.";
    AC_DEBUG << "  Minimum secondary buffer sample rate = " << theDSCaps.dwMinSecondarySampleRate;
    AC_DEBUG << "  Maximum secondary buffer sample rate = " << theDSCaps.dwMaxSecondarySampleRate;
    AC_DEBUG << "  Number of primary buffers = " << theDSCaps.dwPrimaryBuffers;
    AC_DEBUG << "  Maximum hardware buffers = " << theDSCaps.dwMaxHwMixingAllBuffers;
    AC_DEBUG << "  Maximum hardware static buffers = " << theDSCaps.dwMaxHwMixingStaticBuffers ;
    AC_DEBUG << "  Maximum hardware streaming buffers = " << theDSCaps.dwMaxHwMixingStreamingBuffers ;
    AC_DEBUG << "  Free hardware buffers = " << theDSCaps.dwFreeHwMixingAllBuffers ;
    AC_DEBUG << "  Free hardware static buffers = " << theDSCaps.dwFreeHwMixingStaticBuffers ;
    AC_DEBUG << "  Free hardware streaming buffers = " << theDSCaps.dwFreeHwMixingStreamingBuffers ;
    AC_DEBUG << "  Maximum hardware 3D buffers = " << theDSCaps.dwMaxHw3DAllBuffers ;
    AC_DEBUG << "  Maximum hardware 3D static buffers = " << theDSCaps.dwMaxHw3DStaticBuffers ;
    AC_DEBUG << "  Maximum hardware 3D streaming buffers = " << theDSCaps.dwMaxHw3DStreamingBuffers ;
    AC_DEBUG << "  Free hardware 3D buffers = " << theDSCaps.dwFreeHw3DAllBuffers ;
    AC_DEBUG << "  Free hardware 3D static buffers = " << theDSCaps.dwFreeHw3DStaticBuffers ;
    AC_DEBUG << "  Free hardware 3D streaming buffers = " << theDSCaps.dwFreeHw3DStreamingBuffers ;
    AC_DEBUG << "  Total hardware memory = " << theDSCaps.dwTotalHwMemBytes ;
    AC_DEBUG << "  Free hardware memory = " << theDSCaps.dwFreeHwMemBytes ;
    AC_DEBUG << "  Max contiguous free memory = " << theDSCaps.dwMaxContigFreeHwMemBytes ;
    AC_DEBUG << "  Hardware buffer data transfer rate = " << theDSCaps.dwUnlockTransferRateHwBuffers ;
    AC_DEBUG << "  CPU overhead for software buffers = " << theDSCaps.dwPlayCpuOverheadSwBuffers ;
}

}

