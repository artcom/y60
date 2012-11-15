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
//own header
#include "TextRendererManager.h"

#include <y60/glutil/GLUtils.h>
#include <asl/dom/Nodes.h>
#include <asl/math/Vector234.h>
#include <asl/base/Dashboard.h>
#include <asl/base/os_functions.h>

#include <iostream>

#define DB(x) //x

using namespace std;
using namespace asl;

namespace y60 {

    TextRendererManager::TextRendererManager() {
    }

    TextRendererManager::~TextRendererManager() {
    }

    void
    TextRendererManager::setTTFRenderer(const TTFTextRendererPtr & theTTFRenderer) {
        _myTTFRenderer = theTTFRenderer;
    }

    void
    TextRendererManager::render(ViewportPtr theViewport) {
        if (_myTextSnippetsMap.find(theViewport->get<IdTag>()) == _myTextSnippetsMap.end()) {
            return;
        }
        std::vector<TextPtr> & myTextSnippets = _myTextSnippetsMap[theViewport->get<IdTag>()];
        if (myTextSnippets.empty()) {
            return;
        }
        MAKE_GL_SCOPE_TIMER(renderTextSnippets);

        unsigned myWindowWidth = theViewport->get<ViewportWidthTag>();
        unsigned myWindowHeight = theViewport->get<ViewportHeightTag>();
        _myBitmapRenderer.setWindowSize(myWindowWidth, myWindowHeight);
        if (_myTTFRenderer) {
            _myTTFRenderer->setWindowSize(myWindowWidth, myWindowHeight);
        }

        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glActiveTexture(asGLTextureRegister(0));
        glClientActiveTexture(asGLTextureRegister(0));
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        CHECK_OGL_ERROR;

        //glMatrixMode(GL_TEXTURE);
        //glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, double(myWindowWidth), double(myWindowHeight), 0.0);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        Matrix4f myScreenMatrix;
        myScreenMatrix.makeTranslating(Vector3f(-float(myWindowWidth)/2, -float(myWindowHeight)/2,0));
        if (theViewport->get<ViewportOrientationTag>() == PORTRAIT_ORIENTATION) {
            Matrix4f myRotationMatrix;
            myRotationMatrix.makeZRotating(float(asl::PI_2));
            myScreenMatrix.postMultiply(myRotationMatrix);
        }
        myScreenMatrix.translate(Vector3f(float(myWindowWidth)/2, float(myWindowHeight)/2,0));
        glLoadMatrixf(static_cast<const GLfloat *>(myScreenMatrix.getData()));

        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        for (unsigned myTextIndex=0; myTextIndex!=myTextSnippets.size(); ++myTextIndex) {
            // TODO, refactor
            DB(AC_TRACE << "TextRendererManager:: rendering text #" << myTextIndex << endl);
            myTextSnippets[myTextIndex]->_myRenderer->renderText(myTextSnippets[myTextIndex]);
        }

        DB(AC_TRACE << "TextRendererManager:: clearing " << endl);
        myTextSnippets.clear();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glPopAttrib();
        CHECK_OGL_ERROR;
    }

    TextRenderer &
    TextRendererManager::getTextRendererByFont(const string & theFont) {
        if (_myBitmapRenderer.haveFont(theFont)) {
            return _myBitmapRenderer;
        }
        if (_myTTFRenderer && _myTTFRenderer->haveFont(theFont)) {
            return *_myTTFRenderer;
        }
        throw TextRendererManagerException(string("Could not find textrenderer for Font: ") + theFont, PLUS_FILE_LINE);
    }

    void
    TextRendererManager::addText(const Vector2f & thePos, const string & theString, const string & theFont, ViewportPtr theViewport) {
        if (_myTextSnippetsMap.find(theViewport->get<IdTag>()) == _myTextSnippetsMap.end()) {
            _myTextSnippetsMap[theViewport->get<IdTag>()] = std::vector<TextPtr>();
        }
        _myTextSnippetsMap[theViewport->get<IdTag>()].push_back(getTextRendererByFont(theFont).createText(thePos, theString, theFont));
    }

    const std::vector<asl::Vector2i> &
    TextRendererManager::getGlyphPositions() const {
        if (_myTTFRenderer) {
            return _myTTFRenderer->getGlyphPositions();
        } else {
            return _myBitmapRenderer.getGlyphPositions();
        }
    }

const asl::Vector2i &
    TextRendererManager::getTextCursorPosition() const {
        if (_myTTFRenderer) {
            return _myTTFRenderer->getTextCursorPosition();
        } else {
            return _myBitmapRenderer.getTextCursorPosition();
        }
    }

    const unsigned int &
    TextRendererManager::getMaxWidth() const {
        if (_myTTFRenderer) {
            return _myTTFRenderer->getMaxWidth();
        } else {
            return _myBitmapRenderer.getMaxWidth();
        }
    }

    const vector<unsigned int> &
    TextRendererManager::getLineWidths() const {
        if (_myTTFRenderer) {
            return _myTTFRenderer->getLineWidths();
        } else {
            return _myBitmapRenderer.getLineWidths();
        }
    }

    const TextStyle & TextRendererManager::getTextStyle() {
        if (_myTTFRenderer) {
            return _myTTFRenderer->getTextStyle();
        } else {
            return _myBitmapRenderer.getTextStyle();
        }
    }

    void TextRendererManager::setTextStyle(TextStyle & theTextStyle) {
        _myBitmapRenderer.setTextStyle(theTextStyle);
        if (_myTTFRenderer) {
            _myTTFRenderer->setTextStyle(theTextStyle);
        }
    }

    Vector2i
    TextRendererManager::renderTextAsImage(TextureManager & theTextureManager, dom::NodePtr theImageNode,
                                  const std::string & theString,
                                  const std::string & theFont,
                                  unsigned int theTargetWidth, unsigned int theTargetHeight,
                                  const asl::Vector2i & theCursorPos)
    {        
        Vector2i mySize = _myTTFRenderer->renderTextAsImage(theTextureManager,
                                          theImageNode, theString, theFont,
                                          theTargetWidth, theTargetHeight, theCursorPos);
        return mySize;
    }

    void
    TextRendererManager::loadTTF(const std::string & theName, const std::string & theFileName,
                      int theHeight, TTFFontInfo::FONTHINTING & theFonthint,
                      TTFFontInfo::FONTTYPE & theFonttype, int theAscendOffset)
    {
        _myTTFRenderer->loadFont(theName, expandEnvironment(theFileName), theHeight, theFonthint, theFonttype, theAscendOffset);
    }

    bool
    TextRendererManager::getFontMetrics(const std::string & theFontName,
            int & theFontHeight,
            int & theFontAscent, int & theFontDescent,
            int & theFontLineSkip) const
    {
        return _myTTFRenderer->getFontMetrics(theFontName, theFontHeight, theFontAscent, theFontDescent, theFontLineSkip);
    }

    bool
    TextRendererManager::getGlyphMetrics(const std::string & theFontName, const std::string & theCharacter,
                                         asl::Box2f & theGlyphBox, double & theAdvance) const
    {
        return _myTTFRenderer->getGlyphMetrics(theFontName, theCharacter, theGlyphBox, theAdvance);
    }

    double
    TextRendererManager::getKerning(const std::string& theFontName, const std::string& theFirstCharacter, const std::string& theSecondCharacter) const
    {
        return _myTTFRenderer->getKerning(theFontName, theFirstCharacter, theSecondCharacter);
    }

    bool
    TextRendererManager::hasGlyph(const std::string& theFontName, const std::string& theCharacter) const
    {
        return _myTTFRenderer->hasGlyph(theFontName, theCharacter);
    }

}
