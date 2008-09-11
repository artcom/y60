//=============================================================================
//
// Copyright (C) 1993-2007, ART+COM AG
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

#ifndef INCL_Y60_RESAMPLER_H
#define INCL_Y60_RESAMPLER_H

#include <asl/audio/Effect.h>
#include <asl/base/settings.h>
#include <asl/base/Ptr.h>

namespace y60 {

    class Resampler : public asl::Effect {

    public:

        Resampler(asl::SampleFormat theSampleFormat) 
            : asl::Effect(asl::createEffectFunctor<ResamplerFunctor>(theSampleFormat)),
              _myTargetFrames(441)
              //              _myRatio(1.0f)
        {};

//         void setRatio(float theRatio) {
//             _myRatio = theRatio;
//         }
//         float getRatio() {
//             return _myRatio;
//         }

        void setTargetFrames(unsigned theTargetFrames) {
            _myTargetFrames = theTargetFrames;
        }

        unsigned getTargetFrames() {
            return _myTargetFrames;
        }

    private:

        template <class SAMPLE>
        class ResamplerFunctor : public asl::EffectFunctor<SAMPLE> {
        private:
            virtual void operator()(asl::Effect* theEffect, asl::AudioBuffer<SAMPLE> & theBuffer,
                                    asl::Unsigned64 theAbsoluteFrame) {

                Resampler * myResampler = dynamic_cast<Resampler* >(theEffect);

                unsigned myOldNumFrames = theBuffer.getNumFrames();
                //                unsigned myNewNumFrames = (unsigned)(myOldNumFrames * (1.f/myResampler->getRatio()));
                unsigned myNewNumFrames = myResampler->getTargetFrames();
                asl::AudioBuffer<SAMPLE> theTempBuffer = asl::AudioBuffer<SAMPLE>(myNewNumFrames, theBuffer.getNumChannels(), 
                                                                                  theBuffer.getSampleRate());

                SAMPLE* curSample = theTempBuffer.begin();
                unsigned myNumChannels = theBuffer.getNumChannels();
                
                for (unsigned myCurrentFrame = 0; myCurrentFrame < myNewNumFrames-1; myCurrentFrame++) {
                    for (unsigned myChannel = 0; myChannel < myNumChannels; myChannel++) {
                        float myPos = (float)myCurrentFrame / (float)myNewNumFrames;
                        float myPosFrame = myPos * myOldNumFrames;
                        unsigned myLowFrame = (unsigned)floor(myPosFrame);
                        float myOffset = myPosFrame - (float)myLowFrame;

                        //AC_INFO << "myPos: " << myPos << " myPosFrame: " << myPosFrame << " myLoFrame:  " << myLowFrame << " myOffset: " << myOffset;
                        // linear interpolation (XXXX TODO: better resampling with lowpass filter)
                        SAMPLE * lowSample = theBuffer.begin() + myLowFrame * myNumChannels + myChannel;
                        SAMPLE * upSample = lowSample + myNumChannels;
                        *curSample++ = (SAMPLE)((*lowSample) + (*upSample - *lowSample) * myOffset); //linear interpolation
                    }
                }
                for (unsigned myChannel = 0; myChannel < myNumChannels; myChannel++) {
                    *curSample++ = *(theBuffer.begin() + (myOldNumFrames-1) * myNumChannels + myChannel);
                }
                theBuffer.clear();
                theBuffer.resize(theTempBuffer.getNumFrames());
                theBuffer.copyFrames(0, theTempBuffer, 0, theTempBuffer.getNumFrames());
            }
        };

        unsigned _myTargetFrames;

    };

    typedef asl::Ptr<Resampler, asl::MultiProcessor, asl::PtrHeapAllocator<asl::MultiProcessor> > ResamplerPtr;

} // namespace <y60>

#endif // INCL_Y60_RESAMPLER_H
        
        
        

