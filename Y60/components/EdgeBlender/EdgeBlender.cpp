//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: EdgeBlender.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.15 $
//
// Description:
//
//=============================================================================

#include <y60/JSNode.h>
#include <asl/PlugInBase.h>
#include <y60/IRendererExtension.h>
#include <y60/IScriptablePlugin.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/Scene.h>
#include <y60/Movie.h>

#include <asl/string_functions.h>
#include <asl/Exception.h>
#include <asl/Logger.h>

#include <iostream>

using namespace std;
//using namespace asl;
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
        float getBlendValue(float theValue);
        void drawBlackLevel();
        void rotateTo(unsigned theEdge);
        void drawBlendedEdge(float theStart, float theEnd);
        void drawBlendedCorner();
        const static unsigned BLEND_EDGE_LEFT  = 1;
        const static unsigned BLEND_EDGE_RIGHT = 2;
        const static unsigned BLEND_EDGE_UP    = 4;
        const static unsigned BLEND_EDGE_DOWN  = 8;
        unsigned  _myEdges;
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
EdgeBlender::onStartup(jslib::AbstractRenderWindow * theWindow) {
    _myEdges = 15; //BLEND_EDGE_LEFT;
    _myBlendWidth = 0.2f;
    _myNumSubdivisions = 32;
    _myBlackLevel = 0.1f;
    _myPower = 1.5f;
    _myGamma = 1.2f;
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
    }
}

void 
EdgeBlender::onUpdateSettings(dom::NodePtr theSettings) {
    AC_DEBUG << "onUpdateSettings " << *theSettings;
    _myEdges = getSetting(theSettings, "edges", _myEdges);
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
EdgeBlender::drawBlackLevel() {
    float myLeft = _myEdges & BLEND_EDGE_LEFT ? _myBlendWidth : 0.0f;
    float myRight = _myEdges & BLEND_EDGE_RIGHT ? 1.0f - _myBlendWidth : 1.0f;
    float myUp = _myEdges & BLEND_EDGE_UP ? _myBlendWidth : 0.0f;
    float myDown = _myEdges & BLEND_EDGE_DOWN ? 1.0f - _myBlendWidth : 1.0f;
    glLoadIdentity();
    glBegin(GL_TRIANGLE_STRIP);
    glColor4f(1.0f, 1.0f, 1.0f, _myBlackLevel);
    glVertex2f(myLeft, myUp);
    glVertex2f(myLeft, myDown);
    glVertex2f(myRight, myUp);
    glVertex2f(myRight, myDown);
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

void 
EdgeBlender::drawBlendedEdge(float theStart, float theEnd) {
    AC_DEBUG << "drawBlendedEdge start " << theStart << " end " << theEnd;

    float myDelta = 1.0f / _myNumSubdivisions;

    glBegin(GL_TRIANGLE_STRIP);
    for (unsigned i = 0; i <= _myNumSubdivisions; ++i) {
        float myX = i * myDelta;

        float myBlendValue = getBlendValue(myX);

        glColor4f(0.0f, 0.0f, 0.0f, 1.0f - myBlendValue);

        float myXPos = myX * _myBlendWidth;
        glVertex2f(myXPos, theStart);
        glVertex2f(myXPos, theEnd);
        
        AC_DEBUG << "x " << myX << " blend=" << myBlendValue;

    }
    glEnd();
    
}

void 
EdgeBlender::drawBlendedCorner() {

    float myDelta = 1.0f / _myNumSubdivisions;
    
    for (unsigned i = 0; i <= _myNumSubdivisions; ++i) {
        float myY = i * myDelta;
        float myY1BlendValue = getBlendValue( myY);
        float myY2BlendValue = getBlendValue( min(1.0f, myY + myDelta));

        glBegin(GL_TRIANGLE_STRIP);
        for (unsigned j = 0; j <= _myNumSubdivisions; ++j) {
            float myX = j * myDelta;
            float myXBlendValue = getBlendValue( myX);

            float myXPos = myX * _myBlendWidth;
            float myYPos = myY * _myBlendWidth;

            float myBlendValue = myXBlendValue * myY1BlendValue;
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f - myBlendValue);
            glVertex2f(myXPos, myYPos);
            
            myBlendValue = myXBlendValue * myY2BlendValue;
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f - myBlendValue);
            glVertex2f(myXPos, myYPos + _myBlendWidth / _myNumSubdivisions);

            AC_DEBUG << "x " << myX << "y " << myY << " blend=" << myBlendValue;
        }
        glEnd();
    }

}

void
EdgeBlender::onPostRender(AbstractRenderWindow * theRenderer) {
    AC_TRACE << "onPostRender";

    // load ortho projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0f, 1.0f, 1.0f, 0.0f);

    // load modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
 
    // draw stuff
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING); 


    //blended edges
    if (_myEdges & BLEND_EDGE_LEFT) {        
        rotateTo(BLEND_EDGE_LEFT);
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
        drawBlendedCorner(); 
    }
    if (_myEdges & BLEND_EDGE_UP && _myEdges & BLEND_EDGE_RIGHT) {
        rotateTo(BLEND_EDGE_UP);
        drawBlendedCorner(); 
    }
    if (_myEdges & BLEND_EDGE_RIGHT && _myEdges & BLEND_EDGE_DOWN) {
        rotateTo(BLEND_EDGE_RIGHT);
        drawBlendedCorner(); 
    }
    if (_myEdges & BLEND_EDGE_DOWN && _myEdges & BLEND_EDGE_LEFT) {
        rotateTo(BLEND_EDGE_DOWN);
        drawBlendedCorner(); 
    }

    
    //
    //blacklevel
    drawBlackLevel();
    
    
    glPopAttrib();

    // restore modelview
    glPopMatrix();

    // restore projection
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

extern "C"
EXPORT asl::PlugInBase* EdgeBlender_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new EdgeBlender(myDLHandle);
}
