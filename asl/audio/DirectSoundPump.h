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

#ifndef INCL_AUDIO_DIRECTSOUNDPUMP
#define INCL_AUDIO_DIRECTSOUNDPUMP

#include "Pump.h"
#include "SampleFormat.h"

#include "AudioBuffer.h"

#include <asl/base/Singleton.h>

#include <string>
#include <dsound.h>

namespace asl {

class DirectSoundPump : public Singleton<DirectSoundPump>, public Pump
{
        friend class SingletonManager;
    public:
        virtual ~DirectSoundPump();
        virtual void dumpState() const;
        virtual asl::Time getCurrentTime();

    private:
        DirectSoundPump();
        void openOutput();
		void handleDeviceSelection();
        void closeOutput();
        bool isOutputOpen() const;
        void initPrimaryBuffer();
        void initSecondaryBuffer();
        void zeroDSBuffer();
        void initNotification();

        virtual void pump();
        void writeToDS();
        void handleUnderrun();
        void updateFramesPlayed();

        bool _myIsOutOpen;

		LPDIRECTSOUND8 _myDS;
		DSCAPS _myDSCaps;
        LPDIRECTSOUNDBUFFER _myPrimaryBuffer;       // Primary buffer can't be DX8
        LPDIRECTSOUNDBUFFER8 _myDSBuffer;

        HANDLE _myWakeup0Handle;
        HANDLE _myWakeup1Handle;
        HANDLE _myWakeupOnStopHandle;

        unsigned _myFramesPerBuffer;
        DWORD _myWriteCursor;

        AudioBuffer<float> _myOutputBuffer;
        unsigned _myNumUnderruns;

        asl::ThreadLock _myTimeLock;
        Unsigned64 _myFramesPlayed;

		LPCGUID _mySoundCardId;
};

}

#endif 
