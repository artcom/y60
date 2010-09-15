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

// own header
#include "jscpp.h"

#include <y60/glutil/GLUtils.h>

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
#include "JSHermiteSpline.h"
#include "JSSvgPath.h"
#include "JSPerlinNoise.h"
#include "JSSerial.h"
#include "JSStation.h"
#include "JSProcess.h"
#include "JSTextureAtlas.h"

#include <y60/jsbase/JSSphere.h>
#include <y60/jsbase/JSBox.h>
#include <y60/jsbase/JSBox2f.h>
#include <y60/jsbase/JSLine.h>
#include <y60/jsbase/JSPlane.h>
#include <y60/jsbase/JSTriangle.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSQuaternion.h>
#include <y60/jsbase/JSMatrix.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSNodeList.h>
#include <y60/jsbase/JSNamedNodeMap.h>
#include <y60/jsbase/JSResizeableVector.h>
#include <y60/jsbase/JSResizeableRaster.h>
#include <y60/jsbase/JSAccessibleVector.h>
#include <y60/jsbase/JSGlobal.h>
#include <y60/jsbase/JSDomEvent.h>
#include <y60/jsbase/JSFrustum.h>
#include <y60/jsbase/JSBlock.h>
#include <y60/jsbase/JSEnum.h>
#include <y60/jsbase/JSBitset.h>
#include <y60/jsbase/JSWrapper.impl>

#include <y60/base/NodeValueNames.h>

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
    if (!JSNamedNodeMap::initClass(cx, theGlobalObject)) {
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
    if (!JSStation::initClass(cx, theGlobalObject)) {
        return false;
    }
    if (!JSProcess::initClass(cx, theGlobalObject)) {
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
    if (!JSHermiteSpline::initClass(cx, theGlobalObject)) {
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
    if (!JSTextureAtlas::initClass(cx, theGlobalObject)) {
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
