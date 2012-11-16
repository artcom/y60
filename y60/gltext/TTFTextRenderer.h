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
*/

#ifndef AC_Y60_TTFTEXTRENDERER
#define AC_Y60_TTFTEXTRENDERER

#include "y60_gltext_settings.h"

#include <y60/glutil/GLUtils.h>
#include "TextRenderer.h"
#include "Text.h"
#include "TTFFontInfo.h"

#include <y60/scene/TextureManager.h>

#include <map>
#include <string>
#include <vector>

class TTF_FONT;

namespace y60 {

    DEFINE_EXCEPTION(TTFTextRendererException, asl::Exception);

    class TTFTextRenderer : public TextRenderer {
        public:
            virtual void loadFont(const std::string & theName, const std::string & theFileName,
                    int theHeight, TTFFontInfo::FONTHINTING & theFonthint,
                    TTFFontInfo::FONTTYPE theFontType, int theAscendOffset = 0) = 0;
            virtual TextPtr createText(const asl::Vector2f & thePos,
                    const std::string & theString,
                    const std::string & theFontName) = 0;

            virtual bool haveFont(const std::string theFontName) = 0;

            virtual void renderText(TextPtr & theText) = 0;
            virtual asl::Vector2i renderTextAsImage(TextureManager & theTextureManager,
                    dom::NodePtr theImageNode,
                    const std::string & theText, const std::string & theFontName,
                    unsigned int theTextureWidth = 0, unsigned int theTextureHeight = 0,
                    const asl::Vector2i & theCursorPos = asl::Vector2i(0,0)) = 0;


            virtual bool getFontMetrics(const std::string & theFontName,
                    int & theFontHeight,
                    int & theFontAscent, int & theFontDescent,
                    int & theFontLineSkip) const = 0;

            virtual bool getGlyphMetrics(const std::string & theFontName,
                    const std::string & theCharacter,
                    asl::Box2f & theGlyphBox,
                    double & theAdvance) const = 0;

            virtual double getKerning(const std::string & theFontName,
                    const std::string & theFirstCharacter,
                    const std::string & theSecondCharacter) const = 0;

            virtual bool hasGlyph(const std::string & theFontName,
                    const std::string & theCharacter) const = 0;
    };
    typedef asl::Ptr<TTFTextRenderer> TTFTextRendererPtr;

} // namespace y60

#endif
