/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
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
