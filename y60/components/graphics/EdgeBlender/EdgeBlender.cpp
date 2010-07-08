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
//          - use pixelspace instead of texturespace [DS]
//          - avoid rotations; use symmetric bars and corner tiles [DS]
//          - store vertexdata on the GPU; it's constant [DS]
//          - maybe this thing shouldn't exist at all. maybe it's better to do
//            it in a postproc shader? [DS]
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include <asl/base/PlugInBase.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jslib/IRendererExtension.h>
#include <y60/jsbase/JSScriptablePlugin.h>
#include <y60/jslib/AbstractRenderWindow.h>
#include <y60/scene/Scene.h>
#include <y60/glutil/GLUtils.h>

#include <asl/base/string_functions.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/Exception.h>
#include <asl/base/Logger.h>

#include <iostream>


using namespace y60;
using namespace jslib;


class EdgeBlender :
    public asl::PlugInBase,
    public IRendererExtension,
    public IScriptablePlugin
{
    public:

        EdgeBlender(asl::DLHandle theDLHandle);
        ~EdgeBlender();

        void onGetProperty(const std::string & thePropertyName,
                PropertyValue & theReturnValue) const;
        void onSetProperty(const std::string & thePropertyName,
                const PropertyValue & thePropertyValue);
        void onUpdateSettings(dom::NodePtr theSettings);

        void onStartup(jslib::AbstractRenderWindow * theWindow);
        bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
            return true;
        }
        void handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {}
        void onFrame(AbstractRenderWindow * theWindow, double theTime) {}

        void onPreRender(AbstractRenderWindow * theRenderer) {}
        void onPostRender(AbstractRenderWindow * theRenderer);

        const char * ClassName() {
            static const char * myClassName = "EdgeBlender";
            return myClassName;
        }

    private:
        inline
        float
        getXBlendWidth() const {
            return float(_myBlendWidth[0]) / _myWindowWidth;
        }
        inline
        float
        getYBlendWidth() const {
            return float(_myBlendWidth[1]) / _myWindowHeight;
        }
        void renderMultiScreen();
        void renderBlending();

        void drawGrid(const asl::Vector4f & theColor, unsigned theGridSize = 20);
        void preRender();
        void postRender();
        void copyToTexture();
        void renderTexture(float thePosX, float thePosY,
                float theStartX, float theStartY, float theEndX, float theEndY);
        void rotateTo(unsigned theEdge);
        void drawBlendedEdge(float theStart, float theEnd, float theBlendWidth, float theMargin = 0.0f);
        void drawBlendedCorner(float theMarginX, float theMarginY, 
                float theXSize, float theYSize);
        void drawBlackLevel(float theLeft, float theTop, float theRight, float theBottom);
        float getBlendValue(float theValue);
        GLint _mySavedViewport[4];
        unsigned _myWindowWidth, _myWindowHeight;
        GLuint _mySceneTexture;

       /* Modes:
        * MODE_MULTI_PC
        *       multiple PC's render a scene using multiple projectors
        *       each PC uses one projector and shows part of the scene
        *       given by a viewport.
        *       the portion shown has blended edges which may be any combination
        *       of BLEND_EDGE_LEFT, BLEND_EDGE_UP, BLEND_EDGE_RIGHT, BLEND_EDGE_DOWN
        *       according to where the scene continues.
        *
        * MODE_MULTI_SCREEN
        *       one PC renders a scene using many projectors
        *       the scene may be split horizontally or vertically or both
        *       EdgeBlender will render the scene on a texture
        *       and display this texture producing redundancy at the overlapping region.
        *
        *       e.g. if the blending is horizontal and the width is 20%
        *       then 60% of the texture is taken starting from the left border
        *       and rendered on the screen.
        *       than another 60% percent of the texture starting from the right border
        *       are taken and rendered next to the first one.
        *       the screen now covers 120% of the original scene width
        *       the central 20% of the screen are totally redundant
        *       this region is blended.
        *       the projected half-screens should overlap throughout this region.
        *
        *
        * MODE_MULTI_SCREEN_MULTI_PC
        *       Both modi
        */
        enum {
            MODE_MULTI_PC               = 1,
            MODE_MULTI_SCREEN           = 2,
            MODE_MULTI_SCREEN_MULTI_PC  = 3
        };
        unsigned _myMode;

        /*
         * _myEdges is a bit-wise logical combination of BLEND_EDGE_XXX
         */
        enum {
            BLEND_EDGE_LEFT  = 1,
            BLEND_EDGE_RIGHT = 2,
            BLEND_EDGE_UP    = 4,
            BLEND_EDGE_DOWN  = 8
        };
        unsigned  _myEdges;

        unsigned _myRowCount;
        unsigned _myColumnCount;

        asl::Vector2f _myBlendWidth; // in pixels
        unsigned _myNumSubdivisions;

        float _myBlackLevel;
        asl::Vector2f _myBlPosOffset;
        float _myPower;
        float _myGamma;

        bool _myPreGridFlag;
        bool _myPostGridFlag;
        bool _myBlendingFlag;
        bool _myBlackLevelFlag;
        bool _myRoughCalibrationMode;

        bool _myCopyFrameBufferFlag;
        asl::Vector4f _myFrameBufferArea;
        unsigned char * _myFrameBuffer;
        GLint _myMaxTexUnits;
};

EdgeBlender::EdgeBlender(asl::DLHandle theDLHandle) :
    asl::PlugInBase(theDLHandle),
    IRendererExtension("EdgeBlender"),

    _myMode(MODE_MULTI_SCREEN),

    _myEdges( 0 ),
    _myRowCount(1),
    _myColumnCount(2),
    _myBlendWidth( 96.f, 96.f ),
    _myNumSubdivisions( 32 ),
    _myBlackLevel( 0.1f ),
    _myBlPosOffset(0.0f, 0.0f),
    _myPower(1.5f),
    _myGamma(1.2f),

    _myPreGridFlag(false),
    _myPostGridFlag(false),
    _myBlendingFlag(true),
    _myBlackLevelFlag(true),
    _myRoughCalibrationMode(false),

    _myCopyFrameBufferFlag(false),
    _myFrameBuffer(0)
{}

EdgeBlender::~EdgeBlender() {
    if (_myFrameBuffer) {
        delete[] _myFrameBuffer;
    }
}


void
EdgeBlender::onStartup(jslib::AbstractRenderWindow * theWindow)
{
    _myWindowWidth = theWindow->getWidth();
    _myWindowHeight = theWindow->getHeight();
    AC_DEBUG << "window=" << _myWindowWidth << "x" << _myWindowHeight;
    _myFrameBufferArea = asl::Vector4f(0,0, (float) _myWindowWidth, (float) _myWindowHeight);

    unsigned myTextureWidth = asl::nextPowerOfTwo(_myWindowWidth);
    unsigned myTextureHeight = asl::nextPowerOfTwo(_myWindowHeight);
    AC_DEBUG << "texture=" << myTextureWidth << "x" << myTextureHeight;

    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &_myMaxTexUnits);
    AC_DEBUG << "maxTextureUnits=" << _myMaxTexUnits;

    //glActiveTexture(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &_mySceneTexture);
    glBindTexture (GL_TEXTURE_2D, _mySceneTexture);
    AC_DEBUG << "sceneTextureId=" << _mySceneTexture;

    glTexImage2D(GL_TEXTURE_2D, 0, 3,
                 myTextureWidth, myTextureHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    checkOGLError(PLUS_FILE_LINE);
}

void
EdgeBlender::onGetProperty(const std::string & thePropertyName,
        PropertyValue & theReturnValue) const
{
    AC_DEBUG << "onGetProperty " << thePropertyName;
    if (thePropertyName == "edges") {
        theReturnValue.set<unsigned>(_myEdges);
    } else if (thePropertyName == "blendwidth") {
        theReturnValue.set<asl::Vector2f>(_myBlendWidth);
    } else if (thePropertyName == "subdivisions") {
        theReturnValue.set<unsigned>(_myNumSubdivisions);
    } else if (thePropertyName == "blacklevel") {
        theReturnValue.set<float>(_myBlackLevel);
    } else if (thePropertyName == "power") {
        theReturnValue.set<float>(_myPower);
    } else if (thePropertyName == "gamma") {
        theReturnValue.set<float>(_myGamma);
    } else if (thePropertyName == "rows") {
        theReturnValue.set<unsigned>(_myRowCount);
    } else if (thePropertyName == "columns") {
        theReturnValue.set<unsigned>(_myColumnCount);
    } else if (thePropertyName == "mode") {
        theReturnValue.set<unsigned>(_myMode);
    } else if (thePropertyName == "preGridFlag") {
        theReturnValue.set<bool>(_myPreGridFlag);
    } else if (thePropertyName == "postGridFlag") {
        theReturnValue.set<bool>(_myPostGridFlag);
    } else if (thePropertyName == "blendingFlag") {
        theReturnValue.set<bool>(_myBlendingFlag);
    } else if (thePropertyName == "blacklevelFlag") {
        theReturnValue.set<bool>(_myBlackLevelFlag);
    } else if (thePropertyName == "copyFrameBufferFlag") {
        theReturnValue.set<bool>(_myCopyFrameBufferFlag);
    } else if (thePropertyName == "frameBufferArea") {
        //theReturnValue.set<asl::Vector4f>(_myFrameBufferArea);
    }
}

void
EdgeBlender::onSetProperty(const std::string & thePropertyName,
        const PropertyValue & thePropertyValue)
{
    AC_DEBUG << "onSetProperty " << thePropertyName << "=" << thePropertyValue.get<std::string>();
    if (thePropertyName == "edges") {
        _myEdges = thePropertyValue.get<unsigned>();
    } else if (thePropertyName == "blendwidth") {
        _myBlendWidth = thePropertyValue.get<asl::Vector2f>();
    } else if (thePropertyName == "subdivisions") {
        _myNumSubdivisions = thePropertyValue.get<unsigned>();
    } else if (thePropertyName == "blacklevel") {
        _myBlackLevel = thePropertyValue.get<float>();
    } else if (thePropertyName == "power") {
        _myPower = thePropertyValue.get<float>();
    } else if (thePropertyName == "gamma") {
        _myGamma = thePropertyValue.get<float>();
    } else if (thePropertyName == "rows") {
        _myRowCount = thePropertyValue.get<unsigned>();
    } else if (thePropertyName == "columns") {
        _myColumnCount = thePropertyValue.get<unsigned>();
    } else if (thePropertyName == "mode") {
        _myMode = thePropertyValue.get<unsigned>();
    } else if (thePropertyName == "preGridFlag") {
        _myPreGridFlag = thePropertyValue.get<bool>();
    } else if (thePropertyName == "postGridFlag") {
        _myPostGridFlag = thePropertyValue.get<bool>();
    } else if (thePropertyName == "blendingFlag") {
        _myBlendingFlag = thePropertyValue.get<bool>();
    } else if (thePropertyName == "blacklevelFlag") {
        _myBlackLevelFlag = thePropertyValue.get<bool>();
    } else if (thePropertyName == "copyFrameBufferFlag") {
        _myCopyFrameBufferFlag = thePropertyValue.get<bool>();
    } else if (thePropertyName == "frameBufferArea") {
        //_myFrameBufferArea = thePropertyValue.get<asl::Vector4f>();
    }
}

void
EdgeBlender::onUpdateSettings(dom::NodePtr theSettings) {
    AC_DEBUG << "onUpdateSettings " << *theSettings;
    _myMode = getSetting(theSettings, "mode", _myMode);
    _myEdges = getSetting(theSettings, "edges", _myEdges);
    _myColumnCount = getSetting(theSettings, "columns", _myColumnCount);
    _myRowCount = getSetting(theSettings, "rows", _myRowCount);
    _myBlendWidth = getSetting(theSettings, "blendwidth", _myBlendWidth);
    _myNumSubdivisions = getSetting(theSettings, "subdivisions", _myNumSubdivisions);
    _myBlackLevel = getSetting(theSettings, "blacklevel", _myBlackLevel);
    _myBlPosOffset = getSetting(theSettings, "blacklevelposoffset", _myBlPosOffset);
    _myPower = getSetting(theSettings, "power", _myPower);
    _myGamma = getSetting(theSettings, "gamma", _myGamma);

    _myPreGridFlag = 0 != getSetting(theSettings, "preGridFlag", int(_myPreGridFlag));
    _myPostGridFlag = 0 != getSetting(theSettings, "postGridFlag", int(_myPostGridFlag));
    _myBlendingFlag = 0 != getSetting(theSettings, "blendingFlag", int(_myBlendingFlag));
    _myBlackLevelFlag = 0 != getSetting(theSettings, "blacklevelFlag", int(_myBlackLevelFlag));
    _myCopyFrameBufferFlag = 0 != getSetting(theSettings, "copyFrameBufferFlag", int(_myCopyFrameBufferFlag));
    _myFrameBufferArea = getSetting(theSettings, "frameBufferArea", _myFrameBufferArea);
    
    _myRoughCalibrationMode = 0 != getSetting(theSettings, "roughCalibrationMode", int(_myRoughCalibrationMode));
}

void
EdgeBlender::onPostRender(AbstractRenderWindow * theRenderer)
{
    //AC_TRACE << "onPostRender";
    if (_myMode == 0) {
        return;
    }

    preRender();

    // pre-blend grid
    if (_myPreGridFlag) {
        drawGrid(asl::Vector4f(0,1,0,1));
    }

    // edge blending
    switch (_myMode) {
        case MODE_MULTI_SCREEN:
            renderMultiScreen();
            break;
        case MODE_MULTI_PC:
            renderBlending();
            break;
        case MODE_MULTI_SCREEN_MULTI_PC:
            renderMultiScreen();
            renderBlending();
    }

    // post-blend grid
    if (_myPostGridFlag) {
        drawGrid(asl::Vector4f(1,1,0,1));
    }

    postRender();
}

void
EdgeBlender::drawGrid(const asl::Vector4f & theColor, unsigned theGridSize)
{
    glColor4fv(theColor.begin());
    glBegin(GL_LINES);
    for (unsigned y = 0; y < _myWindowHeight; y += theGridSize) {
        float myY = y / (float) _myWindowHeight;
        glVertex2f(0.0f, myY);
        glVertex2f(1.0f, myY);
    }
    for (unsigned x = 0; x < _myWindowWidth; x += theGridSize) {
        float myX = x / (float) _myWindowWidth;
        glVertex2f(myX, 0.0f);
        glVertex2f(myX, 1.0f);
    }
    glEnd();
}

void
EdgeBlender::renderMultiScreen()
{
    copyToTexture();

    float myXBlendWidth = getXBlendWidth();
    float myYBlendWidth = getYBlendWidth();

    // draw scene texture to suitable parts of the screen w/o any blending
    float myTextureOffsetX = (_myColumnCount-1) * myXBlendWidth * 0.5f;
    float myTextureOffsetY = (_myRowCount-1) * myYBlendWidth * 0.5f;
    float myTexCoordY0 = myTextureOffsetY;

    // draw scene texture
    glEnable(GL_TEXTURE_2D);
    for(unsigned i = 0; i < _myRowCount; ++i) {
        float myY  = float(i) / _myRowCount;

        float myTexCoordY1 = myTexCoordY0 + 1.0f / _myRowCount;
        float myTexCoordX0 = myTextureOffsetX;
        for(unsigned j = 0; j < _myColumnCount; ++j) {
            float myX  = float(j) / _myColumnCount;
            float myTexCoordX1 = myTexCoordX0 + 1.0f / _myColumnCount;

            renderTexture(myX, myY, myTexCoordX0, myTexCoordY0, myTexCoordX1, myTexCoordY1);
            myTexCoordX0 = myTexCoordX1 - myXBlendWidth;
        }
        myTexCoordY0 = myTexCoordY1 - myYBlendWidth;
    }
    glDisable(GL_TEXTURE_2D);

    // draw blending over scene texture
    if (_myBlendingFlag) {
        for(unsigned i = 0; i < _myRowCount; ++i) {
            float myY  = float(i) / _myRowCount;
            float myY0 = (i > 0 ? myY + myYBlendWidth : myY);
            float myY1 = float(i+1) / _myRowCount;
            if (i < _myRowCount-1) {
                myY1 -= myYBlendWidth;
            }
            if (_myBlackLevelFlag) {
                myY0 += i > 0 ? _myBlPosOffset[1] : 0.0f;
                myY1 -= i < _myRowCount-1 ? _myBlPosOffset[1] : 0.0f;
            }
            
            for(unsigned j = 0; j < _myColumnCount; ++j) {
                float myX  = float(j) / _myColumnCount;
                float myX0 = (j > 0 ? myX + myXBlendWidth : myX);
                float myX1 = float(j+1) / _myColumnCount;
                if (j < _myColumnCount-1) {
                    myX1 -= myXBlendWidth;
                }

                if (_myBlackLevelFlag) {
                    myX0 += j > 0 ? _myBlPosOffset[0] : 0.0f;
                    myX1 -= j < _myColumnCount-1 ? _myBlPosOffset[0] : 0.0f;
                    drawBlackLevel(myX0, myY0, myX1, myY1);
                }

                if (j > 0) {
                    rotateTo(BLEND_EDGE_RIGHT);
                    drawBlendedEdge(myY0, myY1, myXBlendWidth, myX);
                    rotateTo(BLEND_EDGE_LEFT);
                    drawBlendedEdge(myY0, myY1, myXBlendWidth, myX);
                }
                if (i > 0) {
                    rotateTo(BLEND_EDGE_UP);
                    drawBlendedEdge(myX0, myX1, myYBlendWidth, myY);
                    rotateTo(BLEND_EDGE_DOWN);
                    drawBlendedEdge(myX0, myX1, myYBlendWidth, myY);
                }
                if (j > 0 && i > 0) {
                    // XXX TODO Corners!!!
                    rotateTo(BLEND_EDGE_LEFT);
                    drawBlendedCorner(myX, myY, myXBlendWidth, myYBlendWidth);
                    rotateTo(BLEND_EDGE_UP);
                    drawBlendedCorner(myY, myX, myYBlendWidth, myXBlendWidth);
                    rotateTo(BLEND_EDGE_RIGHT);
                    drawBlendedCorner(myX, myY, myXBlendWidth, myYBlendWidth);
                    rotateTo(BLEND_EDGE_DOWN);
                    drawBlendedCorner(myY, myX, myYBlendWidth, myXBlendWidth);
                }
            }
        }
    }
}

void
EdgeBlender::renderBlending() {
    float myXBlendWidth = getXBlendWidth();
    float myYBlendWidth = getYBlendWidth();

    // blended edges
    if (_myEdges & BLEND_EDGE_LEFT) {
        float myStart = (_myEdges & BLEND_EDGE_UP) ?
            myYBlendWidth : 0.0f;
        float myEnd = (_myEdges & BLEND_EDGE_DOWN) ?
            1.0f - myYBlendWidth : 1.0f;
        rotateTo(BLEND_EDGE_LEFT);
        drawBlendedEdge(myStart, myEnd, myXBlendWidth);
    }
    if (_myEdges & BLEND_EDGE_RIGHT) {
        float myStart = (_myEdges & BLEND_EDGE_DOWN) ?
            myYBlendWidth : 0.0f;
        float myEnd = (_myEdges & BLEND_EDGE_UP) ?
            1.0f - myYBlendWidth : 1.0f;
        rotateTo(BLEND_EDGE_RIGHT);
        drawBlendedEdge(myStart, myEnd, myXBlendWidth);
    }
    if (_myEdges & BLEND_EDGE_UP) {
        float myStart = (_myEdges & BLEND_EDGE_RIGHT) ?
            myXBlendWidth : 0.0f;
        float myEnd = (_myEdges & BLEND_EDGE_LEFT) ?
            1.0f - myXBlendWidth : 1.0f;
        rotateTo(BLEND_EDGE_UP);
        drawBlendedEdge(myStart, myEnd, myYBlendWidth);
    }
    if (_myEdges & BLEND_EDGE_DOWN) {
        float myStart = (_myEdges & BLEND_EDGE_LEFT) ?
            myXBlendWidth : 0.0f;
        float myEnd = (_myEdges & BLEND_EDGE_RIGHT) ?
            1.0f - myXBlendWidth : 1.0f;
        rotateTo(BLEND_EDGE_DOWN);
        drawBlendedEdge(myStart, myEnd, myYBlendWidth);
    }

    // corners
    if (_myEdges & BLEND_EDGE_LEFT && _myEdges & BLEND_EDGE_UP) {
        rotateTo(BLEND_EDGE_LEFT);
        drawBlendedCorner(0.0f, 0.0f, myXBlendWidth, myYBlendWidth);
    }
    if (_myEdges & BLEND_EDGE_UP && _myEdges & BLEND_EDGE_RIGHT) {
        rotateTo(BLEND_EDGE_UP);
        drawBlendedCorner(0.0f, 0.0f, myYBlendWidth, myXBlendWidth);
    }
    if (_myEdges & BLEND_EDGE_RIGHT && _myEdges & BLEND_EDGE_DOWN) {
        rotateTo(BLEND_EDGE_RIGHT);
        drawBlendedCorner(0.0f, 0.0f, myXBlendWidth, myYBlendWidth);
    }
    if (_myEdges & BLEND_EDGE_DOWN && _myEdges & BLEND_EDGE_LEFT) {
        rotateTo(BLEND_EDGE_DOWN);
        drawBlendedCorner(0.0f, 0.0f, myYBlendWidth, myXBlendWidth);
    }

    // blacklevel
    float myLeft = _myEdges & BLEND_EDGE_LEFT ? myXBlendWidth : 0.0f;
    float myRight = _myEdges & BLEND_EDGE_RIGHT ? 1.0f - myXBlendWidth : 1.0f;
    float myTop = _myEdges & BLEND_EDGE_UP ? myYBlendWidth : 0.0f;
    float myBottom = _myEdges & BLEND_EDGE_DOWN ? 1.0f - myYBlendWidth : 1.0f;
    drawBlackLevel(myLeft, myTop, myRight, myBottom);
}

void
EdgeBlender::preRender() {
    // disable all texture units except the first
    for (int i = 1; i < _myMaxTexUnits; ++i) {
        glActiveTexture(GL_TEXTURE0_ARB + i);
        glDisable(GL_TEXTURE_2D);
    }
    glActiveTexture(GL_TEXTURE0_ARB);

    // load ortho projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0f, 1.0f, 1.0f, 0.0f);
    
    // store that section
    glGetIntegerv(GL_VIEWPORT, _mySavedViewport);

    // set viewport to full window
    glViewport(0, 0, _myWindowWidth, _myWindowHeight);

    // load texture matrix
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();

    // load modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    
    // load attribs
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT | GL_POLYGON_BIT); // GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT, GL_FILL);
//    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}

void
EdgeBlender::postRender() {
    // restore attribs
    glPopAttrib();

    // restore matrices
    glMatrixMode(GL_PROJECTION);
    // restore viewport
    glViewport(_mySavedViewport[0], _mySavedViewport[1], _mySavedViewport[2], _mySavedViewport[3]);
    
    glPopMatrix();

    glMatrixMode(GL_TEXTURE);
    glPopMatrix();

    // put into model view
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

float
EdgeBlender::getBlendValue(float theValue) {
    float myBlendValue;
    // blend value
    if (theValue < 0.5f) {
        myBlendValue = 0.5f * powf(2.0f * theValue, _myPower);
    } else {
        myBlendValue = 1.0f - 0.5f * powf(2.0f * (1.0f - theValue), _myPower);
    }

    // gamma correction
    if (_myGamma > 0.0f) {
        myBlendValue = powf(myBlendValue, 1.0f / _myGamma);
    }

    return myBlendValue;
}

void
EdgeBlender::drawBlackLevel(float theLeft, float theTop, float theRight, float theBottom) {
    //AC_DEBUG << "drawBlackLevel " << theLeft << "," << theTop << " - " << theRight << "," << theBottom;

    glColor4f(1.0f, 1.0f, 1.0f, _myBlackLevel);
    glLoadIdentity();
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(theLeft, theTop);
    glVertex2f(theLeft, theBottom);
    glVertex2f(theRight, theTop);
    glVertex2f(theRight, theBottom);
    glEnd();
}

void
EdgeBlender::rotateTo(unsigned theEdge) {
    float myAngle = 0.0f;
    switch (theEdge) {
        case BLEND_EDGE_LEFT:
            myAngle = 0.0f;
            break;
        case BLEND_EDGE_RIGHT:
            myAngle = 180.0f;
            break;
        case BLEND_EDGE_UP:
            myAngle = 90.0f;
            break;
        case BLEND_EDGE_DOWN:
            myAngle = 270.0f;
            break;
        default:
            AC_WARNING << "Unknown edge " << theEdge;
    }
    AC_DEBUG << "rotateTo edge=" << theEdge << " angle=" << myAngle;

    // rotate to the edge
    glLoadIdentity();
    glTranslatef(0.5f, 0.5f, 0.0f);
    glRotatef(myAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-0.5f, -0.5f, 0.0f);
}

/*
 * an edge is a region of the screen
 * where 2 projector images overlap
 * the edge blending function f(x)
 * satisfies f(x) + f(1-x) = 1 forall x
 */
void
EdgeBlender::drawBlendedEdge(float theStart, float theEnd, float theBlendWidth, float theMargin) {
    
    if (_myRoughCalibrationMode) {
        AC_DEBUG << "rough calibration mode: start " << theStart << " end " << theEnd;
        unsigned int myRoughCalibrationBlocks = 16; // XXX put to settings 

        glBegin(GL_QUADS);
        for (unsigned i = 0; i < myRoughCalibrationBlocks; ++i) {
            float myAlpha = i%2;
           
            float myRange = theEnd-theStart;
            float myY1 = theStart + ((float)i/(float)myRoughCalibrationBlocks) * myRange;
            float myY2 = theStart + (((float)i+1)/(float)myRoughCalibrationBlocks) * myRange;

            //glColor4f(0.0f, myBlendValue, 0.0f, max(0.2f, 1.0f - myBlendValue));
            glColor4f(0.0f, 0.0f, 0.0f, myAlpha);

            glVertex2f(theMargin+theBlendWidth, myY1);
            glVertex2f(theMargin, myY1);
            
            glVertex2f(theMargin, myY2);
            glVertex2f(theMargin+theBlendWidth, myY2);
        }
        glEnd();
    
    } else {
        AC_DEBUG << "drawBlendedEdge start " << theStart << " end " << theEnd;
        float myDelta = 1.0f / _myNumSubdivisions;

        glBegin(GL_TRIANGLE_STRIP);
        for (unsigned i = 0; i <= _myNumSubdivisions; ++i) {
            float myX = i * myDelta;

            float myBlendValue = getBlendValue(myX);
            //glColor4f(0.0f, myBlendValue, 0.0f, max(0.2f, 1.0f - myBlendValue));
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f - myBlendValue);

            float myXPos = theMargin + myX * theBlendWidth;
            glVertex2f(myXPos, theStart);
            glVertex2f(myXPos, theEnd);
        }
        glEnd();
    }
}

/*
 * a corner is a region of the screen
 * where 4 projector images overlap
 * given the simple, edge blending function f(x)
 * which satisfies f(x) + f(1-x) = 1 forall x
 * at a corner we basically take as the blending function
 * g(x,y) = f(x)f(y) because then
 * g(x,y) + g(1-x,y) + g(x,1-y) + g(1-x,1-y) = 1 forall x,y
 */
void
EdgeBlender::drawBlendedCorner(float theMarginX, float theMarginY,
        float theXSize, float theYSize) {

    for (unsigned i = 0; i < _myNumSubdivisions; ++i) {
        float myY = float(i) / _myNumSubdivisions;
        float myY1BlendValue = getBlendValue( myY);
        float myY2BlendValue = getBlendValue(float(i+1) / _myNumSubdivisions);

        glBegin(GL_TRIANGLE_STRIP);
        for (unsigned j = 0; j <= _myNumSubdivisions; ++j) {
            float myX = float(j) / _myNumSubdivisions;
            float myXBlendValue = getBlendValue( myX);

            float myXPos = theMarginX + myX * theXSize;
            float myYPos = theMarginY + myY * theYSize;

            float myBlendValue = myXBlendValue * myY1BlendValue;
            //glColor4f(myBlendValue, 0.0f, 0.0f, max(0.2f, 1.0f - myBlendValue));
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f - myBlendValue);
            glVertex2f(myXPos, myYPos);

            myBlendValue = myXBlendValue * myY2BlendValue;
            //glColor4f(myBlendValue, 0.0f, 0.0f, max(0.2f, 1.0f - myBlendValue));
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f - myBlendValue);
            glVertex2f(myXPos, myYPos + theYSize / _myNumSubdivisions);

            AC_DEBUG << "x " << myX << "y " << myY << " blend=" << myBlendValue;
        }
        glEnd();
    }
}

void
EdgeBlender::copyToTexture()
{
    glBindTexture (GL_TEXTURE_2D, _mySceneTexture);

    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, _myWindowWidth, _myWindowHeight, 0);
    CHECK_OGL_ERROR;

    if (_myCopyFrameBufferFlag) {
        if (!_myFrameBuffer) {
            _myFrameBuffer = new unsigned char[_myWindowWidth * _myWindowHeight * 3];
        }

        glReadPixels((GLint) _myFrameBufferArea[0], (GLint) _myFrameBufferArea[1],
                     (GLsizei) _myFrameBufferArea[2], (GLsizei) _myFrameBufferArea[3],
                     GL_RGB, GL_UNSIGNED_BYTE, _myFrameBuffer);
        CHECK_OGL_ERROR;

        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        (GLint) _myFrameBufferArea[0], (GLint) _myFrameBufferArea[1],
                        (GLsizei) _myFrameBufferArea[2], (GLsizei) _myFrameBufferArea[3],
                        GL_RGB, GL_UNSIGNED_BYTE, _myFrameBuffer);
        CHECK_OGL_ERROR;
    }
}

void
EdgeBlender::renderTexture(float thePosX, float thePosY,
        float theStartX, float theStartY, float theEndX, float theEndY)
{
    float myWidth = theEndX - theStartX;
    float myHeight = theEndY - theStartY;

    AC_DEBUG << "renderTexture pos=" << thePosX << "," << thePosY
             << " tex=" << theStartX << "," << theStartY << " - " << theEndX << "," << theEndY
             << " size=" << myWidth << "x" << myHeight;

    glLoadIdentity();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(theStartX, 1.0f - theStartY);
    glVertex2f(thePosX, thePosY);

    glTexCoord2f(theStartX, 1.0f - theEndY);
    glVertex2f(thePosX, thePosY + myHeight);

    glTexCoord2f(theEndX, 1.0f - theEndY);
    glVertex2f(thePosX + myWidth, thePosY + myHeight);

    glTexCoord2f(theEndX, 1.0f - theStartY);
    glVertex2f(thePosX + myWidth, thePosY);
    glEnd();
    CHECK_OGL_ERROR;
}

extern "C"
EXPORT asl::PlugInBase* EdgeBlender_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new EdgeBlender(myDLHandle);
}
