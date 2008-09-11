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

#ifndef INCL_AUDIO_COREAUDIOPUMP
#define INCL_AUDIO_COREAUDIOPUMP

#include "Pump.h"
#include "SampleFormat.h"
#include "HWSampleSink.h"
#include "AudioBuffer.h"

#include <asl/base/Singleton.h>

#include <string>
#include <vector>

#include <CoreServices/CoreServices.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>

namespace asl {

class CoreAudioPump : public Singleton<CoreAudioPump>, public Pump
{
        friend class SingletonManager;
    public:
        virtual ~CoreAudioPump();
        
        virtual Time getCurrentTime();

    private:
        CoreAudioPump();
        OSStatus setupAudioUnit();
        OSStatus matchAUFormats (AudioStreamBasicDescription *theDesc);
        OSStatus setupCallbacks();
        void cleanUp();
        static OSStatus fileRenderProc(void *inRefCon, 
                             AudioUnitRenderActionFlags *inActionFlags,
                             const AudioTimeStamp *inTimeStamp, 
                             UInt32 inBusNumber,
                             UInt32 inNumFrames, 
                             AudioBufferList *ioData);
        OSStatus pump(AudioUnitRenderActionFlags *inActionFlags,
                    const AudioTimeStamp *inTimeStamp, 
                    UInt32 inBusNumber,
                    UInt32 inNumFrames, 
                    AudioBufferList *ioData);
        
        unsigned _curFrame;
        AudioBuffer<float> _myOutputBuffer;
        AudioUnit _myOutputUnit;
        AURenderCallbackStruct  renderCallback;
        AudioStreamBasicDescription _myOutputDesc;
        unsigned _myBytesPerFrame;
};

}

#endif 

