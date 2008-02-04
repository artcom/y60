//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef AC_Y60_SDLTEXTRENDERER
#define AC_Y60_SDLTEXTRENDERER

#include  <y60/GLUtils.h>
#include "SDLText.h"
#include "SDLFontInfo.h"

#include <y60/TTFTextRenderer.h>
#include <y60/TextureManager.h>

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
                    int theHeight, TTFFontInfo::FONTTYPE theFontType);
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

            void setMaxFontFittingSize(unsigned theSize) {
                _myMaxFontFittingSize = theSize;
            }
        private:
            typedef std::map<std::string, SDLFontInfo> FontLibrary;

            struct Format {
                Format() : bold(false), italics(false), underline(false) {}

                bool bold;
                bool italics;
                bool underline;
            };

            struct Word {
                Word(const std::string & theText) :
                    text(theText), surface(0), newline(false), kerning(0), minx(0)
                {}

                std::string   text;
                SDL_Surface * surface;
                Format        format;
                bool          newline;
                double        kerning;
                int           minx;

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
            std::string makeFontName(const std::string & theName, SDLFontInfo::FONTTYPE theFontType = SDLFontInfo::NORMAL) const;
            const TTF_Font * getFont(const std::string & theName) const;

            void setFontFitting(int theHeight);

            void renderWords(std::vector<Word> & theWords,
                    const std::string & theFontName,
                    const asl::Vector4f & theTextColor);

            unsigned parseNewline(const std::string & theText, unsigned thePos);
            unsigned parseHtmlTag(const std::string & theText, unsigned thePos, Format & theFormat);
            unsigned parseWord(const std::string & theText, unsigned thePos);
            void parseWords(const std::string & theText, std::vector<Word> & theResult);

            SDL_Surface *        _myTextureSurface;
            FontLibrary          _myFonts;
            std::string          _myWordDelimiters;
            unsigned             _myMaxFontFittingSize;

    };
    typedef asl::Ptr<SDLTextRenderer> SDLTextRendererPtr;



} // namespace y60

#endif
