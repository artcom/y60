//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: DSoundSoundCard.h,v $
//
//     $Author: thomas $
//
//   $Revision: 1.16 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_DSOUNDSOUNDCARD
#define INCL_DSOUNDSOUNDCARD

#include "SoundCard.h"
#include <dsound.h>

#include <asl/Time.h>
#include <asl/ThreadLock.h>

namespace AudioBase {


/**
 * @ingroup Y60audioaudiobase
 *
 */
class DSoundSoundCard: public SoundCard {
public:
    /**
     * Constructs a DirectSound 8 soundcard object. 
     * @param myGuid GUID of the soundcard as retrieved from DirectSoundEnumerate
     *               in SoundCardManager, or NULL if the default soundcard should
     *               be used.
     * @param name Name of the Soundcard
     * @param myWindow Window Handle to the applications window. A value of 0 does
                       autodetect.
     */
    DSoundSoundCard (LPCGUID myGuid, std::string name, HWND myWindow = 0);
    virtual ~DSoundSoundCard ();

    virtual void openOutput();
    virtual void openInput(int myNumChannels);
    virtual void closeOutput();
    virtual void closeInput();
    virtual int getNumUnderruns() const;
    virtual double getCurrentTime();
    
    virtual std::string asString() const;
    
    virtual void write(std::vector<AudioBuffer*>& theBuffers);
    void writeAC3encoded(std::vector<AudioBuffer*>& theBuffers);
    virtual void read(std::vector<AudioBuffer*>& theBuffers);    
    void play();
    void stop();
    /**
     * (de-)/activates realtime AC3 encoding.
     */
    void setAC3EncodeFlag(bool theFlag);
    bool getAC3EncodeFlag() const;
    //
    // Lockable interface
    //
    void lock() {
        _myLock.lock();
    }
    void unlock() {
        _myLock.unlock();
    }
    private:
        asl::ThreadLock    _myLock;

    /**
     * @return the current delay between a byte written by this objects writeBuffer() function
     *         and its playback.
     */
    virtual double getCurrentDelay();

    /**
     * Initializes a secondary soundbuffer in plain multichannel mode
     * @param myNumChannels number of channels to play back on that buffer.
     */
    void initMultichannelBuffer(int myNumChannels);
    /**
     * Initializes a secondary soundbuffer in AC3 mode.
     * @param myNumChannels number of channels to initialize. This should be the 
     *        number of channels the input stream has. The resulting AC3 data
     *        stream is embedded in a stereo pcm signal.
     */
    void initAC3Buffer(int myNumChannels);
    void initPrimaryBuffer(int myNumChannels);
    void writeBuffer(unsigned char* theData, unsigned theNumBytes);
    /**
     * Waits until theSize bytes are free in the buffer or theTimeout has elapsed.
     * @param theSize number of bytes that need to be free to finish
     * @param theTimeout maximum timeout
     */
    void waitFree(unsigned theSize, unsigned theTimeout = 1000);
    /**
     *  Updates the global DirectSound Cursors. This should be called regularly
     *  so that it can safely detect cursor wraps. 
     */
    void updateCursors();
    LPDIRECTSOUND8 _mySound;
    LPDIRECTSOUNDBUFFER8 _mySecondaryBuffer;
    LPDIRECTSOUNDBUFFER _myPrimaryBuffer;       // Primarybuffer can't be DX8
    double _myGlobalPlayTime;
    DWORD _myGlobalWriteCursor;
    DWORD _myBufferSize;
    unsigned _myNumUnderruns;
    LPCGUID _myGuid;
    bool _isPlaying;
    DWORD _myNumFrames;
    HANDLE *_myWakeupHandles;
    asl::Signed16 * _myAC3SourceBuffer;
    DWORD _myBPS;
    //int _myLastPlayCursor;
    bool _isAC3Mode;
    DWORD _myGlobalPlayCursor;
    DWORD _myGlobalSafeCursor;
    int _myPlayStartOffset;
};

}
#endif
