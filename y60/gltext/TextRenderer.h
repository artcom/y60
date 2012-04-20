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

#ifndef AC_Y60_TEXTRENDERER
#define AC_Y60_TEXTRENDERER

#include "y60_gltext_settings.h"

#include "Text.h"
#include "TextStyle.h"
#include <asl/base/Exception.h>
#include <asl/base/Ptr.h>
#include <asl/math/Vector234.h>
#include <asl/math/Box.h>
#include <asl/dom/Nodes.h>

#include <map>
namespace y60 {
    
    DEFINE_EXCEPTION(GLTextRendererException, asl::Exception);


    class Y60_GLTEXT_DECL TextRenderer {
        public:

            TextRenderer();
            virtual ~TextRenderer();

            virtual TextPtr createText(const asl::Vector2f & thePos,
                                       const std::string & theString,
                                       const std::string & theFont);
            void setWindowSize(const unsigned int & _myWindowWidth, const unsigned int & _myWindowHeight);
            virtual bool haveFont(const std::string theFontName) = 0;
            virtual void renderText(TextPtr & theText) = 0;
            const asl::Vector2i & getTextCursorPosition() const {
                return _myCursorPos;
            }
            const unsigned int & getMaxWidth() const {
                return _myMaxWidth;
            }
            const std::vector<unsigned int> & getLineWidths() const {
                return _myLineWidths;
            }
            const std::vector<asl::Vector2i> & getGlyphPositions() const {
                return _myGlyphPosition;
            }

            void setColor(const asl::Vector4f & theTextColor);
            const asl::Vector4f & getColor();
            void setPadding(int theTop, int theBottom, int theLeft, int theRight);
            void setCursor(asl::Vector2i thePosition);
            void setIndentation(int theIndent);
        	void setVTextAlignment(TextStyle::TextAligment theVAlignment);
	        void setHTextAlignment(TextStyle::TextAligment theHAlignment);
	        void setLineHeight(unsigned theHeight) {
	            _myTextStyle._myLineHeight = theHeight;
	        }
        	void setParagraph(unsigned theTopOffset, unsigned theBottomOffset) {
        	    _myTextStyle._myParagraphTopOffset    = theTopOffset;
        	    _myTextStyle._myParagraphBottomOffset = theBottomOffset;
        	}
            void setTracking(float theTracking) {
                _myTextStyle._myTracking = theTracking;
            }

            const unsigned int & getWindowHeight() const { return _myWindowHeight; }
            const unsigned int & getWindowWidth() const { return _myWindowWidth; }
            const asl::Vector4f & getTextColor() const { return _myTextStyle._myTextColor;}
            const TextStyle & getTextStyle() { return _myTextStyle;}
            void setTextStyle(const TextStyle & theTextStyle);

        protected:
            TextStyle      _myTextStyle;
            unsigned             _myMaxWidth;
            asl::Vector2i _myCursorPos;
            std::vector<unsigned> _myLineWidths;
            std::vector<asl::Vector2i> _myGlyphPosition;
        private:
            unsigned int  _myWindowWidth;
            unsigned int  _myWindowHeight;

    };
    typedef asl::Ptr<TextRenderer> TextRendererPtr;
    typedef std::map<std::string, TextRendererPtr> TextRendererMap;

} // namespace y60

#endif // AC_Y60_TEXTRENDERER
