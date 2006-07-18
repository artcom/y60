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

#include "TextRendererManager.h"
#include <y60/GLUtils.h>
#include <dom/Nodes.h>
#include <asl/Vector234.h>
#include <asl/Dashboard.h>
#include <asl/os_functions.h>

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
        if (_myTextSnippets.empty()) {
            return;
        }
        MAKE_SCOPE_TIMER(renderTextSnippets);

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

        for (unsigned myTextIndex=0; myTextIndex!=_myTextSnippets.size(); ++myTextIndex) {
            // TODO, refactor
            DB(AC_TRACE << "TextRendererManager:: rendering text #" << myTextIndex << endl);
            _myTextSnippets[myTextIndex]->_myRenderer->renderText(_myTextSnippets[myTextIndex]);
        }

        DB(AC_TRACE << "TextRendererManager:: clearing " << endl);
        _myTextSnippets.clear();

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
    TextRendererManager::addText(const Vector2f & thePos, const string & theString, const string & theFont) {
        _myTextSnippets.push_back(getTextRendererByFont(theFont).createText(thePos, theString, theFont));
    }

	void
	TextRendererManager::setColor(const asl::Vector4f & theTextColor) {
        _myBitmapRenderer.setColor(theTextColor);
        if (_myTTFRenderer) {
            _myTTFRenderer->setColor(theTextColor);
        }
    }

	void
	TextRendererManager::setVTextAlignment(TextRenderer::TextAligment theVAlignment) {
        _myBitmapRenderer.setVTextAlignment(theVAlignment);
        if (_myTTFRenderer) {
            _myTTFRenderer->setVTextAlignment(theVAlignment);
        }
    }

	void
	TextRendererManager::setHTextAlignment(TextRenderer::TextAligment theHAlignment) {
        _myBitmapRenderer.setHTextAlignment(theHAlignment);
        if (_myTTFRenderer) {
            _myTTFRenderer->setHTextAlignment(theHAlignment);
        }
    }

	void
	TextRendererManager::setPadding(int topPadding, int bottomPadding, int leftpadding, int rightpadding) {
        _myBitmapRenderer.setPadding(topPadding, bottomPadding, leftpadding, rightpadding);
        if (_myTTFRenderer) {
            _myTTFRenderer->setPadding(topPadding, bottomPadding, leftpadding, rightpadding);
        }
    }

	void
	TextRendererManager::setIndentation(int theIndentation) {
        _myBitmapRenderer.setIndentation(theIndentation);
        if (_myTTFRenderer) {
            _myTTFRenderer->setIndentation(theIndentation);
        }
    }
    void
	TextRendererManager::setLineHeight(unsigned theHeight) {
        _myBitmapRenderer.setLineHeight(theHeight);
        if (_myTTFRenderer) {
            _myTTFRenderer->setLineHeight(theHeight);
        }
    }

	void
	TextRendererManager::setParagraph(unsigned theTopOffset, unsigned theBottomOffset) {
        _myBitmapRenderer.setParagraph(theTopOffset, theBottomOffset);
        if (_myTTFRenderer) {
            _myTTFRenderer->setParagraph(theTopOffset, theBottomOffset);
        }
    }

    Vector2i
    TextRendererManager::renderTextAsImage(TextureManager & theTextureManager, dom::NodePtr theImageNode,
                                  const std::string & theString,
                                  const std::string & theFont,
                                  unsigned int theTargetWidth, unsigned int theTargetHeight)
    {
        return _myTTFRenderer->renderTextAsImage(theTextureManager,
                                              theImageNode, theString, theFont,
                                              theTargetWidth, theTargetHeight);
	}

	void
	TextRendererManager::loadTTF(const std::string & theName, const std::string & theFileName,
	                  int theHeight, TTFFontInfo::FONTTYPE & theFonttype)
    {
        _myTTFRenderer->loadFont(theName, expandEnvironment(theFileName), theHeight, theFonttype);
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

    void
    TextRendererManager::setTracking(float theTracking) {
        _myTTFRenderer->setTracking(theTracking);
    }

    void
    TextRendererManager::setMaxFontFittingSize(unsigned theSize) {
        _myTTFRenderer->setMaxFontFittingSize(theSize);
    }
}
