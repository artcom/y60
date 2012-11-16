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

#ifndef AC_Y60_TTFFONTINFO
#define AC_Y60_TTFFONTINFO

#include "y60_gltext_settings.h"

namespace y60 {

   const char * const FontTypeStrings[] = {
        "Normal",
        "Bold",
        "Italic",
        "BoldItalic",
        0
    };

    const char * const FontHintStrings[] = {
        "NoHinting",
        "NativeHinting",
        "AutoHinting",
        "AutoHintingLight",
        "AutoHintingMono",
        0
    };

    class TTFFontInfo {
        public:
            enum FONTTYPE {
                NORMAL,
                BOLD,
                ITALIC,
                BOLDITALIC
            };

            enum FONTHINTING {
                NOHINTING,
                NATIVEHINTING,
                AUTOHINTING,
                AUTOHINTING_LIGHT,
                AUTOHINTING_MONO
            };

            TTFFontInfo(FONTTYPE theFontType, int theHeight, FONTHINTING theHinting)
                : _myFontType(theFontType),
                _myHeight(theHeight),
                _myFontHinting(theHinting)
            {}

            TTFFontInfo()
                : _myFontType(NORMAL),
                _myHeight(0),
                _myFontHinting(AUTOHINTING)
            {};

            const FONTTYPE getFontType() {
                return _myFontType;
            }
            const FONTHINTING getFontHinting() {
                return _myFontHinting;
            }
            int getHeight() {
                return _myHeight;
            }
        private:
            FONTTYPE   _myFontType;
            int        _myHeight;
            FONTHINTING _myFontHinting;
    };

} // namespace y60

#endif

