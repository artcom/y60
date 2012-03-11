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

#ifndef _ac_y60_DataTypes_h_
#define _ac_y60_DataTypes_h_

#include "y60_base_settings.h"

#include "typedefs.h"
#include "iostream_functions.h"
#include "RankedFeature.h"
#include "NodeValueNames.h"

#include <asl/dom/Value.h>
#include <asl/base/string_functions.h>
#include <asl/math/linearAlgebra.h>
#include <asl/base/Exception.h>
#include <asl/math/Box.h>
#include <asl/math/Matrix4.h>
#include <asl/math/Vector234.h>
#include <asl/math/Quaternion.h>
#include <asl/math/Frustum.h>
#include <asl/math/BSpline.h>

#include <string>
#include <vector>

namespace y60 {
    Y60_BASE_DECL void registerSomImageTypes(dom::ValueFactory & theFactory);
    Y60_BASE_DECL void registerSomMathTypes(dom::ValueFactory & theFactory);
    Y60_BASE_DECL void registerSomVectorTypes(dom::ValueFactory & theFactory);
    Y60_BASE_DECL void registerSomTypes(dom::ValueFactory & theFactory);
}

namespace dom {
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::AcBool, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::BlendFunction, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Vector2i, VectorValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Vector2f, VectorValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Vector2d, VectorValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Vector3i, VectorValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Vector3f, VectorValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Vector3d, VectorValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Vector4i, VectorValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Vector4f, VectorValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Vector4d, VectorValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Point2i, VectorValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Point2f, VectorValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Point2d, VectorValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Point3i, VectorValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Point3f, VectorValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Point3d, VectorValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Point4i, VectorValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Point4f, VectorValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Point4d, VectorValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Quaternionf, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Quaterniond, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Matrix4f, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Matrix4d, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Box2i, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Box2f, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Box2d, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Box3i, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Box3f, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Box3d, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Line<float>, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::LineSegment<float>, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Ray<float>, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Line<double>, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::LineSegment<double>, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Ray<double>, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Plane<float>, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Sphere<float>, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Triangle<float>, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Plane<double>, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Sphere<double>, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Triangle<double>, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::BSpline<float>, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::BSpline<double>, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfBool, VectorValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfUnsignedInt, VectorValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfSignedInt, VectorValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfFloat, VectorValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfVector2i, VectorValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfVector2f, VectorValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfVector3f, VectorValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfVector4f, VectorValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfQuaternionf, VectorValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::VectorOfString, ComplexValue, dom::MakeResizeableVector, StringValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::VectorOfVectorOfString, ComplexValue, dom::MakeResizeableVector, dom::ValueWrapper<y60::VectorOfString>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::VectorOfVectorOfFloat, ComplexValue, dom::MakeResizeableVector, dom::ValueWrapper<y60::VectorOfFloat>::Type );

    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RankedFeature, ComplexValue, dom::MakeOpaqueVector, void);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::VectorOfRankedFeature, ComplexValue, dom::MakeResizeableVector, dom::ValueWrapper<y60::RankedFeature>::Type);

    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::BlendEquation, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::RenderStyle, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::RenderStyles, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::VertexBufferUsage, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfBlendEquation, VectorValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfBlendFunction, VectorValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::TextureApplyMode, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::TextureSampleFilter, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::TextureWrapMode, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::TextureUsage, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::TextureType, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::ResizePolicy, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::ProjectionType, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::PrimitiveType, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::ImageType, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::RGB, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::RGBA, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::BGR, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::BGRA, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::ABGR, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::GRAY, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::GRAY16, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::GRAY32, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::GRAYS16, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::DXT1, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::DXT1a, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::DXT3, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::DXT5, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::RGBA_HALF, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::RGB_HALF, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::RGBA_FLOAT, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::RGB_FLOAT, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::TargetBuffers, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfRGB, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::RGB>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfRGBA, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::RGBA>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfBGR, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::BGR>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfBGRA, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::BGRA>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfABGR, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::ABGR>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfGRAY, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::GRAY>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfGRAY16, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::GRAY16>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfGRAY32, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::GRAY32>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfGRAYS16, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::GRAYS16>::Type);

    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfDXT1, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::DXT1>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfDXT1a, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::DXT1a>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfDXT3, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::DXT3>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfDXT5, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::DXT5>::Type);

    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfRGBA_HALF, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::RGBA_HALF>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfRGB_HALF, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::RGB_HALF>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfRGBA_FLOAT, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::RGBA_FLOAT>::Type);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RasterOfRGB_FLOAT, ComplexValue, dom::MakeResizeableRaster, dom::ValueWrapper<asl::RGB_FLOAT>::Type);
}

#endif
