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
//   $RCSfile: TextRenderer.h,v $
//   $Author: janbo $
//   $Revision: 1.13 $
//   $Date: 2004/10/18 16:22:56 $
//
//  Description: A simple renderer.
//
//=============================================================================

#ifndef AC_Y60_TEXTRENDERER
#define AC_Y60_TEXTRENDERER

#include "Text.h"
#include <asl/Exception.h>
#include <asl/Ptr.h>
#include <asl/Vector234.h>
#include <asl/Box.h>

#include <map>
namespace y60 {


    DEFINE_EXCEPTION(GLTextRendererException, asl::Exception);

    class TextRenderer {
        public:
            enum TextAligment {
                CENTER_ALIGNMENT,
                LEFT_ALIGNMENT,
                RIGHT_ALIGNMENT,
                TOP_ALIGNMENT,
                BOTTOM_ALIGNMENT
            };

            TextRenderer();
            virtual ~TextRenderer();

            virtual TextPtr createText(const asl::Vector2f & thePos,
                                       const std::string & theString,
                                       const std::string & theFont);
            void setWindowSize(const unsigned int & _myWindowWidth, const unsigned int & _myWindowHeight);
            virtual bool haveFont(const std::string theFontName) = 0;
            virtual void renderText(TextPtr & theText) = 0;
            void setColor(const asl::Vector4f & theTextColor);
            void setPadding(int theTop, int theBottom, int theLeft, int theRight);
            void setIndentation(int theIndent);
        	void setVTextAlignment(TextRenderer::TextAligment theVAlignment);
	        void setHTextAlignment(TextRenderer::TextAligment theHAlignment);
	        void setLineHeight(unsigned theHeight) {
	            _myLineHeight = theHeight;
	        }
        	void setParagraph(unsigned theTopOffset, unsigned theBottomOffset) {
        	    _myParagraphTopOffset    = theTopOffset;
        	    _myParagraphBottomOffset = theBottomOffset;
        	}
            void setTracking(float theTracking) {
                _myTracking = theTracking;
            }

            const unsigned int & getWindowHeight() { return _myWindowHeight; }
            const unsigned int & getWindowWidth()  { return _myWindowWidth; }
            const asl::Vector4f & getTextColor()   { return _myCurrentTextColor; }
        protected:
   	        TextAligment  _myHorizontalAlignment;
   	        TextAligment  _myVerticalAlignment;
            int           _myTopPadding;
            int           _myBottomPadding;
            int           _myLeftPadding;
            int           _myRightPadding;
            unsigned      _myLineHeight;
        	unsigned      _myParagraphTopOffset;
        	unsigned      _myParagraphBottomOffset;
            float         _myTracking;
            int           _myIndentation;
        private:
            unsigned int  _myWindowWidth;
            unsigned int  _myWindowHeight;
   	        asl::Vector4f _myCurrentTextColor;

    };
    typedef asl::Ptr<TextRenderer> TextRendererPtr;
    typedef std::map<std::string, TextRendererPtr> TextRendererMap;

} // namespace y60

#endif // AC_Y60_TEXTRENDERER
