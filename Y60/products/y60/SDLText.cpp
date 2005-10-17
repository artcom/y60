//============================================================================
//
// Copyright (C) 2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
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

#include "SDLText.h"
#include <y60/TextRenderer.h>

#include <asl/numeric_functions.h>
#include <asl/file_functions.h>
#include <asl/Dashboard.h>

#include <y60/GLUtils.h>

#include <sdl/SDL_ttf.h>

#include <iostream>

//#define CREATE_PNG_FROM_TEXT

#ifdef CREATE_PNG_FROM_TEXT
#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>
#include <paintlib/planydec.h>
#endif

using namespace std;
using namespace asl;

namespace y60 {

    SDLText::SDLText(TextRenderer * theRenderer,
                     const asl::Vector2f & thePos,
                     const asl::Vector4f & theTextColor,
                     const asl::Vector4f & theBackColor,
                     const std::string & theString,
                     const RENDERSTYLE & theRenderStyle,
                     const std::string & theFontName)
            : Text(theRenderer, thePos, theTextColor, theBackColor, theString, theFontName),
                   _myRenderStyle(theRenderStyle), _glTexture(0)
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
