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

#ifndef INCL_ASL_VOLUMEFADER_H
#define INCL_ASL_VOLUMEFADER_H

#include "asl_audio_settings.h"

#include "Effect.h"

#include <asl/base/settings.h>
#include <asl/base/Ptr.h>

namespace asl {

class ASL_AUDIO_DECL VolumeFader: public Effect {
public:
    static const unsigned DEFAULT_FADE_FRAMES = 200;
    // Note: this isn't really tested with anything other than channel count = 2
    VolumeFader(SampleFormat theSampleFormat, unsigned int theChannelCount = 2);

    void setVolume(float theTargetVolume, unsigned theFadeDurationFrames = DEFAULT_FADE_FRAMES);
    void setVolumes(std::vector<float> theTargetVolumes, unsigned theFadeDurationFrames = DEFAULT_FADE_FRAMES);
    float getVolume() const;
    void getVolumes(std::vector<float> & theVolumes) const;
    float getVolume(Unsigned64 theFrame) const;
    void setChannelCount(unsigned int theCount);
private:
    template <class SAMPLE>
    class VolumeFaderFunctor : public EffectFunctor<SAMPLE> {
        private:
            virtual void operator()(Effect* theEffect, AudioBuffer<SAMPLE> & theBuffer,
                    Unsigned64 theAbsoluteFrame) {
                VolumeFader * myFader = dynamic_cast<VolumeFader *>(theEffect);
                ASSURE(myFader);

                std::vector<float> myVolumeDiffs(theBuffer.getNumChannels());
                for (unsigned i = 0; i < theBuffer.getNumChannels(); ++i) {
                    myVolumeDiffs[i] = myFader->_myEndVolumes[i] - myFader->_myBeginVolumes[i];
                }
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
                    for (unsigned i = 0; i < theBuffer.getNumChannels(); ++i)
                    {
                        myFader->_myCurrentVolumes[i] = myFader->_myBeginVolumes[i]+
                            myFadePercent*myVolumeDiffs[i];
                        *curSample = (SAMPLE)((*curSample) * myFader->_myCurrentVolumes[i]);
                        curSample++;
                    }
                }
                for (unsigned i = 0; i < theBuffer.getNumChannels(); ++i) {
                    myFader->_myCurrentVolumes[i] = myFader->_myEndVolumes[i];
                }
                while (curSample != theBuffer.end()) {
                    for (unsigned i = 0; i < theBuffer.getNumChannels(); ++i) {
                        *curSample = (SAMPLE)((*curSample) * myFader->_myCurrentVolumes[i]);
                        ++curSample;
                    }
                }
                lastFrame = theAbsoluteFrame+theBuffer.getNumFrames();
                myFader->_myCurrentFrame=lastFrame;
            }
    };
    void getVolumes(Unsigned64 theFrame, std::vector<float> & theVolumes) const;
    std::vector<float> _myCurrentVolumes;
    Unsigned64 _myCurrentFrame;

    std::vector<float> _myBeginVolumes, _myEndVolumes;
    Unsigned64 _myFadeBeginFrame, _myFadeEndFrame;
};

typedef Ptr<VolumeFader, MultiProcessor, PtrHeapAllocator<MultiProcessor> > VolumeFaderPtr;

} // namespace

#endif
