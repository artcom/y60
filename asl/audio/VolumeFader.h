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

#ifndef INCL_ASL_VOLUMEFADER_H
#define INCL_ASL_VOLUMEFADER_H

#include "Effect.h"

#include <asl/settings.h>
#include <asl/Ptr.h>

namespace asl {

class VolumeFader: public Effect {
public:
    static const unsigned DEFAULT_FADE_FRAMES = 200;
    VolumeFader(SampleFormat theSampleFormat); 

    void setVolume(float theTargetVolume, unsigned theFadeDurationFrames = DEFAULT_FADE_FRAMES);
    float getVolume() const;
    float getVolume(Unsigned64 theFrame) const;

private:
    template <class SAMPLE>
    class VolumeFaderFunctor : public EffectFunctor<SAMPLE> {
        private:
            virtual void operator()(Effect* theEffect, AudioBuffer<SAMPLE> & theBuffer, 
                    Unsigned64 theAbsoluteFrame) {
                VolumeFader * myFader = dynamic_cast<VolumeFader *>(theEffect);
                ASSURE(myFader);

                float myVolumeDiff = myFader->_myEndVolume-myFader->_myBeginVolume;
                float myFadeDuration = float(myFader->_myFadeEndFrame - myFader->_myFadeBeginFrame);

                SAMPLE * curSample = theBuffer.begin();
                Unsigned64 lastFrame = std::min (theAbsoluteFrame+theBuffer.getNumFrames(), 
                        myFader->_myFadeEndFrame);
                
                for (myFader->_myCurrentFrame = theAbsoluteFrame;
                        myFader->_myCurrentFrame<lastFrame;
                        ++(myFader->_myCurrentFrame))
                {
                    float myFadePercent = 1.0;
                    if (myFadeDuration) {
                        myFadePercent = (myFader->_myCurrentFrame - 
                                         myFader->_myFadeBeginFrame)/myFadeDuration;
                    }
                    myFader->_myCurrentVolume = myFader->_myBeginVolume+
                            myFadePercent*myVolumeDiff;
                    for (int myChannel = 0; myChannel<theBuffer.getNumChannels(); 
                            ++myChannel) 
                    {
                        *curSample = (SAMPLE)((*curSample) * myFader->_myCurrentVolume);
                        curSample++;
                    }
                }
                myFader->_myCurrentVolume = myFader->_myEndVolume;
                lastFrame = theAbsoluteFrame+theBuffer.getNumFrames();
                for (; curSample < theBuffer.end(); ++curSample) {
                    *curSample = (SAMPLE)((*curSample) * myFader->_myCurrentVolume);
                }
                myFader->_myCurrentFrame=lastFrame;
            }
    };

    float _myCurrentVolume;
    Unsigned64 _myCurrentFrame;

    float _myBeginVolume, _myEndVolume;
    Unsigned64 _myFadeBeginFrame, _myFadeEndFrame;
};

typedef Ptr<VolumeFader, MultiProcessor, PtrHeapAllocator<MultiProcessor> > VolumeFaderPtr;
    
} // namespace

#endif
