//============================================================================
//
// Copyright (C) 2002-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "SDLTextRenderer.h"

#include <y60/Image.h>

#include <asl/numeric_functions.h>
#include <asl/file_functions.h>
#include <asl/Dashboard.h>
#include <asl/Logger.h>

#include <iostream>

#include <sdl/SDL_ttf.h>

#define DB(x) //x
#define DB2(x) //x

//#define DUMP_TEXT_AS_PNG
#ifdef DUMP_TEXT_AS_PNG
#include <paintlib/plpngenc.h>
#include <paintlib/pltiffenc.h>
#include <paintlib/planybmp.h>
#endif

using namespace std;
using namespace asl;

namespace y60 {

    SDLTextRenderer::SDLTextRenderer() :
         _myRenderStyle(Text::BLENDED_TEXT), _myTextureSurface(0),
         _myWordDelimiters(" \t;-:\\/|"), _myMaxFontFittingSize(30)
    {
        if ( TTF_Init() < 0 ) {
            AC_ERROR << "Couldn't initialize TTF: "<< SDL_GetError()<<endl;
        }
    }

    SDLTextRenderer::~SDLTextRenderer() {
        if (_myTextureSurface) {
            SDL_FreeSurface(_myTextureSurface);
        }

        for (FontLibrary::iterator myIt = _myFonts.begin(); myIt != _myFonts.end(); ++myIt) {
            TTF_CloseFont((myIt->second).getFont());
        }
        TTF_Quit();
    }

    void
    SDLTextRenderer::setFontFitting(int theHeight) {
        if (theHeight > _myMaxFontFittingSize) {
            TTF_SetFitting(0);
        } else {
            TTF_SetFitting(1);
        }
    }

    std::string
    SDLTextRenderer::makeFontName(const string & theName, SDLFontInfo::FONTTYPE theFontType) const
    {
        return theName + "_" + getStringFromEnum(theFontType, FontTypeStrings);
    }

    void
    SDLTextRenderer::loadFont(const string & theName, const string & theFileName,
                              int theHeight, TTFFontInfo::FONTTYPE theFontType)
    {
        setFontFitting(theHeight);

        string myFontName = makeFontName(theName, theFontType);
        if (_myFonts.find(myFontName) != _myFonts.end()) {
            // Font already loaded
            return;
        }

        if (theFontType != SDLFontInfo::NORMAL) {
            if (_myFonts.find(makeFontName(theName, SDLFontInfo::NORMAL)) == _myFonts.end()) {
                throw GLTextRendererException("You must register a normal style font, before registering bold or italic fonts", PLUS_FILE_LINE);
            }
        }

        if (!fileExists(theFileName)) {
            throw GLTextRendererException(string("Font file '") + theFileName + "' does not exist.", PLUS_FILE_LINE);
        }
        TTF_Font * myFont = TTF_OpenFont(theFileName.c_str(), theHeight);

        if (!myFont) {
            throw GLTextRendererException(string("Could not load font: ") + theName+ ", " + theFileName, PLUS_FILE_LINE);
        }
        _myFonts[myFontName] = SDLFontInfo(myFont, theFontType, theHeight);

        AC_DEBUG << "TTFTextRenderer - loaded font: " << theName << ", " << theFileName << " with size: "
                << theHeight << " and style: " << theFontType << endl;

        AC_DEBUG << "Recommended line skip: " << TTF_FontLineSkip(myFont) << endl;
        AC_DEBUG << "Font familiy name: " << TTF_FontFaceFamilyName(myFont) << endl;
        AC_DEBUG << "Font style name: " << TTF_FontFaceStyleName(myFont) << endl;
    }

    SDLFontInfo &
    SDLTextRenderer::getFontInfo(const string & theName) {
        FontLibrary::iterator myIt = _myFonts.find(theName);
        if (myIt == _myFonts.end()) {
            throw GLTextRendererException(string("Font: ") + theName+ " not found in fontlibrary.", PLUS_FILE_LINE);
        }
        return myIt->second;
    }

    const TTF_Font *
    SDLTextRenderer::getFont(const string & theName) const {
        FontLibrary::const_iterator myIt = _myFonts.find(theName);
        if (myIt == _myFonts.end()) {
            return 0;
        }
        return myIt->second.getFont();
    }

    TextPtr
    SDLTextRenderer::createText(const asl::Vector2f & thePos,
                                const std::string & theString, const std::string & theFontName)
    {
        return TextPtr(new SDLText(this, thePos, getTextColor(), getBackColor(), theString, _myRenderStyle, theFontName));
    }

    bool
    SDLTextRenderer::haveFont(const std::string theName) {
        return (_myFonts.find(makeFontName(theName, SDLFontInfo::NORMAL)) != _myFonts.end());
    }

    void
    SDLTextRenderer::setRenderStyle(Text::RENDERSTYLE theStyle) {
        _myRenderStyle = theStyle;
    }

    Vector2i
    SDLTextRenderer::renderTextAsImage(TextureManager & theTextureManager,
                                            dom::NodePtr theImageNode,
                                            const string & theText,
                                            const string & theFontName,
                                            unsigned int theTargetWidth,
                                            unsigned int theTargetHeight)
    {
        TTF_SetTracking(_myTracking);

        Vector2i myTextSize = createTextSurface(theText, theFontName, _myRenderStyle,
            getTextColor(), getBackColor(),
            theTargetWidth, theTargetHeight);

        ImagePtr myImage = theImageNode->getFacade<y60::Image>();
        unsigned myOldWidth = myImage->get<ImageWidthTag>();
        unsigned myOldHeight = myImage->get<ImageHeightTag>();
        unsigned int myImageDataSize = _myTextureSurface->w * _myTextureSurface->h * sizeof(asl::RGBA);
        myImage->set(_myTextureSurface->w, _myTextureSurface->h, 1, y60::RGBA,
                        ReadableBlockAdapter((unsigned char*)_myTextureSurface->pixels,
                        (unsigned char*)_myTextureSurface->pixels + myImageDataSize));
        if ((_myTextureSurface->w == myOldWidth) && (_myTextureSurface->h == myOldHeight)) {
            // reuse glTexture
            theTextureManager.updateImageData(myImage);
        } else {
            // resize glTexture
            theTextureManager.rebind(myImage);
        }

#ifdef DUMP_TEXT_AS_PNG
        PLAnyBmp myBmp;
        myBmp.Create( _myTextureSurface->w, _myTextureSurface->h, PLPixelFormat::A8B8G8R8,
                (unsigned char*)_myTextureSurface->pixels, _myTextureSurface->w*4);
        PLPNGEncoder myEncoder;
        myEncoder.MakeFileFromBmp("test.png", &myBmp);
#endif

        return myTextSize;
    }

    bool
    SDLTextRenderer::getFontMetrics(const std::string & theFontName,
            int & theFontHeight,
            int & theFontAscent, int & theFontDescent,
            int & theFontLineSkip) const
    {
        SDLFontInfo::FONTTYPE myFontType = SDLFontInfo::NORMAL;
        std::string myFontName = makeFontName(theFontName, myFontType);
        TTF_Font* myFont = const_cast<TTF_Font*>(getFont(myFontName));
        if (!myFont) {
            throw GLTextRendererException(string("Font: ") + myFontName + " not in fontlibrary", PLUS_FILE_LINE);
        }

        theFontHeight = TTF_FontHeight(myFont);
        theFontAscent = TTF_FontAscent(myFont);
        theFontDescent = TTF_FontDescent(myFont);
        theFontLineSkip = TTF_FontLineSkip(myFont);

        return true;
    }

    bool
    SDLTextRenderer::getGlyphMetrics(const std::string & theFontName, const std::string & theCharacter,
                                     asl::Box2f & theGlyphBox, double & theAdvance) const
    {
        SDLFontInfo::FONTTYPE myFontType = SDLFontInfo::NORMAL;
        std::string myFontName = makeFontName(theFontName, myFontType);
        const TTF_Font* myFont = getFont(myFontName);
        if (!myFont) {
            throw GLTextRendererException(string("Font: ") + myFontName + " not in fontlibrary", PLUS_FILE_LINE);
        }

        Uint16 myUnicodeChar[2];
        UTF8_to_UNICODE(myUnicodeChar, theCharacter.c_str(), 1);

        int xmin,xmax, ymin,ymax, advance;
        if (TTF_GlyphMetrics((TTF_Font*) myFont, myUnicodeChar[0], &xmin, &xmax, &ymin, &ymax, &advance) < 0) {
            throw GLTextRendererException(string("Unable to get metrics for font: ") + theFontName, PLUS_FILE_LINE);
        }
        theGlyphBox.setBounds(float(xmin), float(ymin), float(xmax), float(ymax));
        theAdvance = double(advance);

        return true;
    }

    double SDLTextRenderer::getKerning(const std::string& theFontName,
                                       const std::string& theFirstCharacter, const std::string& theSecondCharacter) const
    {
        SDLFontInfo::FONTTYPE myFontType = SDLFontInfo::NORMAL;
        std::string myFontName = makeFontName(theFontName, myFontType);
        const TTF_Font* myFont = getFont(myFontName);
        if (!myFont) {
            throw GLTextRendererException(string("Font: ") + myFontName + " not in fontlibrary", PLUS_FILE_LINE);
        }

        Uint16 myFirstUnicodeChar[2];
        Uint16 mySecondUnicodeChar[2];

        UTF8_to_UNICODE(myFirstUnicodeChar,  theFirstCharacter.c_str(),  1);
        UTF8_to_UNICODE(mySecondUnicodeChar, theSecondCharacter.c_str(), 1);

        double myKerning = TTF_Kerning((TTF_Font*) myFont, myFirstUnicodeChar[0], mySecondUnicodeChar[0]);
        DB(AC_TRACE << "### myFont=" << myFontName << " Ch=" << theFirstCharacter << theSecondCharacter << " myKerning=" << myKerning << endl;)
        return myKerning;
    }

    bool SDLTextRenderer::hasGlyph(const std::string& theFontName,
                                       const std::string& theCharacter) const
    {
        SDLFontInfo::FONTTYPE myFontType = SDLFontInfo::NORMAL;
        std::string myFontName = makeFontName(theFontName, myFontType);
        const TTF_Font* myFont = getFont(myFontName);
        if (!myFont) {
            throw GLTextRendererException(string("Font: ") + myFontName + " not in fontlibrary", PLUS_FILE_LINE);
        }

        Uint16 myUnicodeChar[2];
        UTF8_to_UNICODE(myUnicodeChar,  theCharacter.c_str(),  1);
        double myResult = TTF_HasGlyph((TTF_Font*) myFont, myUnicodeChar[0]);
        DB(AC_TRACE << "### myFont=" << myFontName << " Ch=" << theCharacter << " available=" << myResult << endl;)
        return myResult==0;

    }

    int
    SDLTextRenderer::calcHorizontalAlignment(unsigned theSurfaceWidth, unsigned theLineWidth,
        int theMinX)
    {
        if (theMinX < 0) {
            theMinX = 0;
        }
        switch(_myHorizontalAlignment) {
            case LEFT_ALIGNMENT:
                return _myLeftPadding - theMinX;
            case RIGHT_ALIGNMENT:
                return theSurfaceWidth-_myRightPadding-theLineWidth;
            case CENTER_ALIGNMENT:
                return (_myLeftPadding-_myRightPadding+theSurfaceWidth-theLineWidth) / 2;
        }
        return 0;
    }

    int
    SDLTextRenderer::calcVerticalAlignment(unsigned theSurfaceHeight, unsigned theTextHeight) {
        switch(_myVerticalAlignment) {
            case TOP_ALIGNMENT:
                return _myTopPadding;
            case BOTTOM_ALIGNMENT:
                return theSurfaceHeight-_myBottomPadding-theTextHeight;
            case CENTER_ALIGNMENT:
                return (_myTopPadding-_myBottomPadding+theSurfaceHeight-theTextHeight) / 2;
        }
        return 0;
    }

    void
    SDLTextRenderer::createTargetSurface(unsigned theWidth, unsigned theHeight) {
        DB2(AC_TRACE << "createTargetSurface with size: " << theWidth << "x" << theHeight << endl;)
        SDL_FreeSurface(_myTextureSurface);

        _myTextureSurface = SDL_CreateRGBSurface(
                SDL_SWSURFACE,
                theWidth, theHeight,
                32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
                0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#else
                0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#endif
                );

        if (_myTextureSurface == 0) {
            throw GLTextRendererException("TextRenderer::createTargetSurface - Could not create SDL RGB Surface",
                PLUS_FILE_LINE);
        }
    }

    SDL_Surface *
    SDLTextRenderer::renderToSurface(
        string theText,
        Text::RENDERSTYLE theRenderStyle,
        const TTF_Font * theFont,
        const SDL_Color & theTextColor,
        const SDL_Color & theBackColor)
    {
        if (!theText.size()) {
            theText = " ";
        }

        SDL_Surface * myTextSurface;
        switch (theRenderStyle) {
            case SDLText::BLENDED_TEXT:
                myTextSurface = TTF_RenderUTF8_Blended((TTF_Font*) theFont, theText.c_str(), theTextColor);
                //SDL_SaveBMP(myTextSurface, string(string("word_") + theText + ".bmp" ).c_str());

                break;
            case SDLText::SHADED_TEXT:
                myTextSurface = TTF_RenderUTF8_Shaded((TTF_Font*) theFont, theText.c_str(), theTextColor, theBackColor);
                break;
            case SDLText::SOLID_TEXT:
#ifdef LINUX
                // solid doesn't work under linux,
                // the ft_render_mode_mono always fails,
                // so we use blended instead
                myTextSurface = TTF_RenderUTF8_Blended((TTF_Font*) theFont, theText.c_str(), theTextColor);
#else
                myTextSurface = TTF_RenderUTF8_Solid((TTF_Font*) theFont, theText.c_str(), theTextColor);
#endif
                break;
            default:
                throw GLTextRendererException("SDLTextRenderer - unknown render quality.", PLUS_FILE_LINE);
        }

        if (!myTextSurface) {
            throw GLTextRendererException(string("SDLTextRenderer - TTF text rendering failed while rendering '") +
                    theText + "'.", PLUS_FILE_LINE);
        }

        SDL_SetAlpha(myTextSurface, 0, 0);

        if (theRenderStyle != SDLText::SHADED_TEXT) {
            /*
             * UH: enabling this breaks text rendering on Windows/SDL-1.2.8 but works fine
             *     under Windows/SDL-1.2.6 and Linux.
             */
#ifdef LINUX
            SDL_SetColorKey(myTextSurface, SDL_SRCCOLORKEY, 0);
#endif
        }

        DB(AC_TRACE << "Text Surface: (" << myTextSurface->w << "x" << myTextSurface->h << ")" << endl);
        return myTextSurface;
    }

    // returns offset past end of s if s is the exact next part
    inline unsigned
    read_if_string(const std::string & is, unsigned pos, const std::string & s) {
        int i = 0;
        int n = asl::minimum(s.size(),is.size()-pos);
        while (i < n && pos < is.size() && is[pos+i] == s[i]) {
            ++i;
        }
        if (i == s.size()) {
            return i;
        }
        return 0;
    }

    inline unsigned
    SDLTextRenderer::parseNewline(const string & theText, unsigned thePos) {
        if (theText[thePos] == '\n' && thePos < theText.size()) {
            return 1;
        }
        if (theText.size() > thePos + 3 && theText[thePos] == '<' &&
            theText[thePos + 2] == '/' && theText[thePos + 3] == '>')
        {
            switch (theText[thePos + 1]) {
                case 'p':
                case 'P':
                    return 4;
            }
        }

        return 0;
    }

    inline unsigned
    SDLTextRenderer::parseHtmlTag(const string & theText, unsigned thePos, Format & theFormat) {
        bool myOpenFlag = false;
        char myChar     = 0;
        if (theText.size() > thePos + 2 &&
            theText[thePos] == '<' &&
            theText[thePos + 2] == '>')
        {
            myChar = theText[thePos + 1];
            myOpenFlag = true;
        } else if (theText.size() > thePos + 3 &&
            theText[thePos] == '<' &&
            theText[thePos + 1] == '/' &&
            theText[thePos + 3] == '>')
        {
            myChar = theText[thePos + 2];
            myOpenFlag = false;
        }

        if (myChar) {
            switch (myChar) {
                case 'b':
                case 'B':
                    theFormat.bold = myOpenFlag;
                    return 4 - myOpenFlag;
                case 'i':
                case 'I':
                    theFormat.italics = myOpenFlag;
                    return 4 - myOpenFlag;
                case 'u':
                case 'U':
                    theFormat.underline = myOpenFlag;
                    return 4 - myOpenFlag;
            }
        }

        return 0;
    }

    inline unsigned
    SDLTextRenderer::parseWord(const string & theText, unsigned thePos) {
        for (unsigned i = 0; i < _myWordDelimiters.size(); ++i) {
            if (theText[thePos] == _myWordDelimiters[i]) {
                string::size_type myNextWordPos = theText.find_first_not_of(_myWordDelimiters, thePos);
                if (myNextWordPos == string::npos) {
                    return theText.size() - thePos;
                } else {
                    return myNextWordPos - thePos;
                }
            }
        }

        return 0;
    }

    void
    SDLTextRenderer::parseWords(const string & theText, vector<Word> & theResult) {
        unsigned myTextPos   = 0;
        unsigned myWordStart = 0;
        unsigned myWordEnd   = 0;
        Format myFormat;
        Format myNewFormat;
        while (myTextPos < theText.size()) {
            unsigned myOffset = 0;
            if (myOffset = parseNewline(theText, myTextPos)) {
                theResult.push_back(Word(theText.substr(myWordStart, myWordEnd - myWordStart)));
                theResult.back().newline = true;
                theResult.back().format  = myFormat;
                myTextPos += myOffset;
                myWordStart = myTextPos;
                myWordEnd   = myTextPos;
                DB2(AC_TRACE << "Found newline, word: '" << theResult.back().text << "' new start: " << myWordStart << endl;)
            } else if (myOffset = parseHtmlTag(theText, myTextPos, myNewFormat)) {
                DB2(AC_TRACE << "Found tag: bold: " << myNewFormat.bold << " italics: " << myNewFormat.italics << " underline: "
                     << myNewFormat.underline << endl;)
                if (myWordStart != myWordEnd) {
                    theResult.push_back(Word(theText.substr(myWordStart, myWordEnd - myWordStart)));
                    theResult.back().format = myFormat;
                    DB2(AC_TRACE << "Add word: '" << theResult.back().text << "' new start: " << myWordStart << endl;)
                }
                myFormat = myNewFormat;
                myTextPos  += myOffset;
                myWordStart = myTextPos;
                myWordEnd   = myTextPos;
            } else if (myOffset = parseWord(theText, myTextPos)) {
                theResult.push_back(Word(theText.substr(myWordStart, myWordEnd + myOffset - myWordStart)));
                theResult.back().format = myFormat;
                myTextPos  += myOffset;
                myWordStart = myTextPos;
                myWordEnd   = myTextPos;
                DB2(AC_TRACE << "Found word-end, word: '" << theResult.back().text << "' new start: " << myWordStart << endl;)
            } else {
                myTextPos++;
                myWordEnd++;
            }
        }

        if (myWordStart != myWordEnd) {
            theResult.push_back(Word(theText.substr(myWordStart, myWordEnd - myWordStart)));
            theResult.back().format = myFormat;
            DB2(AC_TRACE << "Add last word: '" << theResult.back().text << "'" << endl;)
        }
    }

    // Calculate line metrics and total blockheight
    unsigned
    SDLTextRenderer::createLines(const vector<Word> & theWords, vector<Line> & theLines,
        unsigned theLineWidth, unsigned theLineHeight)
    {
        DB2(AC_TRACE << "-------  Create lines  -------" << endl;)
        unsigned myNewlineCount = 0;
        theLines.push_back(Line());

        for (unsigned i = 0; i < theWords.size(); ++i) {
            const Word & myWord = theWords[i];

            // Update line metrics
            theLines.back().wordCount++;
            theLines.back().width += myWord.surface->w;

            if (myWord.newline) {
                DB2(AC_TRACE << "  New line with width: " << theLines.back().width <<
                    " and wordcount: "<< theLines.back().wordCount << endl;)
                theLines.back().newline = true;
                theLines.push_back(Line());
                myNewlineCount++;
            } else {
                // Check if next word fits into the line
                if (i < theWords.size() - 1 && theLines.back().width + theWords[i+1].surface->w > theLineWidth) {
                    DB2(AC_TRACE << "  End of line with width: " << theLines.back().width <<
                        " wordcount: "<< theLines.back().wordCount << endl;)
                    theLines.push_back(Line());
                }
            }
        }

        // Calculate total height
        unsigned myTotalHeight = (myNewlineCount + 1) * (_myParagraphBottomOffset + _myParagraphTopOffset);
        if (theWords.size()) {
            // Make sure the last is rendered all the way to the bottom
            myTotalHeight += theWords[0].surface->h;
        }
        if (theWords.size() > 1) {
            myTotalHeight += (theLines.size() - 1) * theLineHeight;
        }
        return myTotalHeight;
    }

    void
    SDLTextRenderer::renderWords(vector<Word> & theWords,
                                 const string & theFontName,
                                 Text::RENDERSTYLE theRenderStyle,
                                 const Vector4f & theTextColor,
                                 const Vector4f & theBackColor)
    {
        DB2(AC_TRACE << "-------  Render Words  -------" << endl;)
        SDLFontInfo mySDLFontInfo = (getFontInfo(makeFontName(theFontName, SDLFontInfo::NORMAL)));
        setFontFitting(mySDLFontInfo.getHeight());

        TTF_Font * myNormalFont = mySDLFontInfo.getFont();
        if (!myNormalFont) {
            throw GLTextRendererException("Internal error: Normal font not defined.", PLUS_FILE_LINE);
        }

        const TTF_Font * myBoldFont       = getFont(makeFontName(theFontName, SDLFontInfo::BOLD));
        const TTF_Font * myItalicFont     = getFont(makeFontName(theFontName, SDLFontInfo::ITALIC));
        const TTF_Font * myBoldItalicFont = getFont(makeFontName(theFontName, SDLFontInfo::BOLDITALIC));

        SDL_Color myTextColor = { Uint8(theTextColor[0] * 255),
                                  Uint8(theTextColor[1] * 255),
                                  Uint8(theTextColor[2] * 255),
                                  Uint8(theTextColor[3] * 255) };
        SDL_Color myBackColor = { Uint8(theBackColor[0] * 255),
                                  Uint8(theBackColor[1] * 255),
                                  Uint8(theBackColor[2] * 255),
                                  Uint8(theBackColor[3] * 255) };

        for (unsigned i = 0; i < theWords.size(); ++i) {
            Word & myWord = theWords[i];
            const TTF_Font * myFont = myNormalFont;

            int mySDLFormat = 0;
            if (myWord.format.underline) {
                mySDLFormat |= TTF_STYLE_UNDERLINE;
            }

            if (myWord.format.bold && myWord.format.italics && myBoldItalicFont) {
                myFont = myBoldItalicFont;
            } else {
                if (myWord.format.bold) {
                    if (myBoldFont) {
                        myFont = myBoldFont;
                    } else {
                        mySDLFormat |= TTF_STYLE_BOLD;
                    }
                }

                if (myWord.format.italics) {
                    if (myItalicFont) {
                        myFont = myItalicFont;
                    } else {
                        mySDLFormat |= TTF_STYLE_ITALIC;
                    }
                }
            }

            DB2(AC_TRACE << "Rendering word: '" << myWord.text << "' with format: " << mySDLFormat << endl;)
            TTF_SetFontStyle((TTF_Font*) myFont, mySDLFormat);

            myWord.surface = renderToSurface(myWord.text, theRenderStyle, myFont, myTextColor, myBackColor);
            myWord.minx = TTF_CurrentLineMinX();

            TTF_SetFontStyle((TTF_Font*) myFont, TTF_STYLE_NORMAL);

            // Calculate kerning
            /*
            if ((i < myWords.size() - 1) && myWords[i].text.size() && myWords[i+1].text.size()) {
                char myLastChar = myWords[i].text[myWords[i].text.size() - 1];
                char myNextChar = myWords[i + 1].text[0];
                DB2(AC_TRACE << "Kerning between '" << myLastChar << "' and '" << myNextChar << "' is: ";)
                myWords[i].kerning = TTF_Kerning(mySDLFontInfo.getFont(), myLastChar, myNextChar);
                DB2(AC_TRACE << myWords[i].kerning << endl;)
            }
            */
        }
    }

    Vector2i
    SDLTextRenderer::createTextSurface(const string & theText, const string & theFontName,
                                       Text::RENDERSTYLE theRenderStyle,
                                       const Vector4f & theTextColor,
                                       const Vector4f & theBackColor,
                                       unsigned int theTargetWidth,
                                       unsigned int theTargetHeight)
    {
        MAKE_SCOPE_TIMER(SDLTextRenderer_createTextSurface);
        unsigned mySurfaceWidth  = 0;
        unsigned mySurfaceHeight = 0;
        DB2(
            AC_TRACE << "----------------------------------" << endl;
            AC_TRACE << "Rendering text: " << theText << endl;
            AC_TRACE << "Target size: " << theTargetWidth << "x" << theTargetHeight << endl;
            AC_TRACE << "-----------  Splitter  -----------" << endl;
        )

        vector<Word> myWords;
        parseWords(theText, myWords);
        renderWords(myWords, theFontName, theRenderStyle, theTextColor, theBackColor);

        if (theTargetWidth == 0) {
            // Use the surface width and (if not given) height
            unsigned myWidth = 0;
            for (unsigned i = 0; i < myWords.size(); ++i) {
                myWidth += myWords[i].surface->w;
            }
            mySurfaceWidth  = myWidth + _myRightPadding + _myLeftPadding;

            DB2(AC_TRACE << "Set auto-width to " << mySurfaceWidth << endl;)
            if (theTargetHeight == 0) {
                mySurfaceHeight = myWords[0].surface->h + _myTopPadding + _myBottomPadding;
            } else {
                mySurfaceHeight = theTargetHeight;
            }
        } else {
            // Use the given size
            mySurfaceWidth  = theTargetWidth;
            mySurfaceHeight = theTargetHeight;
        }

        vector<Line> myLines;
        unsigned myLineHeight = _myLineHeight;
        if (myLineHeight == 0) {
            myLineHeight = TTF_FontLineSkip(getFontInfo(makeFontName(theFontName)).getFont());
        }

        unsigned myTotalLineHeight = createLines(myWords, myLines,
                    mySurfaceWidth-_myLeftPadding-_myRightPadding, myLineHeight);
        if (theTargetHeight == 0) {
            DB2(AC_TRACE << "Set auto-height to " << myTotalLineHeight << endl;)
            mySurfaceHeight = myTotalLineHeight + _myTopPadding + _myBottomPadding;
        }

        createTargetSurface(nextPowerOfTwo(mySurfaceWidth), nextPowerOfTwo(mySurfaceHeight));

        DB2(
            AC_TRACE << "-------- Text puzzel ----------" << endl;
            AC_TRACE << "Target surface size: " << mySurfaceWidth << " x " << mySurfaceHeight << endl;
            AC_TRACE << "Number of lines: " << myLines.size() << endl;
            AC_TRACE << "Total line height: " << myTotalLineHeight << endl << endl;
        )

        // Render lines
        unsigned myWordCount = 0;
        int myYPos = calcVerticalAlignment(mySurfaceHeight, myTotalLineHeight) + _myParagraphTopOffset;

        for (unsigned i = 0; i < myLines.size(); ++i) {
            int myMinX = myLines[i].wordCount ? myWords[myWordCount].minx : 0;
            int myXPos = calcHorizontalAlignment(mySurfaceWidth, myLines[i].width, myMinX);

            DB2(AC_TRACE << "line: " << i << " wordcount: " << myLines[i].wordCount << endl;)
            for (unsigned j = 0; j < myLines[i].wordCount; ++j) {
                const Word & myWord = myWords[myWordCount];
                myWordCount++;

                DB2(
                    AC_TRACE << "  line:  " << i << endl;
                    AC_TRACE << "  word:  " << j << endl;
                    AC_TRACE << "  text:  " << myWord.text << endl;
                    AC_TRACE << "  Size:  " << myWord.surface->w << " x " << myWord.surface->h << endl;
                    AC_TRACE << "  Pos:   " << myXPos << " x " << myYPos << endl;
                )

                unsigned myLineY = myWord.surface->h;
                if (myLineY + myYPos > mySurfaceHeight-_myBottomPadding) {
                    myLineY = mySurfaceHeight-_myBottomPadding-myYPos;
                }

                SDL_Rect srcRectangle  = {0, 0, myWord.surface->w, myLineY};
                SDL_Rect destRectangle = {myXPos, myYPos, myXPos + myWord.surface->w , myXPos + myWord.surface->h};
                DB2(
                    AC_TRACE << "  Blit: " << srcRectangle.x << ", " << srcRectangle.y << ", "
                        << srcRectangle.w << ", " << srcRectangle.h
                        << "  ->  " << destRectangle.x << ", " << destRectangle.y << ", "
                        << destRectangle.w << ", " << destRectangle.h << endl;
                )

                //SDL_SaveBMP(myWord.surface, string(string("word") + asl::as_string(j) + ".bmp").c_str());
                if (SDL_BlitSurface(myWord.surface, &srcRectangle, _myTextureSurface, &destRectangle)) {
                    AC_ERROR << "SDL_BlitSurface failed: " << SDL_GetError();
                }

                myXPos += myWord.surface->w;
                SDL_FreeSurface(myWord.surface);
            }

            myYPos += myLineHeight;
            if (myLines[i].newline) {
                myYPos += _myParagraphBottomOffset + _myParagraphTopOffset;
            }
        }

        return Vector2i(mySurfaceWidth, mySurfaceHeight);
    }

    void
    SDLTextRenderer::renderText(TextPtr & theText) {
        MAKE_SCOPE_TIMER(SDLTextRenderer_renderText);
        SDLTextPtr mySDLText = dynamic_cast_Ptr<SDLText>(theText);
        if (mySDLText == 0) {
            throw GLTextRendererException("SDLTextRenderer - can only render SDLText objects.", PLUS_FILE_LINE);
        }

        Vector2i myTextSize = createTextSurface(mySDLText->_myString,
            mySDLText->_myFont,
            mySDLText->getRenderStyle(),
            mySDLText->_myTextColor,
            mySDLText->_myBackColor);

        GLfloat texMinX = 0.0f;         /* Min X */
        GLfloat texMinY = 0.0f;         /* Min Y */
        GLfloat texMaxX = (GLfloat)myTextSize[0] / _myTextureSurface->w;
        GLfloat texMaxY = (GLfloat)myTextSize[1] / _myTextureSurface->h;

        const GLuint myTexture = mySDLText->createTextureFromSurface(_myTextureSurface);

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glClientActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, myTexture);
        unsigned int myXPos = (unsigned int) (theText->_myPos[0] * getWindowWidth());
        unsigned int myYPos = (unsigned int) (theText->_myPos[1] * getWindowHeight());

        // Reset Texture matrix
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);

        glBegin(GL_QUADS);
            glTexCoord2f(texMinX,texMinY); glVertex2i(myXPos, myYPos);
            glTexCoord2f(texMinX,texMaxY); glVertex2i(myXPos, myYPos + myTextSize[1]);
            glTexCoord2f(texMaxX,texMaxY); glVertex2i(myXPos + myTextSize[0], myYPos + myTextSize[1]);
            glTexCoord2f(texMaxX,texMinY); glVertex2i(myXPos + myTextSize[0], myYPos);
        glEnd();

        DB(AC_TRACE << "unbinding texture" << endl);
        glBindTexture(GL_TEXTURE_2D,0);
        CHECK_OGL_ERROR;
    }
}
