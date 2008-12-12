//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef AC_Y60_TTFTEXTRENDERER
#define AC_Y60_TTFTEXTRENDERER

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
                    TTFFontInfo::FONTTYPE theFontType) = 0;
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

            virtual void setMaxFontFittingSize(unsigned theSize) = 0;
    };
    typedef asl::Ptr<TTFTextRenderer> TTFTextRendererPtr;

} // namespace y60

#endif
