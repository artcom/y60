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

#ifndef INCL_EFFECT
#define INCL_EFFECT

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
class Effect {
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
class NullEffect: public Effect {
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

