//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: DSoundSoundCard.cpp,v $
//
//     $Author: ulrich $
//
//   $Revision: 1.22 $
//
//
// Description:
//
//
//=============================================================================

#include "DSoundSoundCard.h"
#include "SoundException.h"
#include "AudioBuffer.h"


#define DEBUG_LEVEL 1

#include <asl/Auto.h>
#include <asl/Time.h>
#include <asl/numeric_functions.h>
#include <asl/string_functions.h>
#include <asl/Block.h>
#include <asl/assure.h>
#include <asl/Logger.h>

#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

#include <mmsystem.h>
#include <dsound.h>
#include <windows.h>
#include <Mmreg.h>
#include <KS.h>
#include <Ksmedia.h>
#include <math.h>

#include "DSoundMessages.h"

#define DB(x) // x

namespace AudioBase {

using namespace std;
using namespace asl;


DSoundSoundCard::DSoundSoundCard (LPCGUID myGuid, string name, HWND myWindow) :
    SoundCard(), _mySound(0), _myGuid(0), _mySecondaryBuffer(0), _isPlaying(false),
    _myPrimaryBuffer(0), _myBufferSize(0), _myNumUnderruns(0), _myWakeupHandles(0),
    _myGlobalPlayCursor(0), _myGlobalWriteCursor(0), _myNumFrames(0), _myGlobalPlayTime(0),
    _myAC3SourceBuffer(0), _isAC3Mode(false), _myBPS(0), /*_myLastPlayCursor(0),*/ _myPlayStartOffset(0)
{
    if (myGuid) {
        _myGuid = new GUID;
        memcpy((LPVOID)_myGuid, myGuid, sizeof(GUID));
    }
    //asl::Logger::get().setVerbosity(SEV_INFO);
    DSERRORHANDLER(DirectSoundCreate8(_myGuid, &_mySound, NULL));
    DSCAPS dscaps;
    ZeroMemory(&dscaps, sizeof(DSCAPS));
    dscaps.dwSize = sizeof(DSCAPS);
    DSERRORHANDLER(_mySound->GetCaps(&dscaps));
    // Determine window
    if (!myWindow) {
        myWindow = ::GetForegroundWindow();
        if (!myWindow) {
            myWindow = ::GetDesktopWindow();
        }
    }
    if (myWindow) {
        DSERRORHANDLER(_mySound->SetCooperativeLevel(myWindow, DSSCL_PRIORITY));
    } else {
        throw DirectSoundException(string("Could not retrieve a window and hence could not set Cooperative level"), PLUS_FILE_LINE);
    }
    DSERRORHANDLER(_mySound->SetSpeakerConfig(DSSPEAKER_5POINT1));
    stringstream s;
    s << myGuid;
    // XXX Get Max in channels
    unsigned maxInputs = 2;
    init(name, s.str(), 8, maxInputs);
}


DSoundSoundCard::~DSoundSoundCard() {
    if (_mySecondaryBuffer) {
        _mySecondaryBuffer->Stop();
    }
    if (_myGuid) {
        delete _myGuid;
        _myGuid = NULL;
    }
    if (_myAC3SourceBuffer) {
        delete [] _myAC3SourceBuffer;
        _myAC3SourceBuffer = NULL;
    }
    closeOutput(); // free secondbuffer and wakeuphandle
    closeInput();
    if (_myPrimaryBuffer) {
        _myPrimaryBuffer->Release();
        _myPrimaryBuffer = NULL;
    }
    if (_mySound) {
        _mySound->Release();
        _mySound = NULL;
    }
}

void DSoundSoundCard::initMultichannelBuffer(int myNumChannels)
{
    WAVEFORMATEXTENSIBLE waveFormatPCMEx;
    DSBUFFERDESC dsbdesc;
    LPDIRECTSOUNDBUFFER pDsb = NULL;
    unsigned myPacketSize = AudioBuffer::getNumSamples() * myNumChannels * getSampleSize();

    ZeroMemory(&waveFormatPCMEx, sizeof(WAVEFORMATPCMEX));
    WAVEFORMATEX & wfx = waveFormatPCMEx.Format;
    wfx.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wfx.nChannels = myNumChannels;
    wfx.nSamplesPerSec = getSampleRate();
    wfx.wBitsPerSample = getSampleBits();
    wfx.nBlockAlign = (wfx.wBitsPerSample / 8) * myNumChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = sizeof(waveFormatPCMEx) - sizeof(wfx);
    waveFormatPCMEx.Format = wfx;
    waveFormatPCMEx.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;  // Specify PCM
    waveFormatPCMEx.Samples.wValidBitsPerSample = wfx.wBitsPerSample;
    switch (myNumChannels) {
        case 1:
            waveFormatPCMEx.dwChannelMask = KSAUDIO_SPEAKER_MONO;
            break;
        case 2:
            waveFormatPCMEx.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
            break;
        case 4:
            waveFormatPCMEx.dwChannelMask = KSAUDIO_SPEAKER_QUAD;
            break;
        case 6:
            waveFormatPCMEx.dwChannelMask = KSAUDIO_SPEAKER_5POINT1;
            break;
        case 8:
            waveFormatPCMEx.dwChannelMask = KSAUDIO_SPEAKER_7POINT1;
            break;
        default:
            AC_WARNING << "Don't know how to handle Speakerconfig for " << myNumChannels << " Channels.";
            waveFormatPCMEx.dwChannelMask = KSAUDIO_SPEAKER_DIRECTOUT;
            break;
    }
    // Set up DSBUFFERDESC structure.
    ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;
    dsbdesc.dwBufferBytes = myPacketSize * _myNumFrames;//10*wfx.nAvgBytesPerSec;  // 3 Seconds buffer
    dsbdesc.lpwfxFormat = &wfx;
    // Create the Buffer
    _myBufferSize = dsbdesc.dwBufferBytes;
    DSERRORHANDLER(_mySound->CreateSoundBuffer(&dsbdesc, &pDsb, NULL));
    // Cast it to DX8
    HRESULT hr = pDsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*) &_mySecondaryBuffer);
    // Release old DX buffer
    pDsb->Release();
    DSERRORHANDLER(hr);

    // Clear the buffer
    // Lock the DS buffer
    BYTE * myBufferData = NULL;
    DWORD myDataLen;
    DSERRORHANDLER(_mySecondaryBuffer->Lock(0, _myBufferSize, (LPVOID*) &myBufferData, &myDataLen, NULL, 0, 0));
    // Zero the DS buffer
	ZeroMemory(myBufferData, myDataLen);
    // Unlock the DS buffer again
    DSERRORHANDLER(_mySecondaryBuffer->Unlock(myBufferData, myDataLen, NULL, 0));


    LPDIRECTSOUNDNOTIFY8 myNotifier;
    DSERRORHANDLER(_mySecondaryBuffer->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&myNotifier));
    DSBPOSITIONNOTIFY * PositionNotify = new DSBPOSITIONNOTIFY[_myNumFrames];
    _myWakeupHandles = new HANDLE[_myNumFrames];
    for (int i = 0; i < _myNumFrames; ++i) {
        _myWakeupHandles[i] = CreateEvent(NULL, FALSE, TRUE, NULL);
        PositionNotify[i].dwOffset = myPacketSize * i;
        PositionNotify[i].hEventNotify = _myWakeupHandles[i];
    }
    DSERRORHANDLER(myNotifier->SetNotificationPositions(_myNumFrames, PositionNotify));
    delete [] PositionNotify;
    myNotifier->Release();
    _isAC3Mode = false;
}

void DSoundSoundCard::initAC3Buffer(int myNumChannels) {
    WAVEFORMATEX wfx;
    LPDIRECTSOUNDBUFFER myDsb = NULL;
    DSBUFFERDESC dsbdesc;

    //DSERRORHANDLER(_mySound->SetSpeakerConfig(DSSPEAKER_DIRECTOUT));

    unsigned myPacketSize = AudioBuffer::getNumSamples() * getSampleSize() * 2;

    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.wFormatTag = WAVE_FORMAT_DOLBY_AC3_SPDIF;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = 4;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize        = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags       = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GLOBALFOCUS;
    dsbdesc.dwBufferBytes = myPacketSize * _myNumFrames;
    dsbdesc.lpwfxFormat   = &wfx;
    _myBufferSize = dsbdesc.dwBufferBytes;
    DSERRORHANDLER(_mySound->CreateSoundBuffer(&dsbdesc, &myDsb, NULL));
    // Cast it to DX8
    HRESULT hr = myDsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*) &_mySecondaryBuffer);
    // Release old DX buffer
    myDsb->Release();
    DSERRORHANDLER(hr);

    // Clear the buffer
    // Lock the DS buffer
    BYTE * myBufferData = NULL;
    DWORD myDataLen;
    DSERRORHANDLER(_mySecondaryBuffer->Lock(0, _myBufferSize, (LPVOID*) &myBufferData, &myDataLen, NULL, 0, 0));
    // Zero the DS buffer
	ZeroMemory(myBufferData, myDataLen);
    // Unlock the DS buffer again
    DSERRORHANDLER(_mySecondaryBuffer->Unlock(myBufferData, myDataLen, NULL, 0));


    LPDIRECTSOUNDNOTIFY8 myNotifier;
    DSERRORHANDLER(_mySecondaryBuffer->QueryInterface(IID_IDirectSoundNotify8, (LPVOID*)&myNotifier));
    _myWakeupHandles = new HANDLE[_myNumFrames];
    DSBPOSITIONNOTIFY * PositionNotify = new DSBPOSITIONNOTIFY[_myNumFrames];
    for (int i = 0; i < _myNumFrames; ++i) {
        _myWakeupHandles[i] = CreateEvent(NULL, FALSE, TRUE, NULL);
        PositionNotify[i].dwOffset = myPacketSize * i;
        PositionNotify[i].hEventNotify = _myWakeupHandles[i];
    }
    DSERRORHANDLER(myNotifier->SetNotificationPositions(_myNumFrames, PositionNotify));
    delete [] PositionNotify;
    myNotifier->Release();

    initAC3encoder(48000, 640000, myNumChannels);
    const AVCodecContext * myContext = getAC3Context();
    if (_myAC3SourceBuffer)
    {
        delete[] _myAC3SourceBuffer;
        _myAC3SourceBuffer = NULL;
    }
    _myAC3SourceBuffer = new asl::Signed16[myContext->frame_size * myContext->channels];
    _isAC3Mode = true;
}

void DSoundSoundCard::initPrimaryBuffer(int myNumChannels) {
    WAVEFORMATEX wfx;
    DSBUFFERDESC dsbdesc;
    ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_DOLBY_AC3_SPDIF;
    //wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 48000;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = 4;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    // -----------------------------------------------------------------------
    // Create primary buffer and set format just so we can specify our custom format.
    // Otherwise we would be stuck with the default which might be 8 bit or 22050 Hz.
    // Setup the primary buffer description
    ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize        = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags       = DSBCAPS_PRIMARYBUFFER; // all panning, mixing, etc done by synth
    dsbdesc.dwBufferBytes = 0;
    dsbdesc.lpwfxFormat   = NULL;
    // Create the buffer
    DSERRORHANDLER(_mySound->CreateSoundBuffer( &dsbdesc, &_myPrimaryBuffer, NULL));
    DSERRORHANDLER(_myPrimaryBuffer->SetFormat(&wfx));
}


void DSoundSoundCard::openOutput() {
    // compute _myNumFrames which is used later for buffer allocation
    
    _myBPS = getSampleRate() * getNumOutChannels() * getSampleSize();
    unsigned frameSize = getNumOutChannels() * getSampleSize() * AudioBuffer::getNumSamples();
    double bufferSize = ((double)_myBPS * getLatency()) + frameSize; // in samples
    _myNumFrames = DWORD(ceil(bufferSize / frameSize));

    // Compute the real latency
    double realBufferSize = _myNumFrames * frameSize;
    double myLatency = double(realBufferSize - frameSize) / double(_myBPS);
    setLatency(myLatency);

    if (!_isAC3Mode) {
        initMultichannelBuffer(getNumOutChannels());
    } else {
        initAC3Buffer(getNumOutChannels());
    }
    //initPrimaryBuffer(myNumChannels);
    // Let DSound set the starting write position because if we set it to zero, it looks like the
    // buffer is full to begin with. This causes a asl::Signed32 pause before sound starts when using large buffers.
    
    DSERRORHANDLER(_mySecondaryBuffer->GetCurrentPosition(&_myGlobalPlayCursor, &_myGlobalSafeCursor));
    _myPlayStartOffset = _myGlobalWriteCursor = _myGlobalSafeCursor;
    _isPlaying = false;

    SoundCard::openOutput();
    if (!_isAC3Mode) {
        AC_INFO << "DirectSound Soundcard opened for " << getNumOutChannels() << " channels.";
    } else {
        AC_INFO << "DirectSound Soundcard opened for " << getNumOutChannels() << " channels in AC3-Mode.";
    }
}

double DSoundSoundCard::getCurrentTime() {
    updateCursors();
    /*
    DWORD myPlayCursor;
    DWORD myReadCursor;

    _mySecondaryBuffer->GetCurrentPosition(&myPlayCursor, &myReadCursor);
    int myBytesPlayed = (int)myPlayCursor - (int)_myLastPlayCursor;
    if (myBytesPlayed < 0) {
        myBytesPlayed += _myBufferSize;
    }
    //cout << "BufferTime: " << _myBufferSize / (double)myBPS << endl;
    _myLastPlayCursor = myPlayCursor;

    _myGlobalPlayTime += (myBytesPlayed / (double) _myBPS);
    double myGlobalWriteTime = _myGlobalWriteCursor / (double) _myBPS;
    double myBufferTime = _myBufferSize / (double) _myBPS;
    cerr << "Playing: " << _isPlaying << ", Play: " << _myGlobalPlayTime << ", Write: " << myGlobalWriteTime << ", BufferTime=" << myBufferTime << ", Difference (write-play)=" << myGlobalWriteTime-_myGlobalPlayTime << endl;
    if (myGlobalWriteTime < _myGlobalPlayTime || myGlobalWriteTime > _myGlobalPlayTime+myBufferTime) {
        cerr << "### Warning CurrentTime flawed? BufferTime=" << myBufferTime << ", Difference (write-play)=" << myGlobalWriteTime-_myGlobalPlayTime << endl;
    }
    */
    if (_myGlobalWriteCursor < _myGlobalPlayCursor) {
        AC_WARNING << "Cursors flawed? Play > Write! Play: " << _myGlobalPlayCursor << ", Write: " << _myGlobalWriteCursor;
    }
    if (_myGlobalWriteCursor > _myGlobalSafeCursor + _myBufferSize) {
        AC_WARNING << "Cursors flawed? Write > Play+Buffer! Play: " << _myGlobalPlayCursor << ", Write: " << _myGlobalWriteCursor << ", Buffer: " << _myBufferSize;
    }
    return _myGlobalPlayCursor / double(_myBPS);
}

double DSoundSoundCard::getCurrentDelay() {
    updateCursors();
    return (_myGlobalWriteCursor - _myGlobalPlayCursor) / double(_myBPS);
}

int DSoundSoundCard::getNumUnderruns() const {
    return _myNumUnderruns;
}

void DSoundSoundCard::read(std::vector<AudioBuffer*>& theBuffers) {
}

std::string DSoundSoundCard::asString() const {
    return "";
}

void DSoundSoundCard::waitFree(unsigned theSize, unsigned theTimeout) {
    DWORD myDelay, myFreeBuffer;
    do {
        if (WAIT_TIMEOUT == WaitForMultipleObjects(_myNumFrames, _myWakeupHandles, FALSE, theTimeout)) {
            AC_WARNING << "Timeout while waiting for event from buffer";
        }
        updateCursors();
        //myDelay = (_myGlobalWriteCursor - _myGlobalPlayCursor);
        myFreeBuffer = _myGlobalSafeCursor + _myBufferSize - _myGlobalWriteCursor;
        //_mySecondaryBuffer->GetCurrentPosition(NULL, &mySafeCursor);
        //myDelay = (_myGlobalWriteCursor + _myBufferSize - mySafeCursor) % _myBufferSize;
        //myFreeBuffer = _myBufferSize - myDelay;
        if (myFreeBuffer < theSize && !_isPlaying) {
            AC_WARNING << "Buffer overrun prevention. Free: " << myFreeBuffer << " / " << theSize;
            play();
        }
    } while (myFreeBuffer < theSize);
}

void DSoundSoundCard::writeAC3encoded(vector<AudioBuffer*>& theBuffers) {
    DWORD mySize = AudioBuffer::getNumSamples() * 2 * sizeof(asl::Signed16); // Target Data
    // Wait for Event
    waitFree(mySize);

    unsigned myInputSamples = AudioBuffer::getNumSamples() * getNumOutChannels();
    const unsigned myAC3FrameBytes = 1536 * 2 * sizeof(asl::Signed16);    // 1536 Stereo Samples
    const AVCodecContext * myContext = getAC3Context();
    unsigned mySourceBufferSize = myContext->frame_size * myContext->channels;
    unsigned char myTargetBuffer[myAC3FrameBytes];
    unsigned myAC3Bytes = 0;

    // fill sourcebuffer with samples
    unsigned mySourceIndex = 0;
    unsigned myBufferSize = AudioBuffer::getNumSamples();
    for (int i=0; i<myBufferSize; ++i) {
        // XXX Channel order different in ffmpeg?
        // Order should be: L,C,R,SL,SR,LFE
        for (unsigned channel = 0; channel < getNumOutChannels(); ++channel) {
            // get sample data
            _myAC3SourceBuffer[mySourceIndex++] = theBuffers[channel]->getData16(i);
            // Buffer full? Then encode it and continue
            if (mySourceIndex == mySourceBufferSize) {
                // ac3encode
                unsigned myWrittenBytes = 0;
                // initialize buffer with zeros
                memset(myTargetBuffer, 0, myAC3FrameBytes);
                // leave 8 bytes free for header
                myWrittenBytes = encodeAC3frame(myTargetBuffer+8, myAC3FrameBytes-8, _myAC3SourceBuffer);
                if (myWrittenBytes & 1) {
                    // add padding byte
                    *(myTargetBuffer+8+myWrittenBytes) = 0;
                }
                // generate spdif header thing
                asl::Unsigned16 * longBuffer = (asl::Unsigned16*)myTargetBuffer;
                // This seems correct endianess for intel architecture at least
                longBuffer[0] = (asl::Signed16)0xf872;	        /* Pa  sync word 1 */
                longBuffer[1] = 0x4e1f;		        /* Pb  sync word 2 */
                longBuffer[2] = 0x0001;		        /* Pc  burst-info (data-type = AC3) */
                longBuffer[3] = myWrittenBytes*8;		/* Pd  length-code (bits) */
                // bytesex conversion
                for (int j=0; j < (myWrittenBytes+1)/2; ++j)
                {
                    asl::Unsigned16 s = ((myTargetBuffer[2*j+8] << 8) & 0xFF00) | ((myTargetBuffer[2*j+9] & 0xff) & 0xFF);
                    longBuffer[j+4] = s;
                }
                // Padding to 1536 stereo pcm samples, buffer was initialized with zeros,
                // so just shift the pointer

                // write to DirectSound Buffer
                writeBuffer(myTargetBuffer, myAC3FrameBytes);
                mySourceIndex = 0;
            }
        }
    }
    // Encode last crap
    // XXX not enough input data
    if (mySourceIndex > 0) {
        // XXX
        throw;
    }
    DWORD mySafeCursor;
    DWORD myDelay, myFreeBuffer;
    // Lock the sound buffer
    DWORD myPlayCursor;
    _mySecondaryBuffer->GetCurrentPosition(&myPlayCursor, &mySafeCursor);
    // Compute Delay in Bytes, assume PlayCursor is always behind WriteOffset
    myDelay = (_myGlobalWriteCursor - _myGlobalPlayCursor);
    myFreeBuffer = _myBufferSize - myDelay;
    if (!_isPlaying && myDelay >= mySize) { // XXX
        // Start playing if buffer has enough data and it hasn't yet been started
        play();
    }
    // call updateCursors to ensure wrapping buffers are gotten right
    updateCursors();
}

void DSoundSoundCard::writeBuffer(unsigned char* theData, unsigned theNumBytes)
{
    LPBYTE myBuf1 = NULL;
    LPBYTE myBuf2 = NULL;
    DWORD mySize1 = 0;
    DWORD mySize2 = 0;
    DWORD myByteswritten = 0;
    HRESULT hr;

    DWORD myWriteOffset = _myGlobalWriteCursor % _myBufferSize;
    hr = _mySecondaryBuffer->Lock(myWriteOffset, theNumBytes, (LPVOID*)&myBuf1, &mySize1,
        (LPVOID*)&myBuf2, &mySize2, 0);
    if (DSERR_BUFFERLOST == hr)
    {
        // Buffer got lost. Restore and retry.
        AC_WARNING << "Buffer got lost. Restore and retry.";
        _mySecondaryBuffer->Restore();
        hr = _mySecondaryBuffer->Lock(myWriteOffset, theNumBytes, (LPVOID*)&myBuf1, &mySize1, (LPVOID*)&myBuf2, &mySize2, 0);
    }
    if (SUCCEEDED(hr)) {
        memcpy(myBuf1, theData, mySize1);
        myByteswritten = mySize1;
        if (theNumBytes > mySize1 && mySize2 > 0) {
            memcpy(myBuf2, theData+mySize1, mySize2);
            myByteswritten += mySize2;
        }
        _mySecondaryBuffer->Unlock((LPVOID)myBuf1, mySize1, (LPVOID)myBuf2, mySize2);
        if (myByteswritten != theNumBytes) {
            AC_WARNING << "Wrong number of bytes written?!";
        }
        if (myWriteOffset + myByteswritten > _myBufferSize) {
            AC_TRACE << "Write wrapped.";
        }
        _myGlobalWriteCursor += myByteswritten;
    } else {
        AC_ERROR << "Writing failed. Error was: " << DSoundMessages::ErrorMessage(hr);
        // XXX What now?
    }
}

void DSoundSoundCard::write(vector<AudioBuffer*>& theBuffers) {
    if (_isAC3Mode) {
        // redirect
        writeAC3encoded(theBuffers);
        return;
    }

    HRESULT hr;
    LPBYTE myBuf1 = NULL;
    LPBYTE myBuf2 = NULL;
    DWORD mySize1 = 0;
    DWORD mySize2 = 0;
    DWORD myByteswritten1 = 0;
    DWORD myByteswritten2 = 0;
    unsigned sampleSize = getSampleSize();
    double dFactor = (double) ((1 << (getSampleBits()-1))-1);
    //DWORD mySafeCursor;

    DWORD mySize = AudioBuffer::getNumSamples() * getNumOutChannels() * sampleSize;
    // Wait for Event
    waitFree(mySize);
    // Lock the sound buffer
    DWORD myWriteOffset = _myGlobalWriteCursor % _myBufferSize;
    hr = _mySecondaryBuffer->Lock(myWriteOffset, mySize, (LPVOID*)&myBuf1, &mySize1,
                                                         (LPVOID*)&myBuf2, &mySize2, 0);
    if (DSERR_BUFFERLOST == hr)
    {
        // Buffer got lost. Restore and retry.
        AC_WARNING << "Buffer got lost. Restore and retry.";
        _mySecondaryBuffer->Restore();
        hr = _mySecondaryBuffer->Lock(myWriteOffset, mySize, (LPVOID*)&myBuf1, &mySize1, (LPVOID*)&myBuf2, &mySize2, 0);
    }
    if (SUCCEEDED(hr)) {
        unsigned myBufferSize = AudioBuffer::getNumSamples();
        for (int i=0; i<myBufferSize; ++i) {
            for (unsigned channel = 0; channel < getNumOutChannels(); ++channel) {
                double sampleData = (theBuffers[channel]->getData()[i]);
                double dataformatted = sampleData * dFactor;
                int iData = int(dataformatted);
                if (myByteswritten1 < mySize1 && myBuf1 != NULL) {
                    // XXX Byte Order?
                    memcpy(&myBuf1[myByteswritten1], &iData, sampleSize);
                    myByteswritten1 += sampleSize;
                }
                else if (myByteswritten2 < mySize2 && myBuf2 != NULL) {
                    // XXX Byte Order?
                    memcpy(&myBuf2[myByteswritten2], &iData, sampleSize);
                    myByteswritten2 += sampleSize;
                }
                else {
                    AC_ERROR << "Buffer overrun";
                }
            }
        }
        _mySecondaryBuffer->Unlock((LPVOID)myBuf1, myByteswritten1, (LPVOID)myBuf2, myByteswritten2);
        if (myByteswritten1 + myByteswritten2 != mySize) {
            AC_WARNING << "Wrong number of bytes written?!";
        }
        if (myWriteOffset + myByteswritten1 + myByteswritten2 > _myBufferSize) {
            AC_TRACE << "DirectSound Write wrapped.";
        }
        _myGlobalWriteCursor += myByteswritten1 + myByteswritten2;

        if (!_isPlaying) {
            play();
        }
    } else {
        AC_WARNING << "Writing failed. Error was: " << DSoundMessages::ErrorMessage(hr);
        // XXX What now?
    }
    updateCursors();
}

void
DSoundSoundCard::closeOutput() {
    stop();
    if (_myWakeupHandles) {
        for (unsigned i = 0; i < _myNumFrames; ++i) {
            CloseHandle(_myWakeupHandles[i]);
            _myWakeupHandles[i] = NULL;
        }
        delete [] _myWakeupHandles;
        _myWakeupHandles = NULL;
    }
    if (_mySecondaryBuffer) {
        _mySecondaryBuffer->Release();
        _mySecondaryBuffer = NULL;
    }
    if (_myPrimaryBuffer) {
        _myPrimaryBuffer->Stop();
        _myPrimaryBuffer->Release();
        _myPrimaryBuffer = NULL;
    }
    if (_isAC3Mode) {
        stopAC3encoder();
        _isAC3Mode = false;
    }
    if (_myAC3SourceBuffer) {
        delete [] _myAC3SourceBuffer;
        _myAC3SourceBuffer = NULL;
    }
    SoundCard::closeOutput();
}

void
DSoundSoundCard::closeInput() {
}

void
DSoundSoundCard::openInput(int myNumChannels) {
}

void DSoundSoundCard::play() {
    if (!_isPlaying) {
        _isPlaying = true;
        AC_INFO << "Starting Playback";
        _mySecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
        updateCursors();
        //DWORD dwPlayCursor;
        //_mySecondaryBuffer->GetCurrentPosition(&dwPlayCursor, NULL);
        //_myLastPlayCursor = dwPlayCursor;
    }
}

void DSoundSoundCard::stop() {
    if (_mySecondaryBuffer && _isPlaying) {
        _mySecondaryBuffer->Stop();
    }
    _isPlaying = false;
}

void DSoundSoundCard::setAC3EncodeFlag(bool theFlag) {
    if (isOutputOpen()) {
        throw DirectSoundException("Can't set AC3 Flag on open output", PLUS_FILE_LINE);
    } else {
        _isAC3Mode = theFlag;
    }
}

bool DSoundSoundCard::getAC3EncodeFlag() const {
    return _isAC3Mode;
}

void DSoundSoundCard::updateCursors() {
    asl::AutoLocker<DSoundSoundCard> myLocker(*this); // protect shared vars 
    //ASSURE(_mySecondaryBuffer);
    DWORD myPlayCursor, mySafeCursor;
    _mySecondaryBuffer->GetCurrentPosition(&myPlayCursor, &mySafeCursor);

    // We assume the playcursor always goes forward
    int myDelta = (int)myPlayCursor - int(_myGlobalPlayCursor % _myBufferSize);
    if (myDelta < 0) {
        myDelta += _myBufferSize;
    }
    //ASSURE(myDelta >= 0);
    _myGlobalPlayCursor += myDelta;

    myDelta = (int)mySafeCursor - int(_myGlobalSafeCursor % _myBufferSize);
    if (myDelta < 0) {
        myDelta += _myBufferSize;
    }
    //ASSURE(myDelta >= 0);
    _myGlobalSafeCursor += myDelta;
    AC_TRACE << "Playing: " << _isPlaying << " | GlobalPlay=" << _myGlobalPlayCursor << " | GlobalSafe=" << _myGlobalSafeCursor << " | GlobalWrite=" << _myGlobalWriteCursor;
}


} // namespace AudioBase
