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

// own header
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

    void WindowFunction::setOverlapFactor(float theOverlapFactor) {
        _myOverlapFactor = theOverlapFactor;
    }

    float WindowFunction::getOverlapFactor() {
        return _myOverlapFactor;
    }

    void WindowFunction::createHannWindow(unsigned theSize) {
        AC_DEBUG << "WindowFunction::createHannWindow()";
        _myWindow.clear();
        _myWindow.reserve(theSize);
        for (unsigned i = 0; i < theSize; i++) {
            float value = 0.5f * (1.0f - cosf(2.f*3.14159265f*i/(theSize-1.f))); // Hann Window
            _myWindow.push_back(value);
        }
    }
}
