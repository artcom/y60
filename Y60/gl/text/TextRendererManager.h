//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef AC_Y60_TEXTRENDERERMANAGER
#define AC_Y60_TEXTRENDERERMANAGER

#include "Text.h"
#include "TextRenderer.h"
#include "BitmapTextRenderer.h"
#include "TTFTextRenderer.h"
#include "TTFFontInfo.h"

#include <asl/Ptr.h>
#include <asl/Vector234.h>
#include <asl/Box.h>
#include <asl/Matrix4.h>
#include <asl/Exception.h>

#include <vector>
#include <string>

namespace y60 {

    DEFINE_EXCEPTION(TextRendererManagerException, asl::Exception);
    class TextRendererManager {
        public:
            TextRendererManager();
            virtual ~TextRendererManager();
            void setTTFRenderer(const TTFTextRendererPtr & theTTFRenderer);
            void render(const asl::Matrix4f & theWindowMatrix);
            void updateWindow(const unsigned int & _myWindowWidth, const unsigned int & _myWindowHeight);
            void addText(const asl::Vector2f & thePos, const std::string & theString,
                    const std::string & theFont);
            void setColor(const asl::Vector4f & theTextColor, const asl::Vector4f & theBackColor);
            void setPadding(int topPadding, int bottomPadding, int leftpadding, int rightpadding);
            void setVTextAlignment(TextRenderer::TextAligment theVAlignment);
            void setHTextAlignment(TextRenderer::TextAligment theHAlignment);
            void setLineHeight(unsigned theHeight);
            void setParagraph(unsigned theTopOffset, unsigned theBottomOffset);
            void loadTTF(const std::string & theName, const std::string & theFileName,
                    int theHeight, TTFFontInfo::FONTTYPE & theFonttype);
            void setTextStyle(Text::RENDERSTYLE theStyle);
            asl::Vector2i renderTextAsImage(TextureManager & theTextureManager, dom::NodePtr theImageNode,
                    const std::string & theString,
                    const std::string & theFont,
                    unsigned int theTargetWidth, unsigned int theTargetHeight);

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
            void setMaxFontFittingSize(unsigned theSize);

        private:
            TextRenderer & getTextRendererByFont(const std::string & theFont);
            BitmapTextRenderer      _myBitmapRenderer;
            TTFTextRendererPtr      _myTTFRenderer;
            std::vector<TextPtr>    _myTextSnippets;
            unsigned int            _myWindowWidth;
            unsigned int            _myWindowHeight;
    };

} // namespace y60

#endif // AC_Y60_TEXTRENDERERMANAGER
