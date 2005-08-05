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

#ifndef INCL_AUDIO_ALSAPUMP
#define INCL_AUDIO_ALSAPUMP

#include "Pump.h"
#include "SampleFormat.h"
#include "HWSampleSink.h"

#include "AudioBuffer.h"

#include <asl/Singleton.h>

#include <string>
#include <alsa/asoundlib.h>

namespace asl {

class ALSAPump : public Singleton<ALSAPump>, public Pump
{
        friend class SingletonManager;
    public:
        virtual ~ALSAPump();
        
        virtual void dumpState() const;

    private:
        ALSAPump();
        void openOutput();
        void closeOutput();
        bool isOutputOpen() const;
        snd_pcm_t * openDevice(snd_pcm_stream_t theDeviceType);
        void setHWParams(snd_pcm_t * theDevice, int myNumChannels);
        void setSWParams (snd_pcm_t * theDevice);

        virtual void pump();
        void handleUnderrun (int err);

        bool _myIsOutOpen;

        snd_pcm_t * _myOutputDevice;
        unsigned _myFramesPerBuffer;

        AudioBuffer<float> _myOutputBuffer;

};

}

#endif 

