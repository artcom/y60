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

#include "Effect.h"

namespace asl {

Effect::Effect(asl::Ptr<EffectFunctorBase> myFunctor) 
    : _myFunctor(myFunctor)
{
}
    
void Effect::apply(AudioBufferBase & theBuffer, Unsigned64 theAbsoluteFrame) {
    (*_myFunctor)(this, theBuffer, theAbsoluteFrame);
}

NullEffect::NullEffect(SampleFormat theSampleFormat)
    : Effect(createEffectFunctor<NullEffectFunctor>(theSampleFormat))
{
}

} // namespace

