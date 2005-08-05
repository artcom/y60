//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: WinMMSoundCard.h,v $
//
//     $Author: thomas $
//
//   $Revision: 1.6 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_WINMMSOUNDCARD
#define INCL_WINMMSOUNDCARD

#include "SoundCard.h"
#include "WinWaveBuffer.h"

#include <asl/Time.h>
#include <asl/ThreadLock.h>

namespace AudioBase {

class WinMMSoundCard: public SoundCard {
public:
    WinMMSoundCard (int mySystemID);
    virtual ~WinMMSoundCard ();

    virtual void setParams(double myLatency, int mySampleRate, int mySampleBits, unsigned theNumChannels);

    virtual void openOutput();
    virtual void openInput(int myNumChannels);
    virtual void closeOutput();
    virtual void closeInput();
    
    virtual std::string asString() const;
    
    virtual void write(std::vector<AudioBuffer*>& theBuffers);
    virtual void read(std::vector<AudioBuffer*>& theBuffers);
    
    virtual int getNumUnderruns() const;
    virtual double getCurrentTime();

private:
    void initDevice ();

    void allocateBuffers(std::vector<WinWaveBufferPtr> & theBuffers, 
        int NumChannels, bool isOutput, HANDLE theDevice);
    void deallocateBuffers(std::vector<WinWaveBufferPtr> & theBuffers, 
         bool isOutput);

    static void CALLBACK outputWaveProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
            DWORD dwParam1, DWORD dwParam2);
    static void CALLBACK inputWaveProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
            DWORD dwParam1, DWORD dwParam2);
    
    static void handleWavError(MMRESULT theRetVal, const std::string& what, 
        const std::string& where);
    static std::string stringFromWavError(MMRESULT err);
    
    int        _mySystemID;
    HWAVEOUT   _myAudioOutDevice;
    HWAVEIN    _myAudioInDevice;
    
    std::vector<WinWaveBufferPtr> _myWaveOutBuffers;
    std::vector<WinWaveBufferPtr> _myWaveInBuffers;

    HANDLE     _myOutFreeBufferSem;
    HANDLE     _myInFreeBufferSem;

    asl::Signed64  _mySentBlocks;
    asl::Signed64  _myReadBlocks;
    double     _myLastFrameTime;

    int        _myCurOutputBuffer;
    int        _myCurInputBuffer;

    asl::ThreadLock _myTimeLock;
};

}
#endif
