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
//    $RCSfile: TextRenderer.cpp,v $
//
//     $Author: janbo $
//
//   $Revision: 1.13 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "TextRenderer.h"

#include <y60/GLUtils.h>

#include <asl/Vector234.h>
#include <asl/Dashboard.h>
#include <asl/numeric_functions.h>

#include <iostream>


using namespace std;
using namespace asl;

namespace y60 {

    TextRenderer::TextRenderer() :
        _myLineHeight(0),
        _myCurrentTextColor(Vector4f(1.0, 1.0, 1.0, 1.0)),
        _myCurrentBackColor(Vector4f(1.0, 1.0, 1.0, 1.0)),
        _myHorizontalAlignment(LEFT_ALIGNMENT),
        _myVerticalAlignment(TOP_ALIGNMENT),
        _myParagraphTopOffset(0),
        _myParagraphBottomOffset(0),
        _myTopPadding(0),
        _myBottomPadding(0),
        _myRightPadding(0),
        _myLeftPadding(0)
    {}

    TextRenderer::~TextRenderer() {}

    TextPtr
    TextRenderer::createText(const Vector2f & thePos,
                             const string & theString,
                             const string & theFont)
    {
        return TextPtr(new Text(this, thePos, getTextColor(), getBackColor(), theString, theFont));
    }


    void
    TextRenderer::updateWindow(const unsigned int & theWindowWidth,
                               const unsigned int & theWindowHeight)
    {
        _myWindowWidth  = theWindowWidth;
        _myWindowHeight = theWindowHeight;
    }


    void
    TextRenderer::setColor(const asl::Vector4f & theTextColor,
                           const asl::Vector4f & theBackColor)
    {
        _myCurrentTextColor = theTextColor;
        _myCurrentBackColor = theBackColor;
    }

    void
    TextRenderer::setPadding(int theTop, int theBottom, int theLeft, int theRight) {
        _myTopPadding = theTop;
        _myBottomPadding = theBottom;
        _myLeftPadding = theLeft;
        _myRightPadding = theRight;
    }
    
	void
	TextRenderer::setVTextAlignment(TextRenderer::TextAligment theVAlignment) {
	    _myVerticalAlignment = theVAlignment;
	}

    void
    TextRenderer::setHTextAlignment(TextRenderer::TextAligment theHAlignment) {
        _myHorizontalAlignment = theHAlignment;
	}
}
