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
//   $RCSfile: SDLFontInfo.h,v $
//   $Author: martin $
//   $Revision: 1.1 $
//   $Date: 2004/11/07 10:01:43 $
//
//  Description: A simple renderer.
//
//=============================================================================

#ifndef AC_Y60_SDLFONTINFO
#define AC_Y60_SDLFONTINFO

#include <y60/gltext/TTFFontInfo.h>

#include <sdl/ttf2/SDL_ttf.h>

namespace y60 {

    class SDLFontInfo : public TTFFontInfo {
        public:
            SDLFontInfo(TTF_Font* theFont, FONTTYPE theFontType, int theHeight, FONTHINTING theHinting ) :
                _myFont(theFont), TTFFontInfo(theFontType, theHeight, theHinting) {}

            SDLFontInfo() {};

            TTF_Font * getFont() {
                return _myFont;
            }
            const TTF_Font * getFont() const {
                return _myFont;
            }
        private:
            TTF_Font * _myFont;
    };

} // namespace y60

#endif // AC_Y60_SDLFONTINFO
