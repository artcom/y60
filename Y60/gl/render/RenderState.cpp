//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: RenderState.cpp,v $
//   $Author: david $
//   $Revision: 1.3 $
//   $Date: 2005/03/23 17:49:09 $
//
//=============================================================================

#include "RenderState.h"


#ifdef WIN32
#   include <GL/glh_extensions.h>
#else
#   include <GL/gl.h>
#endif

namespace y60 {

    void
    RenderState::init() {
        commitWireframe(_myWireframeFlag);
        commitLighting(_myLightingFlag);
        commitBackfaceCulling(_myBackfaceCullingFlag);
        commitFlatShading(_myFlatShadingFlag);
        commitTexturing(_myTexturingFlag);
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
            glCullFace(GL_BACK);
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
    RenderState::commitTexturing(bool theFlag) {
        if (theFlag) {
            glEnable(GL_TEXTURE_2D);
        } else {
            glDisable(GL_TEXTURE_2D);
        }
        _myTexturingFlag = theFlag;
    }
}
