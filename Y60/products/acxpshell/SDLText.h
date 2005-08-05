//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: SDLText.h,v $
//   $Author: martin $
//   $Revision: 1.1 $
//   $Date: 2004/11/07 10:01:43 $
//
//  Description: A simple renderer.
//
//=============================================================================

#ifndef AC_Y60_SDLTEXT
#define AC_Y60_SDLTEXT

#include <y60/Text.h>
#include <y60/GLUtils.h>

#include <asl/Ptr.h>
#include <asl/Vector234.h>

#include <SDL/SDL.h>

#include <string>

namespace y60 {
    class TextRenderer;

    class SDLText : public Text {
        public:
            SDLText(TextRenderer * theRenderer,
                    const asl::Vector2f & thePos,
                    const asl::Vector4f & theTextColor,
                    const asl::Vector4f & theBackColor,
                    const std::string & theString,
                    const RENDERSTYLE & theRenderStyle,
                    const std::string & theFontName);
            virtual ~SDLText();
            const GLuint createTextureFromSurface(SDL_Surface *surface);
            const RENDERSTYLE & getRenderStyle() { return _myRenderStyle; }
        private:
        	GLuint      _glTexture;
   	        RENDERSTYLE _myRenderStyle;
    };
    typedef asl::Ptr<SDLText> SDLTextPtr;



} // namespace y60

#endif // AC_Y60_SDLTEXT
