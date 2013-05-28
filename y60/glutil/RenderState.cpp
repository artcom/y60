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

// own header
#include "RenderState.h"

#include "GLUtils.h"
#include <y60/scene/Viewport.h>
#include <y60/scene/Canvas.h>

using namespace asl;

namespace y60 {
    RenderState::RenderState() :
        _myWireframeFlag(false),
        _myLightingFlag(true),
        _myBackfaceCullingFlag(false),
        _myFlatShadingFlag(false),
        _myTexturingFlag(true),
        _myIgnoreDepthFlag(false),
        _myDepthWritesFlag(true),
        _myPolygonOffsetFlag(false),
        _myBlendFlag(true),
        _myAlphaTestFlag(true),
        _myScissorTestFlag(false),
        _myFrontFaceCCWFlag(true),
        _myEnabledClippingPlanes(0),
        _myScissorParams(0,0,1,1),
        _myCullFaces(GL_BACK)
    {
    }

    void
    RenderState::init() {
        commitWireframe(_myWireframeFlag);
        commitLighting(_myLightingFlag);
        commitBackfaceCulling(_myBackfaceCullingFlag);
        commitFlatShading(_myFlatShadingFlag);
        commitTexturing(_myTexturingFlag);
        commitIgnoreDepth(_myIgnoreDepthFlag);
        commitDepthWrites(_myDepthWritesFlag);
        commitPolygonOffset(_myPolygonOffsetFlag);
        commitBlend(_myBlendFlag);
        commitAlphaTest(_myAlphaTestFlag);
        commitCullFaces(_myCullFaces);
        commitFrontFaceCCW(_myFrontFaceCCWFlag);
    }


    void
    RenderState::commitFrontFaceCCW(bool theFlag) {
        if (theFlag) {
            glFrontFace(GL_CCW);
        } else {
            glFrontFace(GL_CW);
        }
        _myFrontFaceCCWFlag = theFlag;
    }

    void
    RenderState::commitWireframe(bool theFlag) {
        if (theFlag) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        _myWireframeFlag = theFlag;
    }

    void
    RenderState::commitLighting(bool theFlag) {
        //AC_PRINT << (theFlag ? "enable" : "disable") << " LIGHTING, current=" << _myLightingFlag;
        if (theFlag) {
            glEnable(GL_LIGHTING);
        } else {
            glDisable(GL_LIGHTING);
        }
        _myLightingFlag = theFlag;
    }

    void
    RenderState::commitBackfaceCulling(bool theFlag) {
        if (theFlag) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }
        _myBackfaceCullingFlag = theFlag;
    }

    void
    RenderState::commitFlatShading(bool theFlag) {
        if (theFlag) {
            glShadeModel(GL_FLAT);
        } else {
            glShadeModel(GL_SMOOTH);
        }
        _myFlatShadingFlag = theFlag;
    }

    void
    RenderState::commitScissorTest(bool theFlag) {
        if (theFlag) {
            glEnable(GL_SCISSOR_TEST);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }
        _myScissorTestFlag = theFlag;
    }

    void
    RenderState::commitTexturing(bool theFlag) {
        if (theFlag) {
            glEnable(GL_TEXTURE_2D);
        } else {
            glDisable(GL_TEXTURE_2D);
        }
        _myTexturingFlag = theFlag;
    }

    void
    RenderState::setClippingPlanes(const std::vector<asl::Planef> & thePlanes) {
        for (unsigned i = 0; i < thePlanes.size(); ++i) {
            GLenum myGLPlaneId = asGLClippingPlaneId(i);
            glEnable(myGLPlaneId);
            glClipPlane(myGLPlaneId, thePlanes[i].getCoefficients<double>().begin());
        }
        for (int i = thePlanes.size(); i < _myEnabledClippingPlanes; ++i) {
            glDisable(asGLClippingPlaneId(i));
        }
        _myEnabledClippingPlanes = thePlanes.size();
    }
    void
    RenderState::setScissorBox(const asl::Box2f & theBox, const Viewport & theViewport) {
        if (theBox.contains(asl::Box2f(0,0,1,1))) {
            // box is >= screen, turn off scissor test
            setScissorTest(false);
            return;
        }
        setScissorTest(true);
        asl::Vector4i myParams(0,0,0,0);
        if (!theBox.isEmpty()) {
            // transform box to screen (pixel) coords
            const dom::Node * myCanvasNode = theViewport.getNode().parentNode();
            const CanvasPtr & myCanvas = myCanvasNode->getFacade<Canvas>();
            int myHeight = myCanvas->getHeight();
            int myWidth = myCanvas->getWidth();

            int myXMin = static_cast<int>(theBox.getMin()[0] * myWidth);
            int myXMax = static_cast<int>(theBox.getMax()[0] * myWidth);
            int myYMin = static_cast<int>(theBox.getMin()[1] * myHeight);
            int myYMax = static_cast<int>(theBox.getMax()[1] * myHeight);

            myParams = Vector4i(myXMin, myYMin, myXMax-myXMin, myYMax-myYMin);
        }
        if (myParams != _myScissorParams) {
            glScissor(myParams[0], myParams[1], myParams[2], myParams[3]);
            _myScissorParams = myParams;
        }
    }

    void
    RenderState::commitIgnoreDepth(bool theFlag) {
        if (theFlag) {
            if (_myDepthWritesFlag) { 
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_ALWAYS);
            } else {
                glDisable(GL_DEPTH_TEST);
            }
            AC_PRINT << "disable alpaTest";
        } else {
            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);
            AC_PRINT << "enable alpaTest";
        }
        _myIgnoreDepthFlag = theFlag;
    }

    void
    RenderState::commitDepthWrites(bool theFlag) {
        if (theFlag) {
            AC_PRINT << "enable depth mask write";
            glDepthMask(GL_TRUE);
            if (!_myIgnoreDepthFlag) {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_ALWAYS);                
            }
        } else {
            glDepthMask(GL_FALSE);
            AC_PRINT << "disable depth mask write";
        }
        _myDepthWritesFlag = theFlag;
    }

    void
    RenderState::commitPolygonOffset(bool theFlag) {
        if (theFlag) {
            glEnable(GL_POLYGON_OFFSET_POINT);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0, 1.0);
        } else {
            glDisable(GL_POLYGON_OFFSET_POINT);
            glDisable(GL_POLYGON_OFFSET_LINE);
            glDisable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(0.0, 0.0);
        }
        _myPolygonOffsetFlag = theFlag;
    }

    void
    RenderState::commitBlend(bool theFlag) {
        if (theFlag) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
        _myBlendFlag = theFlag;
    }

    void
    RenderState::commitAlphaTest(bool theFlag) {
        if (theFlag) {
            glEnable(GL_ALPHA_TEST);
        } else {
            glDisable(GL_ALPHA_TEST);
        }
        _myAlphaTestFlag = theFlag;
    }

    void
    RenderState::commitCullFaces(GLenum theFaces) {
        glCullFace(theFaces);
        _myCullFaces = theFaces;
    }
}
