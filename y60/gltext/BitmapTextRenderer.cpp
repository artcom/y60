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

// own header
#include "BitmapTextRenderer.h"

#include "textUtilities.h"

#include "Bookman72.h"
#include "Courier24.h"
#include "Helv24.h"
#include "Screen13.h"
#include "Screen15.h"
#include "Screen8.h"
#include "SyntaxBold18.h"
#include "Times12.h"
#include "SyntaxMed144.h"
#include "Times14.h"
#include "Times144.h"

#include <asl/base/string_functions.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <iostream>

using namespace std;
using namespace asl;

namespace y60 {

    BitmapTextRenderer::BitmapTextRenderer() {
    }

    BitmapTextRenderer::~BitmapTextRenderer() {
    }


    void
    BitmapTextRenderer::renderText(TextPtr & theText) {
	    glColor4fv(theText->_myTextStyle._myTextColor.begin());
        glDisable(GL_LIGHTING);

        unsigned int myXPos = (unsigned int) (theText->_myPos[0] * getWindowWidth() + 0.5);
        unsigned int myYPos = (unsigned int) (theText->_myPos[1] * getWindowHeight() + 0.5);

	    glRasterPos2i(myXPos, myYPos);

        BITMAPFONT myFontEnum = BITMAPFONT(asl::getEnumFromString(theText->_myFont, myBitmapFontStr));
        switch(myFontEnum) {
            case BOOKMAN72_FONT:
                glDrawString(theText->_myString.c_str(), Bookman72);
                break;
            case COURIER24_FONT:
                glDrawString(theText->_myString.c_str(), ::Courier24);
                break;
            case HELV24_FONT:
                glDrawString(theText->_myString.c_str(), Helv24);
                break;
            case SCREEN8_FONT:
                glDrawString(theText->_myString.c_str(), Screen8);
                break;
            case SCREEN13_FONT:
                glDrawString(theText->_myString.c_str(), Screen13);
                break;
            case SCREEN15_FONT:
                glDrawString(theText->_myString.c_str(), Screen15);
                break;
            case SYNTAXBOLD18_FONT:
                glDrawString(theText->_myString.c_str(), SyntaxBold18);
                break;
            case SYNTAXMED144_FONT:
                glDrawString(theText->_myString.c_str(), SyntaxMed144);
                break;
            case TIMES12_FONT:
                glDrawString(theText->_myString.c_str(), Times12);
                break;
            case TIMES14_FONT:
                glDrawString(theText->_myString.c_str(), Times14);
                break;
            case TIMES144_FONT:
                glDrawString(theText->_myString.c_str(), Times144);
                break;
            default:
                throw GLTextRendererException("Sorry, font not found: " + theText->_myFont, PLUS_FILE_LINE);
        };

    }

    bool
    BitmapTextRenderer::haveFont(const std::string theName) {
        int myCounter= -1;
        while (myBitmapFontStr[++myCounter]) {
            if (theName == myBitmapFontStr[myCounter]) {
                return true;
            }
        }
        return false;
    }

}
