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
//
//    $RCSfile: SDLText.cpp,v $
//
//     $Author: martin $
//
//   $Revision: 1.1 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

// own header
#include "SDLText.h"

#include <y60/gltext/TextRenderer.h>

#include <asl/math/numeric_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/Dashboard.h>

#include <y60/glutil/GLUtils.h>

#include <sdl/ttf2/SDL_ttf.h>

#include <iostream>

//#define CREATE_PNG_FROM_TEXT

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#ifdef CREATE_PNG_FROM_TEXT
#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>
#include <paintlib/planydec.h>
#endif
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

using namespace std;
using namespace asl;

namespace y60 {

    SDLText::SDLText(TextRenderer * theRenderer,
                     const asl::Vector2f & thePos,
                     const asl::Vector4f & theTextColor,
                     const std::string & theString,
                     const std::string & theFontName)
            : Text(theRenderer, thePos, theTextColor, theString, theFontName),
                   _glTexture(0)
    {
        glGenTextures(1, &_glTexture);
        glBindTexture(GL_TEXTURE_2D, _glTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    SDLText::~SDLText() {

        if (_glTexture) {
            glDeleteTextures(1, &_glTexture);
        }
    }

    const GLuint
    SDLText::createTextureFromSurface(SDL_Surface * theTextureSurface)
    {
        glBindTexture(GL_TEXTURE_2D, _glTexture);
        glTexImage2D(GL_TEXTURE_2D,
                0,
                GL_RGBA,
                theTextureSurface->w,
                theTextureSurface->h,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                theTextureSurface->pixels);
#ifdef CREATE_PNG_FROM_TEXT
        PLAnyPicDecoder myDecoder;
        PLAnyBmp myBmp;
        myBmp.Create( w, h, 4 * 8, true, false,
                      (PLBYTE*)theTextureSurface->pixels);
        PLPNGEncoder myEncoder;
        myEncoder.MakeFileFromBmp("SDLText_createTextureFromSurface.png", &myBmp);
#endif
        CHECK_OGL_ERROR;
        return _glTexture;
    }
}
