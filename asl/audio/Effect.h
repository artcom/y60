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

#ifndef INCL_EFFECT
#define INCL_EFFECT

#include "asl_audio_settings.h"

#include "AudioBuffer.h"
#include "SampleFormat.h"

namespace asl {

class Effect;
    
class EffectFunctorBase {
    public:
        virtual void operator()(Effect * theEffect, AudioBufferBase & theBuffer, 
                Unsigned64 theAbsoluteFrame) = 0;
};

/**
 * Mostly-abstract base class for Effects.
 */
class ASL_AUDIO_EXPORT Effect {
public:
    Effect(asl::Ptr<EffectFunctorBase> myFunctor);
    virtual ~Effect() {}
    
    virtual void apply(AudioBufferBase &, Unsigned64 theAbsoluteFrame);
    
private:
    asl::Ptr<EffectFunctorBase> _myFunctor;
};


/**
 * 
 */
template <class SAMPLE>
class EffectFunctor: public EffectFunctorBase {
public:
    void operator()(Effect * theEffect, AudioBufferBase & theBuffer,
            Unsigned64 theAbsoluteFrame) 
    {
        AudioBuffer<SAMPLE> * myBuffer = dynamic_cast<AudioBuffer<SAMPLE>*>(&theBuffer);
        if (myBuffer) {
            (*this)(theEffect, *myBuffer, theAbsoluteFrame);
        } else {
            AC_FATAL << "Failed to cast to AudioBuffer<SAMPLE>";
        }
    }

private:
    virtual void operator()(Effect * theEffect, AudioBuffer<SAMPLE> & theBuffer, 
            Unsigned64 theAbsoluteFrame) = 0;
};

/**
 * Functor factory.
 */
template <template<class> class EFFECTFUNCTOR>
asl::Ptr<EffectFunctorBase> createEffectFunctor(SampleFormat theSampleFormat) {
    switch (theSampleFormat) {
        case SF_S16:
            return asl::Ptr<EffectFunctorBase>(new EFFECTFUNCTOR<short>());
        case SF_F32:
            return asl::Ptr<EffectFunctorBase>(new EFFECTFUNCTOR<float>());
        default:
            AC_FATAL << "Unsupported SampleFormat for Effectfunctor: " << theSampleFormat;
            return asl::Ptr<EffectFunctorBase>(0);
    }
}

/**
 * NullEffect that effectively does not affect anybody.
 */
class ASL_AUDIO_EXPORT NullEffect: public Effect {
public:
    NullEffect(SampleFormat theSampleFormat);
        
private:
    template <class SAMPLE>
        class NullEffectFunctor : public EffectFunctor<SAMPLE> {
            protected:
                virtual void operator()(Effect* theEffect, AudioBuffer<SAMPLE> & theBuffer, 
                        Unsigned64 theAbsoluteFrame) {
                }
        };
};

}

#endif

