//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: ALSASoundCard.h,v $
//
//     $Author: ulrich $
//
//   $Revision: 1.4 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_ALSASOUNDCARD
#define INCL_ALSASOUNDCARD

#include "SoundCard.h"
#include <alsa/asoundlib.h>

typedef struct _snd_pcm snd_pcm_t;

#include <asl/Time.h>
#include <asl/ThreadLock.h>

namespace AudioBase {

class ALSASoundCard: public SoundCard {
public:
    ALSASoundCard (int myIndex);
    virtual ~ALSASoundCard ();

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
    snd_pcm_t *
    openDevice(snd_pcm_stream_t theDeviceType);

    void allocateInputBuffers();
    void deallocateInputBuffers();

    void handleUnderrun (int err);
    void handleOverrun (int err);

    int _myIndex;
    snd_pcm_t *         _myOutputDevice;   
    snd_pcm_t *         _myInputDevice;   

    int                 _myNumUnderruns;
    int                 _myNumOverruns;

    asl::Signed64 _mySentBlocks;
    asl::Signed64 _myReadBlocks;

    double    _myLastTime;
    asl::Time _myLastFrameTime;
    double    _myMaxFrameTime;
    double    _myLatency;
    int       _myPeriodSize;

    asl::Signed32 ** _myInputChannels;

    asl::ThreadLock _myTimeLock;

    void setHWParams (snd_pcm_t * theDevice, int myNumChannels);
    void setSWParams (snd_pcm_t * theDevice);
    void outputSWParams (snd_pcm_t * theDevice);
};

}
#endif
