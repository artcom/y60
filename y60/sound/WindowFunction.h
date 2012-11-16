/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef INCL_Y60_WINDOWFUNCTION_H
#define INCL_Y60_WINDOWFUNCTION_H

#include "y60_sound_settings.h"

#include <asl/audio/Effect.h>
#include <asl/base/settings.h>
#include <asl/base/Ptr.h>

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
                //float theOverlapFactor = myWindowFunction->getOverlapFactor();

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
