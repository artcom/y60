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

#ifndef AC_Y60_SDLTEXTRENDERER
#define AC_Y60_SDLTEXTRENDERER

#include "y60_sdlengine_settings.h"

#include <y60/glutil/GLUtils.h>
#include "SDLText.h"
#include "SDLFontInfo.h"

#include <y60/gltext/TTFTextRenderer.h>
#include <y60/scene/TextureManager.h>

#include <SDL/SDL.h>

#include <map>
#include <string>
#include <vector>

class TTF_FONT;

namespace y60 {

    class SDLTextRenderer : public TTFTextRenderer {
        public:
            SDLTextRenderer();
            ~SDLTextRenderer();

            void loadFont(const std::string & theName, const std::string & theFileName,
                    int theHeight, TTFFontInfo::FONTHINTING & theFonthint,
                    TTFFontInfo::FONTTYPE theFontType, int theAscendOffset = 0);
            TextPtr createText(const asl::Vector2f & thePos,
                    const std::string & theString,
                    const std::string & theFontName);

            bool haveFont(const std::string theFontName);
            void renderText(TextPtr & theText);
            asl::Vector2i renderTextAsImage(TextureManager & theTextureManager,
                    dom::NodePtr theImageNode,
                    const std::string & theText, const std::string & theFontName,
                    unsigned int theTextureWidth = 0, unsigned int theTextureHeight = 0,
                    const asl::Vector2i & theCursorPos = asl::Vector2i(0,0));

            bool getFontMetrics(const std::string & theFontName,
                    int & theFontHeight,
                    int & theFontAscent, int & theFontDescent,
                    int & theFontLineSkip) const;

            bool getGlyphMetrics(const std::string & theFontName, const std::string & theCharacter, asl::Box2f & theGlyphBox, double & theAdvance) const;
            double getKerning(const std::string & theFontName, const std::string & theFirstCharacter, const std::string & theSecondCharacter) const;
            bool hasGlyph(const std::string& theFontName, const std::string& theCharacter) const;

        private:
            typedef std::map<std::string, SDLFontInfo> FontLibrary;

            struct Format {
                Format() : bold(false), italics(false), underline(false), indent(false), color(asl::Vector4f(-1,-1,-1,1)) {}

                bool bold;
                bool italics;
                bool underline;
                bool indent;
                asl::Vector4f color;
            };

            struct Word {
                Word(const std::string & theText) :
                    text(theText), surface(0), taglength(0), newline(false), kerning(0), minx(0)
                {}

                std::string   text;
                SDL_Surface * surface;
                Format        format;
                unsigned      taglength;
                bool          newline;
                double        kerning;
                int           minx;
                std::vector<int> sdl_x_position;
                private:
                Word() {}
            };

            struct Line {
                Line() : width(0), wordCount(0), newline(false), indent(0) {}

                unsigned width;
                unsigned wordCount;
                bool     newline;
                unsigned indent;
            };

            asl::Vector2i createTextSurface(const std::string & theText, const std::string & theFontName,
                    const asl::Vector4f & theTextColor,
                    unsigned int theTextureWidth = 0,
                    unsigned int theTextureHeight = 0);
            void createTargetSurface(unsigned theWidth, unsigned theHeight, const asl::Vector4f & theTextColor);

            SDL_Surface * renderToSurface(std::string theText,
                    const TTF_Font * theFont,
                    const SDL_Color & theTextColor);

            unsigned createLines(const std::vector<Word> & theWords, std::vector<Line> & theLines,
                    unsigned theLineWidth, unsigned theLineHeight, unsigned theSurfaceHeight);

            int calcHorizontalAlignment(unsigned theTextWidth, const Line & theLine,
                    int theMinX);
            int calcVerticalAlignment(unsigned theTextHeight, unsigned theBlockHeight);
            SDLFontInfo & getFontInfo(const std::string & theName);
            const TTFFontInfo::FONTHINTING & getFontHint(const std::string & theName) const;

            std::string makeFontName(const std::string & theName, SDLFontInfo::FONTTYPE theFontType = SDLFontInfo::NORMAL) const;
            const TTF_Font * getFont(const std::string & theName) const;


            void renderWords(std::vector<Word> & theWords,
                    const std::string & theFontName,
                    const asl::Vector4f & theTextColor);

            unsigned parseNewline(const std::string & theText, unsigned thePos, Format & theFormat);
            unsigned parseHtmlTag(const std::string & theText, unsigned thePos, Format & theFormat);
            std::string extractTag(const std::string & theText, unsigned thePos);

            unsigned parseWord(const std::string & theText, unsigned thePos);
            void parseWords(const std::string & theText, std::vector<Word> & theResult);

            SDL_Surface *        _myTextureSurface;
            FontLibrary          _myFonts;
            std::string          _myWordDelimiters;
            std::map<std::string, TTFFontInfo::FONTHINTING> _myFontHintingMap;

    };
    typedef asl::Ptr<SDLTextRenderer> SDLTextRendererPtr;



} // namespace y60

#endif
