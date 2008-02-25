//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include <y60/GLUtils.h>

#include "JSStringMover.h"
#include "JSRequestWrapper.h"
#include "JSRequestManager.h"
#include "JSRenderer.h"
#include "JSKeyframe.h"
#include "JSCoordSpline.h"
#include "JSModellingFunctions.h"
#include "JSLogger.h"
#include "JSLocale.h"
#include "JSfile_functions.h"
#include "JSScene.h"
#include "JSGLResourceManager.h"
#include "JSOffscreenRenderArea.h"
#include "JSZipWriter.h"
#include "JSTestServer.h"
#include "JSBSpline.h"
#include "JSSvgPath.h"
#include "JSPerlinNoise.h"
#include "JSSerial.h"

#include "jscpp.h"

#include <y60/JSSphere.h>
#include <y60/JSBox.h>
#include <y60/JSBox2f.h>
#include <y60/JSLine.h>
#include <y60/JSPlane.h>
#include <y60/JSTriangle.h>
#include <y60/JSVector.h>
#include <y60/JSQuaternion.h>
#include <y60/JSMatrix.h>
#include <y60/JSNode.h>
#include <y60/JSNodeList.h>
#include <y60/JSNamedNodeMap.h>
#include <y60/JSResizeableVector.h>
#include <y60/JSResizeableRaster.h>
#include <y60/JSAccessibleVector.h>
#include <y60/JSGlobal.h>
#include <y60/JSDomEvent.h>
#include <y60/JSFrustum.h>
#include <y60/JSBlock.h>
#include <y60/JSEnum.h>
#include <y60/JSBitset.h>
#include <y60/JSWrapper.impl>

#include <y60/NodeValueNames.h>

namespace jslib {

template class JSWrapper<y60::RenderStyles>;
template class JSWrapper<y60::TargetBuffers>;

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
    if (!JSBox2f::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSSphere::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSBSpline::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSSvgPath::initClass(cx, theGlobalObject)) {
        return false;
    }

    if (!JSStringMover::initClass(cx, theGlobalObject)) {
        return false;
    }

    if (!JS_DefineFunctions(cx,theGlobalObject,Global::Functions())) {
        return false;
    }
    createFunctionDocumentation("MathFunctions", Global::Functions());

    if (!JSDomEvent::initClass(cx, theGlobalObject)) {
        return false;
    }
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
    if (!JSLocale::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSFrustum::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSGLResourceManager::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSOffscreenRenderArea::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSBlock::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSZipWriter::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSTestServer::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSPerlinNoise::initClass(cx, theGlobalObject)) {
        return false;
    }
    /*
    if (!JSEnum<asl::MatrixType>::initClass(cx, theGlobalObject)) {
        return false;
    }   
    */
    if (!JSEnum<y60::BlendEquation>::initClass(cx, theGlobalObject)) {
        return false;
    }   
    if (!JSEnum<y60::TextureWrapMode>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSEnum<y60::TextureApplyMode>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSEnum<y60::TextureSampleFilter>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSEnum<y60::BlendFunction>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSEnum<y60::VertexBufferUsage>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSEnum<asl::ResizePolicy>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSEnum<y60::ImageType>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSEnum<asl::ProjectionType>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSBitset<y60::RenderStyles>::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSBitset<y60::TargetBuffers>::initClass(cx, theGlobalObject)) {
        return false;
    }

    if (!JS_DefineFunctions(cx,theGlobalObject, JSFileFunctions::Functions())) {
        return false;
    }
    createFunctionDocumentation("FileFunctions", JSFileFunctions::Functions());


//    createFunctionDocumentation("ProcFunctions", JSProcFunctions::Functions());

    return true;
}

}
