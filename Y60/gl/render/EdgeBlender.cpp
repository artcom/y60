//============================================================================
//
// Copyright (C) 2002, ART+COM AG Berlin
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
//   $Revision: 1.6 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "EdgeBlender.h"
#include <y60/GLUtils.h>
#include <asl/Logger.h>

#include <math.h>
#include <iostream>
#ifdef WIN32
// Note: GLH_EXT_SINGLE_FILE must be defined only in one object file
// it makes the header file to define the function pointer variables
// never set it in a .h file or any other file that shall be linked
// with this object file
// If you use GLH_EXT_SINGLE_FILE 1 make sure that glh_extensions.h
// and glh_genext.h has not been included from another include file
// already without GLH_EXT_SINGLE_FILE set
//
//#   define GLH_EXT_SINGLE_FILE
#   include <GL/glh_extensions.h>
#   include <GL/glh_genext.h>
#endif
#include <GL/gl.h>

#include <y60/GLAlloc.h>

#define DB(x) // x
#define DB2(x) // x

using namespace std;

namespace y60 {

	EdgeBlender::EdgeBlender(EDGEBLENDINGMODE theMode):
		_myScreenWidth(0),
		_myScreenHeight(0),
		_myData(0),
		_myMode(theMode)
	{
        for (int i=0; i< 6; i++) {
            _drawAt[i] = false;
            _myWidth[i] = 0;
        }
        reshape(800,600);
	};

	EdgeBlender::~EdgeBlender(){
	};

    int
    EdgeBlender::getWidthCorrection() const  {
        if (_myMode == BLIT) {
            return _myWidth[VERTICAL_SPLIT];
        } else {
            return 0;
        }
    }

	void
	EdgeBlender::reshape(int theWidth, int theHeight) {
		_myScreenWidth = theWidth;
		_myScreenHeight = theHeight;
	}

    void
    EdgeBlender::preDisplay() {
        // set the cheapest opengl state
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_LIGHTING);

        glDisable(GL_MULTISAMPLE_ARB);
        glDisable(GL_SAMPLE_COVERAGE_ARB);
        glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_FASTEST);

        glDisable(GL_FOG);

        if (_myMode == BLIT || _myMode == BLENDBORDER) {
            // disable all texture units
            int myTextureCount;
            glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &myTextureCount);
            for (unsigned myTextureCounter = 0;
                    myTextureCounter < myTextureCount; ++myTextureCounter) {
                glActiveTextureARB(asGLTextureRegister(myTextureCounter));
                glClientActiveTextureARB(asGLTextureRegister(myTextureCounter));
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                glDisable(GL_TEXTURE_2D);
            }
        }

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, _myScreenWidth, 0.0, _myScreenHeight);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(0, 0, _myScreenWidth, _myScreenHeight);

    }


    void
    EdgeBlender::setupViewport() {
       if (_drawAt[HORIZONTAL_SPLIT] || _drawAt[VERTICAL_SPLIT]) {
            int myNewWidth;
            if (_drawAt[HORIZONTAL_SPLIT]) {
                myNewWidth = (_myScreenWidth - _myWidth[HORIZONTAL_SPLIT]);
            } else {
                myNewWidth = (_myScreenWidth - _myWidth[VERTICAL_SPLIT]);
            }
            glViewport(0, 0, myNewWidth, _myScreenHeight);
        }
    }

    void
    EdgeBlender::postDisplay(bool theLightingFlag, bool theAntialiasingFlag, bool theTexturingFlag) {
        if (theLightingFlag) {
            glEnable(GL_LIGHTING);
        }
        if (theAntialiasingFlag) {
            glEnable(GL_MULTISAMPLE_ARB);
            glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
        }

        glEnable(GL_BLEND);
        glEnable(GL_DITHER);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        // enable all texture units
        int myTextureCount;
        glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &myTextureCount);
        for (unsigned myTextureCounter = 0;
                myTextureCounter < myTextureCount; ++myTextureCounter) {
            glActiveTextureARB(asGLTextureRegister(myTextureCounter));
            glClientActiveTextureARB(asGLTextureRegister(myTextureCounter));
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            if (theTexturingFlag) {
                glEnable(GL_TEXTURE_2D);
            }
        }


        glColor4f(1.0,1.0,1.0, 1.0);
    }

    void
    EdgeBlender::preBlitFramebuffer() {
        glDisable(GL_BLEND);
    }
    void
    EdgeBlender::postBlitFramebuffer() {
        glEnable(GL_BLEND);

    }

    void
    EdgeBlender::blitFramebuffer() {
       if (_drawAt[VERTICAL_SPLIT]) {
            glRasterPos2i( int((_myScreenWidth / 2.0)), 0);

            //glReadPixels( int((_myScreenWidth / 2.0 )- _myWidth[VERTICAL_SPLIT]) - 1 , 0,
            //              int(_myScreenWidth /2.0),_myScreenHeight,GL_RGBA, GL_UNSIGNED_BYTE, _myData.begin());
            //glDrawPixels(int(_myScreenWidth /2.0), _myScreenHeight, GL_RGBA, GL_UNSIGNED_BYTE, _myData.begin());
            // with antialiasing enabled, this is way to slow to be used
            glCopyPixels( int((_myScreenWidth / 2.0 )- _myWidth[VERTICAL_SPLIT]), 0,
                          int((_myScreenWidth /2.0)) , _myScreenHeight, GL_COLOR);
       }
    }

	void
	EdgeBlender::display(bool theLightingFlag, bool theAntialiasingFlag, bool theTextureingFlag) {
        preDisplay();
        {

            if (_drawAt[VERTICAL_SPLIT]) {
                if (_myMode == BLIT) {
                    preBlitFramebuffer();
                    blitFramebuffer();
                    postBlitFramebuffer();
                }
                if (_myMode == PBUFFERED) {
                    // draw textured quad with our pbuffered texture
                    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                    glEnable(GL_TEXTURE_2D);
                    glBegin( GL_QUADS );
                        glTexCoord2f(0.0f,0.0f); glVertex2f(0.0f, 0.0f);
                        glTexCoord2f(0.0f,1.0f); glVertex2f(0.0f, (float)_myScreenHeight);
                        glTexCoord2f(0.5f,1.0f); glVertex2f(((float)_myScreenWidth / 2.0f ), (float)_myScreenHeight);
                        glTexCoord2f(0.5f,0.0f); glVertex2f(((float)_myScreenWidth / 2.0f ), 0.0f);

                    glEnd();
                    glBegin( GL_QUADS );
                        glTexCoord2f(0.4f,0.0f); glVertex2f(((float)_myScreenWidth / 2.0f ), 0.0f);
                        glTexCoord2f(0.4f,1.0f); glVertex2f(((float)_myScreenWidth / 2.0f ), (float)_myScreenHeight);
                        glTexCoord2f(0.9f,1.0f); glVertex2f((float)_myScreenWidth, (float)_myScreenHeight);
                        glTexCoord2f(0.9f,0.0f); glVertex2f((float)_myScreenWidth, 0.0f);

                    glEnd();
                    glDisable(GL_TEXTURE_2D);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                }
                int theTopAlphaSize = _toAlpha[VERTICAL_SPLIT].size();
                // render left ramp
                for(int poly = 0; poly < theTopAlphaSize -1; poly++) {
                    glBegin(GL_QUADS);
                    {
                        float myPolyWidth = (float(_myWidth[VERTICAL_SPLIT]) / (theTopAlphaSize -1 ));
                        float theAlpha = _toAlpha[VERTICAL_SPLIT][poly ];
                        glColor4f(0,0,0, theAlpha);
                        glVertex2f( float((_myScreenWidth / 2.0 ) - (myPolyWidth * poly)),  0);
                        glVertex2f( float((_myScreenWidth / 2.0 ) - (myPolyWidth * poly)), float(_myScreenHeight));


                        theAlpha = _toAlpha[VERTICAL_SPLIT][poly + 1];
                        glColor4f(0,0,0, theAlpha);
                        glVertex2f( float((_myScreenWidth / 2.0 ) - (myPolyWidth * (poly+1))), float(_myScreenHeight));
                        glVertex2f( float((_myScreenWidth / 2.0 ) - (myPolyWidth * (poly+1))),  0);
                    }
                    glEnd();
                }
                // render right ramp
                for(int poly = 0; poly < theTopAlphaSize -1; poly++) {
                    glBegin(GL_QUADS);
                    {
                        float theAlpha1 = _toAlpha[VERTICAL_SPLIT][poly];
                        float theAlpha2 = _toAlpha[VERTICAL_SPLIT][poly + 1 ];
                        float myPolyWidth = (float(_myWidth[VERTICAL_SPLIT]) / (theTopAlphaSize -1 ));

                        glColor4f(0,0,0, theAlpha1);
                        glVertex2f( float((_myScreenWidth / 2.0 ) + (myPolyWidth * poly)),  0);

                        glColor4f(0,0,0, theAlpha2);
                        glVertex2f( float((_myScreenWidth / 2.0 ) + (myPolyWidth * (poly+1))),  0);

                        glColor4f(0,0,0, theAlpha2);
                        glVertex2f( float((_myScreenWidth / 2.0 ) + (myPolyWidth * (poly+1))), float(_myScreenHeight));

                        glColor4f(0,0,0, theAlpha1);
                        glVertex2f( float((_myScreenWidth / 2.0 ) + (myPolyWidth * poly)), float(_myScreenHeight));
                    }
                    glEnd();
                }
                glDisable(GL_BLEND);
                glColor4f(1.0,1.0,1.0, 1.0);
            }
/*            if (_drawAt[HORIZONTAL_SPLIT]) {
                blitFramebuffer();
            }
            if (_drawAt[TOP]) {
                int theTopAlphaSize = _toAlpha[TOP].size();
                for(int poly = 0; poly < theTopAlphaSize -1; poly++) {
                    glBegin(GL_QUADS);
                    {

                        glColor4f(0.0, 0.0, 0.0, _toAlpha[TOP][poly]);
                        float myWidth =   (_myWidth[TOP] / float(_myScreenHeight));
                        float myPolyWidth = (myWidth / (theTopAlphaSize -1 ));
                        glVertex2f(-0.5,  0.5 - (myPolyWidth * poly));
                        glVertex2f( 0.5,  0.5 - (myPolyWidth * poly));

                        //cerr << "EdgeBlender::display(): myWidth = " << myWidth << endl;
                        glColor4f(0.0, 0.0, 0.0, _toAlpha[TOP][poly+1]);
                        glVertex2f( 0.5, 0.5 - (myPolyWidth * (poly + 1)));
                        glVertex2f(-0.5, 0.5 - (myPolyWidth * (poly + 1)));
                    }
                    glEnd();
                }
            }
            if (_drawAt[BOTTOM]) {
                int theBottomAlphaSize = _toAlpha[BOTTOM].size();
                for(int poly = 0; poly<theBottomAlphaSize -1; poly++) {
                    glBegin(GL_QUADS);
                    {
                        glColor4f(0.0, 0.0, 0.0, _toAlpha[BOTTOM][poly]);
                        float myWidth =   (_myWidth[BOTTOM] / float(_myScreenHeight));
                        float myPolyWidth = (myWidth / (theBottomAlphaSize -1 ));
                        glVertex2f(-0.5, -0.5 + (myPolyWidth * poly));
                        glVertex2f( 0.5, -0.5 + (myPolyWidth * poly));

                        //cerr << "EdgeBlender::display(): myWidth = " << myWidth << endl;
                        glColor4f(0.0, 0.0, 0.0, _toAlpha[BOTTOM][poly+1]);
                        glVertex2f( 0.5, -0.5 + (myPolyWidth * (poly + 1)));
                        glVertex2f(-0.5, -0.5 + (myPolyWidth * (poly + 1)));
                    }
                    glEnd();
                }
            }

            if (_drawAt[LEFT]) {
                int theLeftAlphaSize = _toAlpha[LEFT].size();
                for(int poly = 0; poly<theLeftAlphaSize -1; poly++) {
                    glBegin(GL_QUADS);
                    {
                        glColor4f(0.0, 0.0, 0.0, _toAlpha[LEFT][poly]);
                        float myWidth =   (_myWidth[LEFT] /  float (_myScreenWidth));
                        float myPolyWidth = (myWidth / (theLeftAlphaSize -1 ));
                        glVertex2f(-0.5 + (myPolyWidth * poly), -0.5);
                        glVertex2f(-0.5 + (myPolyWidth * poly),  0.5);

                        glColor4f(0.0, 0.0, 0.0, _toAlpha[LEFT][poly+1]);
                        glVertex2f( -0.5 + (myPolyWidth * (poly +1) ), 0.5);
                        glVertex2f( -0.5 + (myPolyWidth * (poly +1) ), -0.5);
                    }
                    glEnd();
                }
            }

            if (_drawAt[RIGHT]) {
                int theRightAlphaSize = _toAlpha[RIGHT].size();
                for(int poly = 0; poly<theRightAlphaSize -1; poly++) {
                    glBegin(GL_QUADS);
                    {
                        glColor4f(0.0, 0.0, 0.0, _toAlpha[RIGHT][poly]);
                        float myWidth =   (_myWidth[RIGHT] / float (_myScreenWidth));
                        float myPolyWidth = (myWidth / (theRightAlphaSize -1 ));
                        glVertex2f( 0.5 - (myPolyWidth * poly), -0.5);
                        glVertex2f( 0.5 - (myPolyWidth * poly),  0.5);

                        glColor4f(0.0, 0.0, 0.0, _toAlpha[RIGHT][poly+1]);
                        glVertex2f( 0.5 - (myPolyWidth * (poly + 1)), 0.5);
                        glVertex2f(0.5 - (myPolyWidth * (poly + 1)), -0.5);
                    }
                    glEnd();
                }
            }
*/
        }
        //postDisplay(theLightingFlag, theAntialiasingFlag, theTextureingFlag);
    }

	void
	EdgeBlender::setBlendValues(BLENDAREA theBlendArea, vector<float> & toAlpha) {
		_toAlpha[theBlendArea] = toAlpha;
	}


	void
	EdgeBlender::addBlendArea(BLENDAREA theBlendArea, int theWidth) {
		_drawAt[theBlendArea] = true;
		_myWidth[theBlendArea] = theWidth;
	}

	void
	EdgeBlender::addBlendArea(BLENDAREA theBlendArea, int theWidth, int thePoints, float theGamma) {
		_drawAt[theBlendArea] = true;
		_myWidth[theBlendArea] = theWidth;
        computeGammaRamp(theBlendArea, thePoints, theGamma);
	}

	void
	EdgeBlender::removeBlendArea(BLENDAREA theBlendArea) {
		_drawAt[theBlendArea] = false;
	}


    void
    EdgeBlender::computeGammaRamp(BLENDAREA theBlendArea, int thePoints, float theGamma) {
        DB2("AC_DEBUG << computeGammaRamp(): set BlendValues of BLENDAREA: " << theBLENDAREA <<
                " with Gamma: " << theGamma << " resulting in: ");

        vector<float> blendValues;
        for (int i = 0; i < thePoints; i++) {
            float value = float(pow((i/ (float(thePoints)-1.)), (1.0/theGamma) ));
            blendValues.push_back( 1.0f - value );
            DB2("\tpixel " << i << ": " << value);
        }
        setBlendValues(theBlendArea, blendValues );
    }

} // namespace video

//==============================================================================
//
// $Log: EdgeBlender.cpp,v $
// Revision 1.6  2005/04/18 19:59:29  pavel
// cleanup, fixed typos, removed old AC_DB macros
// added Vertex Buffer Object support
//
// Revision 1.5  2005/02/16 10:24:43  christian
// - added texture toggeling
// - fixed key mover with quaternions
//
// Revision 1.4  2004/10/18 16:22:56  janbo
// moved namespaces renderer, scene, renderutil, input to y60
// fixed debug test renderer extension
//
// Revision 1.3  2004/03/01 14:40:08  christian
// - fixed exporter bug (broken geometry)
// - cleanup
//
// Revision 1.2  2004/02/12 12:45:33  christian
// - improved render state handling with glPushAttrib/glPopAttrib
//
// Revision 1.1  2004/02/11 14:50:08  martin
// moved renderer to gl/renderer, text and util
//
// Revision 1.8  2004/02/05 17:30:21  martin
// Glut->SDL Port
//
// Revision 1.7  2003/11/21 18:09:58  valentin
// - pbuffer next step
//
// Revision 1.6  2003/11/21 12:26:21  valentin
// - pbuffer support, II
//
// Revision 1.5  2003/11/20 16:47:44  valentin
// - pbuffer next step
//
// Revision 1.4  2003/11/20 15:35:28  valentin
// - initial pbuffer support
//
// Revision 1.3  2003/11/19 15:30:43  martin
// restored linux build
//
// Revision 1.2  2003/11/19 11:08:54  valentin
// - marriage between edgeblending and antialiasing
//
// Revision 1.1  2003/11/17 11:37:07  valentin
// - edgeblending support and control
//
// Revision 1.2  2002/09/06 18:18:03  valentin
// merged linuxport_2nd_try branch into trunk
// -branch tag before merge : branch_before_merge_final
// -trunk tag before merge  : trunk_before_merge
//
//
// -Merge conflicts:
//  image/glutShowImage/Makefile
//  image/glutShowImage/glShowImage.C
//  loader/libPfIv/pfiv.c++
//  lso/script/vrfm/Intercom.h
//  tools/Pfconvert/pfconvert.c
//  vrap/libMover/HMD.c++
//
//  - compiled && testrun
//  - commit
//  - merged trunk tag : trunk_after_merge
//
// Revision 1.1.2.6  2002/08/30 16:26:51  martin
// more std-ization
//
// Revision 1.1.2.5  2002/08/27 23:16:03  christian
// added setter for gamma
//
// Revision 1.1.2.4  2002/07/10 12:59:30  stefan
// - added gamma support (now computes gamma ramp by itself!)
//
// Revision 1.1.2.3  2002/06/27 08:37:51  stefan
// - fixed bug where video would edge-blend ONLY if water was dispalyed first
//
// Revision 1.1.2.2  2002/03/20 12:53:10  strat
//
// - fixed edge blending on left and right BLENDAREAs
//
// Revision 1.1.2.1  2002/02/05 09:47:22  david
// added EdgeBlender
//
//
//==============================================================================
