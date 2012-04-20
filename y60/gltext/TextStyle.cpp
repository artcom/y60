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

//own header
#include "TextStyle.h"


//#include <y60/glutil/GLUtils.h>

#include <asl/math/Vector234.h>
//#include <asl/base/Dashboard.h>
//#include <asl/math/numeric_functions.h>

//#include <iostream>


using namespace std;
using namespace asl;

namespace y60 {    
    static const char * const TOP_PADDING_NAME = "topPad";
    static const char * const BOTTOM_PADDING_NAME = "bottomPad";
    static const char * const RIGHT_PADDING_NAME = "rightPad";
    static const char * const LEFT_PADDING_NAME = "leftPad";
    static const char * const LINE_HEIGHT_NAME = "lineHeight";
    static const char * const HORIZONTAL_ALIGNMENT_NAME = "hAlign";
    static const char * const VERTICAL_ALIGNMENT_NAME = "vAlign";
    static const char * const TRACKING_NAME = "tracking";
    static const char * const TEXTCOLOR_NAME = "textColor";

    void getAttributeFromNode(dom::NodePtr theStyleNode, const std::string& theAttributeName, TextStyle::TextAligment & theMember) {
        if (theStyleNode->getAttribute(theAttributeName)) {
            std::string myAttribute = theStyleNode->getAttribute(theAttributeName)->nodeValue();
            if (myAttribute == "center") {
                theMember = TextStyle::CENTER_ALIGNMENT;
            } else if (myAttribute == "top") {
                theMember = TextStyle::TOP_ALIGNMENT;
            } else if (myAttribute == "bottom") {
                theMember = TextStyle::BOTTOM_ALIGNMENT;
            } else if (myAttribute == "left") {
                theMember = TextStyle::LEFT_ALIGNMENT;
            } else if (myAttribute == "right") {
                theMember = TextStyle::RIGHT_ALIGNMENT;
            }
        }
    }
    void getAttributeFromNode(dom::NodePtr theStyleNode, const std::string& theAttributeName, int & theMember) {
        if (theStyleNode->getAttribute(theAttributeName)) {
            theMember =  asl::as<int>(theStyleNode->getAttribute(theAttributeName)->nodeValue());
        }
    }
    void getAttributeFromNode(dom::NodePtr theStyleNode, const std::string& theAttributeName, unsigned int & theMember) {
        if (theStyleNode->getAttribute(theAttributeName)) {
            theMember =  asl::as<int>(theStyleNode->getAttribute(theAttributeName)->nodeValue());
        }
    }
    void getAttributeFromNode(dom::NodePtr theStyleNode, const std::string& theAttributeName, float & theMember) {
        if (theStyleNode->getAttribute(theAttributeName)) {
            theMember =  asl::as<float>(theStyleNode->getAttribute(theAttributeName)->nodeValue());
        }
    }
    void getColorAttributeFromNode(dom::NodePtr theStyleNode, const std::string& theAttributeName, asl::Vector4f & theMember) {
        if (theStyleNode->getAttribute(theAttributeName)) {
            std::string myAttribute = theStyleNode->getAttribute(theAttributeName)->nodeValue();
            if (myAttribute.substr(0, 1) == "[") {
                asl::fromString(myAttribute, theMember);
            } else if (myAttribute.size() == 6) {
                // hex color conversion
                unsigned r = 0;
                unsigned g = 0;
                unsigned b = 0;
                bool success = is_hex_number(myAttribute.substr(0, 2), r);
                success     &= is_hex_number(myAttribute.substr(2, 2), g);
                success     &= is_hex_number(myAttribute.substr(4, 2), b);
                if (success) {
                    theMember[0]= r/255.0f;
                    theMember[1]= g/255.0f;
                    theMember[2]= b/255.0f;
                    theMember[3] = 1.0f;
                }
            } else {
                throw asl::Exception("unknown textstyle color format " + myAttribute, PLUS_FILE_LINE);
            }
        }
    }
    TextStyle::TextStyle() {fillDefault();}
    TextStyle::TextStyle(const asl::Vector4f &  myTextColor) {
        fillDefault();
        _myTextColor = myTextColor;
    }
    TextStyle::TextStyle(dom::NodePtr myStyleNode) {
        fillDefault();
        getAttributeFromNode(myStyleNode, TOP_PADDING_NAME, _myTopPadding);        
        getAttributeFromNode(myStyleNode, BOTTOM_PADDING_NAME, _myBottomPadding);        
        getAttributeFromNode(myStyleNode, LEFT_PADDING_NAME, _myLeftPadding);        
        getAttributeFromNode(myStyleNode, RIGHT_PADDING_NAME, _myRightPadding);        
        getAttributeFromNode(myStyleNode, LINE_HEIGHT_NAME, _myLineHeight);        
        getAttributeFromNode(myStyleNode, HORIZONTAL_ALIGNMENT_NAME, _myHorizontalAlignment);        
        getAttributeFromNode(myStyleNode, VERTICAL_ALIGNMENT_NAME, _myVerticalAlignment);        
        getAttributeFromNode(myStyleNode, TRACKING_NAME, _myTracking);        
        getColorAttributeFromNode(myStyleNode, TEXTCOLOR_NAME, _myTextColor);        
    }
    TextStyle::~TextStyle() {}
    void TextStyle::fillDefault() {
        _myTextColor = Vector4f(0.0, 0.0, 0.0, 1.0);
        _myTopPadding = 0;
        _myBottomPadding = 0;
        _myLeftPadding = 0;
        _myRightPadding = 0;
        _myHorizontalAlignment = TextStyle::LEFT_ALIGNMENT;
        _myVerticalAlignment = TextStyle::TOP_ALIGNMENT;
        _myLineHeight = 0;
        _myParagraphTopOffset = 0;
        _myParagraphBottomOffset = 0;
        _myTracking = 0.0f;
        _myIndentation = 0;
    }

}
