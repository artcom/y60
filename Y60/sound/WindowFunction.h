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

#ifndef INCL_Y60_WINDOWFUNCTION_H
#define INCL_Y60_WINDOWFUNCTION_H

#include <asl/Effect.h>
#include <asl/settings.h>
#include <asl/Ptr.h>

#include <vector>

namespace y60 {
    
    class WindowFunction : public asl::Effect {

    public:

        WindowFunction(asl::SampleFormat theSampleFormat);
        
        void setOverlapFactor(float theOverlapFactor);
        float getOverlapFactor();
        
        void setWindow(const std::vector<float> & theWindow);
        const std::vector<float> & getWindow() const;

        void createHannWindow(unsigned theSize);

    private:        

        template <class SAMPLE>
        class MultiplierFunctor : public asl::EffectFunctor<SAMPLE> {
            
        private:
            
            virtual void operator()(asl::Effect* theEffect, asl::AudioBuffer<SAMPLE> & theBuffer,
                                    asl::Unsigned64 theAbsoluteFrame) {
                //AC_TRACE << "WindowFunction::operator";
                WindowFunction * myWindowFunction = dynamic_cast<WindowFunction* >(theEffect);
                ASSURE(myWindowFunction);
                const std::vector<float> & theWindow = myWindowFunction->getWindow();
                float theOverlapFactor = myWindowFunction->getOverlapFactor();

                SAMPLE * curSample = theBuffer.begin();
                unsigned thisRange = theBuffer.getNumFrames();
                unsigned thatRange = theWindow.size();
                
                for (unsigned i = 0; i < thisRange; i++) {
                    float pos = (float)i/(float)thisRange * (float)thatRange;
                    unsigned low = (unsigned)floor(pos);
                    // linear interpolation...
                    float theWindowVal = (low != pos) ? theWindow[low] + (theWindow[low+1] - theWindow[low]) * (pos - low) : theWindow[low];
                    for (unsigned j = 0; j < theBuffer.getNumChannels(); j++) {
                        (*curSample++) *= theBuffer.floatToSample(theWindowVal);
                    }
                }
            }
        };
        
        std::vector<float> _myWindow;
        float _myOverlapFactor;
        
    };

    typedef asl::Ptr<WindowFunction, asl::MultiProcessor, asl::PtrHeapAllocator<asl::MultiProcessor> > WindowFunctionPtr;

}


#endif // INCL_Y60_WINDOWFUNCTION_H
