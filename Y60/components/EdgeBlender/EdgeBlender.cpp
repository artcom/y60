//============================================================================
//
// Copyright (C) 2000-20011, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include <y60/JSNode.h>
#include <asl/PlugInBase.h>
#include <y60/IRendererExtension.h>
#include <y60/IScriptablePlugin.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/Scene.h>
#include <y60/Movie.h>
#include <y60/OffscreenBuffer.h>

#include <asl/string_functions.h>
#include <asl/numeric_functions.h>
#include <asl/Exception.h>
#include <asl/Logger.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;
using namespace dom;
using namespace y60;


class EdgeBlender :
    public asl::PlugInBase,
    public y60::IRendererExtension,
    public IScriptablePlugin
{
    public:

        EdgeBlender(asl::DLHandle theDLHandle);

        void onStartup(jslib::AbstractRenderWindow * theWindow);

        bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow);

        void handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent);
        void onFrame(AbstractRenderWindow * theWindow , double t);

        void onGetProperty(const std::string & thePropertyName, 
                PropertyValue & theReturnValue) const;
        void onSetProperty(const std::string & thePropertyName, 
                const PropertyValue & thePropertyValue);

        void onPreRender(AbstractRenderWindow * theRenderer);
        void onPostRender(AbstractRenderWindow * theRenderer);

        const char * ClassName() {
            static const char * myClassName = "EdgeBlender";
            return myClassName;
        }

        void onUpdateSettings(dom::NodePtr theSettings);

    private:
        void renderMultiScreen();
        void renderMultiPC();

        void preRender();
        void postRender();
        void renderBlending();
        void copyToTexture();
        void renderTexture(float thePosX, float thePosY,
                float theStartX, float theStartY, float theEndX, float theEndY);
        void rotateTo(unsigned theEdge);
        void drawBlendedEdge(float theStart, float theEnd, float theMargin = 0.0f);
        void drawBlendedCorner(float theMarginX, float theMarginY);
        void drawBlackLevel(float theLeft, float theTop, float theRight, float theBottom);
        float getBlendValue(float theValue);
        

        unsigned _mySceneWidth, _mySceneHeight;
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
        **/

        enum { MODE_MULTI_PC               = 1, 
               MODE_MULTI_SCREEN           = 2
        };

        unsigned _myMode;
        /*
         * _myEdges is a bit-wise logical combination of BLEND_EDGE_XXX
         */
        const static unsigned BLEND_EDGE_LEFT  = 1;
        const static unsigned BLEND_EDGE_RIGHT = 2;
        const static unsigned BLEND_EDGE_UP    = 4;
        const static unsigned BLEND_EDGE_DOWN  = 8;
        
        unsigned  _myEdges;

        unsigned _myRowCount, _myColumnCount;
        
        float _myBlendWidth;
        unsigned _myNumSubdivisions;

        float _myBlackLevel;
        float _myPower;
        float _myGamma;

};

EdgeBlender :: EdgeBlender(asl::DLHandle theDLHandle) :
    asl::PlugInBase(theDLHandle),
    IRendererExtension("EdgeBlender")
{}

void
EdgeBlender::onPostRender(AbstractRenderWindow * theRenderer) {
    AC_TRACE << "onPostRender";
    switch (_myMode) {
        case MODE_MULTI_SCREEN:
            renderMultiScreen();
        break;
        case MODE_MULTI_PC:
            renderMultiPC();
        break;            
    }        
}

void
EdgeBlender::onStartup(jslib::AbstractRenderWindow * theWindow) {
    _myMode = MODE_MULTI_SCREEN;
    _myEdges = 15;  
    _myBlendWidth = 0.2f;
    _myNumSubdivisions = 32;
    _myBlackLevel = 0.1f;
    _myPower = 1.5f;
    _myGamma = 1.2f;
    _myRowCount = 2;
    _myColumnCount = 3;
    
    //setup texture for rendering
    _mySceneWidth = theWindow->getWidth();
    _mySceneHeight = theWindow->getHeight();
    int mySceneTextureWidth = nextPowerOfTwo(_mySceneWidth);
    int mySceneTextureHeight = nextPowerOfTwo(_mySceneHeight);
    
    glGenTextures(1, &_mySceneTexture);
    glBindTexture (GL_TEXTURE_2D, _mySceneTexture );
    glTexImage2D(GL_TEXTURE_2D, 0, 3, mySceneTextureWidth, mySceneTextureHeight, 
                 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    checkOGLError(PLUS_FILE_LINE);
    AC_INFO << "bind texture " << _mySceneTexture << " " 
            << _mySceneWidth << "x" << _mySceneHeight;
}

void
EdgeBlender::onGetProperty(const std::string & thePropertyName,
        PropertyValue & theReturnValue) const
{
    AC_DEBUG << "onGetProperty " << thePropertyName;
    if (thePropertyName == "edges") {
        theReturnValue.set<unsigned>(_myEdges);
    } else if (thePropertyName == "blendwidth") {
        theReturnValue.set<float>(_myBlendWidth);
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
    }
}

void
EdgeBlender::onSetProperty(const std::string & thePropertyName,
        const PropertyValue & thePropertyValue)
{
    AC_DEBUG << "onSetProperty " << thePropertyName;
    if (thePropertyName == "edges") {
        _myEdges = thePropertyValue.get<unsigned>();
    } else if (thePropertyName == "blendwidth") {
        _myBlendWidth = thePropertyValue.get<float>();
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
    _myPower = getSetting(theSettings, "power", _myPower);
    _myGamma = getSetting(theSettings, "gamma", _myGamma);
    
}

bool
EdgeBlender::onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
    AC_DEBUG << "onSceneLoaded";
    return true;
}

void
EdgeBlender::handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {
}

void
EdgeBlender::onFrame(AbstractRenderWindow * theWindow , double t) {
    AC_TRACE << "onFrame";
}

void
EdgeBlender::onPreRender(AbstractRenderWindow * theRenderer) {
    AC_TRACE << "onPreRender";
}

void 
EdgeBlender::renderMultiScreen() {
    preRender();
    copyToTexture();

    glClear(GL_COLOR_BUFFER_BIT); 
    
    float myTextureOffsetX = (_myColumnCount-1)*_myBlendWidth/2;
    float myTextureOffsetY = (_myRowCount-1)*_myBlendWidth/2;
    float myTexCoordY0 = myTextureOffsetY;


    for(int i = 0; i < _myRowCount; ++i) {
        float myY  = float(i) / _myRowCount;

        float myTexCoordY1 = myTexCoordY0 + 1.0f / _myRowCount;
        float myTexCoordX0 = myTextureOffsetX;
        for(int j = 0; j < _myColumnCount; ++j) {
            float myX  = float(j) / _myColumnCount; 
            float myTexCoordX1 = myTexCoordX0 + 1.0f / _myColumnCount;
            
            renderTexture(myX, myY, myTexCoordX0, myTexCoordY0, myTexCoordX1, myTexCoordY1);
            
            myTexCoordX0 = myTexCoordX1 - _myBlendWidth;
        }        
        myTexCoordY0 = myTexCoordY1 - _myBlendWidth;
    }
    
    for(int i = 0; i < _myRowCount; ++i) {
        float myY  = float(i) / _myRowCount;
        float myY0 = (i > 0 ? myY + _myBlendWidth : myY);
        float myY1 = float(i+1) / _myRowCount;
        if (i < _myRowCount-1) {
            myY1 -= _myBlendWidth;
        }
        
        for(int j = 0; j < _myColumnCount; ++j) {
            float myX  = float(j) / _myColumnCount; 
            float myX0 = (j > 0 ? myX + _myBlendWidth : myX);
            float myX1 = float(j+1) / _myColumnCount;
            if (j < _myColumnCount-1) {
                myX1 -= _myBlendWidth;
            }
            
            drawBlackLevel(myX0, myY0, myX1, myY1);

            if (j > 0) {
                rotateTo(BLEND_EDGE_RIGHT);
                drawBlendedEdge(myY0, myY1, myX); 
                rotateTo(BLEND_EDGE_LEFT);                
                drawBlendedEdge(myY0, myY1, myX); 

            }
            if (i > 0) {
                rotateTo(BLEND_EDGE_UP);
                drawBlendedEdge(myX0, myX1, myY); 
                rotateTo(BLEND_EDGE_DOWN);
                drawBlendedEdge(myX0, myX1, myY); 
            }                
            if (j > 0 && i > 0) {
                rotateTo(BLEND_EDGE_LEFT);
                drawBlendedCorner(myX, myY);
                rotateTo(BLEND_EDGE_UP);
                drawBlendedCorner(myY, myX);
                rotateTo(BLEND_EDGE_RIGHT);
                drawBlendedCorner(myX, myY);
                rotateTo(BLEND_EDGE_DOWN);
                drawBlendedCorner(myY, myX);
            }                
        }        
    }

    postRender();
}

void 
EdgeBlender::renderMultiPC() {
    preRender();
    renderBlending();
    postRender();
}
    
void 
EdgeBlender::renderBlending() {

    //blended edges
    if (_myEdges & BLEND_EDGE_LEFT) {        
        float myStart = (_myEdges & BLEND_EDGE_UP) ?
            _myBlendWidth : 0.0f;
        float myEnd = (_myEdges & BLEND_EDGE_DOWN) ?
            1.0f - _myBlendWidth : 1.0f;
        rotateTo(BLEND_EDGE_LEFT);
        drawBlendedEdge(myStart, myEnd);
    }        
    if (_myEdges & BLEND_EDGE_RIGHT) {
        float myStart = (_myEdges & BLEND_EDGE_DOWN) ?
            _myBlendWidth : 0.0f;
        float myEnd = (_myEdges & BLEND_EDGE_UP) ?
            1.0f - _myBlendWidth : 1.0f;
        rotateTo(BLEND_EDGE_RIGHT);
        drawBlendedEdge(myStart, myEnd);
    }        
    if (_myEdges & BLEND_EDGE_UP) {
        float myStart = (_myEdges & BLEND_EDGE_RIGHT) ?
            _myBlendWidth : 0.0f;
        float myEnd = (_myEdges & BLEND_EDGE_LEFT) ?
            1.0f - _myBlendWidth : 1.0f;
        rotateTo(BLEND_EDGE_UP);
        drawBlendedEdge(myStart, myEnd);
    }        
    if (_myEdges & BLEND_EDGE_DOWN) {
        float myStart = (_myEdges & BLEND_EDGE_LEFT) ?
            _myBlendWidth : 0.0f;
        float myEnd = (_myEdges & BLEND_EDGE_RIGHT) ?
            1.0f - _myBlendWidth : 1.0f;
        rotateTo(BLEND_EDGE_DOWN);
        drawBlendedEdge(myStart, myEnd);
    }        

    //corners
    if (_myEdges & BLEND_EDGE_LEFT && _myEdges & BLEND_EDGE_UP) {
        rotateTo(BLEND_EDGE_LEFT);
        drawBlendedCorner(0.0f, 0.0f); 
    }
    if (_myEdges & BLEND_EDGE_UP && _myEdges & BLEND_EDGE_RIGHT) {
        rotateTo(BLEND_EDGE_UP);
        drawBlendedCorner(0.0f, 0.0f); 
    }
    if (_myEdges & BLEND_EDGE_RIGHT && _myEdges & BLEND_EDGE_DOWN) {
        rotateTo(BLEND_EDGE_RIGHT);
        drawBlendedCorner(0.0f, 0.0f); 
    }
    if (_myEdges & BLEND_EDGE_DOWN && _myEdges & BLEND_EDGE_LEFT) {
        rotateTo(BLEND_EDGE_DOWN);
        drawBlendedCorner(0.0f, 0.0f); 
    }

    
    //blacklevel
    float myLeft = _myEdges & BLEND_EDGE_LEFT ? _myBlendWidth : 0.0f;
    float myRight = _myEdges & BLEND_EDGE_RIGHT ? 1.0f - _myBlendWidth : 1.0f;
    float myTop = _myEdges & BLEND_EDGE_UP ? _myBlendWidth : 0.0f;
    float myBottom = _myEdges & BLEND_EDGE_DOWN ? 1.0f - _myBlendWidth : 1.0f;
    drawBlackLevel(myLeft, myTop, myRight, myBottom);
    
}

void 
EdgeBlender::preRender() {
    // load ortho projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0f, 1.0f, 1.0f, 0.0f);

    //load texture matrix
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    
    // load modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
 
    // load attribs
    //glPushAttrib(CURRENT_BIT | GL_ENABLE_BIT);
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING); 
    glDisable(GL_DEPTH_TEST);
}

void 
EdgeBlender::postRender() {
    //restore attribs
    glPopAttrib();

    // restore modelview
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    
    //put into model view
    glMatrixMode(GL_MODELVIEW);
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
    AC_DEBUG << "drawBlackLevel " << theLeft << "," << theTop
             << " - " << theRight << "," << theBottom;
    glLoadIdentity();
    glBegin(GL_TRIANGLE_STRIP);
    glColor4f(1.0f, 1.0f, 1.0f, _myBlackLevel);
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
            AC_WARNING << "theEdge " << theEdge << " unknown.";
    }

    //rotate to the edge
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
EdgeBlender::drawBlendedEdge(float theStart, float theEnd, float theMargin) {
    AC_DEBUG << "drawBlendedEdge start " << theStart << " end " << theEnd;

    float myDelta = 1.0f / _myNumSubdivisions;

    glBegin(GL_TRIANGLE_STRIP);
    for (unsigned i = 0; i <= _myNumSubdivisions; ++i) {
        float myX = i * myDelta;

        float myBlendValue = getBlendValue(myX);

        //glColor4f(0.0f, myBlendValue, 0.0f, max(0.2f, 1.0f - myBlendValue));
        glColor4f(0.0f, 0.0f, 0.0f, 1.0f - myBlendValue);

        float myXPos = theMargin + myX * _myBlendWidth;
        glVertex2f(myXPos, theStart);
        glVertex2f(myXPos, theEnd);
        
        AC_DEBUG << "x " << myX << " blend=" << myBlendValue;

    }
    glEnd();
    
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
EdgeBlender::drawBlendedCorner(float theMarginX, float theMarginY) {

    for (unsigned i = 0; i < _myNumSubdivisions; ++i) {
        float myY = float(i) / _myNumSubdivisions;
        float myY1BlendValue = getBlendValue( myY);
        float myY2BlendValue = getBlendValue(float(i+1) / _myNumSubdivisions);

        glBegin(GL_TRIANGLE_STRIP);
        for (unsigned j = 0; j <= _myNumSubdivisions; ++j) {
            float myX = float(j) / _myNumSubdivisions;
            float myXBlendValue = getBlendValue( myX);

            float myXPos = theMarginX + myX * _myBlendWidth;
            float myYPos = theMarginY + myY * _myBlendWidth;

            float myBlendValue = myXBlendValue * myY1BlendValue;
            //glColor4f(myBlendValue, 0.0f, 0.0f, max(0.2f, 1.0f - myBlendValue));
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f - myBlendValue);
            glVertex2f(myXPos, myYPos);
            
            myBlendValue = myXBlendValue * myY2BlendValue;
            //glColor4f(myBlendValue, 0.0f, 0.0f, max(0.2f, 1.0f - myBlendValue));
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f - myBlendValue);
            glVertex2f(myXPos, myYPos + _myBlendWidth / _myNumSubdivisions);

            AC_DEBUG << "x " << myX << "y " << myY << " blend=" << myBlendValue;
        }
        glEnd();
    }

}

void 
EdgeBlender::copyToTexture() {
    
    glEnable(GL_TEXTURE_2D);

    glBindTexture (GL_TEXTURE_2D, _mySceneTexture );
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, _mySceneWidth, _mySceneHeight, 0);

    glBindTexture (GL_TEXTURE_2D, 0 );
    glDisable(GL_TEXTURE_2D);
    CHECK_OGL_ERROR;
}

void 
EdgeBlender::renderTexture(float thePosX, float thePosY, 
        float theStartX, float theStartY, float theEndX, float theEndY) 
{
    float myWidth = theEndX - theStartX;
    float myHeight = theEndY - theStartY;

    AC_DEBUG << "renderTexture " << thePosX << "," << thePosY
             << " TEX " << theStartX << "," << theStartY << " - " << theEndX << "," << theEndY
             << " WxH " << myWidth << "x" << myHeight;
 
    glLoadIdentity();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture (GL_TEXTURE_2D, _mySceneTexture );

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

    glBindTexture (GL_TEXTURE_2D, 0 );
    glDisable(GL_TEXTURE_2D);
    CHECK_OGL_ERROR;
}

extern "C"
EXPORT asl::PlugInBase* EdgeBlender_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new EdgeBlender(myDLHandle);
}
