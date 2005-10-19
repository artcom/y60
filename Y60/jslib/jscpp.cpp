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
//   $RCSfile: jscpp.cpp,v $
//   $Author: christian $
//   $Revision: 1.12 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSSphere.h"
#include "JSBox.h"
#include "JSLine.h"
#include "JSPlane.h"
#include "JSTriangle.h"
#include "JSVector.h"
#include "JSQuaternion.h"
#include "JSMatrix.h"
#include "JSNode.h"
#include "JSNodeList.h"
#include "JSNamedNodeMap.h"
#include "JSResizeableVector.h"
#include "JSResizeableRaster.h"
#include "JSAccessibleVector.h"
#include "JSSerial.h"
#include "JSGlobal.h"
#include "JSStringMover.h"
#include "JSRequestWrapper.h"
#include "JSRequestManager.h"
#include "JSRenderer.h"
#include "JSKeyframe.h"
#include "JSCoordSpline.h"
#include "JSModellingFunctions.h"
#include "JSLogger.h"
#include "JSdirectory_functions.h"
#include "JSfile_functions.h"
#include "JSScene.h"
#include "JSFrustum.h"
#include "JSGLResourceManager.h"
#include "JSOffScreenRenderArea.h"
#include "JSBlock.h"

#include "jscpp.h"

namespace jslib {

bool initCppClasses(JSContext *cx, JSObject *theGlobalObject) {

    if (!JSVector<asl::Vector2f>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVector<asl::Vector3f>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVector<asl::Vector4f>::initClass(cx, theGlobalObject)) {
        return false;
    }

    if (!JSVector<asl::Vector2d>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVector<asl::Vector3d>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVector<asl::Vector4d>::initClass(cx, theGlobalObject)) {
        return false;
    }

    if (!JSVector<asl::Vector2i>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVector<asl::Vector3i>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVector<asl::Vector4i>::initClass(cx, theGlobalObject)) {
        return false;
    }

    if (!JSVector<asl::Point2f>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVector<asl::Point3f>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVector<asl::Point4f>::initClass(cx, theGlobalObject)) {
        return false;
    }

    if (!JSVector<asl::Point2d>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVector<asl::Point3d>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVector<asl::Point4d>::initClass(cx, theGlobalObject)) {
        return false;
    }

    if (!JSVector<asl::Point2i>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVector<asl::Point3i>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSVector<asl::Point4i>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if ( ! JSQuaternion::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSMatrix::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSNode::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSNodeList::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSAccessibleVector::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSResizeableVector::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSResizeableRaster::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSSerial::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSTriangle::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSPlane::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSLine::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSRay::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSLineSegment::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSBox3f::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSSphere::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSStringMover::initClass(cx, theGlobalObject)) {
        return false;
    }

    if (!JS_DefineFunctions(cx,theGlobalObject,Global::Functions())) {
        return false;
    }
    createFunctionDocumentation("MathFunctions", Global::Functions());

    if (!JSRequestWrapper::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSRequestManager::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSRenderer::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSKeyframe::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSCoordSpline::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSScene::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSModellingFunctions::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSLogger::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSFrustum::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSGLResourceManager::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSOffScreenRenderArea::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSBlock::initClass(cx, theGlobalObject)) {
        return false;
    }

    if (!JS_DefineFunctions(cx,theGlobalObject, JSDirectoryFunctions::Functions())) {
        return false;
    }
    createFunctionDocumentation("DirectoryFunctions", JSDirectoryFunctions::Functions());

    if (!JS_DefineFunctions(cx,theGlobalObject, JSFileFunctions::Functions())) {
        return false;
    }
    createFunctionDocumentation("FileFunctions", JSFileFunctions::Functions());


//    createFunctionDocumentation("ProcFunctions", JSProcFunctions::Functions());

    return true;
}

}
