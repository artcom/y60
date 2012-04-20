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

// own header
#include "SDLTextRenderer.h"

#include <iostream>

#include <asl/math/numeric_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/Dashboard.h>
#include <asl/base/Logger.h>
#include <asl/zip/PackageManager.h>

#include <y60/image/Image.h>

#include <sdl/ttf2/SDL_ttf.h>

//#define DUMP_TEXT_AS_PNG
#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#ifdef DUMP_TEXT_AS_PNG
#include <paintlib/plpngenc.h>
#include <paintlib/pltiffenc.h>
#include <paintlib/planybmp.h>
#endif
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#define DB(x) //x
#define DB2(x) //x

using namespace std;
using namespace asl;

namespace y60 {

    SDLTextRenderer::SDLTextRenderer() :
         _myTextureSurface(0),
         _myWordDelimiters(" \t;-:\\/|")
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


    std::string
    SDLTextRenderer::makeFontName(const string & theName, SDLFontInfo::FONTTYPE theFontType) const
    {
        return theName + "_" + getStringFromEnum(theFontType, FontTypeStrings);
    }

    void
    SDLTextRenderer::loadFont(const string & theName, const string & theFileName,
                              int theHeight, TTFFontInfo::FONTHINTING & theFonthint,
                              TTFFontInfo::FONTTYPE theFontType,
                              int theAscendOffset)
    {
        const string myFileName = AppPackageManager::get().getPtr()->searchFile(theFileName);

        string myFontName = makeFontName(theName, theFontType);
        if (_myFonts.find(myFontName) != _myFonts.end()) {
            // Font already loaded
            return;
        }
        _myFontHintingMap[theName] = theFonthint;

        if (theFontType != SDLFontInfo::NORMAL) {
            if (_myFonts.find(makeFontName(theName, SDLFontInfo::NORMAL)) == _myFonts.end()) {
                throw GLTextRendererException("You must register a normal style font with the same name, before registering bold or italic fonts", PLUS_FILE_LINE);
            }
        }

        if (!fileExists(myFileName)) {
            throw GLTextRendererException(string("Font file '") + myFileName + "' does not exist.", PLUS_FILE_LINE);
        }
        TTF_Font * myFont = TTF_OpenFont(myFileName.c_str(), theHeight, theAscendOffset);

        if (!myFont) {
            throw GLTextRendererException(string("Could not load font: ") + theName+ ", " + myFileName, PLUS_FILE_LINE);
        }
        TTF_SetFontHinting(myFont, (int)_myFontHintingMap[theName]);
        _myFonts[myFontName] = SDLFontInfo(myFont, theFontType, theHeight, theFonthint);

        AC_DEBUG << "TTFTextRenderer - loaded font: " << theName << ", " << myFileName << " with size: "
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

    const TTFFontInfo::FONTHINTING &
    SDLTextRenderer::getFontHint(const string & theName) const {
        std::map<std::string, TTFFontInfo::FONTHINTING>::const_iterator myIt = _myFontHintingMap.find(theName);
        if (myIt == _myFontHintingMap.end()) {
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
        return TextPtr(new SDLText(this, thePos, getTextColor(), theString, theFontName));
    }

    bool
    SDLTextRenderer::haveFont(const std::string theName) {
        return (_myFonts.find(makeFontName(theName, SDLFontInfo::NORMAL)) != _myFonts.end());
    }

    Vector2i
    SDLTextRenderer::renderTextAsImage(TextureManager & theTextureManager,
                                            dom::NodePtr theImageNode,
                                            const string & theText,
                                            const string & theFontName,
                                            unsigned int theTargetWidth,
                                            unsigned int theTargetHeight,
                                            const asl::Vector2i & theCursorPos)
    {
        MAKE_GL_SCOPE_TIMER(SDLTextRenderer_renderTextAsImage);
        TTF_SetTracking(_myTextStyle._myTracking);

        ImagePtr myImage = theImageNode->getFacade<y60::Image>();

        if (myImage->getRasterEncoding() != y60::RGBA) {
            myImage->createRaster(1, 1, 1, y60::RGBA);
        }

        _myCursorPos = theCursorPos;
        MAKE_GL_SCOPE_TIMER(SDLTextRenderer_renderTextAsImage2);
        Vector2i myTextSize = createTextSurface(theText, theFontName, getTextColor(),
            theTargetWidth, theTargetHeight);
        if ( myTextSize[0] == 0 && myTextSize[1] == 0) {
            myImage->getRasterPtr()->clear();
            return myTextSize;
        }

        unsigned int myImageDataSize = _myTextureSurface->w * _myTextureSurface->h * sizeof(asl::RGBA);


#ifdef DUMP_TEXT_AS_PNG
        MAKE_GL_SCOPE_TIMER(SDLTextRenderer_renderTextAsImage_dumpAsPng);
        PLAnyBmp myBmp;
        myBmp.Create( _myTextureSurface->w, _myTextureSurface->h, PLPixelFormat::A8B8G8R8,
                (unsigned char*)_myTextureSurface->pixels, _myTextureSurface->w*4);
        PLPNGEncoder myEncoder;
        myEncoder.MakeFileFromBmp("test.png", &myBmp);
#endif
        MAKE_GL_SCOPE_TIMER(SDLTextRenderer_renderTextAsImage_assign);
        myImage->getRasterPtr()->assign(_myTextureSurface->w, _myTextureSurface->h,
                        ReadableBlockAdapter((unsigned char*)_myTextureSurface->pixels,
                        (unsigned char*)_myTextureSurface->pixels + myImageDataSize));


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
        const TTF_Font* myFont = getFont(myFontName);
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
        double myResult = TTF_GlyphIsProvided((TTF_Font*) myFont, myUnicodeChar[0]);
        DB(AC_TRACE << "### myFont=" << myFontName << " Ch=" << theCharacter << " available=" << myResult << endl;)
        return myResult==0;

    }

    int
    SDLTextRenderer::calcHorizontalAlignment(unsigned theSurfaceWidth, const Line & theLine,
        int theMinX)
    {
        if (theMinX < 0) {
            theMinX = 0;
        }
        switch(_myTextStyle._myHorizontalAlignment) {
            case TextStyle::LEFT_ALIGNMENT:
                return _myTextStyle._myLeftPadding - theMinX + static_cast<int>(theLine.indent);
            case TextStyle::RIGHT_ALIGNMENT:
                return static_cast<int>(theSurfaceWidth)-_myTextStyle._myRightPadding-static_cast<int>(theLine.width);
            case TextStyle::CENTER_ALIGNMENT:
                return (_myTextStyle._myLeftPadding-_myTextStyle._myRightPadding+static_cast<int>(theSurfaceWidth)-static_cast<int>(theLine.width)) / 2;
            default:
                break; // avoid unused enum value warning
        }
        return 0;
    }

    int
    SDLTextRenderer::calcVerticalAlignment(unsigned theSurfaceHeight, unsigned theTextHeight) {
        switch(_myTextStyle._myVerticalAlignment) {
            case TextStyle::TOP_ALIGNMENT:
                return _myTextStyle._myTopPadding;
            case TextStyle::BOTTOM_ALIGNMENT:
                return static_cast<int>(theSurfaceHeight)-_myTextStyle._myBottomPadding-static_cast<int>(theTextHeight);
            case TextStyle::CENTER_ALIGNMENT:
                return (_myTextStyle._myTopPadding-_myTextStyle._myBottomPadding+static_cast<int>(theSurfaceHeight)-static_cast<int>(theTextHeight)) / 2;
            default:
                break; // avoid unused enum value warning
        }
        return 0;
    }

    void
    SDLTextRenderer::createTargetSurface(unsigned theWidth, unsigned theHeight, const Vector4f & theTextColor) {
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

        Uint32 myTextColor = SDL_MapRGBA(_myTextureSurface->format, Uint8(theTextColor[0] * 255),
            Uint8(theTextColor[1] * 255), Uint8(theTextColor[2] * 255), 0);

        // Fill background image with text color, to avoid OpenGl texture filtering artefacts.
        SDL_FillRect(_myTextureSurface, 0, myTextColor);
    }

    SDL_Surface *
    SDLTextRenderer::renderToSurface(
        string theText,
        const TTF_Font * theFont,
        const SDL_Color & theTextColor)
    {
        if (!theText.size()) {
            theText = " ";
        }

        SDL_Surface * myTextSurface;
        myTextSurface = TTF_RenderUTF8_Blended((TTF_Font*) theFont, theText.c_str(), theTextColor);
        //SDL_SaveBMP(myTextSurface, string(string("word_") + theText + ".bmp" ).c_str());

        if (!myTextSurface) {
            throw GLTextRendererException(string("SDLTextRenderer - TTF text rendering failed while rendering '") +
                    theText + "'.", PLUS_FILE_LINE);
        }

        SDL_SetAlpha(myTextSurface, 0, 0);

        /*
         * UH: enabling this breaks text rendering on Windows/SDL-1.2.8 but works fine
         *     under Windows/SDL-1.2.6 and Linux.
         */
#ifdef LINUX
        SDL_SetColorKey(myTextSurface, SDL_SRCCOLORKEY, 0);
#endif

        DB(AC_TRACE << "Text Surface: (" << myTextSurface->w << "x" << myTextSurface->h << ")" << endl);
        return myTextSurface;
    }

    // returns offset past end of s if s is the exact next part
    inline unsigned
    read_if_string(const std::string & is, unsigned pos, const std::string & s) {
        std::string::size_type i = 0;
        std::string::size_type n = asl::minimum(s.size(),is.size()-pos);
        while (i < n && pos < is.size() && is[pos+i] == s[i]) {
            ++i;
        }
        if (i == s.size()) {
            return i;
        }
        return 0;
    }

    inline unsigned
    SDLTextRenderer::parseNewline(const string & theText, unsigned thePos, Format & theFormat) {
        if (thePos < theText.size() && theText[thePos] == '\x0A')
        {
            return 1;
        }
        if (thePos + 1 < theText.size() && theText[thePos] == '\x0D' && theText[thePos+1] == '\x0A')
        {
            return 2;
        }

        std::string myTag = extractTag(theText, thePos);
        if ( myTag == "p" || myTag == "P" || myTag == "P/" || myTag =="p/") {
            return myTag.length()+2;
        } else if ( myTag == "br" || myTag == "BR" || 
                    myTag == "Br" || myTag == "bR" ||
                    myTag == "br/" || myTag == "BR/" || 
                    myTag == "Br/" || myTag == "bR/")
        {
            return myTag.length()+2;
        } else if ( myTag == "/li" || myTag == "/LI" || 
                    myTag == "/Li" || myTag == "/lI")
        {
            theFormat.indent = false;
            return myTag.length()+2;
        }

        return 0;
    }

    std::string
    SDLTextRenderer::extractTag(const std::string & theText, unsigned thePos) {
        const std::string & myRemainingText = theText.substr(thePos);
        size_t myOpenIndex = myRemainingText.find_first_of("<");
        size_t myCloseIndex = myRemainingText.find_first_of(">");
        if (myOpenIndex == std::string::npos || myCloseIndex == std::string::npos || 
            myCloseIndex < myOpenIndex || myOpenIndex != 0) {
            return std::string("");
        }
        return myRemainingText.substr(1, myCloseIndex - 1);
    }
    
    unsigned
    SDLTextRenderer::parseHtmlTag(const string & theText, unsigned thePos, Format & theFormat) {
        std::string myTag = extractTag(theText, thePos);
        if (myTag == "/font") {
            theFormat.color[0] = -1.0f;
            theFormat.color[1] = -1.0f;
            theFormat.color[2] = -1.0f;
        } else if (myTag.find("font") == 0) {
            size_t myColorIndex = 0;
            if ((myColorIndex = myTag.find("color=")) != std::string::npos) {
                std::string myFontColor = myTag.substr(myColorIndex+6);
                if ((myFontColor.find("'#") == 0) || (myFontColor.find("\"#") == 0)) {
                    myFontColor = myFontColor.substr(2, 6);
                    unsigned myRed = 0;
                    unsigned myGreen = 0;
                    unsigned myBlue = 0;
                    std::string myRedStr = myFontColor.substr(0,2);
                    asl::is_hex_number(myRedStr, myRed);
                    std::string myGreenStr = myFontColor.substr(2,2);
                    asl::is_hex_number(myGreenStr, myGreen);
                    std::string myBlueStr = myFontColor.substr(4,2);
                    asl::is_hex_number(myBlueStr, myBlue);
                    theFormat.color[0] = myRed/255.0f;
                    theFormat.color[1] = myGreen/255.0f;
                    theFormat.color[2] = myBlue/255.0f;
                }
            }
        } else if (myTag == "b" || myTag == "B") {
            theFormat.bold = true;
        } else if (myTag == "i" || myTag == "I") {
            theFormat.italics = true;
        } else if (myTag == "u" || myTag == "U") {
            theFormat.underline = true;
        } else if (myTag == "/b" || myTag == "/B") {
            theFormat.bold = false;
        } else if (myTag == "/i" || myTag == "/I") {
            theFormat.italics = false;
        } else if (myTag == "/u" || myTag == "/U") {
            theFormat.underline = false;
        } else if (myTag == "/p" || myTag == "/P") {
            // just remove closing tag
        } else if (myTag == "/br" || myTag == "/BR" ||
                   myTag == "/bR" || myTag == "/Br") 
        {
            // just remove closing tag
        } else if (myTag == "li" || myTag == "LI" ||
                   myTag == "lI" || myTag == "Li") 
        {
            theFormat.indent = true;
        } else {
            return 0;
        }

        if (myTag.length() == 0) {
            return 0;
        } else {
            return myTag.length()+2;
        }
    }

    inline unsigned
    SDLTextRenderer::parseWord(const string & theText, unsigned thePos) {
        for (unsigned i = 0; i < _myWordDelimiters.size(); ++i) {
            if (theText[thePos] == _myWordDelimiters[i]) {
                string::size_type myNextWordPos;
                myNextWordPos = theText.find_first_not_of(_myWordDelimiters, thePos);

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
        unsigned myNextWordOffset = 0;
        unsigned myHtmlTagLength = 0;
        
        Format myFormat;
        Format myNewFormat;

        while (myTextPos < theText.size()) {
            unsigned myOffset = 0;

            if (myNextWordOffset > 0) {
                myTextPos += myNextWordOffset;
                myWordEnd   = myTextPos;
                myNextWordOffset = 0;
            }

            if ( 0 != (myOffset = parseNewline(theText, myTextPos, myFormat)) ) {
                theResult.push_back(Word(theText.substr(myWordStart, myWordEnd - myWordStart)));
                theResult.back().newline = true;
                theResult.back().format  = myFormat;
                myNewFormat.indent = myFormat.indent;
                myHtmlTagLength = 0;
                myTextPos += myOffset;
                myWordStart = myTextPos;
                myWordEnd   = myTextPos;
                DB2(AC_TRACE << "Found newline, word: '" << theResult.back().text << "' new start: " << myWordStart << endl;)
            } else if ( 0 != (myOffset = parseHtmlTag(theText, myTextPos, myNewFormat)) ) {
                DB2(AC_TRACE << "Found tag: bold: " << myNewFormat.bold << " italics: " << myNewFormat.italics << " underline: "
                     << myNewFormat.underline << endl;)
                if (myWordStart != myWordEnd) {
                    theResult.push_back(Word(theText.substr(myWordStart, myWordEnd - myWordStart)));
                    theResult.back().format = myFormat;
                    theResult.back().taglength  = myHtmlTagLength;
                    myHtmlTagLength = 0;
                    DB2(AC_TRACE << "Add word: '" << theResult.back().text << "' new start: " << myWordStart << endl;)
                }
                myFormat = myNewFormat;
                myHtmlTagLength = myOffset;
                myTextPos  += myOffset;
                myWordStart = myTextPos;
                myWordEnd   = myTextPos;
            } else if ( 0 != (myOffset = parseWord(theText, myTextPos))) {
                if (_myTextStyle._myHorizontalAlignment == TextStyle::RIGHT_ALIGNMENT) {
                    theResult.push_back(Word(theText.substr(myWordStart, myWordEnd - myWordStart)));
                    myNextWordOffset = myOffset;
                } else {
                    theResult.push_back(Word(theText.substr(myWordStart, myWordEnd + myOffset - myWordStart)));
                    myTextPos  += myOffset;
                }

                theResult.back().format = myFormat;
                theResult.back().taglength  = myHtmlTagLength;
                myHtmlTagLength = 0;

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
            theResult.back().taglength  = myHtmlTagLength;
            myHtmlTagLength = 0;

            DB2(AC_TRACE << "Add last word: '" << theResult.back().text << "'" << endl;)
        }
    }

    // Calculate line metrics and total blockheight
    unsigned
    SDLTextRenderer::createLines(const vector<Word> & theWords, vector<Line> & theLines,
        unsigned theLineWidth, unsigned theLineHeight, unsigned theSurfaceHeight)
    {
        DB2(AC_TRACE << "-------  Create lines  -------" << endl;)
        unsigned myNewlineCount = 0;
        theLines.push_back(Line());
        theLines.back().width = _myCursorPos[0];
        theLines.back().indent = _myCursorPos[0];
        _myCursorPos[0] = 0;
        bool myIndentNextLine = false;

        for (unsigned i = 0; i < theWords.size(); ++i) {
            const Word & myWord = theWords[i];
            myIndentNextLine = myWord.format.indent;
            if (theLines.back().width + myWord.surface->w > theLineWidth && ((i+1)*theLineHeight <= theSurfaceHeight)) {
                // start new line
                theLines.push_back(Line());
                theLines.back().indent = (myIndentNextLine) ? _myTextStyle._myIndentation:0;
                theLines.back().width = (myIndentNextLine) ? _myTextStyle._myIndentation:0;
            }

            // Update line metrics
            theLines.back().wordCount++;
            theLines.back().width += myWord.surface->w;

            if (myWord.newline) {
                DB2(AC_TRACE << "  New line with width: " << theLines.back().width <<
                    " and wordcount: "<< theLines.back().wordCount << endl;)
                theLines.back().newline = true;
                theLines.push_back(Line());
                theLines.back().indent =  (myIndentNextLine) ? _myTextStyle._myIndentation:0;
                theLines.back().width =  (myIndentNextLine) ? _myTextStyle._myIndentation:0;
                myNewlineCount++;
            } else {
                // Check if next word fits into the line
                if (i < theWords.size() - 1 && theLines.back().width + theWords[i+1].surface->w > theLineWidth) {
                    DB2(AC_TRACE << "  End of line with width: " << theLines.back().width <<
                        " wordcount: "<< theLines.back().wordCount << endl;)
                    // start new line
                    theLines.push_back(Line());
                    theLines.back().indent =  (myIndentNextLine) ? _myTextStyle._myIndentation:0;
                    theLines.back().width =  (myIndentNextLine) ? _myTextStyle._myIndentation:0;
                }
            }
        }

        // Calculate total height
        unsigned myTotalHeight = (myNewlineCount + 1) * (_myTextStyle._myParagraphBottomOffset + _myTextStyle._myParagraphTopOffset);
        if (theWords.size()) {
            // Make sure the last is rendered all the way to the bottom
            myTotalHeight += theWords[0].surface->h;
        }
        myTotalHeight += (theLines.size() - 1) * theLineHeight;
        return myTotalHeight;
    }

    void
    SDLTextRenderer::renderWords(vector<Word> & theWords,
                                 const string & theFontName,
                                 const Vector4f & theTextColor)
    {
        DB2(AC_TRACE << "-------  Render Words  -------" << endl;)
        SDLFontInfo mySDLFontInfo = (getFontInfo(makeFontName(theFontName, SDLFontInfo::NORMAL)));

        TTF_Font * myNormalFont = mySDLFontInfo.getFont();
        if (!myNormalFont) {
            throw GLTextRendererException("Internal error: Normal font not defined.", PLUS_FILE_LINE);
        }

        if (TTF_GetFontHinting (myNormalFont) != (int)getFontHint(theFontName)) {
            TTF_SetFontHinting(myNormalFont, (int)getFontHint(theFontName));
        }

        const TTF_Font * myBoldFont       = getFont(makeFontName(theFontName, SDLFontInfo::BOLD));
        const TTF_Font * myItalicFont     = getFont(makeFontName(theFontName, SDLFontInfo::ITALIC));
        const TTF_Font * myBoldItalicFont = getFont(makeFontName(theFontName, SDLFontInfo::BOLDITALIC));

        SDL_Color myTextColor = { Uint8(theTextColor[0] * 255),
                                  Uint8(theTextColor[1] * 255),
                                  Uint8(theTextColor[2] * 255),
                                  Uint8(theTextColor[3] * 255) };

        for (unsigned i = 0; i < theWords.size(); ++i) {
            SDL_Color myWordColor = myTextColor;
            Word & myWord = theWords[i];
            const TTF_Font * myFont = myNormalFont;

            int mySDLFormat = 0;
            if (myWord.format.underline) {
                mySDLFormat |= TTF_STYLE_UNDERLINE;
            }

            if(myWord.format.color[0] != -1) {
                SDL_Color myNewTextColor = { Uint8(myWord.format.color[0] * 255),
                                Uint8(myWord.format.color[1] * 255),
                                Uint8(myWord.format.color[2] * 255),
                                Uint8(myWord.format.color[3] * 255) };
                myWordColor = myNewTextColor;
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

            myWord.surface = renderToSurface(myWord.text, myFont, myWordColor);
            int* myXPositions = TTF_getCurrentGlyphXPositions();
            int myReallyBlittedCharacterCount = TTF_getCurrentGlyphXPositionsCount();
            myWord.sdl_x_position.clear();
            for (int i = 0; i < myReallyBlittedCharacterCount/2 ;i++ ) {
                myWord.sdl_x_position.push_back(myXPositions[i*2]);
                myWord.sdl_x_position.push_back(myXPositions[(i*2)+1]);
                //i +=2;
            }
            myWord.minx = TTF_CurrentLineMinX();
            TTF_SetFontStyle((TTF_Font*) myFont, TTF_STYLE_NORMAL);
        }
    }

    Vector2i
    SDLTextRenderer::createTextSurface(const string & theText, const string & theFontName,
                                       const Vector4f & theTextColor,
                                       unsigned int theTargetWidth,
                                       unsigned int theTargetHeight)
    {
        unsigned mySurfaceWidth  = 0;
        unsigned mySurfaceHeight = 0;

        DB2(
            {AC_TRACE << "----------------------------------" << endl;}
            {AC_TRACE << "Rendering text: " << theText << endl;}
            {AC_TRACE << "Target size: " << theTargetWidth << "x" << theTargetHeight << endl;}
            {AC_TRACE << "-----------  Splitter  -----------" << endl;}
        )
        _myGlyphPosition.clear();

        vector<Word> myWords;
        parseWords(theText, myWords);
        if (myWords.empty()) {
            return Vector2i(0,0);
        }
        renderWords(myWords, theFontName, theTextColor);

        if (theTargetWidth == 0) {
            // Use the surface width and (if not given) height
            unsigned myWidth = 0;
            unsigned myMaxWidth = 0;
            for (unsigned i = 0; i < myWords.size(); ++i) {
                myWidth += myWords[i].surface->w;
                if (myWords[i].newline) {
                    myMaxWidth = std::max( myMaxWidth, myWidth );
                    myWidth = 0;
                }
            }
            myMaxWidth = std::max( myMaxWidth, myWidth );
            mySurfaceWidth  = myMaxWidth + _myTextStyle._myRightPadding + _myTextStyle._myLeftPadding;

            DB2(AC_TRACE << "Set auto-width to " << mySurfaceWidth << endl;)
            if (theTargetHeight == 0) {
                mySurfaceHeight = myWords[0].surface->h + _myTextStyle._myTopPadding + _myTextStyle._myBottomPadding;
            } else {
                mySurfaceHeight = theTargetHeight;
            }
        } else {
            // Use the given size
            mySurfaceWidth  = theTargetWidth;
            mySurfaceHeight = theTargetHeight;
        }

        vector<Line> myLines;
        unsigned myLineHeight = _myTextStyle._myLineHeight;
        if (myLineHeight == 0) {
            myLineHeight = TTF_FontLineSkip(getFontInfo(makeFontName(theFontName)).getFont());
        }

        unsigned myTotalLineHeight = createLines(myWords, myLines,
                    mySurfaceWidth-_myTextStyle._myLeftPadding-_myTextStyle._myRightPadding, myLineHeight, mySurfaceHeight);
        if (theTargetHeight == 0) {
            DB2(AC_TRACE << "Set auto-height to " << myTotalLineHeight << endl;)
            mySurfaceHeight = myTotalLineHeight + _myTextStyle._myTopPadding +_myCursorPos[1]+ _myTextStyle._myBottomPadding;
        }

        _myMaxWidth = 0;
        _myLineWidths.clear();
        _myLineWidths.reserve(myLines.size());
        for (unsigned i = 0; i < myLines.size(); ++i) {
            _myMaxWidth = std::max(myLines[i].width, _myMaxWidth);
            _myLineWidths.push_back(myLines[i].width);
        }

        createTargetSurface( mySurfaceWidth, mySurfaceHeight, theTextColor);

        DB2(
            {AC_TRACE << "-------- Text puzzle ----------" << endl;}
            {AC_TRACE << "Target surface size: " << mySurfaceWidth << " x " << mySurfaceHeight << endl;}
            {AC_TRACE << "Number of words: " << myWords.size() << endl;}
            {AC_TRACE << "Number of lines: " << myLines.size() << endl;}
            {AC_TRACE << "Total line height: " << myTotalLineHeight << endl << endl;}
        )

        // Render lines
        unsigned myWordCount = 0;
        int myYPos = calcVerticalAlignment(mySurfaceHeight, myTotalLineHeight) + _myTextStyle._myParagraphTopOffset + _myCursorPos[1];
        for (unsigned i = 0; i < myLines.size(); ++i) {
            int myMinX = myLines[i].wordCount ? myWords[myWordCount].minx : 0;
            int myXPos = calcHorizontalAlignment(mySurfaceWidth, myLines[i], myMinX);
            
            DB2(AC_TRACE << "line: " << i << " wordcount: " << myLines[i].wordCount << endl;)
            for (unsigned j = 0; j < myLines[i].wordCount; ++j) {
                const Word & myWord = myWords[myWordCount];
                myWordCount++;

                DB2(
                    {AC_TRACE << "  line:  " << i << endl;}
                    {AC_TRACE << "  word:  " << j << endl;}
                    {AC_TRACE << "  text:  " << myWord.text << endl;}
                    {AC_TRACE << "  Size:  " << myWord.surface->w << " x " << myWord.surface->h << endl;}
                    {AC_TRACE << "  Pos:   " << myXPos << " x " << myYPos << endl;}
                )

                unsigned mySrcHeight = myWord.surface->h;
                unsigned mySrcWidth  = myWord.surface->w;

                int myGlyphXPos = myXPos;
                for (unsigned int i = 0;  i < myWord.sdl_x_position.size();) {
                    _myGlyphPosition.push_back(Vector2i(myGlyphXPos + myWord.sdl_x_position[i], unsigned(_myTextureSurface->h) - myYPos -1));
                    _myGlyphPosition.push_back(Vector2i(myGlyphXPos + myWord.sdl_x_position[i+1], unsigned(_myTextureSurface->h) - (myYPos + mySrcHeight)));
                    i +=2;
                }
                if (int(mySurfaceHeight) > (_myTextStyle._myBottomPadding + myYPos)) {
                    if (int(mySrcHeight + myYPos) > int(mySurfaceHeight-_myTextStyle._myBottomPadding)) {
                        mySrcHeight = mySurfaceHeight-_myTextStyle._myBottomPadding-myYPos;
                    }
                } else {
                    mySrcHeight = 0;
                    AC_TRACE << "SDLTextRenderer::createTextSurface: surface too small for text";
                }

                mySrcWidth = asl::minimum(mySrcWidth, unsigned(_myTextureSurface->w - myXPos));
                mySrcHeight = asl::minimum(mySrcHeight, unsigned(_myTextureSurface->h - myYPos));

                // [CH] We use a custom blitting function, because SDL does not provide the right
                // functionality.

                asl::Unsigned32 * mySrcLinePtr = (asl::Unsigned32 *)myWord.surface->pixels;
                asl::Unsigned32 * myDstLinePtr = (asl::Unsigned32 *)_myTextureSurface->pixels + myYPos * _myTextureSurface->w + myXPos;
                for (unsigned i = 0; i < mySrcHeight; ++i) {
                    asl::Unsigned32 * mySrcPixelPtr = mySrcLinePtr;
                    asl::Unsigned32 * myDstPixelPtr = myDstLinePtr;
                    for (unsigned j = 0; j < mySrcWidth; ++j) {
                        if (*mySrcPixelPtr >> 24 && myDstPixelPtr >= (asl::Unsigned32 *)_myTextureSurface->pixels /*do not copy ahead of dstpixelptr (vs)*/) {
                            *myDstPixelPtr = *mySrcPixelPtr;
                        }
                        ++mySrcPixelPtr;
                        ++myDstPixelPtr;
                    }

                    mySrcLinePtr += myWord.surface->w;
                    myDstLinePtr += _myTextureSurface->w;
                }
                // what about interword kerning and tracking ?? (2011,vs)
                myXPos += myWord.surface->w;
                SDL_FreeSurface(myWord.surface);
            }
            _myCursorPos[0] = myXPos - _myTextStyle._myRightPadding;

            if (myLines[i].newline) {
                myYPos += _myTextStyle._myParagraphBottomOffset + _myTextStyle._myParagraphTopOffset;
            }
            _myCursorPos[1] = myYPos - calcVerticalAlignment(mySurfaceHeight, myTotalLineHeight) - _myTextStyle._myParagraphTopOffset;
            myYPos += myLineHeight;
        }

        return Vector2i(mySurfaceWidth, mySurfaceHeight);
    }

    void
    SDLTextRenderer::renderText(TextPtr & theText) {
        MAKE_GL_SCOPE_TIMER(SDLTextRenderer_renderText);
        SDLTextPtr mySDLText = dynamic_cast_Ptr<SDLText>(theText);
        if (mySDLText == 0) {
            throw GLTextRendererException("SDLTextRenderer - can only render SDLText objects.", PLUS_FILE_LINE);
        }

        Vector2i myTextSize = createTextSurface(mySDLText->_myString,
            mySDLText->_myFont,
            mySDLText->_myTextStyle._myTextColor);

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
