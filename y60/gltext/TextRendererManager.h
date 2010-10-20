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

#ifndef AC_Y60_TEXTRENDERERMANAGER
#define AC_Y60_TEXTRENDERERMANAGER

#include "y60_gltext_settings.h"

#include "Text.h"
#include "TextRenderer.h"
#include "BitmapTextRenderer.h"
#include "TTFTextRenderer.h"
#include "TTFFontInfo.h"

#include <y60/scene/Viewport.h>

#include <asl/base/Ptr.h>
#include <asl/math/Vector234.h>
#include <asl/math/Box.h>
#include <asl/math/Matrix4.h>
#include <asl/base/Exception.h>

#include <vector>
#include <string>
#include <map>

namespace y60 {

    DEFINE_EXCEPTION(TextRendererManagerException, asl::Exception);
    class Y60_GLTEXT_DECL TextRendererManager {
        public:
            TextRendererManager();
            virtual ~TextRendererManager();
            void setTTFRenderer(const TTFTextRendererPtr & theTTFRenderer);
            void render(ViewportPtr theViewport);
            void addText(const asl::Vector2f & thePos, const std::string & theString,
                    const std::string & theFont, ViewportPtr theViewport);
            void setColor(const asl::Vector4f & theTextColor);
            const asl::Vector4f & getColor();
            void setPadding(int topPadding, int bottomPadding, int leftpadding, int rightpadding);
	        void setIndentation(int theIndentation);
            void setVTextAlignment(TextRenderer::TextAligment theVAlignment);
            void setHTextAlignment(TextRenderer::TextAligment theHAlignment);
            void setLineHeight(unsigned theHeight);
            void setParagraph(unsigned theTopOffset, unsigned theBottomOffset);
            void loadTTF(const std::string & theName, const std::string & theFileName,
                    int theHeight, TTFFontInfo::FONTHINTING & theFonthint, TTFFontInfo::FONTTYPE & theFonttype, 
                    int theAscendOffse = 0);
            asl::Vector2i renderTextAsImage(TextureManager & theTextureManager, dom::NodePtr theImageNode,
                    const std::string & theString,
                    const std::string & theFont,
                    unsigned int theTargetWidth, unsigned int theTargetHeight,
                    const asl::Vector2i & theCursorPos = asl::Vector2i(0,0));
            const asl::Vector2i & getTextCursorPosition() const;
            const unsigned int & getMaxWidth() const;
            const std::vector<unsigned int> & getLineWidths() const;

            bool getFontMetrics(const std::string & theFontName,
                    int & theFontHeight,
                    int & theFontAscent, int & theFontDescent,
                    int & theFontLineSkip) const;
            bool getGlyphMetrics(const std::string & theFontName,
                    const std::string & theCharacter,
                    asl::Box2f & theGlyphBox, double & theAdvance) const;
            double getKerning(const std::string & theFontName, const std::string & theFirstCharacter, const std::string & theSecondCharacter) const;
            bool hasGlyph(const std::string & theFontName, const std::string & theCharacter) const;
            void setTracking(float theTracking);

        private:
            TextRenderer & getTextRendererByFont(const std::string & theFont);
            BitmapTextRenderer      _myBitmapRenderer;
            TTFTextRendererPtr      _myTTFRenderer;
            std::vector<TextPtr>    _myTextSnippets;
            std::map<std::string, std::vector<TextPtr> > _myTextSnippetsMap;
    };

} // namespace y60

#endif // AC_Y60_TEXTRENDERERMANAGER
