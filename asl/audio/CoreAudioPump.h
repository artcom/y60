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

