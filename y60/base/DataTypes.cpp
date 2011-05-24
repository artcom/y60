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

//own header
#include "DataTypes.h"

#include "NodeValueNames.h"
#include "FrustumValue.h"

namespace y60 {

    void registerSomImageTypes(dom::ValueFactory & theFactory) {

        typedef dom::ValueWrapper<RasterOfRGB>::Type WrapperType;
        theFactory.registerPrototype("RasterOfRGB",
                dom::ValuePtr(new WrapperType(RasterOfRGB(0,0,0),0)));
        theFactory.registerPrototype("RasterOfRGBA",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfRGBA>::Type(RasterOfRGBA(0,0,0),0)));
        theFactory.registerPrototype("RasterOfBGR",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfBGR>::Type(RasterOfBGR(0,0,0),0)));
        theFactory.registerPrototype("RasterOfBGRA",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfBGRA>::Type(RasterOfBGRA(0,0,0),0)));
        theFactory.registerPrototype("RasterOfABGR",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfABGR>::Type(RasterOfABGR(0,0,0),0)));
        theFactory.registerPrototype("RasterOfGRAY",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfGRAY>::Type(RasterOfGRAY(0,0,0),0)));
        theFactory.registerPrototype("RasterOfALPHA",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfGRAY>::Type(RasterOfGRAY(0,0,0),0)));
        theFactory.registerPrototype("RasterOfDEPTH",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfGRAY>::Type(RasterOfGRAY(0,0,0),0)));
        theFactory.registerPrototype("RasterOfGRAY16",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfGRAY16>::Type(RasterOfGRAY16(0,0,0),0)));
        theFactory.registerPrototype("RasterOfGRAY32",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfGRAY32>::Type(RasterOfGRAY32(0,0,0),0)));

        theFactory.registerPrototype("RasterOfGRAYS16",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfGRAYS16>::Type(RasterOfGRAYS16(0,0,0),0)));

        theFactory.registerPrototype("RasterOfDXT1",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfDXT1>::Type(RasterOfDXT1(0,0,0),0)));
        theFactory.registerPrototype("RasterOfDXT1a",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfDXT1a>::Type(RasterOfDXT1a(0,0,0),0)));
        theFactory.registerPrototype("RasterOfDXT3",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfDXT3>::Type(RasterOfDXT3(0,0,0),0)));
        theFactory.registerPrototype("RasterOfDXT5",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfDXT5>::Type(RasterOfDXT5(0,0,0),0)));

        theFactory.registerPrototype("RasterOfRGBA_HALF",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfRGBA_HALF>::Type(RasterOfRGBA_HALF(0,0,0),0)));
        theFactory.registerPrototype("RasterOfRGB_HALF",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfRGB_HALF>::Type(RasterOfRGB_HALF(0,0,0),0)));
        theFactory.registerPrototype("RasterOfRGBA_FLOAT",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfRGBA_FLOAT>::Type(RasterOfRGBA_FLOAT(0,0,0),0)));
        theFactory.registerPrototype("RasterOfRGB_FLOAT",
                dom::ValuePtr(new dom::ValueWrapper<RasterOfRGB_FLOAT>::Type(RasterOfRGB_FLOAT(0,0,0),0)));
    }

    void registerSomMathTypes(dom::ValueFactory & theFactory) {
        theFactory.registerPrototype("Vector2i", dom::ValuePtr(new dom::ValueWrapper<asl::Vector2i>::Type(asl::Vector2i(0,0),0)));
        theFactory.registerPrototype("Vector2f", dom::ValuePtr(new dom::ValueWrapper<asl::Vector2f>::Type(asl::Vector2f(0.0,0.0),0)));
        theFactory.registerPrototype("Vector2d", dom::ValuePtr(new dom::ValueWrapper<asl::Vector2d>::Type(asl::Vector2d(0.0,0.0),0)));

        theFactory.registerPrototype("Vector3i", dom::ValuePtr(new dom::ValueWrapper<asl::Vector3i>::Type(asl::Vector3i(0,0,0),0)));
        theFactory.registerPrototype("Vector3f", dom::ValuePtr(new dom::ValueWrapper<asl::Vector3f>::Type(asl::Vector3f(0,0,0),0)));
        theFactory.registerPrototype("Vector3d", dom::ValuePtr(new dom::ValueWrapper<asl::Vector3d>::Type(asl::Vector3d(0,0,0),0)));

        theFactory.registerPrototype("Vector4f", dom::ValuePtr(new dom::ValueWrapper<asl::Vector4f>::Type(asl::Vector4f(0,0,0,0),0)));
        theFactory.registerPrototype("Vector4d", dom::ValuePtr(new dom::ValueWrapper<asl::Vector4d>::Type(asl::Vector4d(0,0,0,0),0)));

        theFactory.registerPrototype("Point2i", dom::ValuePtr(new dom::ValueWrapper<asl::Point2i>::Type(asl::Point2i(0,0),0)));
        theFactory.registerPrototype("Point2f", dom::ValuePtr(new dom::ValueWrapper<asl::Point2f>::Type(asl::Point2f(0.0f,0.0f),0)));
        theFactory.registerPrototype("Point2d", dom::ValuePtr(new dom::ValueWrapper<asl::Point2d>::Type(asl::Point2d(0.0,0.0),0)));

        theFactory.registerPrototype("Point3f", dom::ValuePtr(new dom::ValueWrapper<asl::Point3f>::Type(asl::Point3f(0,0,0),0)));
        theFactory.registerPrototype("Point3d", dom::ValuePtr(new dom::ValueWrapper<asl::Point3d>::Type(asl::Point3d(0,0,0),0)));

        theFactory.registerPrototype("Point4f", dom::ValuePtr(new dom::ValueWrapper<asl::Point4f>::Type(asl::Point4f(0,0,0,0),0)));
        theFactory.registerPrototype("Point4d", dom::ValuePtr(new dom::ValueWrapper<asl::Point4d>::Type(asl::Point4d(0,0,0,0),0)));

        theFactory.registerPrototype("Matrix4f", dom::ValuePtr(new dom::ValueWrapper<asl::Matrix4f>::Type(asl::Matrix4f(),0)));
        theFactory.registerPrototype("Matrix4d", dom::ValuePtr(new dom::ValueWrapper<asl::Matrix4d>::Type(asl::Matrix4d(),0)));

        theFactory.registerPrototype("Quaternionf", dom::ValuePtr(new dom::ValueWrapper<asl::Quaternionf>::Type(asl::Quaternionf(),0)));
        theFactory.registerPrototype("Frustum", dom::ValuePtr(new dom::ValueWrapper<asl::Frustum>::Type(asl::Frustum(),0)));

        theFactory.registerPrototype("Planef", dom::ValuePtr(new dom::ValueWrapper<asl::Planef>::Type(asl::Planef(),0)));

        theFactory.registerPrototype("Box2i", dom::ValuePtr(new dom::ValueWrapper<asl::Box2i>::Type(asl::Box2i(),0)));
        theFactory.registerPrototype("Box2f", dom::ValuePtr(new dom::ValueWrapper<asl::Box2f>::Type(asl::Box2f(),0)));
        theFactory.registerPrototype("Box2d", dom::ValuePtr(new dom::ValueWrapper<asl::Box2d>::Type(asl::Box2d(),0)));

        theFactory.registerPrototype("Box3i", dom::ValuePtr(new dom::ValueWrapper<asl::Box3i>::Type(asl::Box3i(),0)));
        theFactory.registerPrototype("Box3f", dom::ValuePtr(new dom::ValueWrapper<asl::Box3f>::Type(asl::Box3f(),0)));
        theFactory.registerPrototype("Box3d", dom::ValuePtr(new dom::ValueWrapper<asl::Box3d>::Type(asl::Box3d(),0)));
    }

    void registerSomVectorTypes(dom::ValueFactory & theFactory) {
        theFactory.registerPrototype("VectorOfBool",
                dom::ValuePtr(new dom::ValueWrapper<VectorOfBool>::Type(VectorOfBool(),0)));

        theFactory.registerPrototype("VectorOfUnsignedInt",
                dom::ValuePtr(new dom::ValueWrapper<VectorOfUnsignedInt>::Type(VectorOfUnsignedInt(),0)));

        theFactory.registerPrototype("VectorOfSignedInt",
                dom::ValuePtr(new dom::ValueWrapper<VectorOfSignedInt>::Type(VectorOfSignedInt(),0)));

        theFactory.registerPrototype("VectorOfFloat",
                dom::ValuePtr(new dom::ValueWrapper<VectorOfFloat>::Type(VectorOfFloat(),0)));

        theFactory.registerPrototype("VectorOfVector2i",
                dom::ValuePtr(new dom::ValueWrapper<VectorOfVector2i>::Type(VectorOfVector2i(),0)));

        theFactory.registerPrototype("VectorOfVector2f",
                dom::ValuePtr(new dom::ValueWrapper<VectorOfVector2f>::Type(VectorOfVector2f(),0)));

        theFactory.registerPrototype("VectorOfVector3f",
                dom::ValuePtr(new dom::ValueWrapper<VectorOfVector3f>::Type(VectorOfVector3f(),0)));

        theFactory.registerPrototype("VectorOfVector4f",
                dom::ValuePtr(new dom::ValueWrapper<VectorOfVector4f>::Type(VectorOfVector4f(),0)));

        theFactory.registerPrototype("VectorOfQuaternionf",
                dom::ValuePtr(new dom::ValueWrapper<VectorOfQuaternionf>::Type(VectorOfQuaternionf(),0)));

        theFactory.registerPrototype("VectorOfString",
                dom::ValuePtr(new dom::ComplexValue<VectorOfString,dom::MakeResizeableVector,dom::StringValue>(VectorOfString(),0)));

        theFactory.registerPrototype("VectorOfVectorOfString",
                dom::ValuePtr(new dom::ValueWrapper<VectorOfVectorOfString>::Type(VectorOfVectorOfString(),0)));

        theFactory.registerPrototype("VectorOfVectorOfFloat",
                dom::ValuePtr(new dom::ValueWrapper<VectorOfVectorOfFloat>::Type(VectorOfVectorOfFloat(),0)));

        theFactory.registerPrototype("VectorOfRankedFeature",
                dom::ValuePtr(new dom::ValueWrapper<VectorOfRankedFeature>::Type(VectorOfRankedFeature(),0)));
    }

    void registerEnumBitsetTypes(dom::ValueFactory & theFactory) {
        theFactory.registerPrototype("VectorOfBlendEquation",
                                     dom::ValuePtr(new dom::ValueWrapper<y60::VectorOfBlendEquation>::Type(VectorOfBlendEquation(),0)));
        theFactory.registerPrototype("VectorOfBlendFunction",
                                     dom::ValuePtr(new dom::ValueWrapper<VectorOfBlendFunction>::Type(VectorOfBlendFunction(),0)));

        theFactory.registerPrototype("RenderStyle", dom::ValuePtr(new dom::ValueWrapper<y60::RenderStyle>::Type(FRONT,0)));
        theFactory.registerPrototype("RenderStyles", dom::ValuePtr(new dom::ValueWrapper<y60::RenderStyles>::Type(y60::RenderStyles(0),0)));
        theFactory.registerPrototype("TargetBuffers", dom::ValuePtr(new dom::ValueWrapper<y60::TargetBuffers>::Type(y60::TargetBuffers(0),0)));
        theFactory.registerPrototype("VertexBufferUsage", dom::ValuePtr(new dom::ValueWrapper<y60::VertexBufferUsage>::Type(y60::getDefaultVertexBufferUsage(),0)));

        theFactory.registerPrototype("TextureApplyMode", dom::ValuePtr(new dom::ValueWrapper<y60::TextureApplyMode>::Type(y60::TextureApplyMode(MODULATE),0)));
        theFactory.registerPrototype("TextureSampleFilter", dom::ValuePtr(new dom::ValueWrapper<y60::TextureSampleFilter>::Type(y60::TextureSampleFilter(LINEAR),0)));
        theFactory.registerPrototype("TextureWrapMode", dom::ValuePtr(new dom::ValueWrapper<y60::TextureWrapMode>::Type(y60::TextureWrapMode(REPEAT),0)));

        theFactory.registerPrototype("TextureType", dom::ValuePtr(new dom::ValueWrapper<y60::TextureType>::Type(y60::TextureType(TEXTURE_2D),0)));
        theFactory.registerPrototype("TextureUsage", dom::ValuePtr(new dom::ValueWrapper<y60::TextureUsage>::Type(y60::TextureUsage(PAINT),0)));
        theFactory.registerPrototype("ResizePolicy", dom::ValuePtr(new dom::ValueWrapper<asl::ResizePolicy>::Type(asl::ResizePolicy(asl::ADAPT_VERTICAL),0)));

        theFactory.registerPrototype("ImageType", dom::ValuePtr(new dom::ValueWrapper<y60::ImageType>::Type(y60::ImageType(SINGLE),0)));
        theFactory.registerPrototype("PrimitiveType", dom::ValuePtr(new dom::ValueWrapper<y60::PrimitiveType>::Type(y60::PrimitiveType(POINTS),0)));
    }

    void registerSomTypes(dom::ValueFactory & theFactory) {
        registerSomMathTypes(theFactory);
        registerSomVectorTypes(theFactory);
        registerSomImageTypes(theFactory);
        registerEnumBitsetTypes(theFactory);
    }
}
