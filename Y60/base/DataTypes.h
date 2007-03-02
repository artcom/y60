//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: DataTypes.h,v 1.2 2005/04/08 17:34:41 david Exp $
//   $RCSfile: DataTypes.h,v $
//   $Author: david $
//   $Revision: 1.2 $
//   $Date: 2005/04/08 17:34:41 $
//
//
//  Description: This file contains all the wellknown names for the
//               SOM
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_y60_DataTypes_h_
#define _ac_y60_DataTypes_h_

#include "typedefs.h"
#include "iostream_functions.h"
#include "RankedFeature.h"
#include "NodeValueNames.h"

#include <dom/Value.h>
#include <asl/string_functions.h>
#include <asl/linearAlgebra.h>
#include <asl/Exception.h>
#include <asl/Box.h>
#include <asl/Matrix4.h>
#include <asl/Vector234.h>
#include <asl/Quaternion.h>
#include <asl/Frustum.h>
#include <asl/BSpline.h>

#include <string>
#include <vector>

namespace y60 {

	void registerSomImageTypes(dom::ValueFactory & theFactory);
	void registerSomMathTypes(dom::ValueFactory & theFactory);
	void registerSomVectorTypes(dom::ValueFactory & theFactory);
    void registerSomTypes(dom::ValueFactory & theFactory);

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

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Frustum, SimpleValue);
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
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfVectorOfString, ComplexValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfVectorOfFloat, ComplexValue, dom::MakeResizeableVector);
    
    DEFINE_VALUE_WRAPPER_TEMPLATE3(y60::RankedFeature, ComplexValue, dom::MakeOpaqueVector, void);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfRankedFeature, ComplexValue, dom::MakeResizeableVector);

    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::BlendEquation, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::RenderStyles, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::VertexBufferUsage, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::VectorOfBlendFunction, VectorValue, dom::MakeResizeableVector);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::TextureApplyMode, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::TextureSampleFilter, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::TextureWrapMode, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::TextureUsage, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(y60::ImageType, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::ResizePolicy, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::ProjectionType, SimpleValue);
    
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

    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfRGB, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfRGBA, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfBGR, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfBGRA, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfABGR, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfGRAY, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfGRAY16, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfGRAY32, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfGRAYS16, ComplexValue, dom::MakeResizeableRaster);
    
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfDXT1, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfDXT1a, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfDXT3, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfDXT5, ComplexValue, dom::MakeResizeableRaster);

    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfRGBA_HALF, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfRGB_HALF, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfRGBA_FLOAT, ComplexValue, dom::MakeResizeableRaster);
    DEFINE_VALUE_WRAPPER_TEMPLATE2(y60::RasterOfRGB_FLOAT, ComplexValue, dom::MakeResizeableRaster);
}

#endif
