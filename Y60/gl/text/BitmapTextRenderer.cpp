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
//    $RCSfile: BitmapTextRenderer.cpp,v $
//
//     $Author: christian $
//
//   $Revision: 1.6 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

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

#include <asl/string_functions.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
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
	    glColor4fv(theText->_myTextColor.begin());
        glDisable(GL_LIGHTING);

        unsigned int myXPos = (unsigned int) (theText->_myPos[0] * getWindowWidth());
        unsigned int myYPos = (unsigned int) (theText->_myPos[1] * getWindowHeight());

	    glRasterPos2i(myXPos, myYPos);

        BITMAPFONT myFontEnum = BITMAPFONT(asl::getEnumFromString(theText->_myFont, myBitmapFontStr));
        switch(myFontEnum) {
            case BOOKMAN72_FONT:
                glDrawString(theText->_myString.c_str(), Bookman72);
                break;
            case COURIER24_FONT:
                glDrawString(theText->_myString.c_str(), Courier24);
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
