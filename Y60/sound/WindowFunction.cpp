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

#include "WindowFunction.h"


namespace y60 {

    WindowFunction::WindowFunction(asl::SampleFormat theSampleFormat) 
        : asl::Effect(asl::createEffectFunctor<MultiplierFunctor>(theSampleFormat))
    {
        _myWindow.push_back(1.0);
    }

    void WindowFunction::setWindow(const std::vector<float> & theWindow) {
        _myWindow = theWindow;
    }

    const std::vector<float> & WindowFunction::getWindow() const {
        return _myWindow;
    }

    void WindowFunction::createHannWindow(unsigned theSize) {
        AC_DEBUG << "WindowFunction::createHannWindow()";
        _myWindow.clear();
        _myWindow.reserve(theSize);
        for (unsigned i = 0; i < theSize; i++) {
            float value = 0.5 * (1.0 -cos(2*3.14159265f*i/(theSize-1))); // Hann Window
            _myWindow.push_back(value); 
        }
    }

}
