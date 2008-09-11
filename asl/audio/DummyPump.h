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

#ifndef INCL_AUDIO_DUMMYPUMP
#define INCL_AUDIO_DUMMYPUMP

#include "Pump.h"
#include "SampleFormat.h"
#include "HWSampleSink.h"
#include "AudioBuffer.h"

#include <asl/base/Singleton.h>

#include <string>
#include <vector>

namespace asl {

class DummyPump : public Singleton<DummyPump>, public Pump
{
        friend class SingletonManager;
    public:
        virtual ~DummyPump();
        
        virtual Time getCurrentTime();

    private:
        DummyPump();
        virtual void pump();
        
        unsigned _curFrame;
        AudioBuffer<float> _myOutputBuffer;
};

}

#endif 

