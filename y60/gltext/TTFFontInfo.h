//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: TTFFontInfo.h,v $
//   $Author: martin $
//   $Revision: 1.1 $
//   $Date: 2004/11/07 10:01:43 $
//
//  Description: A simple renderer.
//
//=============================================================================

#ifndef AC_Y60_TTFFONTINFO
#define AC_Y60_TTFFONTINFO

#include "y60_gltext_settings.h"

namespace y60 {

    static const char * FontTypeStrings[] = {
        "Normal",
        "Bold",
        "Italic",
        "BoldItalic",
        0
    };
    
    static const char * FontHintStrings[] = {
        "NoHinting",
        "NativeHinting",
        "AutoHinting",
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
                AUTOHINTING
            };

            TTFFontInfo(FONTTYPE theFontType, int theHeight, FONTHINTING theHinting) :
                _myFontType(theFontType), _myHeight(theHeight), 
                _myFontHinting(theHinting)  {}

            TTFFontInfo() : _myFontType(NORMAL),_myFontHinting(NOHINTING) {};

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
            FONTHINTING _myFontHinting;
            int        _myHeight;
    };

} // namespace y60

#endif 

